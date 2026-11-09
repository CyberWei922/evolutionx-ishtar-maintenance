# License texts and upstream notices

This directory stores general and selected project-level license material for
the combined source archive.

```text
Apache-2.0.txt
GPL-2.0-only.txt
upstream/frameworks/base/NOTICE
upstream/frameworks/native/NOTICE
upstream/system/sepolicy/NOTICE
upstream/system/update_engine/NOTICE
upstream/vendor/lineage/LICENSE
```

The selected `NOTICE` and license files are copied from the corresponding
pinned upstream projects. They do not replace per-file SPDX identifiers or
notices retained inside the imported device, hardware and DTS trees.

Kernel and external-module source remains external to this archive. Its own
`COPYING`, `LICENSES/` and file-level notices remain authoritative when the
source trees are restored.

No license text in this directory grants rights to the proprietary Xiaomi,
Qualcomm, Google or other payloads that are intentionally absent.
