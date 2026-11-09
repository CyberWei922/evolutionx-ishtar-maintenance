# Build guide

## Scope

This archive does not contain a complete Android tree. The build process has
three inputs:

1. a pinned Android/Evolution X/LineageOS checkout;
2. the maintained device, hardware and DTS trees in this archive;
3. external proprietary, GMS, firmware, Recovery/root and signing inputs.

The Build 305 source tree was approximately 110.4 GiB, including roughly
36.3 GiB under `.repo`. Use a case-sensitive Linux filesystem and enough free
space for source, output and temporary files.

## Prerequisites

- Linux x86-64 host with the normal AOSP build packages.
- `git`, `repo`, `rsync`, `python3`, `shasum`/`sha256sum` and standard patch
  tools.
- Case-sensitive filesystem. Do not use a default case-insensitive macOS
  volume for the Android checkout.
- Matching external inputs listed in [External inputs](EXTERNAL_INPUTS.md).

Keep this archive outside the Android source root. Soong would otherwise see
duplicate copies of device and hardware source.

## Initialize the pinned platform checkout

The checked-in `platform-pinned.xml` is a complete platform manifest snapshot
whose project revisions are full 40-character SHAs. The Evolution X manifest
repository supplies the normal initialization framework.

```bash
export ISHTAR_ARCHIVE=/absolute/path/to/ishtar-maintenance

mkdir android17
cd android17
repo init -u https://github.com/Evolution-X/manifest -b cnb --git-lfs
cp "$ISHTAR_ARCHIVE/manifests/platform-pinned.xml" \
  .repo/manifests/ishtar-pinned.xml
repo init -m ishtar-pinned.xml
repo sync -c -j6 --no-clone-bundle --no-tags
```

`ishtar-system-lock.xml` is the compact 312-project system lock. It is
provided for audit and for reconstructing the exact subset; all of its
revisions also use full SHAs.

Do not combine this archive with an arbitrary already-modified worktree.
Review local changes first.

## Integrate the maintained source

Preflight is the default and performs no writes:

```bash
"$ISHTAR_ARCHIVE/scripts/apply.sh" "$PWD"
```

The preflight:

- validates the source root and XML manifests;
- requires every manifest revision to be a full SHA;
- maps each platform patch to exactly one manifest project path;
- checks that each target is a Git worktree at the pinned revision;
- classifies every patch as forward-applicable, already applied or conflict;
- exits before any modification if a conflict is found.

Apply only after preflight passes:

```bash
"$ISHTAR_ARCHIVE/scripts/apply.sh" --apply "$PWD"
```

The apply step copies the maintained device, hardware and DTS trees with
`rsync` and applies the forward-applicable platform patches. It does not use
`--delete`, so existing proprietary payloads and unrelated local files remain
in place. Re-running it accepts already-applied patches.

## Kernel and modules

The archive stores deltas, not the full kernel or module trees:

```text
kernel/patches/linux-5.15.224-ishtar.patch
kernel/patches/sm8550-modules.patch
kernel/xiaomi/sm8550-devicetrees/
```

Apply the kernel and module patches to their recorded matching trees as
described in [Kernel](KERNEL.md). Do not attempt to build the archive as if it
contained `kernel/xiaomi/sm8550` or `kernel/xiaomi/sm8550-modules`.

## External inputs

At minimum, restore the paths documented in
[External inputs](EXTERNAL_INPUTS.md):

- accepted `.307` Xiaomi vendor payloads;
- matching ishtar camera APK/libraries and vendor settings;
- the reviewed KernelSU `init_boot` image;
- platform-signed `IWlanService`;
- manifest-matched `vendor/gms` and LFS objects;
- build/test signing material.

Do not substitute an arbitrary regional firmware or vendor package and assume
equivalence to the tested `.307` baseline.

## Compile

The 305 build used the `cp2a` target release, six ordinary jobs, one
high-memory job and Full LTO:

```bash
source build/envsetup.sh
source vendor/lineage/vars/aosp_target_release
lunch "lineage_ishtar-${aosp_target_release}-userdebug"
export OUT_DIR=out
export NINJA_HIGHMEM_NUM_JOBS=1
m -j6 otapackage target-files-package check-vintf-all \
  'KERNEL_CC=CC=" clang" LD="ld.lld --threads=1"'
```

The build host used `MemoryHigh=25G`, `MemoryMax=27G` and temporary swap during
the kernel transition. Those are host resource controls, not phone properties.

## Artifact expectations

A successful `otapackage` is not by itself proof that the result matches the
305 release. Recompute the ROM, target-files, partition, AVB and selected
payload checks. Build 305 used test keys and was not published.

## Reproducibility boundary

The archive records the exact source deltas, pinned project revisions and
artifact hashes. A future clean-room build can reproduce the process only when
all external inputs and signing assumptions are also available. This archive
does not claim a new clean-room build was performed on 2026-11-09.
