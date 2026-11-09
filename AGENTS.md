# Repository maintenance rules

## Branch contract

- `main` is the Android 17 Evolution X baseline for Xiaomi 13 Ultra / ishtar.
- Continue future Android 17 work on `main`; do not create Android 17 release
  tags unless the owner explicitly changes this policy.
- Never rewrite, force-push, retag or otherwise alter `android-16`. Its frozen
  revision is `cefdce3ddcc13cb04ab4b0183933c72f6dddcea2`.
- Do not delete or move the existing historical tag
  `ishtar-1792226532-maintenance`.
- Keep the repository private until the owner explicitly authorizes a
  visibility change.

## Source boundaries

- This is a maintenance archive, not a complete Android source checkout, ROM,
  target-files bundle or standalone buildable AOSP tree.
- Never add Recovery images, `init_boot`, APKs, shared libraries, kernel
  objects, firmware, vendor proprietary payloads, GMS payloads, target-files,
  release ZIPs, signing keys or credentials.
- Large binaries and proprietary inputs belong in the separately managed
  SourceForge area. Do not invent a SourceForge URL before it exists.
- Preserve external-input paths referenced by `BoardConfig.mk`,
  `Android.bp`, `Android.mk` and `device.mk`. Do not make the archive appear
  buildable by deleting those references.
- Keep `vendor/xiaomi/*/proprietary/` external. Files under the archive's
  `vendor/xiaomi/` are build recipes and metadata only.

## Technical accuracy

- Describe the kernel as **based on Linux 5.15.224 with ACK/vendor/ishtar
  adaptation**, not as pristine upstream 5.15.224.
- `official_gki_abi_certification` is false. Do not claim GKI ABI
  certification.
- `patches/platform/` is the authoritative final cumulative patch set.
  `provenance/release-patches/` is issue-by-issue history and must not be
  blindly stacked on the final tree.
- Build 305 is installed and verified only within the scope recorded in
  `docs/VERIFICATION.md` and `releases/305.json`. Do not generalize this to
  unverified physical, carrier or long-duration behavior.
- Recovery logs did not expose the new 305 payload. The installation evidence
  is host sideload exit 0 plus the 34 installed-partition hashes. Keep
  `recovery_install_status` as `null`.
- The 305 artifact is testkey signed and unpublished.
- The historical camera-provider tombstone predates the 305 boot and is not a
  305 regression.

## Safe changes

- Run `scripts/verify-archive.sh` after changing files.
- Regenerate `SHA256SUMS` after the final file set changes.
- Keep manifest revisions at full 40-character Git SHAs.
- Update `releases/<incremental>.json`, provenance and documentation when a
  new maintained build is established.
- Do not build, flash a phone, change runtime settings or modify an original
  Android checkout as a side effect of editing this repository.
- Do not publish, open official OTA requests, contact maintainers or apply for
  official status without separate owner authorization.
