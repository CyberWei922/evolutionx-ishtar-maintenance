// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2026 CyberWei
#include "store.h"
extern "C" {
#include <ext2fs/ext2fs.h>
}
#ifdef clamp
#undef clamp
#endif
#include <fcntl.h>
#include <selinux/label.h>
#include <selinux/selinux.h>
#include <sys/statfs.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <cerrno>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
namespace backup_store {
namespace {
void Ext(errcode_t rc, const char* operation) {
  if (rc) throw std::runtime_error(std::string(operation) + " ext2fs error=" + std::to_string(rc));
}
struct Image {
  ext2_filsys fs = nullptr;
  ~Image() { if (fs) ext2fs_close_free(&fs); }
};
class Labels {
 public:
  Labels() {
    static const char* files[] = {"plat_file_contexts", "system_ext_file_contexts", "product_file_contexts", "vendor_file_contexts", "odm_file_contexts"};
    for (auto* name : files) {
      const auto directory = Local("/postinstall/.broker/contexts");
      std::string leaf; auto parent = Parent(directory, name, false, &leaf);
      struct stat info {};
      if (fstatat(parent.n, leaf.c_str(), &info, AT_SYMLINK_NOFOLLOW) < 0 && errno == ENOENT) {
        Need(std::string(name) != "plat_file_contexts", "new platform file contexts missing"); continue;
      }
      #ifdef __ANDROID__
      constexpr uid_t expected_owner = 0;
#else
      const uid_t expected_owner = getuid();
#endif
      Need(S_ISREG(info.st_mode) && info.st_uid == expected_owner && !(info.st_mode & 0222), "contexts must be root-owned read-only regular files");
      auto input = ReadFile(directory, name); Need(info.st_size > 0 && info.st_size <= 2 * 1024 * 1024, "contexts size limit");
      // A held fd and the protected broker directory prevent replacement.
      paths_.push_back("/proc/self/fd/" + std::to_string(input.n)); inputs_.push_back(std::move(input));
    }
    for (const auto& path : paths_) options_.push_back({SELABEL_OPT_PATH, path.c_str()});
    handle_ = selabel_open(SELABEL_CTX_FILE, options_.data(), options_.size());
    Need(handle_ != nullptr, "open NEW target file contexts");
  }
  ~Labels() { if (handle_) selabel_close(handle_); }
  std::string Lookup(const std::string& path, mode_t mode) {
    char* context = nullptr;
    Need(selabel_lookup(handle_, &context, path.c_str(), mode) == 0, "lookup target label " + path);
    std::string value(context); freecon(context);
    Need(value.compare(0, 11, "u:object_r:") == 0 && value.find(':', 11) != std::string::npos && value.size() < 512, "invalid target label");
    return value;
  }
 private:
  std::vector<Fd> inputs_;
  std::vector<std::string> paths_;
  std::vector<selinux_opt> options_;
  selabel_handle* handle_ = nullptr;
};
[[maybe_unused]] void NotMounted(dev_t device) {
  std::ifstream input("/proc/self/mountinfo"); Need(bool(input), "open mountinfo"); std::string line;
  while (std::getline(input, line)) {
    std::istringstream parts(line); std::string id, parent, number;
    Need(bool(parts >> id >> parent >> number), "mountinfo parse");
    Need(number != std::to_string(major(device)) + ":" + std::to_string(minor(device)), "target is still mounted");
  }
  Need(!input.bad(), "read mountinfo");
}
Fd Device(const std::string& path, struct stat* info) {
  Fd fd(open(path.c_str(), O_PATH | O_CLOEXEC)); Need(fd.n >= 0 && fstat(fd.n, info) == 0, "open device identity");
#ifdef __ANDROID__
  Need(S_ISBLK(info->st_mode), "real block device required");
#else
  Need(S_ISREG(info->st_mode), "host image must be regular");
#endif
  return fd;
}
std::string Identity(const struct stat& info) {
#ifdef __ANDROID__
  return std::to_string(major(info.st_rdev)) + ":" + std::to_string(minor(info.st_rdev));
#else
  return std::to_string(info.st_dev) + ":" + std::to_string(info.st_ino);
#endif
}
void Inventory(const std::string& partition, const struct stat& target, const struct stat& source) {
  auto input = ReadFile(Local("/postinstall/.broker"), "devices"); struct stat state {};
  Need(fstat(input.n, &state) == 0, "stat protected inventory");
#ifdef __ANDROID__
  constexpr uid_t owner = 0;
  struct statfs mount {}; Need(fstatfs(input.n, &mount) == 0 && mount.f_type == 0x01021994, "inventory must be broker tmpfs");
  char* label = nullptr; Need(fgetfilecon(input.n, &label) >= 0, "inventory context");
  const bool protected_label = label && std::string(label) == "u:object_r:backuptool_broker_root_file:s0"; freecon(label);
  Need(protected_label, "inventory is not broker-owned state");
#else
  const uid_t owner = getuid();
#endif
  Need(state.st_uid == owner && !(state.st_mode & 0222), "mutable/untrusted target inventory");
  std::istringstream lines(ReadBounded(input.n, 4096)); std::string line; bool found = false; std::set<std::string> seen;
  while (std::getline(lines, line)) {
    std::istringstream fields(line); std::string name, to, from, extra;
    Need(bool(fields >> name >> to >> from) && !(fields >> extra) && seen.insert(name).second, "malformed target inventory");
    if (name == partition) { Need(to == Identity(target) && from == Identity(source), "offline arguments differ from broker inventory"); found = true; }
  }
  Need(found, "offline partition absent from broker inventory");
}
ext2_ino_t Lookup(ext2_filsys fs, ext2_ino_t parent, const std::string& name, bool* present) {
  ext2_ino_t inode = 0; auto rc = ext2fs_lookup(fs, parent, name.c_str(), name.size(), nullptr, &inode);
  if (rc == EXT2_ET_FILE_NOT_FOUND) { *present = false; return 0; }
  Ext(rc, "lookup inode"); *present = true; return inode;
}
ext2_ino_t Directory(ext2_filsys fs, ext2_ino_t parent, const std::string& name, bool* present) {
  auto inode = Lookup(fs, parent, name, present); if (!*present) return 0;
  struct ext2_inode info {}; Ext(ext2fs_read_inode(fs, inode, &info), "read directory inode");
  Need(LINUX_S_ISDIR(info.i_mode), "target path has a symlink/non-directory parent"); return inode;
}
void Attributes(ext2_filsys fs, ext2_ino_t inode, const Meta& meta, const std::string& label) {
  ext2_xattr_handle* attributes = nullptr; Ext(ext2fs_xattrs_open(fs, inode, &attributes), "open inode xattrs");
  try {
    Ext(ext2fs_xattrs_read(attributes), "read inode xattrs");
    for (const auto& [name, value] : meta.xattrs)
      Ext(ext2fs_xattr_set(attributes, name.c_str(), value.data(), value.size()), "restore non-SELinux xattr");
    Ext(ext2fs_xattr_set(attributes, "security.selinux", label.c_str(), label.size() + 1), "set persistent target security label");
    Ext(ext2fs_xattrs_close(&attributes), "close inode xattrs");
  } catch (...) { if (attributes) ext2fs_xattrs_close(&attributes); throw; }
}
void Metadata(ext2_filsys fs, ext2_ino_t inode, const Meta& meta, const std::string& label) {
  struct ext2_inode_large info {};
  Ext(ext2fs_read_inode_full(fs, inode, reinterpret_cast<ext2_inode*>(&info), sizeof(info)), "read metadata inode");
  info.i_mode = meta.mode; info.i_uid = meta.uid & 65535; info.i_gid = meta.gid & 65535;
  ext2fs_set_i_uid_high(info, meta.uid >> 16); ext2fs_set_i_gid_high(info, meta.gid >> 16);
  info.i_atime = meta.atime; info.i_mtime = meta.mtime; info.i_ctime = std::time(nullptr);
  // Existing images use 256-byte inodes. Preserve nanoseconds when their
  // extra inode fields are present; do not alter unrelated inode/xattr bytes.
  if (EXT2_INODE_SIZE(fs->super) >= sizeof(info) && info.i_extra_isize >= 16) {
    info.i_atime_extra = meta.atime_ns << 2;
    info.i_mtime_extra = meta.mtime_ns << 2;
    info.i_ctime_extra = 0;
  }
  Ext(ext2fs_write_inode_full(fs, inode, reinterpret_cast<ext2_inode*>(&info), sizeof(info)), "restore inode metadata");
  Attributes(fs, inode, meta, label);
}
struct Root { ext2_ino_t inode; std::string prefix; };
Root ImageRoot(ext2_filsys fs, const std::string& partition) {
  bool present = false; auto candidate = Directory(fs, EXT2_ROOT_INO, partition, &present);
  if (present) return {candidate, "/" + partition};
  // Non-SAR system and split partition images expose their content at root.
  return {EXT2_ROOT_INO, "/" + partition};
}
ext2_ino_t WalkParent(ext2_filsys fs, Root root, const std::string& relative,
                       Labels& labels, bool create, std::string* leaf) {
  std::istringstream parts(relative); std::vector<std::string> names; std::string part;
  while (std::getline(parts, part, '/')) names.push_back(part);
  Need(!names.empty(), "empty target path"); *leaf = names.back(); names.pop_back();
  auto inode = root.inode; std::string logical = root.prefix; bool missing = false;
  for (const auto& name : names) {
    logical += "/" + name; bool present = false;
    auto next = missing ? 0 : Directory(fs, inode, name, &present);
    if (!present) {
      (void)labels.Lookup(logical, S_IFDIR | 0755);
      if (!create) { missing = true; continue; }
      Ext(ext2fs_mkdir(fs, inode, 0, name.c_str()), "create target parent");
      next = Directory(fs, inode, name, &present); Need(present, "created directory absent");
      Meta meta; meta.mode = S_IFDIR | 0755; meta.atime = meta.mtime = std::time(nullptr);
      Metadata(fs, next, meta, labels.Lookup(logical, meta.mode));
    }
    inode = next;
  }
  return missing ? 0 : inode;
}
void Remove(ext2_filsys fs, ext2_ino_t parent, const std::string& name) {
  bool present = false; auto inode = Lookup(fs, parent, name, &present); if (!present) return;
  struct ext2_inode info {}; Ext(ext2fs_read_inode(fs, inode, &info), "read obsolete inode");
  Need(LINUX_S_ISREG(info.i_mode) || LINUX_S_ISLNK(info.i_mode), "only obsolete file/symlink may be removed");
  Need(info.i_links_count > 0, "invalid target link count");
  Ext(ext2fs_unlink(fs, parent, name.c_str(), inode, 0), "unlink obsolete target");
  if (--info.i_links_count) { Ext(ext2fs_write_inode(fs, inode, &info), "decrement hardlink count"); return; }
  if (ext2fs_inode_has_valid_blocks2(fs, &info))
    Ext(ext2fs_punch(fs, inode, &info, nullptr, 0, ~blk64_t(0)), "free obsolete blocks");
  // libext2fs owns the large-inode/xattr layout; do not pass a short inode.
  Ext(ext2fs_write_inode(fs, inode, &info), "persist released blocks");
  Ext(ext2fs_free_ext_attr(fs, inode, nullptr), "free obsolete xattrs");
  Ext(ext2fs_read_inode(fs, inode, &info), "reread released inode");
  info.i_dtime = std::time(nullptr); Ext(ext2fs_write_inode(fs, inode, &info), "free obsolete inode");
  ext2fs_inode_alloc_stats2(fs, inode, -1, 0);
}
void Put(ext2_filsys fs, Root root, Entry& entry, Labels& labels) {
  std::string leaf; auto parent = WalkParent(fs, root, entry.relative, labels, true, &leaf);
  bool present = false; auto old = Lookup(fs, parent, leaf, &present);
  const auto context = labels.Lookup(entry.logical, entry.meta.mode);
  if (S_ISDIR(entry.meta.mode)) {
    if (present) { bool directory = false; (void)Directory(fs, parent, leaf, &directory); return; }
    Ext(ext2fs_mkdir(fs, parent, 0, leaf.c_str()), "create staged directory");
    old = Lookup(fs, parent, leaf, &present); Need(present, "staged directory missing");
    // Synthetic staging parents do not propagate their private 0700 mode.
    Meta meta = entry.meta; meta.mode = S_IFDIR | 0755;
    Metadata(fs, old, meta, context); return;
  }
  if (present) Remove(fs, parent, leaf);
  ext2_ino_t inode = 0;
  if (S_ISLNK(entry.meta.mode)) {
    Ext(ext2fs_symlink(fs, parent, 0, leaf.c_str(), entry.meta.link.c_str()), "restore symlink");
    inode = Lookup(fs, parent, leaf, &present); Need(present, "restored symlink absent");
  } else {
    Ext(ext2fs_new_inode(fs, parent, entry.meta.mode, fs->inode_map, &inode), "allocate restored inode");
    struct ext2_inode info {}; info.i_mode = entry.meta.mode; info.i_links_count = 1;
    Ext(ext2fs_write_new_inode(fs, inode, &info), "initialize restored inode");
    ext2fs_inode_alloc_stats2(fs, inode, 1, 0);
    auto rc = ext2fs_link(fs, parent, leaf.c_str(), inode, EXT2_FT_REG_FILE);
    if (rc == EXT2_ET_DIR_NO_SPACE) { Ext(ext2fs_expand_dir(fs, parent), "expand target directory"); rc = ext2fs_link(fs, parent, leaf.c_str(), inode, EXT2_FT_REG_FILE); }
    Ext(rc, "link restored inode");
    ext2_file_t file = nullptr; Ext(ext2fs_file_open(fs, inode, EXT2_FILE_WRITE, &file), "open restored data");
    try {
      Need(lseek(entry.data.n, 0, SEEK_SET) == 0, "rewind staged data"); std::array<char, 65536> buffer{}; uint64_t total = 0;
      for (;;) {
        ssize_t count = read(entry.data.n, buffer.data(), buffer.size()); if (count < 0 && errno == EINTR) continue;
        Need(count >= 0, "read restored data"); if (!count) break;
        unsigned int written = 0; Ext(ext2fs_file_write(file, buffer.data(), count, &written), "write restored data");
        Need(written == static_cast<unsigned int>(count), "short ext4 write"); total += count;
      }
      Need(total == entry.bytes, "staged data changed after validation");
      Ext(ext2fs_file_close(file), "close restored data"); file = nullptr;
    } catch (...) { if (file) ext2fs_file_close(file); throw; }
  }
  Metadata(fs, inode, entry.meta, context);
}
}  // namespace
int Offline(const std::string& partition, const std::string& target, const std::string& source, bool apply) {
  auto entries = Scan(partition); auto deletes = Deletions(partition);
  if (entries.empty() && deletes.empty()) return 0;
  Need(Filesystem(partition) == "ext4", "EROFS is immutable; refusing any staged write");
  struct stat target_info {}, source_info {};
  auto device = Device(target, &target_info); auto active = Device(source, &source_info);
#ifdef __ANDROID__
  Need(target_info.st_rdev != source_info.st_rdev, "target aliases active source device"); NotMounted(target_info.st_rdev);
#else
  Need(target_info.st_dev != source_info.st_dev || target_info.st_ino != source_info.st_ino, "target aliases source image");
#endif
  Inventory(partition, target_info, source_info);
  Image image; const auto path = "/proc/self/fd/" + std::to_string(device.n);
  Ext(ext2fs_open(path.c_str(), apply ? EXT2_FLAG_RW | EXT2_FLAG_EXCLUSIVE : 0, 0, 0, unix_io_manager, &image.fs), "open unmounted ext4");
  Need((image.fs->super->s_state & EXT2_VALID_FS) != 0, "ext4 must be clean");
  Need(!ext2fs_has_feature_journal_needs_recovery(image.fs->super) && !ext2fs_has_feature_shared_blocks(image.fs->super), "recovery/shared-block filesystem is not writable safely");
  Ext(ext2fs_read_bitmaps(image.fs), "read allocation bitmaps"); Labels labels; const auto root = ImageRoot(image.fs, partition);
  uint64_t required = entries.size(), inodes = entries.size(); std::set<std::string> restored;
  // Validate the whole request before the first block write. Existing target
  // parent symlinks are never followed, including links to another partition.
  for (const auto& e : entries) {
    Need(e.meta.atime >= 0 && e.meta.mtime >= 0 && uint64_t(e.meta.atime) <= UINT32_MAX && uint64_t(e.meta.mtime) <= UINT32_MAX, "timestamp outside ext4 base fields");
    (void)labels.Lookup(e.logical, e.meta.mode);
    std::string leaf; auto parent = WalkParent(image.fs, root, e.relative, labels, false, &leaf);
    if (parent) {
      bool present = false; auto inode = Lookup(image.fs, parent, leaf, &present);
      if (present) {
        ext2_inode info {}; Ext(ext2fs_read_inode(image.fs, inode, &info), "validate existing target");
        Need(S_ISDIR(e.meta.mode) ? LINUX_S_ISDIR(info.i_mode) : (LINUX_S_ISREG(info.i_mode) || LINUX_S_ISLNK(info.i_mode)), "target inode type conflict");
      }
    }
    required += (e.bytes + image.fs->blocksize - 1) / image.fs->blocksize; restored.insert(e.relative);
  }
  for (const auto& rel : deletes) {
    Need(!restored.count(rel), "obsolete path also requested for restore");
    std::string leaf; auto parent = WalkParent(image.fs, root, rel, labels, false, &leaf);
    if (parent) {
      bool present = false; auto inode = Lookup(image.fs, parent, leaf, &present);
      if (present) { ext2_inode info {}; Ext(ext2fs_read_inode(image.fs, inode, &info), "validate deletion"); Need(LINUX_S_ISREG(info.i_mode) || LINUX_S_ISLNK(info.i_mode), "deletion of directory/special inode refused"); }
    }
  }
  Need(required <= ext2fs_free_blocks_count(image.fs->super) && inodes <= image.fs->super->s_free_inodes_count, "insufficient spare blocks/inodes for safe restore");
  if (apply) {
    for (auto& e : entries) Put(image.fs, root, e, labels);
    for (const auto& rel : deletes) { std::string leaf; auto parent = WalkParent(image.fs, root, rel, labels, false, &leaf); if (parent) Remove(image.fs, parent, leaf); }
  }
  Ext(ext2fs_close_free(&image.fs), "flush/close ext4");
  std::cout << "backuptool_store: " << (apply ? "applied " : "validated ") << partition << " files=" << entries.size() << " deletions=" << deletes.size() << '\n';
  return 0;
}
}  // namespace backup_store
