// Copyright (C) 2026 CyberWei
// SPDX-License-Identifier: Apache-2.0
#include "broker.h"
#include "worker.h"
#include <fcntl.h>
#include <linux/fs.h>
#include <sched.h>
#include <selinux/selinux.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/wait.h>
#include <unistd.h>
#include <array>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
namespace {
constexpr char kRoot[] = "/postinstall";
constexpr char kRootContext[] = "u:object_r:backuptool_broker_root_file:s0";
constexpr char kWorkContext[] = "u:object_r:backuptool_workspace_file:s0";
constexpr char kTargetOptions[] = "context=u:object_r:postinstall_file:s0";
struct Fd { int n; explicit Fd(int value) : n(value) {} ~Fd() { if (n >= 0) close(n); } };
struct Part { std::string name, target, source, filesystem; dev_t target_id = 0, source_id = 0; };
bool Error(const std::string& what) {
  std::cerr << "backuptool_broker: " << what << ": " << std::strerror(errno) << '\n'; return false;
}
bool Directory(const std::string& path) {
  struct stat info {}; return lstat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}
bool Mounted(const std::string& path, bool* found, dev_t device = 0) {
  std::ifstream input("/proc/self/mountinfo"); if (!input.is_open()) return Error("open mountinfo");
  *found = false; std::string line;
  while (std::getline(input, line)) {
    std::istringstream fields(line); std::string id, parent, number, root, mountpoint;
    if (!(fields >> id >> parent >> number >> root >> mountpoint)) { errno = EIO; return Error("parse mountinfo"); }
    if ((!path.empty() && mountpoint == path) || (device && number == std::to_string(major(device)) + ":" + std::to_string(minor(device)))) *found = true;
  }
  if (input.bad()) { errno = EIO; return Error("read mountinfo"); } return true;
}
bool Block(const std::string& path, dev_t* device, std::string* filesystem) {
  Fd fd(open(path.c_str(), O_RDONLY | O_CLOEXEC)); struct stat info {};
  if (fd.n < 0 || fstat(fd.n, &info) != 0 || !S_ISBLK(info.st_mode)) { errno = EINVAL; return Error("block identity " + path); }
  std::array<unsigned char, 4> magic{}; std::array<unsigned char, 2> ext4{};
  if (pread(fd.n, ext4.data(), ext4.size(), 0x438) != 2 || pread(fd.n, magic.data(), magic.size(), 0x400) != 4) return Error("read filesystem magic");
  if (ext4[0] == 0x53 && ext4[1] == 0xef) *filesystem = "ext4";
  else if (magic == std::array<unsigned char, 4>{0xe2, 0xe1, 0xf5, 0xe0}) *filesystem = "erofs";
  else { errno = EINVAL; return Error("unsupported filesystem"); }
  *device = info.st_rdev; return true;
}
bool SetReadOnly(const std::string& path, bool readonly) {
  Fd fd(open(path.c_str(), O_RDONLY | O_CLOEXEC)); int value = readonly ? 1 : 0;
  return (fd.n >= 0 && ioctl(fd.n, BLKROSET, &value) == 0) || Error("restore/set block mode " + path);
}
int Wait(pid_t child) {
  int status = 0; pid_t waited;
  do { waited = waitpid(child, &status, 0); } while (waited < 0 && errno == EINTR);
  if (waited != child) { Error("wait child"); return 128; }
  if (WIFEXITED(status)) return WEXITSTATUS(status);
  return WIFSIGNALED(status) ? 128 + WTERMSIG(status) : 129;
}
bool Write(int fd, const void* data, size_t bytes) {
  const auto* cursor = static_cast<const char*>(data);
  while (bytes) {
    auto n = write(fd, cursor, bytes); if (n < 0 && errno == EINTR) continue;
    if (n <= 0) return Error("write protected broker state");
    cursor += n; bytes -= n;
  }
  return true;
}
class Real final : public backuptool_broker::Operations {
 public:
  explicit Real(std::vector<Part> parts) : parts_(std::move(parts)) {}
  bool Validate() override {
    char* context = nullptr; if (getcon(&context) != 0) return Error("getcon");
    const bool domain = context && std::string(context) == "u:r:update_engine:s0"; freecon(context);
    if (!domain) { errno = EPERM; return Error("caller domain"); }
    std::set<dev_t> targets, sources; std::set<std::string> names;
    for (auto& p : parts_) {
      std::string source_fs;
      if (!names.insert(p.name).second || !Block(p.target, &p.target_id, &p.filesystem) || !Block(p.source, &p.source_id, &source_fs)) return false;
      if (!targets.insert(p.target_id).second || !sources.insert(p.source_id).second || (p.name == "system" && (p.filesystem != "ext4" || source_fs != "ext4"))) { errno = EINVAL; return Error("duplicate device or primary filesystem"); }
      bool mounted = false;
      if (!Mounted("", &mounted, p.target_id) || mounted) { errno = EBUSY; return Error("target is already mounted"); }
    }
    for (auto target : targets) if (sources.count(target)) { errno = EINVAL; return Error("inactive device aliases active source"); }
    bool mounted = false;
    if (!Directory(kRoot) || !Mounted(kRoot, &mounted)) return false;
    if (mounted) { errno = EBUSY; return Error("workspace is already mounted"); } return true;
  }
  bool IsolateMounts() override {
    if (unshare(CLONE_NEWNS) != 0) return Error("private mount namespace");
    return mount(nullptr, "/", nullptr, MS_REC | MS_PRIVATE, nullptr) == 0 || Error("disable mount propagation");
  }
  bool MountWorkspace() override {
    return mount("tmpfs", kRoot, "tmpfs", MS_NOSUID | MS_NODEV,
        "size=1g,mode=0700,rootcontext=u:object_r:backuptool_broker_root_file:s0") == 0 || Error("mount bounded workspace");
  }
  bool MakeDirectory(const std::string& path) override {
    const char* label = path == "/postinstall/tmp" ? kWorkContext : kRootContext;
    if (setfscreatecon(label) != 0) return Error("set directory label");
    bool made = mkdir(path.c_str(), 0700) == 0; int saved = errno;
    if (setfscreatecon(nullptr) != 0) return Error("reset create label");
    errno = saved;
    return made || Error("mkdir " + path);
  }
  bool MountTarget(const std::string& path) override {
    return Mount(parts_.front(), path);
  }
  bool BindSystem(const std::string& root, const std::string& view) override { return Bind(root, view, "system"); }
  bool MountExtras() override {
    for (size_t i = 1; i < parts_.size(); ++i) {
      const auto raw = "/postinstall/inactive-" + parts_[i].name;
      if (!Mount(parts_[i], raw) || !Bind(raw, "/postinstall/" + parts_[i].name, parts_[i].name)) return false;
    }
    return true;
  }
  bool SnapshotContexts() override {
    std::string manifest, devices;
    for (const auto& p : parts_) {
      manifest += p.name + " " + p.filesystem + "\n";
      devices += p.name + " " + std::to_string(major(p.target_id)) + ":" + std::to_string(minor(p.target_id)) + " " + std::to_string(major(p.source_id)) + ":" + std::to_string(minor(p.source_id)) + "\n";
    }
    if (!State("/postinstall/partitions", manifest) || !State("/postinstall/.broker/devices", devices)) return false;
    for (const auto& p : parts_) {
      const auto name = p.name == "system" ? "plat_file_contexts" : p.name + "_file_contexts";
      const auto base = "/postinstall/" + p.name + "/etc/selinux/";
      if (!Directory("/postinstall/" + p.name + "/etc") || !Directory(base.substr(0, base.size() - 1))) {
        errno = ENOENT; return Error("target context directory " + p.name);
      }
      Fd input(open((base + name).c_str(), O_RDONLY | O_NOFOLLOW | O_CLOEXEC));
      if (input.n < 0) {
        if (errno == ENOENT && p.name != "system") continue;
        return Error("target context file " + name);
      }
      struct stat info {}; if (fstat(input.n, &info) != 0 || !S_ISREG(info.st_mode) || info.st_size <= 0 || info.st_size > 2 * 1024 * 1024) { errno = EINVAL; return Error("context input size/type"); }
      std::string bytes(info.st_size, '\0'); size_t offset = 0;
      while (offset < bytes.size()) {
        auto n = read(input.n, bytes.data() + offset, bytes.size() - offset); if (n < 0 && errno == EINTR) continue;
        if (n <= 0) return Error("read target context");
        offset += n;
      }
      if (!State("/postinstall/.broker/contexts/" + name, bytes)) return false;
    }
    return true;
  }
  int RunWorker() override { return backuptool_broker::RunStagedWorker(); }
  bool UnmountIfMounted(const std::string& path) override {
    bool mounted = false; if (!Mounted(path, &mounted)) return false;
    if (!mounted) return true;
    return umount(path.c_str()) == 0 || Error("strict unmount " + path);
  }
  int Commit() override {
    // Validate every target first. A V3 attempt to mutate EROFS, a bad
    // symlink, missing label or insufficient capacity aborts before any write.
    for (const auto& p : parts_) if (Store(p, "--check") != 0) return 40;
    for (const auto& p : parts_) {
      if (p.filesystem != "ext4") continue;
      if (!SetReadOnly(p.target, false)) return 41;
      int status = Store(p, "--apply");
      const bool readonly = SetReadOnly(p.target, true);
      if (status) return status;
      if (!readonly) return 42;
    }
    return 0;
  }
 private:
  bool Mount(const Part& p, const std::string& destination) {
    return mount(p.target.c_str(), destination.c_str(), p.filesystem.c_str(),
        MS_RDONLY | MS_NOSUID | MS_NODEV | MS_NOATIME, kTargetOptions) == 0 || Error("read-only inactive " + p.name);
  }
  bool Bind(const std::string& raw, const std::string& view, const std::string& name) {
    const auto nested = raw + "/" + name; struct stat info {}; std::string source = raw;
    if (lstat(nested.c_str(), &info) == 0) {
      if (!S_ISDIR(info.st_mode)) { errno = EINVAL; return Error("inactive root layout/symlink"); }
      source = nested;
    } else if (errno != ENOENT) return Error("inactive root stat");
    return mount(source.c_str(), view.c_str(), nullptr, MS_BIND, nullptr) == 0 || Error("bind inactive " + name);
  }
  bool State(const std::string& path, const std::string& data) {
    if (setfscreatecon(kRootContext) != 0) return Error("broker file context");
    Fd fd(open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0400));
    const int saved = errno; const bool reset = setfscreatecon(nullptr) == 0; errno = saved;
    if (!reset || fd.n < 0) return Error("create protected broker state");
    return (Write(fd.n, data.data(), data.size()) && fsync(fd.n) == 0) || Error("flush broker state");
  }
  int Store(const Part& p, const char* action) {
    const pid_t broker = getpid();
    const pid_t child = fork(); if (child < 0) { Error("fork offline committer"); return 43; }
    if (!child) {
      if (prctl(PR_SET_PDEATHSIG, SIGKILL) != 0 || getppid() != broker) _exit(126);
      execl("/system/bin/backuptool_store", "backuptool_store", action,
            p.name.c_str(), p.target.c_str(), p.source.c_str(), static_cast<char*>(nullptr));
      Error("exec offline committer"); _exit(127);
    }
    return Wait(child);
  }
  std::vector<Part> parts_;
};
}  // namespace
int main(int argc, char** argv) {
  if (argc < 4 || std::string(argv[1]) != "--v2" || (argc - 4) % 3) {
    std::cerr << "Usage: backuptool_broker --v2 <system-target> <system-source> [name target source]...\n"; return 2;
  }
  std::vector<Part> parts = {{"system", argv[2], argv[3], ""}};
  for (int i = 4; i < argc; i += 3) {
    const std::string name = argv[i];
    if (name != "vendor" && name != "product" && name != "system_ext" && name != "odm") return 2;
    parts.push_back({name, argv[i + 1], argv[i + 2], ""});
  }
  if (parts.size() > 5) return 2;
  const pid_t caller = getppid();
  if (caller <= 1 || prctl(PR_SET_PDEATHSIG, SIGKILL) != 0 || getppid() != caller) return 2;
  Real operations(std::move(parts)); return backuptool_broker::Run(operations);
}
