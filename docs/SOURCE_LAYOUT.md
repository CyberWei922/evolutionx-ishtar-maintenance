# Source layout

## Repository tree

```text
.
├── device/xiaomi/ishtar/
├── device/xiaomi/sm8550-common/
├── hardware/xiaomi/
├── kernel/
│   ├── xiaomi/sm8550-devicetrees/
│   └── patches/
│       ├── linux-5.15.224-ishtar.patch
│       └── sm8550-modules.patch
├── manifests/
│   ├── platform-pinned.xml
│   └── ishtar-system-lock.xml
├── patches/platform/
├── vendor/xiaomi/
├── provenance/
├── releases/
├── scripts/
├── docs/
└── licenses/
```

## Maintained complete trees

These are imported as ordinary files in this repository, not Git submodules:

| Path | Content |
| --- | --- |
| `device/xiaomi/ishtar` | Product definition, board config, overlays, audio, camera policy, init, Recovery configuration and sepolicy |
| `device/xiaomi/sm8550-common` | SM8550 common device tree, VINTF, audio, power, sensors, UDFPS, IWLAN patch tooling and sepolicy |
| `hardware/xiaomi` | Xiaomi fingerprint/HAL support source |
| `kernel/xiaomi/sm8550-devicetrees` | Full ishtar DTS/DTBO source tree |
| `vendor/xiaomi` | Soong/Make recipes and payload metadata only |

The imported trees have no nested `.git` directories and no proprietary
payload directory in the archive. Their upstream history is not rewritten or
impersonated.

## Platform source

Platform projects such as `frameworks/base`, `frameworks/native`,
`system/update_engine`, `system/netd` and `vendor/lineage` are not copied in
full. They are reconstructed from:

1. the full-SHA revisions in `manifests/platform-pinned.xml`;
2. the final cumulative delta in `patches/platform/`.

This keeps the GitHub archive small while preserving the exact maintenance
changes.

## Kernel and module source

The full Linux and SM8550 module trees are deliberately absent. They are much
larger and are represented by reviewed patches. The DTS tree is included
because ishtar carries substantial board-level device-tree source.

The kernel patch is not a sequence of 2,661 separate files. It is one
reviewable cumulative delta whose source result is identified by a merge
commit and tree hash.

## Vendor recipes versus payloads

Files in this archive under `vendor/xiaomi/` are recipes such as `Android.bp`,
`Android.mk`, `camera.mk` and payload manifests. Paths such as:

```text
vendor/xiaomi/ishtar/proprietary/
vendor/xiaomi/ishtar-camera/proprietary/
```

are intentionally absent. Build references to them remain because removing
those references would misrepresent the real product configuration.

## Provenance

`provenance/` stores canonical machine-readable records from the Build 305
workflow. Some files contain absolute build-host paths because they are kept
byte-for-byte for audit; those paths are not live repository-local paths.

`provenance/release-patches/` records individual issue-level changes. It is
historical evidence, not a superseding patch series. The final tree is defined
by `patches/platform/`, the manifests and the complete imported source trees.

## Size policy

The archive should remain on the order of tens of MiB. Do not commit:

- complete kernel/module trees;
- vendor proprietary directories;
- images, ROM ZIPs or target-files;
- APK/SO/KO payloads;
- caches, `out/` or `.repo/`.

See the enforced limits in `scripts/verify-archive.sh`.
