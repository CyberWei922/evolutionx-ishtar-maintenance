# Evolution X 12.3 / Android 17 for Xiaomi 13 Ultra (`ishtar`)

Maintenance archive for the Xiaomi 13 Ultra (`ishtar`) Android 17 device
adaptation. This repository records the source changes, device trees, pinned
platform revisions and reviewed kernel delta used by the Evolution X 12.3
maintenance line.

**Status:** private source archive, snapshot dated 2026-11-09. Build 305 is an
installed, owner-scoped verification candidate. It has not been published and
is still signed with test keys.

## What this repository is

- The Android 17 baseline for all future work on `main`.
- A compact record of ishtar, SM8550 common and Xiaomi hardware source.
- A pinned manifest for the Android/Evolution X/LineageOS platform projects.
- The reviewed Linux 5.15.224 ishtar kernel delta and the SM8550 module delta.
- The final cumulative platform patch set used by Build 305.
- Machine-readable provenance, artifact hashes and verification scope.

## What it is not

- It is not a complete 110 GiB Android checkout.
- It is not a standalone buildable AOSP tree or a complete ROM.
- It does not contain Recovery, KernelSU `init_boot`, vendor blobs, GMS,
  firmware, target-files, signing keys or release ZIPs.
- It does not relicense third-party source.

The complete checked-out working source was approximately **110.4 GiB**,
including about **36.3 GiB** under `.repo`. This archive is approximately
**45 MiB** before Git metadata, so GitHub is appropriate for the source record.
Large external inputs will be hosted separately on SourceForge when published;
no SourceForge URL is claimed yet.

## Branches

| Branch | Purpose |
| --- | --- |
| `main` | Android 17 Evolution X source baseline. All future Android 17 updates continue here. |
| `android-16` | Frozen Android 16 archive at `cefdce3ddcc13cb04ab4b0183933c72f6dddcea2`. Do not rewrite it. |

This project uses **no Android 17 release tags**. The old
`ishtar-1792226532-maintenance` tag is retained unchanged as historical
metadata.

## Archive layout

```text
device/xiaomi/ishtar/                   Device product, overlays, policy and recovery config
device/xiaomi/sm8550-common/            SM8550 common device tree
hardware/xiaomi/                        Xiaomi hardware support source
kernel/xiaomi/sm8550-devicetrees/       Complete ishtar DTS/DTBO source tree
kernel/patches/                         Linux 5.15.224 and SM8550 module deltas
manifests/                              Full-SHA platform and system lock manifests
patches/platform/                       Final cumulative platform changes
vendor/xiaomi/                          Build recipes only; proprietary payloads are absent
provenance/                             Canonical release evidence and issue history
releases/                               Per-build metadata
scripts/                                Preflight/apply and archive verification tools
docs/                                   Build, architecture, kernel and maintenance notes
```

See [Source layout](docs/SOURCE_LAYOUT.md) and
[Architecture](docs/ARCHITECTURE.md) for details.

## Source preparation

Use a case-sensitive Linux build filesystem. Reproduce the pinned platform
checkout, then run the archive integration script in preflight mode:

```bash
git clone --branch main https://github.com/CyberWei922/evolutionx-ishtar-maintenance.git ishtar-maintenance

export ISHTAR_ARCHIVE="$PWD/ishtar-maintenance"
mkdir android17
cd android17
repo init -u https://github.com/Evolution-X/manifest -b cnb --git-lfs
cp "$ISHTAR_ARCHIVE/manifests/platform-pinned.xml" .repo/manifests/ishtar-pinned.xml
repo init -m ishtar-pinned.xml
repo sync -c -j6 --no-clone-bundle --no-tags

"$ISHTAR_ARCHIVE/scripts/apply.sh" "$PWD"
"$ISHTAR_ARCHIVE/scripts/apply.sh" --apply "$PWD"
```

`apply.sh` is read-only by default. It validates every manifest revision,
classifies all platform patches and refuses to write if any patch conflicts.
It copies the maintained source trees without deleting existing proprietary
content. Kernel and module patches are documented separately because this
archive does not contain either full source tree.

Read [Build](docs/BUILD.md), [External inputs](docs/EXTERNAL_INPUTS.md) and
[Maintenance](docs/MAINTENANCE.md) before building or changing a device.

## Build 305 identity

| Field | Value |
| --- | --- |
| Product | `lineage_ishtar` |
| OS | Android 17 / SDK 37 |
| ROM | Evolution X 12.3, `EvolutionX-17.0-20261107-ishtar-12.3-Unofficial` |
| Incremental | `1794048426` |
| Kernel | Based on Linux `5.15.224`; ACK/vendor/ishtar adaptation applied |
| Installed slot | `_b` |
| SELinux | Enforcing |
| ROM SHA256 | `205107f2548f7f7c239130cd89dec6fd7987ed28f7d9487919152e319ec2ee7e` |
| target-files SHA256 | `dc68886525608feb11ac46893f892217ed2ea547ba016e15f0437f9afd7e4f7e` |

The kernel is **based on 5.15.224**, but it is not a pristine upstream
5.15.224 tree. `official_gki_abi_certification` is false. The single kernel
patch captures the reviewed delta; the roughly 2,661 upstream changes do not
need to be uploaded as thousands of individual commits.

## Verification scope

The installed 305 build passed its recorded static artifact checks,
preserved-data sideload checks, 34-partition comparison, selected functional
regressions, WiFi PNO checks, OIS protocol checks, composition-cache probes, a
limited UI matrix and a 180-second observation. See
[Verification](docs/VERIFICATION.md) and
[`releases/305.json`](releases/305.json).

Physical call quality, carrier VoLTE/WFC, long unplugged standby, dark-room
ALS, physical OIS image quality, collision-log workload, NFC transactions,
Bluetooth accessories/audio, haptic feel and long-term graphics stability have
not been physically verified. Do not turn the scoped acceptance above into a
general claim that every feature is validated.

## Documentation

- [简体中文入口](README.zh-CN.md)
- [Build](docs/BUILD.md)
- [Source layout](docs/SOURCE_LAYOUT.md)
- [Architecture and Evolution X adaptation](docs/ARCHITECTURE.md)
- [Platform patches](docs/PLATFORM_PATCHES.md)
- [Kernel](docs/KERNEL.md)
- [External inputs](docs/EXTERNAL_INPUTS.md)
- [Verification](docs/VERIFICATION.md)
- [Maintenance](docs/MAINTENANCE.md)
- [Licensing and distribution](docs/LICENSE_AND_DISTRIBUTION.md)
- [Provenance](provenance/README.md)

## License

This is a combined repository with no blanket replacement license. Every
imported component retains its original copyright, SPDX identifier and license.
See [LICENSE.md](LICENSE.md), [NOTICE](NOTICE) and
[Licensing and distribution](docs/LICENSE_AND_DISTRIBUTION.md).
