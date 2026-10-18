// Copyright (C) 2026 CyberWei
// SPDX-License-Identifier: Apache-2.0
#include "worker.h"
#include <linux/audit.h>
#include <linux/sched.h>
#include <linux/seccomp.h>
#include <selinux/selinux.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <cerrno>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <time.h>
namespace backuptool_broker {
namespace {
constexpr uint32_t kNamespaces = CLONE_NEWNS | CLONE_NEWCGROUP | CLONE_NEWUTS |
    CLONE_NEWIPC | CLONE_NEWUSER | CLONE_NEWPID | CLONE_NEWNET;
void Architecture(std::vector<sock_filter>& code, uint32_t arch, uint32_t pgid,
                  uint32_t sid, uint32_t clone, uint32_t unshare,
                  uint32_t setns, uint32_t mask = UINT32_MAX) {
  std::vector<sock_filter> body;
  body.push_back(BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(seccomp_data, nr)));
  if (mask != UINT32_MAX) body.push_back(BPF_STMT(BPF_ALU | BPF_AND | BPF_K, mask));
  for (auto number : {pgid, sid, unshare, setns}) {
    body.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, number, 0, 1));
    body.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | EPERM));
  }
  // clone3's pointed-to flags cannot be safely inspected by classic BPF.
  // ENOSYS permits libc to fall back to traditional clone/fork.
  body.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 435, 0, 1));
  body.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | ENOSYS));
  body.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, clone, 0, 4));
  body.push_back(BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(seccomp_data, args[0])));
  body.push_back(BPF_STMT(BPF_ALU | BPF_AND | BPF_K, kNamespaces));
  body.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 1, 0));
  body.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | EPERM));
  body.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW));
  code.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, arch, 0, static_cast<uint8_t>(body.size())));
  code.insert(code.end(), body.begin(), body.end());
}
#ifndef __ANDROID__
const WorkerFixture* fixture = nullptr;
#endif
bool Filter() {
#ifndef __ANDROID__
  if (fixture && fixture->fail_filter) { errno = EPERM; return false; }
  // Rootless host fixtures cannot use Android updater CAP_SYS_ADMIN. This
  // host-only no_new_privs setup is NOT compiled into the device executable;
  // device domain transition must retain the normal SELinux exec semantics.
  if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0) return false;
#endif
  auto bytes = WorkerFilter();
  sock_fprog program{static_cast<unsigned short>(bytes.size()), bytes.data()};
  return prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &program) == 0;
}
bool Context() {
#ifndef __ANDROID__
  if (fixture) { if (fixture->fail_context) errno = EPERM; return !fixture->fail_context; }
#endif
  return setexeccon("u:r:backuptool:s0") == 0;
}
const char* Script() {
#ifndef __ANDROID__
  if (fixture) return fixture->script;
#endif
  return "/system/bin/backuptool_staged.sh";
}
const char* Workspace() {
#ifndef __ANDROID__
  if (fixture) return fixture->workspace;
#endif
  return "/postinstall/tmp";
}
const char* Path() {
#ifndef __ANDROID__
  if (fixture) return fixture->path;
#endif
  return "/system/bin";
}
void ParentDied(int) {
  // Installed only AFTER this controller creates its own process group.
  // Its live PID reserves this group ID, so no unrelated group is targeted.
  kill(-getpid(), SIGKILL);
  _exit(129);
}
int Status(int value) {
  if (WIFEXITED(value)) return WEXITSTATUS(value);
  return WIFSIGNALED(value) ? 128 + WTERMSIG(value) : 129;
}
}  // namespace
std::vector<sock_filter> WorkerFilter() {
  std::vector<sock_filter> bytes;
  bytes.push_back(BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(seccomp_data, arch)));
  // Numbers follow Android arm64/ARM UAPI syscall tables. The ARM mask also
  // blocks historical OABI-number spellings rather than permitting an alias.
  Architecture(bytes, AUDIT_ARCH_AARCH64, 154, 157, 220, 97, 268);
  Architecture(bytes, AUDIT_ARCH_ARM, 57, 66, 120, 337, 375, 0x000fffff);
