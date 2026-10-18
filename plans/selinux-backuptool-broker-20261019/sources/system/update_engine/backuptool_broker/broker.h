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

#pragma once
#include <string>

namespace backuptool_broker {
// The worker still retains its existing permissive policy. A private mount
// namespace prevents mount propagation; it is not a worker security sandbox.
class Operations {
 public:
  virtual ~Operations() = default;
  virtual bool Validate() = 0;
  virtual bool IsolateMounts() = 0;
  virtual bool MountWorkspace() = 0;
  virtual bool MakeDirectory(const std::string& path) = 0;
  virtual bool MountTarget(const std::string& path) = 0;
  virtual bool BindSystem(const std::string& root, const std::string& view) = 0;
  virtual int RunWorker() = 0;
  virtual bool UnmountIfMounted(const std::string& path) = 0;
};
// First failure is preserved; every owned cleanup is attempted.
int Run(Operations& operations);
}  // namespace backuptool_broker
