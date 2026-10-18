// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2026 CyberWei
#include "store.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
int main(int argc, char** argv) {
  using namespace backup_store;
  bool stage_caller_verified = false;
  try {
#ifndef __ANDROID__
    if (argc >= 4 && std::string(argv[1]) == "--host-root") {
      host_root = argv[2]; Need(!host_root.empty() && host_root.front() == '/' && host_root.back() != '/', "absolute fixture root");
      argv += 2; argc -= 2;
    }
#endif
    Need(argc >= 3, "operation and arguments required"); const std::string action = argv[1];
    if (action == "--check" || action == "--apply") {
      RequireDomain("u:r:update_engine:s0"); Need(argc == 5, "offline operation arguments");
      auto path = ParsePath("/" + std::string(argv[2]) + "/placeholder");
      return Offline(path.partition, argv[3], argv[4], action == "--apply");
    }
    RequireDomain("u:r:backuptool:s0"); stage_caller_verified = true;
    if (action == "--backup") { Need(argc == 3, "backup arguments"); Backup(argv[2]); }
    else if (action == "--restore") { Need(argc == 3 || argc == 4, "restore arguments"); Restore(argv[2], argc == 4 ? argv[3] : ""); }
    else if (action == "--output") { Need(argc == 3, "output arguments"); Output(argv[2]); }
    else Fail("unknown operation");
    return 0;
  } catch (const std::exception& failure) {
    if (stage_caller_verified) {
      // Preserve API failures even when a third-party script ignores a return
      // code. This marker is inside the isolated tmpfs, never an active file.
      try {
        auto directory = OpenDirectory(Work(""));
        int fd = openat(directory.n, "store-failures", O_WRONLY | O_CREAT | O_APPEND | O_NOFOLLOW | O_CLOEXEC, 0600);
        if (fd >= 0) { const char message[] = "native staging operation failed\n"; const auto saved = write(fd, message, sizeof(message) - 1); (void)saved; close(fd); }
      } catch (const std::exception&) { /* Original failure remains authoritative. */ }
    }
    std::cerr << "backuptool_store: " << failure.what() << '\n'; return 1;
  }
}
