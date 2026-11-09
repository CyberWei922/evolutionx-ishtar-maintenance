# Kernel, modules and DTS

## Identity

Build 305 used a kernel whose source result is based on Linux **5.15.224**.
It is an ishtar/Qualcomm/Android Common Kernel adaptation, not a pristine
upstream Linux tree and not an officially ABI-certified GKI kernel.

| Item | Revision or property |
| --- | --- |
| Maintained 5.15.211 baseline | `cf026dd0512672a7723836c948b0251b48ed01e5` |
| Upstream stable 5.15.211 | `c86c4726e7f044ab73b493c6f00527aafef640cd` |
| Ishtar 5.15.211 source | `34eaaccbb3d71edb8e03eb903d113e0e105b6237` |
| Upstream 5.15.224 | `700e62ea909a70b9f72692d8b7a192ac7ebde0aa` |
| Merged ishtar 5.15.224 commit | `bd41fd0f7e2fbb6a040c3411fd87ff191dbeff97` |
| Merged source tree | `57b99a00de67e9c3c50f81ee6413acc4d567a315` |
| Official GKI ABI certification | false |

## Patch

```text
kernel/patches/linux-5.15.224-ishtar.patch
bytes:  6307910
sha256: 7d80ec9272d75fb8d670c68d15a028e447b2c472be4fb24687c40ca9223086f6
```

The reviewed merge reports:

- 2,667 changed paths in the no-rename deployment delta;
- 2,664 logical changes when rename detection is applied;
- 21 files and 28 hunks that required reviewed conflict adaptation;
- `diff --check` passed;
- configuration fragments unchanged;
- `official_gki_abi_certification=false`.

The two path counts differ because two logical renames count as four paths in a
no-renames delta. The recorded kernel patch is one cumulative result; it is not
a claim that each of the roughly 2,661 upstream changes was imported as an
independent Git commit.

Two illustrative compiler-proven adaptations are:

- `fs/f2fs/sysfs.c`: use the existing embedded F2FS `gc_thread` and its task
  guard for the urgent-mid wakeup.
- `drivers/usb/dwc3/gadget.c`: retain the vendor soft-reset/event-buffer/start
  sequence while integrating upstream error recovery and serialization.

The timer-shutdown prerequisite also retains the Android KABI layout and the
legacy timer exports needed by this vendor kernel.

## Applying the kernel patch

Use the matching maintained kernel tree with the recorded baseline. Confirm both
the forward and reverse state before writing:

```bash
cd /path/to/kernel/xiaomi/sm8550
git apply --check "$ISHTAR_ARCHIVE/kernel/patches/linux-5.15.224-ishtar.patch"
git apply "$ISHTAR_ARCHIVE/kernel/patches/linux-5.15.224-ishtar.patch"
git apply --check -R "$ISHTAR_ARCHIVE/kernel/patches/linux-5.15.224-ishtar.patch"
```

The reverse check succeeding after application confirms the final delta is
present. Do not apply the patch to an arbitrary stable tree and call the result
reproducible without reviewing vendor context.

The full kernel tree is intentionally not in this repository.

## SM8550 external modules

```text
kernel/patches/sm8550-modules.patch
bytes:  7964
sha256: c2304da1491f44291a3c064eea811ddacd243e872c1aa442de2218799e085d7b
```

The six patched paths are:

```text
qcom/opensource/camera-kernel/drivers/cam_sensor_module/
├── cam_eeprom/cam_eeprom_core.c
├── cam_ois/cam_ois_core.c
├── cam_cci/cam_cci_core.c
├── cam_cci/cam_cci_dev.h
├── cam_cci/cam_cci_soc.c
└── cam_sensor/cam_sensor_core.c
```

Apply from the matching `kernel/xiaomi/sm8550-modules` tree:

```bash
cd /path/to/kernel/xiaomi/sm8550-modules
git apply --check "$ISHTAR_ARCHIVE/kernel/patches/sm8550-modules.patch"
git apply "$ISHTAR_ARCHIVE/kernel/patches/sm8550-modules.patch"
```

The module patch was reverse-checked against the Build 305 source. The full
module tree is intentionally not in this repository.

## DTS/DTBO

The complete ishtar DTS/DTBO source tree is included at:

```text
kernel/xiaomi/sm8550-devicetrees/
```

This is board source, not a prebuilt DTBO. Build it with the matching kernel and
device-tree build rules. The archive has no compiled `.dtbo`, `.dtb` or image.

## Correct description

Use:

> Linux 5.15.224 base with ACK, Qualcomm vendor and ishtar adaptation.

Do not use:

> Pristine upstream Linux 5.15.224

or:

> Official GKI 5.15.224 certified kernel

The observed build reports `5.15.224`, and the source result is based on that
stable release. That is different from an unmodified upstream tree.
