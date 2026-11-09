# External inputs

## Why inputs are external

This archive records source that should be versioned and reviewed. It does not
redistribute device-specific binaries, Google payloads, firmware, full OTA
artifacts or signing material. Those inputs are large, have separate terms, or
must match the exact device state.

When published, the external asset bundle is intended for SourceForge. **No
SourceForge URL is currently recorded.** Do not add a guessed URL.

## Required build inputs

| Input | Expected location | Identity or constraint |
| --- | --- | --- |
| KernelSU `init_boot` image | `device/xiaomi/ishtar/root/init_boot-KernelSU-v3.3.1.img` | 8,388,608 bytes; SHA256 `acd9eb583ccdebe8cf84fb238608387b384ddbb4f0927fc19b0b6a2e6e05806c` |
| Reviewed IWLAN unsigned APK | `device/xiaomi/sm8550-common/iwlan-fix/payload/IWlanService-unsigned.apk` | 1,551,694 bytes; SHA256 `c66f190fd4aa8f010a8f7d03a157e57bd5d379b125e24d1b4282d987a5047a68` |
| MIUI Camera APK | `vendor/xiaomi/ishtar-camera/proprietary/system/priv-app/MiuiCamera/MiuiCamera.apk` | 183,994,431 bytes; SHA256 `b7b2c4c77c4a4656e94569b918b208293f4d1a0d60900c0e805082770452f9ea` |
| Xiaomi ishtar vendor payloads | `vendor/xiaomi/ishtar/proprietary/` | Accepted `.307` baseline; vendor security patch `2026-02-01` |
| SM8550 common vendor payloads | `vendor/xiaomi/sm8550-common/proprietary/` | Must match the accepted `.307` base |
| Camera system libraries and settings | `vendor/xiaomi/ishtar-camera/proprietary/` | See `accepted-payloads.json` |
| Google applications/APEX | `vendor/gms/` and manifest-matched LFS inputs | Subject to Google distribution terms |
| Firmware | Matching ishtar firmware package | Do not substitute an untested regional package |
| Platform signing keys | External secure build environment | Never commit; Build 305 used test keys |

The device tree's `extract-files.py`, `setup-makefiles.py`,
`proprietary-files.txt` and `proprietary-firmware.txt` are source-controlled so
the payload inventory can be reviewed and regenerated.

## Recovery

Build 305's artifact audit found:

- Recovery was built from source;
- the image matched target-files;
- there were no prebuilt TWRP/KSU entries in the 305 image.

Recovery is still a deployment image and is not stored in this GitHub archive.
It belongs with the external release assets.

`device/xiaomi/ishtar/recovery/README.md` is a historical note from an earlier
prebuilt/TWRP phase. It mentions a different image and must not be used as the
identity of the 305 Recovery.

## Verification evidence

When the SourceForge bundle is created, publish at least:

- final filenames, byte sizes and SHA256 values;
- the exact vendor base and firmware assumptions;
- a manifest of every external input;
- testkey or release-signing status;
- the relationship between the external files and `releases/305.json`.

Do not publish a private signing key, signing keystore, access token, ADB
serial or build-host credential.

## Runtime versus build inputs

Some runtime observations used a temporary probe or override. Those temporary
changes are not required build inputs and must not be represented as source
configuration. Build inputs are the files consumed by Soong/Make and the
recorded external products.
