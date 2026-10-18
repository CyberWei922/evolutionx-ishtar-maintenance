// Copyright (C) 2026 CyberWei
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <linux/filter.h>
#include <vector>
namespace backuptool_broker {
std::vector<sock_filter> WorkerFilter();
int RunStagedWorker();
#ifndef __ANDROID__
// Host-only fixture entry; absent from the Android executable.
struct WorkerFixture {
  const char* script;
  const char* workspace;
  const char* path;
  bool fail_filter = false;
  bool fail_context = false;
};
int RunStagedWorkerForTest(const WorkerFixture& fixture);
#endif
}
