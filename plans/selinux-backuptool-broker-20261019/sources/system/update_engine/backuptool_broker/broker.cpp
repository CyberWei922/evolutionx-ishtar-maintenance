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
#include <array>

namespace backuptool_broker {
int Run(Operations& operations) {
  if (!operations.Validate()) return 20;
  if (!operations.IsolateMounts()) return 21;
  if (!operations.MountWorkspace()) return 22;

  constexpr const char* root = "/postinstall/inactive-root";
  constexpr const char* system = "/postinstall/system";
  int result = 0;
  const auto finish = [&]() {
    // V3 scripts currently own these optional mounts. Even if their cleanup
    // fails, attempt all known locations before removing the broker's views.
    constexpr std::array<const char*, 6> mounts = {
        "/postinstall/system_ext", "/postinstall/product",
        "/postinstall/vendor", system, root, "/postinstall"};
    for (const char* path : mounts) {
      if (!operations.UnmountIfMounted(path) && result == 0) result = 70;
    }
    return result;
  };
  // These directories exist only on the fresh tmpfs, never on the target image.
  for (const char* path : {root, system, "/postinstall/tmp",
                           "/postinstall/vendor", "/postinstall/product",
                           "/postinstall/system_ext"}) {
    if (!operations.MakeDirectory(path)) {
      result = 23;
      return finish();
    }
  }
  if (!operations.MountTarget(root)) {
    result = 24;
    return finish();
  }
  if (!operations.BindSystem(root, system)) {
    result = 25;
    return finish();
  }
  result = operations.RunWorker();
  return finish();
}
}  // namespace backuptool_broker
