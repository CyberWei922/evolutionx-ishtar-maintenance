# ishtar maintenance repository

This is an incremental maintenance source repository for Xiaomi 13 Ultra (ishtar), Evolution X Android 16. Read README.md and docs/CURRENT_STATE.md first.

- The owner has disabled Gemini delegation. Do not invoke Gemini Delegate MCP, delegate.py or AGY as an alternative. Older delegation preferences are superseded.

- Preserve exact upstream project revisions in manifests/. Patches are relative to their recorded project HEADs.
- Preserve existing device/common/kernel/KernelSU modifications, not only the newest fix.
- No ROMs, APKs, shared libraries, partition images, private keys, tokens, device-private logs or complete Android checkout in Git. Keep binary dependencies as sizes/hashes/locations.
- SELinux remains globally Permissive. Do not claim Enforcing validation or blindly generate allows from denials.
- Current startup refresh fix is accepted for one first-boot desktop swipe. Four new and 24 mode-switch tests pass; six older tests remain failing in the selected 57. Do not expand acceptance.
- MIUI as the default camera is planned for a future version; it is not yet implemented. Preserve working Aperture as fallback initially.
- No device reboot, flashing, data wipe, bootloader relock, automatic log capture, repo sync/reset/clean, or production changes are implied by maintaining this repository.
- Review proposed source and metadata differences before applying. A repository snapshot is not a flash instruction.

## License-scoped upload (2026-10-17)

- Read LICENSE.md, NOTICE and docs/LICENSE_AND_DISTRIBUTION.md. Preserve per-file upstream licenses; original maintenance additions use Apache-2.0.
- Only export reviewed paths in provenance/source-files.json, provenance/source-overrides.json or provenance/candidate-source-files.json. The candidate inventory is separate from installed acceptance. Never classify arbitrary text as safe to upload.
- Original vendor text and derived MIUI implementation with unconfirmed redistribution permission stay external. Keep hashes/paths, not payloads. Preserve local frozen deliveries.
- Full source snapshots preserve build-source bytes. Adjacent .license notices and project patches describe modifications; do not copy .license sidecars into build rules.
- Read each plan's actual status. SELinux phases 3–5 are applied, compiled and installed as 1792246397; 29 files, 21 partition ranges, live labels and loaded policy are verified. Owner reported normal boot; camera/audio/fingerprint acceptance for this build remains pending. SELinux remains Permissive.
- The accepted source snapshots/tag still describe the previous accepted 1792226532 baseline. Apply the separately recorded cumulative candidate patch to reproduce current 1792246397 sources; do not relabel old acceptance as current feature validation.
- Current main/tag replace the previous single-commit snapshot after a local-only backup. Do not merge the excluded historical commit back into this repository.
