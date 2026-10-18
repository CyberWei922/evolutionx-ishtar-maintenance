// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2026 CyberWei
#pragma once
#include <sys/stat.h>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
namespace backup_store {
struct Fd {
  int n = -1;
  Fd() = default;
  explicit Fd(int value) : n(value) {}
  ~Fd();
  Fd(Fd&& other) noexcept;
  Fd& operator=(Fd&& other) noexcept;
  Fd(const Fd&) = delete;
  Fd& operator=(const Fd&) = delete;
};
struct Path { std::string partition, relative, logical; };
struct Meta {
  uint32_t mode = 0, uid = 0, gid = 0;
  int64_t atime = 0, mtime = 0;
  uint32_t atime_ns = 0, mtime_ns = 0;
  std::string link;
  std::map<std::string, std::string> xattrs;
};
struct Entry {
  std::string relative, logical;
  Meta meta;
  Fd data;
  uint64_t bytes = 0;
};
extern std::string host_root;
std::string Local(const std::string& absolute);
std::string Work(const std::string& suffix);
[[noreturn]] void Fail(const std::string& message);
void Need(bool success, const std::string& message);
Path ParsePath(const std::string& path);
void RequireDomain(const char* wanted);
Fd OpenDirectory(const std::string& path, bool create = false);
Fd Parent(const std::string& root, const std::string& relative, bool create, std::string* leaf);
Fd ReadFile(const std::string& root, const std::string& relative);
std::string ReadBounded(int fd, size_t limit);
void WriteAll(int fd, const void* bytes, size_t size);
void WriteFile(const std::string& root, const std::string& relative, const std::string& data);
void CopyData(int input, int output);
Meta ReadMeta(int parent, const std::string& leaf);
std::string Encode(const Meta& meta);
Meta Decode(const std::string& bytes);
void Backup(const std::string& path);
void Restore(const std::string& path, const std::string& obsolete);
void Output(const std::string& path);
std::string Filesystem(const std::string& partition);
std::vector<Entry> Scan(const std::string& partition);
std::vector<std::string> Deletions(const std::string& partition);
int Offline(const std::string& partition, const std::string& target,
            const std::string& source, bool apply);
}  // namespace backup_store