#ifndef __ANDROID__
  Architecture(bytes, AUDIT_ARCH_X86_64, 109, 112, 56, 272, 308, 0xbfffffff);
  Architecture(bytes, AUDIT_ARCH_I386, 57, 66, 120, 310, 346);
#endif
  bytes.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS));
  return bytes;
}
int RunStagedWorker() {
  // Only descendants of this short-lived broker are adopted. No global init,
  // cgroup configuration, or unrelated process is changed.
  if (prctl(PR_SET_CHILD_SUBREAPER, 1) != 0) return 26;
  struct sigaction child_action {}; child_action.sa_handler = SIG_DFL;
  sigemptyset(&child_action.sa_mask);
  if (sigaction(SIGCHLD, &child_action, nullptr) != 0) return 26;
  const pid_t broker = getpid(); const pid_t controller = fork();
  if (controller < 0) return 26;
  if (!controller) {
    if (setpgid(0, 0) != 0) _exit(27);
    struct sigaction action {}; action.sa_handler = ParentDied;
    sigemptyset(&action.sa_mask);
    sigset_t unblock; sigemptyset(&unblock); sigaddset(&unblock, SIGTERM);
    if (sigaction(SIGTERM, &action, nullptr) != 0 ||
        sigprocmask(SIG_UNBLOCK, &unblock, nullptr) != 0 ||
        prctl(PR_SET_PDEATHSIG, SIGTERM) != 0) _exit(27);
    if (getppid() != broker) ParentDied(SIGTERM);
    const pid_t worker = fork(); if (worker < 0) _exit(28);
    if (!worker) {
      signal(SIGTERM, SIG_DFL);
      if (!Filter()) _exit(125);
      if (setenv("BACKUPTOOL_BROKER_PROTOCOL", "2", 1) != 0 ||
          setenv("BACKUPTOOL_BROKER_WORKSPACE", "1", 1) != 0 ||
          setenv("PATH", Path(), 1) != 0 || chdir(Workspace()) != 0) _exit(29);
      unsetenv("BACKUPTOOL_LEGACY_WORKSPACE");
      if (!Context()) _exit(126);
      execl(Script(), "backuptool_staged.sh", static_cast<char*>(nullptr));
      _exit(127);
    }
    int status = 0; pid_t waited;
    do { waited = waitpid(worker, &status, 0); } while (waited < 0 && errno == EINTR);
    _exit(waited == worker ? Status(status) : 128);
  }
  // WNOWAIT keeps the private group leader's zombie/PID reserved until killpg.
  // Never reap first and then signal a numeric group ID that could be reused.
  siginfo_t event {}; int waited;
  do { waited = waitid(P_PID, controller, &event, WEXITED | WNOWAIT); } while (waited < 0 && errno == EINTR);
  if (waited != 0) return 128;
  if (kill(-controller, SIGKILL) != 0 && errno != ESRCH) return 130;
  int status = 0; pid_t pid;
  do { pid = waitpid(controller, &status, 0); } while (pid < 0 && errno == EINTR);
  int result = pid == controller ? Status(status) : 128;
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  for (;;) {
    int ignored = 0; pid = waitpid(-controller, &ignored, WNOHANG);
    if (pid > 0 || (pid < 0 && errno == EINTR)) continue;
    if (pid < 0 && errno == ECHILD) return result;
    if (pid < 0 || std::chrono::steady_clock::now() >= deadline) return result ? result : 131;
    timespec pause{0, 10000000}; nanosleep(&pause, nullptr);
  }
}
#ifndef __ANDROID__
int RunStagedWorkerForTest(const WorkerFixture& options) {
  fixture = &options; int result = RunStagedWorker(); fixture = nullptr; return result;
}
#endif
}  // namespace backuptool_broker
