# Platform patches

## Authority

`patches/platform/` is the final cumulative platform patch set for the
Android 17 baseline in this repository. It is the authoritative source delta
for the platform projects.

`provenance/release-patches/` contains issue-by-issue history from the 300,
303 and 304/305 work. Those patches are useful for review but may overlap or
assume earlier stages. Do not stack them blindly on the final tree.

## File naming

Each file maps to one manifest project by replacing `/` with `__`:

```text
device/qcom/sepolicy_vndr/sm8550
→ device__qcom__sepolicy_vndr__sm8550.patch
```

`scripts/apply.sh` resolves the path from the pinned manifest, verifies the
project Git worktree and applies the patch from that project directory.

## Patch inventory

| Patch | Target project | Files | Added | Removed |
| --- | --- | ---: | ---: | ---: |
| `bionic.patch` | `bionic` | 1 | 11 | 3 |
| `build__soong.patch` | `build/soong` | 1 | 10 | 0 |
| `device__lineage__sepolicy.patch` | `device/lineage/sepolicy` | 4 | 68 | 4 |
| `device__qcom__sepolicy.patch` | `device/qcom/sepolicy` | 1 | 1 | 1 |
| `device__qcom__sepolicy_vndr__sm8550.patch` | `device/qcom/sepolicy_vndr/sm8550` | 6 | 6 | 6 |
| `frameworks__base.patch` | `frameworks/base` | 3 | 70 | 2 |
| `frameworks__native.patch` | `frameworks/native` | 10 | 386 | 15 |
| `frameworks__opt__telephony.patch` | `frameworks/opt/telephony` | 1 | 93 | 0 |
| `hardware__qcom-caf__thermal.patch` | `hardware/qcom-caf/thermal` | 8 | 338 | 5 |
| `packages__apps__GameSpace.patch` | `packages/apps/GameSpace` | 1 | 13 | 5 |
| `system__netd.patch` | `system/netd` | 3 | 87 | 0 |
| `system__sepolicy.patch` | `system/sepolicy` | 2 | 5 | 1 |
| `system__update_engine.patch` | `system/update_engine` | 15 | 1780 | 31 |
| `vendor__gms.patch` | `vendor/gms` | 1 | 2 | 0 |
| `vendor__lineage.patch` | `vendor/lineage` | 4 | 274 | 154 |
| `vendor__qcom__opensource__vibrator.patch` | `vendor/qcom/opensource/vibrator` | 1 | 36 | 7 |
| `vendor__unbundled_google__packages__SystemUIGoogle.patch` | `vendor/unbundled_google/packages/SystemUIGoogle` | 1 | 2 | 0 |

Counts are patch line statistics, not runtime test counts.

## Main concerns

### SurfaceFlinger and ALS

`frameworks/native.patch` adds Ishtar ALS frame-hint code and touches the
Scheduler, SurfaceFlinger and transaction tests. The device also sets client
composition cache behavior through its vendor properties. Verification found
real cache reuse in controlled probes, but this is not a full graphics stress
qualification.

### Preserved-data OTA

`system/update_engine.patch` adds backuptool broker/store components and a
postinstall runner integration. `vendor/lineage.patch` carries the matching
backuptool scripts. Together they support the device's preserved-data A/B
update path. This is source behavior, not proof that every future OTA path has
been tested.

### Thermal

`hardware/qcom-caf/thermal.patch` adds the Ishtar skin implementation,
netlink utilities and service integration. The installed build preserved the
existing thermal policy readability checks.

### Telephony and networking

The telephony patch completes LTE physical-channel metadata after a matching
cell identity is obtained. The netd patch adds ishtar VPN routing behavior and
a focused probe. Recorded tests cover the stated cases, not all carriers,
SIMs, VPN applications or long-duration network transitions.

### UI and framework

The framework patch covers notification background initialization and app
filter behavior. SystemUI/SystemUIGoogle changes and the GameSpace exception
fix are included so the device UI and game overlay remain compatible with the
Evolution X product.

## Applying

Use:

```bash
scripts/apply.sh /path/to/android17
scripts/apply.sh --apply /path/to/android17
```

Preflight must pass for all 17 patches before `--apply` writes. A patch is
accepted if it applies forward or if `git apply --reverse --check` shows the
exact final change is already present. Any other state is a conflict and
aborts the run.

## Reviewing the final result

The final patch set was reverse-checked against the Build 305 worktree. For
each project, compare:

```bash
git -C <project> apply --check /path/to/patches/platform/<name>.patch
git -C <project> apply --check -R /path/to/patches/platform/<name>.patch
```

Exactly one direction should be applicable on a clean baseline; an integrated
final worktree should normally pass the reverse check.
