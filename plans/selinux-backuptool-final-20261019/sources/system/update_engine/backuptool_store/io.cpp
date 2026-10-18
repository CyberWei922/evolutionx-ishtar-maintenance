// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2026 CyberWei
#include "store.h"
#include <dirent.h>
#include <fcntl.h>
#include <selinux/selinux.h>
#include <sys/xattr.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
namespace backup_store {
std::string host_root;
constexpr size_t kMaxFile = 512 * 1024 * 1024;
Fd::~Fd() { if (n >= 0) close(n); }
Fd::Fd(Fd&& other) noexcept : n(other.n) { other.n = -1; }
Fd& Fd::operator=(Fd&& other) noexcept {
  if (this != &other) { if (n >= 0) close(n); n = other.n; other.n = -1; }
  return *this;
}
[[noreturn]] void Fail(const std::string& message) {
  throw std::runtime_error(message + ": " + std::strerror(errno));
}
void Need(bool success, const std::string& message) { if (!success) Fail(message); }
std::string Local(const std::string& path) { return host_root + path; }
std::string Work(const std::string& suffix) { return Local("/postinstall/tmp" + (suffix.empty() ? std::string() : "/" + suffix)); }
void RequireDomain(const char* wanted) {
#ifdef __ANDROID__
  char* label = nullptr;
  Need(getcon(&label) == 0, "getcon");
  const bool valid = label && std::string(label) == wanted;
  freecon(label);
  errno = EPERM;
  Need(valid, "unexpected caller domain");
#else
  (void)wanted;
  Need(!host_root.empty(), "host fixture root is required");
#endif
}
Path ParsePath(const std::string& input) {
  Need(!input.empty() && input.size() < 4096 && input.front() == '/', "absolute bounded path required");
  for (unsigned char ch : input) Need(ch >= 32 && ch != 127, "control character in path");
  std::string path = input;
  for (const char* p : {"vendor", "product", "system_ext"}) {
    const std::string old = std::string("/system/") + p + "/";
    if (path.compare(0, old.size(), old) == 0) path = std::string("/") + p + "/" + path.substr(old.size());
  }
  auto end = path.find('/', 1);
  Need(end != std::string::npos && end + 1 < path.size(), "file below partition root required");
  Path result{path.substr(1, end - 1), path.substr(end + 1), path};
  Need(result.partition == "system" || result.partition == "vendor" || result.partition == "product" ||
       result.partition == "system_ext" || result.partition == "odm", "partition outside backup scope");
  std::istringstream pieces(result.relative);
  std::string piece;
  while (std::getline(pieces, piece, '/'))
    Need(!piece.empty() && piece != "." && piece != ".." && piece.size() <= 255, "unsafe path component");
  Need(result.relative.back() != '/', "trailing slash");
  return result;
}
Fd OpenDirectory(const std::string& path, bool create) {
  Need(!path.empty() && path.front() == '/', "directory must be absolute");
  Fd fd(open("/", O_RDONLY | O_DIRECTORY | O_CLOEXEC));
  Need(fd.n >= 0, "open root");
  std::istringstream parts(path.substr(1));
  std::string part;
  while (std::getline(parts, part, '/')) {
    Need(!part.empty() && part != "." && part != "..", "unsafe directory component");
    Fd next(openat(fd.n, part.c_str(), O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC));
    if (next.n < 0 && errno == ENOENT && create) {
      Need(mkdirat(fd.n, part.c_str(), 0700) == 0, "mkdir " + part);
      next = Fd(openat(fd.n, part.c_str(), O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC));
    }
    Need(next.n >= 0, "open directory " + part); fd = std::move(next);
  }
  return fd;
}
Fd Parent(const std::string& root, const std::string& relative, bool create, std::string* leaf) {
  Need(!relative.empty() && relative.front() != '/' && relative.back() != '/', "invalid relative path");
  Fd fd = OpenDirectory(root, create);
  std::istringstream parts(relative);
  std::string piece;
  std::vector<std::string> all;
  while (std::getline(parts, piece, '/')) {
    Need(!piece.empty() && piece != "." && piece != ".." && piece.size() <= 255, "unsafe relative component");
    all.push_back(piece);
  }
  Need(!all.empty(), "empty path"); *leaf = all.back(); all.pop_back();
  for (const auto& part : all) {
    Fd next(openat(fd.n, part.c_str(), O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC));
    if (next.n < 0 && errno == ENOENT && create) {
      Need(mkdirat(fd.n, part.c_str(), 0700) == 0, "mkdir parent");
      next = Fd(openat(fd.n, part.c_str(), O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC));
    }
    Need(next.n >= 0, "symlink/non-directory parent rejected"); fd = std::move(next);
  }
  return fd;
}
Fd ReadFile(const std::string& root, const std::string& relative) {
  std::string leaf; auto parent = Parent(root, relative, false, &leaf);
  Fd file(openat(parent.n, leaf.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
  Need(file.n >= 0, "open file " + relative);
  struct stat st {}; Need(fstat(file.n, &st) == 0 && S_ISREG(st.st_mode), "regular file required");
  return file;
}
std::string ReadBounded(int fd, size_t limit) {
  std::string bytes; std::array<char, 4096> buffer{};
  for (;;) {
    ssize_t count = read(fd, buffer.data(), buffer.size());
    if (count < 0 && errno == EINTR) continue;
    Need(count >= 0, "read"); if (!count) return bytes;
    Need(bytes.size() + static_cast<size_t>(count) <= limit, "input size limit");
    bytes.append(buffer.data(), count);
  }
}
void WriteAll(int fd, const void* input, size_t size) {
  const char* bytes = static_cast<const char*>(input);
  while (size) {
    ssize_t count = write(fd, bytes, size);
    if (count < 0 && errno == EINTR) continue;
    Need(count > 0, "write"); bytes += count; size -= count;
  }
}
void WriteFile(const std::string& root, const std::string& relative, const std::string& bytes) {
  std::string leaf; auto parent = Parent(root, relative, true, &leaf);
  Fd output(openat(parent.n, leaf.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW | O_CLOEXEC, 0600));
  Need(output.n >= 0, "write file " + relative); WriteAll(output.n, bytes.data(), bytes.size());
  Need(fsync(output.n) == 0, "fsync metadata");
}
void CopyData(int input, int output) {
  std::array<char, 65536> bytes{}; size_t total = 0;
  for (;;) {
    ssize_t count = read(input, bytes.data(), bytes.size());
    if (count < 0 && errno == EINTR) continue;
    Need(count >= 0, "read source"); if (!count) break;
    total += count; Need(total <= kMaxFile, "file exceeds backup size limit"); WriteAll(output, bytes.data(), count);
  }
  Need(fsync(output) == 0, "fsync staged file");
}
Meta ReadMeta(int parent, const std::string& leaf) {
  struct stat st {}; Need(fstatat(parent, leaf.c_str(), &st, AT_SYMLINK_NOFOLLOW) == 0, "stat source");
  Need(S_ISREG(st.st_mode) || S_ISLNK(st.st_mode) || S_ISDIR(st.st_mode), "special file is not preservable");
  Meta m; m.mode = st.st_mode; m.uid = st.st_uid; m.gid = st.st_gid;
  m.atime = st.st_atim.tv_sec; m.atime_ns = st.st_atim.tv_nsec;
  m.mtime = st.st_mtim.tv_sec; m.mtime_ns = st.st_mtim.tv_nsec;
  if (S_ISLNK(st.st_mode)) {
    std::array<char, 4096> link{}; ssize_t n = readlinkat(parent, leaf.c_str(), link.data(), link.size());
    Need(n > 0 && n < static_cast<ssize_t>(link.size()), "bounded symlink target required");
    m.link.assign(link.data(), n); return m;
  }
  Fd fd(openat(parent, leaf.c_str(), O_RDONLY | O_NOFOLLOW | O_CLOEXEC | (S_ISDIR(st.st_mode) ? O_DIRECTORY : 0)));
  Need(fd.n >= 0, "open metadata source");
  ssize_t size = flistxattr(fd.n, nullptr, 0);
  if (size < 0 && (errno == EOPNOTSUPP || errno == ENOTSUP)) return m;
  Need(size >= 0 && size <= 65536, "bounded source xattrs");
  std::string names(size, '\0');
  if (size) Need(flistxattr(fd.n, names.data(), names.size()) == size, "read xattr list");
  for (size_t at = 0; at < names.size();) {
    const auto end = names.find('\0', at); Need(end != std::string::npos, "malformed xattr list");
    std::string name = names.substr(at, end - at); at = end + 1;
    if (name == "security.selinux") continue;  // Always derived from the NEW target's policy.
    Need(!name.empty() && name.size() <= 255 && m.xattrs.size() < 1024, "xattr bounds");
    ssize_t n = fgetxattr(fd.n, name.c_str(), nullptr, 0); Need(n >= 0 && n <= 65536, "xattr value bounds");
    std::string value(n, '\0'); Need(fgetxattr(fd.n, name.c_str(), value.data(), value.size()) == n, "read xattr");
    m.xattrs.emplace(std::move(name), std::move(value));
  }
  return m;
}
static void Number(std::string& out, uint64_t n) { for (int j = 0; j < 8; ++j) { out.push_back(n & 255); n >>= 8; } }
static void Blob(std::string& out, const std::string& value) { Number(out, value.size()); out += value; }
std::string Encode(const Meta& m) {
  std::string out = "BTM2";
  for (uint64_t n : {uint64_t(m.mode), uint64_t(m.uid), uint64_t(m.gid), uint64_t(m.atime), uint64_t(m.atime_ns), uint64_t(m.mtime), uint64_t(m.mtime_ns)}) Number(out, n);
  Blob(out, m.link); Number(out, m.xattrs.size());
  for (const auto& [key, value] : m.xattrs) { Blob(out, key); Blob(out, value); }
  Need(out.size() <= 1024 * 1024, "metadata total limit"); return out;
}
Meta Decode(const std::string& bytes) {
  Need(bytes.size() <= 1024 * 1024 && bytes.compare(0, 4, "BTM2") == 0, "metadata magic/size");
  size_t at = 4;
  const auto num = [&]() { Need(at + 8 <= bytes.size(), "truncated metadata"); uint64_t n = 0; for (int j = 0; j < 8; ++j) n |= uint64_t(static_cast<unsigned char>(bytes[at++])) << (8 * j); return n; };
  const auto blob = [&]() { uint64_t n = num(); Need(n <= bytes.size() - at, "metadata length overflow"); auto out = bytes.substr(at, n); at += n; return out; };
  Meta m; auto narrow = [&]() { auto n = num(); Need(n <= UINT32_MAX, "metadata integer overflow"); return static_cast<uint32_t>(n); };
  m.mode = narrow(); m.uid = narrow(); m.gid = narrow(); m.atime = num(); m.atime_ns = narrow(); m.mtime = num(); m.mtime_ns = narrow();
  m.link = blob(); const auto count = num(); Need(count <= 1024 && m.atime_ns < 1000000000 && m.mtime_ns < 1000000000, "metadata bounds");
  for (uint64_t i = 0; i < count; ++i) {
    auto key = blob(); auto value = blob();
    Need(!key.empty() && key.size() <= 255 && key.find('\0') == std::string::npos && key != "security.selinux" && value.size() <= 65536, "unsafe xattr metadata");
    Need(m.xattrs.emplace(std::move(key), std::move(value)).second, "duplicate xattr");
  }
  Need(at == bytes.size(), "trailing metadata");
  Need(S_ISREG(m.mode) || S_ISLNK(m.mode) || S_ISDIR(m.mode), "metadata special inode type");
  Need(!S_ISLNK(m.mode) || (!m.link.empty() && m.link.size() < 4096 && m.link.find('\0') == std::string::npos), "invalid symlink metadata");
  return m;
}
std::string Filesystem(const std::string& partition) {
  auto file = ReadFile(Local("/postinstall"), "partitions");
  std::istringstream lines(ReadBounded(file.n, 4096)); std::string name, type;
  std::set<std::string> seen; std::string found;
  while (lines >> name >> type) {
    Need(seen.insert(name).second && (type == "ext4" || type == "erofs"), "invalid partition manifest");
    if (name == partition) found = type;
  }
  Need(!found.empty(), "partition not supplied by updater"); return found;
}
static void CopyArtifact(const std::string& root, const std::string& relative,
                         const Meta& meta, int input) {
  std::string leaf; auto parent = Parent(root, relative, true, &leaf);
  Fd output(openat(parent.n, leaf.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW | O_CLOEXEC, 0600));
  Need(output.n >= 0, "create cache/stage artifact");
  if (S_ISLNK(meta.mode)) WriteAll(output.n, meta.link.data(), meta.link.size());
  else { Need(S_ISREG(meta.mode), "backup API accepts files and symlinks"); CopyData(input, output.n); }
  Need(fsync(output.n) == 0, "fsync artifact");
}
void Backup(const std::string& source) {
  const Path path = ParsePath(source);
  (void)Filesystem(path.partition);
  std::string leaf; Fd parent;
  try { parent = Parent(Local("/" + path.partition), path.relative, false, &leaf); }
  catch (const std::runtime_error&) { if (errno == ENOENT) return; throw; }
  struct stat st {};
  if (fstatat(parent.n, leaf.c_str(), &st, AT_SYMLINK_NOFOLLOW) < 0 && errno == ENOENT) return;
  auto meta = ReadMeta(parent.n, leaf); Fd input;
  if (S_ISREG(meta.mode)) {
    input = Fd(openat(parent.n, leaf.c_str(), O_RDONLY | O_NOFOLLOW | O_CLOEXEC));
    Need(input.n >= 0, "open backup source");
  }
  CopyArtifact(Work("backupdir"), path.partition + "/" + path.relative, meta, input.n);
  WriteFile(Work("backupmeta"), path.partition + "/" + path.relative, Encode(meta));
  // Historical V3 scripts may test $C/$S/product/... while the native path is
  // canonical /product/.... Keep a regular-file hardlink alias in tmpfs;
  // never introduce a parent symlink into native path traversal.
  if (path.partition == "vendor" || path.partition == "product" || path.partition == "system_ext") {
    std::string from, to;
    auto original = Parent(Work("backupdir"), path.partition + "/" + path.relative, false, &from);
    auto alias = Parent(Work("backupdir"), "system/" + path.partition + "/" + path.relative, true, &to);
    if (unlinkat(alias.n, to.c_str(), 0) < 0) Need(errno == ENOENT, "remove old V3 alias");
    Need(linkat(original.n, from.c_str(), alias.n, to.c_str(), 0) == 0, "preserve V3 cache alias");
  }
}
static void StageBackup(const Path& path) {
  const auto relative = path.partition + "/" + path.relative;
  auto metadata = ReadFile(Work("backupmeta"), relative);
  auto meta = Decode(ReadBounded(metadata.n, 1024 * 1024));
  auto data = ReadFile(Work("backupdir"), relative);
  CopyArtifact(Work("restore"), relative, meta, data.n);
  WriteFile(Work("restoremeta"), relative, Encode(meta));
}
void Restore(const std::string& source, const std::string& obsolete) {
  const auto path = ParsePath(source);
  Need(Filesystem(path.partition) == "ext4", "restoring onto a read-only EROFS target is unsupported");
  // Absence is success, matching the historical backup_file/restore_file API.
  std::string leaf; Fd parent;
  try { parent = Parent(Work("backupdir"), path.partition + "/" + path.relative, false, &leaf); }
  catch (const std::runtime_error&) { if (errno == ENOENT) return; throw; }
  struct stat st {};
  if (fstatat(parent.n, leaf.c_str(), &st, AT_SYMLINK_NOFOLLOW) < 0 && errno == ENOENT) return;
  StageBackup(path);
  if (!obsolete.empty()) {
    auto old = ParsePath(obsolete); Need(old.logical != path.logical, "replacement cannot delete itself");
    Need(Filesystem(old.partition) == "ext4", "obsolete file on read-only partition");
    auto dir = OpenDirectory(Work(""));
    Fd out(openat(dir.n, "deletions", O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC | O_NOFOLLOW, 0600));
    Need(out.n >= 0, "append obsolete-file manifest");
    const auto line = old.logical + "\n"; WriteAll(out.n, line.data(), line.size()); Need(fsync(out.n) == 0, "fsync deletions");
  }
}
void Output(const std::string& source) {
  const auto path = ParsePath(source);
  Need(Filesystem(path.partition) == "ext4", "output path on read-only EROFS target");
  const auto relative = path.partition + "/" + path.relative;
  std::string leaf; auto stage_parent = Parent(Work("restore"), relative, true, &leaf);
  struct stat st {};
  if (fstatat(stage_parent.n, leaf.c_str(), &st, AT_SYMLINK_NOFOLLOW) == 0) {
    Need(S_ISREG(st.st_mode), "staged output is not regular");
  } else {
    Need(errno == ENOENT, "stat staged output");
    Fd target_parent;
    try { target_parent = Parent(Local("/postinstall/" + path.partition), path.relative, false, &leaf); }
    catch (const std::runtime_error&) { if (errno != ENOENT) throw; }
    if (target_parent.n >= 0 && fstatat(target_parent.n, leaf.c_str(), &st, AT_SYMLINK_NOFOLLOW) == 0) {
      auto meta = ReadMeta(target_parent.n, leaf);
      Need(S_ISREG(meta.mode), "editing output path requires a regular target file");
      Fd input(openat(target_parent.n, leaf.c_str(), O_RDONLY | O_NOFOLLOW | O_CLOEXEC));
      Need(input.n >= 0, "open read-only target template");
      CopyArtifact(Work("restore"), relative, meta, input.n);
      WriteFile(Work("restoremeta"), relative, Encode(meta));
    } else Need(target_parent.n < 0 || errno == ENOENT, "stat target template");
  }
  std::cout << Work("restore/" + relative) << '\n';
}
static void ScanDirectory(const std::string& partition, int directory, const std::string& prefix,
                          std::vector<Entry>* result, uint64_t* bytes) {
  int duplicate = dup(directory); Need(duplicate >= 0, "dup staged directory");
  struct CloseDirectory { void operator()(DIR* p) const { if (p) closedir(p); } };
  using Dir = std::unique_ptr<DIR, CloseDirectory>;
  Dir stream(fdopendir(duplicate)); Need(bool(stream), "fdopendir");
  for (;;) {
    errno = 0; auto* row = readdir(stream.get());
    if (!row) { Need(errno == 0, "readdir staged directory"); break; }
    std::string name(row->d_name); if (name == "." || name == "..") continue;
    auto path = ParsePath("/" + partition + "/" + prefix + name);
    Need(result->size() < 10000, "staged entry count limit");
    struct stat st {}; Need(fstatat(directory, name.c_str(), &st, AT_SYMLINK_NOFOLLOW) == 0, "stat staged node");
    Entry entry; entry.relative = path.relative; entry.logical = path.logical;
    entry.meta = ReadMeta(directory, name);
    if (S_ISDIR(st.st_mode)) {
      result->push_back(std::move(entry));
      Fd next(openat(directory, name.c_str(), O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC));
      Need(next.n >= 0, "open staged directory");
      ScanDirectory(partition, next.n, prefix + name + "/", result, bytes);
    } else {
      Need(S_ISREG(st.st_mode) || S_ISLNK(st.st_mode), "special staged file rejected");
      if (S_ISREG(st.st_mode)) {
        Need(st.st_size >= 0 && uint64_t(st.st_size) <= kMaxFile, "staged file size bound");
        entry.data = Fd(openat(directory, name.c_str(), O_RDONLY | O_NOFOLLOW | O_CLOEXEC));
        Need(entry.data.n >= 0, "open staged data"); entry.bytes = st.st_size;
      }
      // A native restore carries original ownership, times and non-SELinux
      // xattrs in a bounded sidecar. Generic script edits use their own stat.
      std::string meta_leaf;
      Fd metadata;
      try { metadata = ReadFile(Work("restoremeta"), partition + "/" + entry.relative); }
      catch (const std::runtime_error&) { if (errno != ENOENT) throw; }
      if (metadata.n >= 0) {
        entry.meta = Decode(ReadBounded(metadata.n, 1024 * 1024));
        Need(S_ISREG(entry.meta.mode) || S_ISLNK(entry.meta.mode), "file sidecar type");
      }
      *bytes += entry.bytes; Need(*bytes <= 512ULL * 1024 * 1024, "total staged bytes bound");
      result->push_back(std::move(entry));
    }
  }
}
std::vector<Entry> Scan(const std::string& partition) {
  std::vector<Entry> entries; uint64_t bytes = 0;
  const std::string root = Work("restore/" + partition);
  struct stat st {}; if (lstat(root.c_str(), &st) < 0 && errno == ENOENT) return entries;
  auto dir = OpenDirectory(root); ScanDirectory(partition, dir.n, "", &entries, &bytes);
  std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.relative < b.relative; });
  return entries;
}
std::vector<std::string> Deletions(const std::string& partition) {
  const auto path = Work("deletions"); struct stat st {};
  if (lstat(path.c_str(), &st) < 0 && errno == ENOENT) return {};
  auto file = ReadFile(Work(""), "deletions");
  std::istringstream lines(ReadBounded(file.n, 65536)); std::string line; std::set<std::string> result;
  while (std::getline(lines, line)) {
    const auto p = ParsePath(line); if (p.partition == partition) result.insert(p.relative);
    Need(result.size() <= 10000, "deletion count limit");
  }
  return {result.begin(), result.end()};
}
}  // namespace backup_store
