# License scope

This is a mixed-license maintenance source collection, not a license for an entire ROM.

- Original maintenance scripts, documentation and newly authored integration/policy code are provided under Apache-2.0, copyright 2026 CyberWei922.
- Existing upstream source and changes to it retain their original file/directory licenses and notices. See `licenses/component-map.json`, adjacent `.license` notices and `NOTICE`.
- Kernel and camera driver changes are GPL-2.0-only as indicated by their original source/directory licenses. KernelSU's kernel directory is GPLv2; its root LICENSE is GPLv3 and does not replace the kernel directory's license.
- Recovery init rc and the two retained vendor color XML files carry their complete original BSD-style grants in the files. Those texts, including their patent disclaimers, remain authoritative.
- Five optional Qualcomm test-domain policy snapshots in the boot-contracts plan retain their full BSD-3-Clause Linux Foundation notices; see `provenance/boot-contracts-source-files.json`.
- Paths, checksums and dependency metadata do not grant permission to copy or redistribute external firmware, proprietary applications, vendor libraries or Google software.
- No single repository-wide license overrides upstream terms. ROM distribution and corresponding-source obligations require a separate release review.

Full license texts and upstream notices are under `licenses/`. This repository does not assert ownership of upstream code or grant rights to LineageOS, Evolution X, Android, Xiaomi, Google or Qualcomm trademarks.
