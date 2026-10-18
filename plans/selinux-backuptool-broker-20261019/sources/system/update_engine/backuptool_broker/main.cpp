// Copyright (C) 2026 CyberWei
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "broker.h"

#include <fcntl.h>
#include <sched.h>
#include <selinux/selinux.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {
constexpr char kWorkspace[] = "/postinstall";
constexpr char kWorkspaceContext[] = "u:object_r:backuptool_workspace_file:s0";
constexpr char kScript[] = "/postinstall/system/bin/backuptool_postinstall.sh";

bool Error(const std::string& operation) {
  std::cerr << "backuptool_broker: " << operation << ": "
            << std::strerror(errno) << '\n';
  return false;
}
// The paths queried here are fixed names with no mountinfo escapes.
// Treat an unreadable mount table as an error, never as 'not mounted'.
bool Mounted(const std::string& path, bool* mounted) {
  std::ifstream input("/proc/self/mountinfo");
  if (!input.is_open()) return Error("open mountinfo");
  *mounted = false;
  std::string line;
  while (std::getline(input, line)) {
    std::istringstream fields(line);
    std::string field;
    for (int i = 0; i < 5; ++i) {
      if (!(fields >> field)) { errno = EIO; return Error("parse mountinfo"); }
    }
    if (field == path) *mounted = true;
  }
  if (input.bad()) { errno = EIO; return Error("read mountinfo"); }
  return true;
}
bool Directory(const std::string& path) {
  struct stat info {};
  return lstat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}
bool Ext4Block(const std::string& path, dev_t* device) {
  const int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
  if (fd < 0) return Error("open block " + path);
  struct stat info {};
  unsigned char magic[2] = {};
  const bool valid = fstat(fd, &info) == 0 && S_ISBLK(info.st_mode) &&
      pread(fd, magic, sizeof(magic), 0x438) == sizeof(magic) &&
      magic[0] == 0x53 && magic[1] == 0xef;
  const int close_status = close(fd);
  if (!valid || close_status != 0) {
    errno = EINVAL;
    return Error("require ext4 block " + path);
  }
  *device = info.st_rdev;
  return true;
}

class RealOperations final : public backuptool_broker::Operations {
 public:
  RealOperations(std::string target, std::string source)
      : target_(std::move(target)), source_(std::move(source)) {}
  bool Validate() override {
    char* context = nullptr;
    if (getcon(&context) != 0) return Error("getcon");
    const bool expected = context != nullptr &&
        std::string(context) == "u:r:update_engine:s0";
    freecon(context);
    if (!expected) { errno = EPERM; return Error("caller domain"); }
    dev_t target = 0, source = 0;
    if (!Ext4Block(target_, &target) || !Ext4Block(source_, &source)) return false;
    if (target == source) { errno = EINVAL; return Error("distinct slot devices"); }
    bool mounted = false;
    if (!Directory(kWorkspace) || !Mounted(kWorkspace, &mounted)) return false;
    if (mounted) { errno = EBUSY; return Error("workspace already mounted"); }
    return true;
  }
  bool IsolateMounts() override {
    if (unshare(CLONE_NEWNS) != 0) return Error("private mount namespace");
    if (mount(nullptr, "/", nullptr, MS_REC | MS_PRIVATE, nullptr) != 0)
      return Error("disable mount propagation");
    return true;
  }
  bool MountWorkspace() override {
    // Only the root inode receives this initial label. Do not use context=:
    // it would force every inode label and prevent cp -a preserving xattrs.
    return mount("tmpfs", kWorkspace, "tmpfs", MS_NOSUID | MS_NODEV,
                 "mode=0700,rootcontext=u:object_r:backuptool_workspace_file:s0")
               == 0 || Error("mount workspace");
  }
  bool MakeDirectory(const std::string& path) override {
    // There is no worker until setup completes; these are fixed names on a
    // newly mounted tmpfs. Explicit fscreate avoids depending on inheritance.
    if (setfscreatecon(kWorkspaceContext) != 0) return Error("fscreate label");
    const bool created = mkdir(path.c_str(), 0700) == 0;
    const int saved = errno;
    const bool reset = setfscreatecon(nullptr) == 0;
    if (!reset) return Error("reset fscreate label");
    errno = saved;
    return created || Error("mkdir " + path);
  }
  bool MountTarget(const std::string& path) override {
    return mount(target_.c_str(), path.c_str(), "ext4",
                 MS_NOATIME | MS_NODEV | MS_NOSUID | MS_NODIRATIME,
                 "seclabel") == 0 || Error("mount inactive system");
  }
  bool BindSystem(const std::string& root, const std::string& view) override {
    const std::string source = Directory(root + "/system") ? root + "/system" : root;
    if (!Directory(source + "/bin")) {
      errno = EINVAL;
      return Error("inactive system layout");
    }
    return mount(source.c_str(), view.c_str(), nullptr, MS_BIND, nullptr) == 0 ||
           Error("bind inactive system view");
  }
  int RunWorker() override {
    if (setenv("BACKUPTOOL_BROKER_WORKSPACE", "1", 1) != 0) {
      Error("workspace environment"); return 26;
    }
    const pid_t child = fork();
    if (child < 0) { Error("fork worker"); return 27; }
    if (child == 0) {
      // Change only the worker's next exec; the broker and update_engine
      // never carry a pending exec label or enter the backuptool domain.
      if (setexeccon("u:r:backuptool:s0") != 0) {
        Error("worker next-exec label"); _exit(126);
      }
      execl(kScript, kScript, static_cast<char*>(nullptr));
      Error("exec worker"); _exit(127);
    }
    int status = 0;
    pid_t waited;
    do { waited = waitpid(child, &status, 0); } while (waited < 0 && errno == EINTR);
    if (waited != child) { Error("wait worker"); return 28; }
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);
    return 29;
  }
  bool UnmountIfMounted(const std::string& path) override {
    bool mounted = false;
    if (!Mounted(path, &mounted)) return false;
    if (!mounted) return true;
    // No lazy detach: a busy filesystem is a reported cleanup failure.
    return umount(path.c_str()) == 0 || Error("unmount " + path);
  }
 private:
  const std::string target_;
  const std::string source_;
};
}  // namespace

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: backuptool_broker <inactive-ext4-block> <active-ext4-block>\n";
    return 2;
  }
  RealOperations operations(argv[1], argv[2]);
  return backuptool_broker::Run(operations);
}
