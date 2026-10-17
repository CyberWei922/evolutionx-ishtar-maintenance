# ishtar maintenance repository

This is an incremental maintenance source repository for Xiaomi 13 Ultra (ishtar), Evolution X Android 16. Read README.md and docs/CURRENT_STATE.md first.

- The owner has disabled Gemini delegation. Do not invoke Gemini Delegate MCP, delegate.py or AGY as an alternative. Older delegation preferences are superseded.

- Preserve exact upstream project revisions in manifests/. Patches are relative to their recorded project HEADs.
- Preserve existing device/common/kernel/KernelSU modifications, not only the newest fix.
- No ROMs, APKs, shared libraries, partition images, private keys, tokens, device-private logs or complete Android checkout in Git. Keep binary dependencies as sizes/hashes/locations.
- SELinux remains globally Permissive. Do not claim Enforcing validation or blindly generate allows from denials.
- Current startup refresh fix is accepted for one first-boot desktop swipe. Four new and 24 mode-switch tests pass; six older tests remain failing in the selected 57. Do not expand acceptance.
- MIUI default routes and the owner preference migration are installed as 1792252694: six resolver checks and the dedicated app domain passed. Owner confirmed requested data/default camera/photo-video sound/fingerprint checks normal. Preserve Aperture as fallback; external capture-result return and Enforcing validation are still separate.
- No device reboot, flashing, data wipe, bootloader relock, automatic log capture, repo sync/reset/clean, or production changes are implied by maintaining this repository.
- Review proposed source and metadata differences before applying. A repository snapshot is not a flash instruction.

## License-scoped upload (2026-10-17)

- Read LICENSE.md, NOTICE and docs/LICENSE_AND_DISTRIBUTION.md. Preserve per-file upstream licenses; original maintenance additions use Apache-2.0.
- Only export reviewed paths in provenance/source-files.json, provenance/source-overrides.json, provenance/candidate-source-files.json, provenance/next-source-files.json or provenance/framework-api-source-files.json. Candidate inventories are separate from installed acceptance. Never classify arbitrary text as safe to upload.
- Original vendor text and derived MIUI implementation with unconfirmed redistribution permission stay external. Keep hashes/paths, not payloads. Preserve local frozen deliveries.
- Full source snapshots preserve build-source bytes. Adjacent .license notices and project patches describe modifications; do not copy .license sidecars into build rules.
- Read each plan's actual status. SELinux phases 3–5 are applied, compiled and installed as 1792246397; 29 files, 21 partition ranges, live labels and loaded policy are verified. Owner reported normal boot; camera/audio/fingerprint acceptance for this build remains pending. SELinux remains Permissive.
- The accepted source snapshots/tag still describe the previous accepted 1792226532 baseline. Apply the separately recorded cumulative candidate patch to reproduce the previous installed 1792246397 baseline sources; do not relabel old acceptance as current feature validation.
- Next-wave working source has 59 pinned files and a further 14-path patch on top of 1792246397. Its policy/helper/native preferred XML and full ROM 1792252694 are built and checked against all 34 actual payload partitions; 1792252694 is installed and owner-tested within the recorded scope, and no Enforcing trial exists. Do not use either older source map to validate the changed Ubuntu checkout. Respect concurrent Ubuntu work: no process termination; keep own build low priority; this completed build initially used two CPUs and expanded only its own descendants to four measured-idle CPUs for packaging.
- Current main/tag replace the previous single-commit snapshot after a local-only backup. Do not merge the excluded historical commit back into this repository.

## Post-install next batch (2026-10-18)

- The working checkout now has a further one-file app_api_service/radio_service find change; use plans/selinux-framework-api-20261018/working-source-files.json (59 files), not the installed baseline inventory. New policy is not installed or in a new ROM.
- A 25-second marker-only detached watchdog dry run passed in the retained ksu domain; it never called setenforce. Actual restoration from Enforcing is unverified and needs new explicit trial authorization. No startup hook or module exists. Never reuse an expired run token or PID.
- Postboot01 stopped on ADB error: closed; postboot02 succeeded with 98 command attempts, no retries, 32 file and 21 partition-range hashes, labels, all six defaults, data baseline and loaded policy checked. Keep failed evidence. No original phone logs or policy binaries in this repository.
