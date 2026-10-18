// Copyright (C) 2026 CyberWei
// SPDX-License-Identifier: Apache-2.0
#include "broker.h"
#include <array>
namespace backuptool_broker {
int Run(Operations& op) {
  if (!op.Validate()) return 20;
  if (!op.IsolateMounts()) return 21;
  if (!op.MountWorkspace()) return 22;
  constexpr std::array<const char*, 10> views = {
      "/postinstall/odm", "/postinstall/inactive-odm",
      "/postinstall/system_ext", "/postinstall/inactive-system_ext",
      "/postinstall/product", "/postinstall/inactive-product",
      "/postinstall/vendor", "/postinstall/inactive-vendor",
      "/postinstall/system", "/postinstall/inactive-root"};
  int result = 0;
  const auto unmount_views = [&]() {
    bool success = true;
    for (const char* path : views) if (!op.UnmountIfMounted(path)) success = false;
    return success;
  };
  const auto finish = [&]() {
    if (!unmount_views() && !result) result = 70;
    if (!op.UnmountIfMounted("/postinstall") && !result) result = 70;
    return result;
  };
  for (const char* path : {"/postinstall/.broker", "/postinstall/.broker/contexts",
       "/postinstall/inactive-root", "/postinstall/system", "/postinstall/tmp",
       "/postinstall/vendor", "/postinstall/inactive-vendor",
       "/postinstall/product", "/postinstall/inactive-product",
       "/postinstall/system_ext", "/postinstall/inactive-system_ext",
       "/postinstall/odm", "/postinstall/inactive-odm"}) {
    if (!op.MakeDirectory(path)) { result = 23; return finish(); }
  }
  if (!op.MountTarget("/postinstall/inactive-root")) { result = 24; return finish(); }
  if (!op.BindSystem("/postinstall/inactive-root", "/postinstall/system")) { result = 25; return finish(); }
  if (!op.MountExtras()) { result = 30; return finish(); }
  if (!op.SnapshotContexts()) { result = 31; return finish(); }
  result = op.RunWorker();
  if (result) return finish();
  // Offline ext4 writes cannot coexist with a kernel mount of the target,
  // even a read-only one. Never commit after any failed unmount.
  if (!unmount_views()) { result = 32; return finish(); }
  result = op.Commit();
  return finish();
}
}
