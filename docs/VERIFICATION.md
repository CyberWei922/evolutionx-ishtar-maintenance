# Verification record

## Status

Build 305 is the installed, owner-scoped verification candidate for this
Android 17 baseline. It is **not published**, is **Unofficial**, and is signed
with **test keys**.

The canonical runtime summary is:

```text
provenance/completion305-runtime-summary.json
```

The static artifact evidence is:

```text
provenance/artifact-check305.json
```

## Artifact identity

| Field | Value |
| --- | --- |
| Incremental | `1794048426` |
| Build name | `EvolutionX-17.0-20261107-ishtar-12.3-Unofficial` |
| ROM bytes | 4,550,939,112 |
| ROM SHA256 | `205107f2548f7f7c239130cd89dec6fd7987ed28f7d9487919152e319ec2ee7e` |
| target-files bytes | 10,441,170,216 |
| target-files SHA256 | `dc68886525608feb11ac46893f892217ed2ea547ba016e15f0437f9afd7e4f7e` |
| Kernel release | `5.15.224` |
| Source pins | 2,844 candidate / 130 protected |
| Installed slot | `_b` |
| Boot ID | `92e5a35a-02f7-432f-9e0d-c2afd86e7704` |
| SELinux | Enforcing |

## Static checks

The recorded Build 305 audit passed:

- ROM CRC and payload-property checks;
- CMS, AVB and whole-file signature checks against the expected test key;
- 34 actual payload partitions and their hashes;
- 403 actual kernel modules;
- previously introduced camera, OIS, ALS, blur, collision, IWLAN, netd,
  desktop, WiFi PNO and composition-cache repairs;
- WiFi resource overlay signature and actual boolean value;
- source-built Recovery body and target-files match;
- KernelSU static compatibility and actual `init_boot` identity.

The first static-audit attempt stopped because a DEX checker hard-coded `p0`
and produced a false negative. The original failure is retained; the checker
was corrected and retry 2 passed. The OTA was not changed by that checker-only
fix.

## Installation

The only accepted 305 installation was a preserved-data sideload:

```text
host sideload exit: 0
slot: _b
partitions verified: 34
user applications preserved: 44
app-data identity preserved: yes
root/module state preserved: yes
wipe/factory reset: no
```

Recovery logs did **not** expose the new payload. Therefore:

```text
recovery_install_status = null
```

The installation proof is the host exit status plus the byte lengths and
hashes read from all 34 installed partitions. Do not replace `null` with a
made-up success result.

## Focused functional checks

The recorded 305 checks include:

- **B03:** fresh notification-row blur initialization occurred on the UI
  thread, with zero async-inflate assertion failures.
- **B04:** earpiece output used channel mask `0x0001`; telephony receive input
  remained `0x0010`.
- **B05:** three allow/disallow cycles completed with GameSpace alive and no
  fatal/ANR.
- **R02:** LTE Band 3, downlink channel 1506, PCI 141 and ServiceState metadata
  matched.
- **WiFi PNO:** two screen-off hardware-PNO rounds without a temporary
  override obtained IPv4 in `2.4562825s` and `2.607194875s`, one result event
  each and 0% LAN loss.
- **OIS:** three preview cycles released and three real park/trigger operations
  succeeded; five historical error classes were zero.
- **Composition cache:** a controlled probe observed
  `usesClientComposition=true`, `reusedClientComposition=true` and
  `flipClientTarget=false`.
- **UI matrix:** portrait/landscape, notification shade, recents, app drawer,
  lock screen and HOME were exercised. An H.264 1440x3200 capture decoded all
  548/548 frames.
- **Observation:** 180.53 seconds with seven real core PIDs stable, no
  fatal/ANR/kernel panic and an empty crash buffer.

Raw AVC counts and `audit_lost` counters are retained as raw values. They are
not evidence of zero denials or complete audit coverage.

## Historical tombstone

`/data/tombstones/tombstone_15` is a historical vendor camera-provider
`SIGABRT` from `2026-11-07 11:26:03 +0800`, before the current 305 boot. It
contains only an older boot's ashmem name. It is not a 305 regression.

## Not verified

The following remain outside the recorded physical acceptance:

- real call audio quality;
- carrier VoLTE and WiFi Calling;
- long unplugged standby and doze power;
- dark-room ambient light sensor behavior;
- physical OIS image quality;
- real collision-log rotation workload;
- NFC card/payment transaction;
- Bluetooth accessory pairing and audio;
- physical haptic feel;
- full graphics stress and long-term stability;
- overall daily usability.

Temporary probes and restored settings are not product acceptance.
