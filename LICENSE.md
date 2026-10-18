# License scope

This is a mixed-license maintenance source collection, not a license for an entire ROM.

- Original maintenance scripts, documentation and newly authored integration/policy code are provided under Apache-2.0, copyright 2026 CyberWei922.
- Existing upstream source and changes to it retain their original file/directory licenses and notices. See `licenses/component-map.json`, adjacent `.license` notices and `NOTICE`.
- Kernel and camera driver changes are GPL-2.0-only as indicated by their original source/directory licenses. KernelSU's kernel directory is GPLv2; its root LICENSE is GPLv3 and does not replace the kernel directory's license.
- Recovery init rc and the two retained vendor color XML files carry their complete original BSD-style grants in the files. Those texts, including their patent disclaimers, remain authoritative.
- Five optional Qualcomm test-domain policy snapshots in the boot-contracts plan retain their full BSD-3-Clause Linux Foundation notices; see `provenance/boot-contracts-source-files.json`.
- The debug/OTA plan retains Linux Foundation and Qualcomm Innovation Center BSD notices, Apache-2.0 AOSP/LineageOS licensing and the complete AOSP sepolicy public-domain NOTICE; see `provenance/debug-ota-source-files.json`.
- Paths, checksums and dependency metadata do not grant permission to copy or redistribute external firmware, proprietary applications, vendor libraries or Google software.
- No single repository-wide license overrides upstream terms. ROM distribution and corresponding-source obligations require a separate release review.

Full license texts and upstream notices are under `licenses/`. This repository does not assert ownership of upstream code or grant rights to LineageOS, Evolution X, Android, Xiaomi, Google or Qualcomm trademarks.

- The native Backuptool broker plan retains AOSP Apache-2.0 notices, vendor/lineage licensing and device/lineage/sepolicy REUSE Apache-2.0 annotations; original additions are Copyright 2026 CyberWei. See `provenance/broker-source-files.json`.

## Backuptool Store

`plans/selinux-backuptool-final-20261019/sources/system/update_engine/backuptool_store/` 为GPL-2.0-only（完整COPYING在目录内），其余本批broker/worker/脚本及集成改动保持Apache-2.0。原始版权/NOTICE及LineageOS REUSE声明保留。具体归属和依赖HEAD见provenance/backuptool-final-source-files.json；不据此声明整个ROM的再分发许可。
