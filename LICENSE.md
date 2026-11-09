# Component licenses

This repository combines independently licensed upstream source, patches and
maintenance material. There is no single replacement license covering every
file.

- Device configuration and Xiaomi hardware source retain their per-file
  copyright and SPDX declarations, commonly Apache-2.0.
- The Android platform patches derive their rights and obligations from their
  target AOSP, Evolution X, LineageOS and Qualcomm projects.
- The Linux kernel patch derives from the Linux kernel and Android Common
  Kernel. Kernel source is GPL-2.0-only except where a file states otherwise.
- DTS/DTBO source retains its original Linux and Qualcomm licensing.
- External kernel modules retain their per-file notices.
- Selected project-level notices are copied under `licenses/upstream/`.
- Original archival documentation and helper scripts added here are provided
  under Apache-2.0, without relicensing any imported source, patch or external
  dependency.

Full general texts:

- [Apache License 2.0](licenses/Apache-2.0.txt)
- [GNU General Public License version 2 only](licenses/GPL-2.0-only.txt)

Project-specific notices are listed in
`licenses/upstream/<project>/NOTICE`. Source origins and revision pins are in
`provenance/` and `manifests/`.

Proprietary Xiaomi, Qualcomm, Google and other third-party payloads are not
included and are not relicensed by this repository. Their original terms
continue to apply.

This file is an inventory and attribution statement, not legal advice. It does
not grant rights beyond those supplied by each upstream license.
