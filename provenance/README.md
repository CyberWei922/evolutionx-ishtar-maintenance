# Provenance files

## Source inputs

`source-inputs305.json` is the canonical machine-readable description of the
Build 305 source state, candidate/protected pins, selected source paths and
artifact-related source hashes. It is preserved as evidence, not as a live
configuration file.

## Runtime and artifact evidence

`completion305-runtime-summary.json` is the authoritative post-flash completion
record for Build 305. It supersedes earlier static "not yet installed" snapshots
for runtime state, while preserving their identity by hash.

`artifact-check305.json` records the static target-files, partition, signature,
payload and module audit. It also preserves several raw counters and historical
checks that must be read with the scope in `docs/VERIFICATION.md`.

## Release-level patches

`release-patches/` contains issue-by-issue evidence:

```text
wifi-pno300.patch
composition-cache303.patch
errorfix304-305-all.patch
errorfix305.patch
```

These files show how individual findings were addressed. They are not the
final integration series and must not be blindly applied after
`patches/platform/` has already been applied.

## Absolute paths

Some canonical JSON records contain absolute paths from the build and test
hosts. They are deliberately kept byte-for-byte so hashes and external audit
references remain meaningful. Those strings are evidence, not repository-local
paths. Do not rewrite them merely to make a grep clean.

## Reproducibility rule

When a future build changes any of the following, create a new provenance
record rather than editing the 305 record in place:

- source pin;
- platform patch;
- kernel or module patch;
- external input hash;
- ROM/target-files hash;
- installation result;
- verification scope.

The final source tree is defined by the archive trees, the pinned manifests and
`patches/platform/`. The provenance directory explains how the 305 result was
obtained and verified.
