# Architecture and Evolution X adaptation

## Product identity

The system is **Evolution X 12.3 on Android 17**, not pure AOSP. The ishtar
product is named `lineage_ishtar` and inherits:

```make
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)
```

That supplies the Evolution X/LineageOS full-phone product behavior. The
`device/xiaomi/ishtar` and `device/xiaomi/sm8550-common` trees then add the
Xiaomi 13 Ultra hardware integration.

## Layers

| Layer | Ishtar responsibility |
| --- | --- |
| Evolution X/Lineage product | Full-phone policy, common apps, OTA packaging and backuptool integration |
| Platform patches | Framework, SurfaceFlinger, telephony, netd, update_engine, policy and UI behavior needed by ishtar |
| Device trees | Product composition, board configuration, overlays and hardware service setup |
| Xiaomi hardware source | Fingerprint and Xiaomi-specific HAL support |
| Kernel/DTS/modules | SM8550 platform, camera, display, touch, NFC, thermal and board devices |
| External payloads | Xiaomi vendor libraries, firmware, camera app, GMS and Recovery/root images |

## Device-specific adaptation

`device/xiaomi/ishtar` includes:

- `lineage_ishtar.mk`, `device.mk` and `BoardConfig.mk`;
- ishtar and common resource overlays;
- Cirrus/TDM audio paths and stock backend configuration selection;
- camera preference, MIUI Camera integration and camera navigation overlay;
- FTS first-stage firmware, UDFPS and ST NFC configuration;
- radio restart, init, properties, VINTF and power profiles;
- device and vendor SELinux policy;
- native Recovery integration;
- display/ALS-related configuration and properties.

`device/xiaomi/sm8550-common` adds SM8550-wide audio, media, sensors, thermal,
power, USB, fingerprint, display, RIL, WiFi and sepolicy configuration.

## Evolution X/Lineage adaptation in platform source

The final cumulative patches in `patches/platform/` include real integration
work beyond the device trees:

| Area | Adaptation |
| --- | --- |
| `frameworks/native` | Ishtar ALS frame-hint source and SurfaceFlinger scheduler/transaction behavior; client composition cache configuration |
| `frameworks/base` | Notification blur initialization and app filter behavior |
| `frameworks/opt/telephony` | LTE physical-channel metadata completion for the device's telephony stack |
| `hardware/qcom-caf/thermal` | Ishtar skin-temperature implementation and netlink-backed thermal policy |
| `system/netd` | Ishtar VPN routing changes and a focused probe |
| `system/update_engine` | Backuptool store/broker and postinstall integration for preserved-data A/B updates |
| `vendor/lineage` | Backuptool runtime scripts coordinated with update_engine |
| `system/sepolicy` / `device/*/sepolicy` | Recovery/update, testservice, Sony/QTI and ishtar-only domains |
| `packages/apps/GameSpace` | Notification-list SecurityException handling in the game overlay listener |
| `vendor/qcom/opensource/vibrator` | Xiaomi vibrator effect integration |
| `vendor/gms` / `SystemUIGoogle` | Manifest-matched Google configuration and build wiring |

Some patches are small compatibility changes; others, particularly
SurfaceFlinger ALS and update_engine backuptool support, are substantial.

## Recovery

Build 305 used a Recovery binary built from source and recorded that no
prebuilt TWRP or KSU Recovery entry was used. Recovery and the KernelSU
`init_boot` are still separate deployment inputs from this source archive.

`device/xiaomi/ishtar/recovery/README.md` contains historical notes from an
earlier TWRP/prebuilt phase. For the 305 artifact, use
`provenance/artifact-check305.json`, `docs/VERIFICATION.md` and
`releases/305.json` as the current evidence.

## SELinux

The installed 305 product ran in Enforcing mode. The source includes narrow
Recovery/update and vendor policy exceptions required by the device. Do not
describe raw AVC counters as zero-denial proof: the runtime records preserve
the original AVC and audit-loss counters and only state that no new
305-regression pattern was established.

## Compatibility boundary

This source represents an Unofficial maintenance line. It is not an official
Evolution X device port, has no official GKI ABI certification and does not
claim that every hardware feature has been physically verified.
