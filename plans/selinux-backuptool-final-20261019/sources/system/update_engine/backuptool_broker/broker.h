// Copyright (C) 2026 CyberWei
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <string>
namespace backuptool_broker {
// All filesystem writes are staged; only the native offline committer receives
// block-device access. Namespace isolation alone is not a complete sandbox.
class Operations {
 public:
  virtual ~Operations() = default;
  virtual bool Validate() = 0;
  virtual bool IsolateMounts() = 0;
  virtual bool MountWorkspace() = 0;
  virtual bool MakeDirectory(const std::string& path) = 0;
  virtual bool MountTarget(const std::string& path) = 0;
  virtual bool BindSystem(const std::string& root, const std::string& view) = 0;
  virtual bool MountExtras() = 0;
  virtual bool SnapshotContexts() = 0;
  virtual int RunWorker() = 0;
  virtual bool UnmountIfMounted(const std::string& path) = 0;
  virtual int Commit() = 0;
};
int Run(Operations& operations);
}
