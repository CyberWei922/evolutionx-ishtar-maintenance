# Maintenance and future Android 17 updates

## Branch policy

- `main` is the only Android 17 maintenance branch.
- No Android 17 release tag is created by default.
- Continue each future Android 17 update as a normal commit on `main`.
- `android-16` is frozen at
  `cefdce3ddcc13cb04ab4b0183933c72f6dddcea2`; never alter or force-push it.
- Do not delete the existing historical tag.
- Keep the repository private until the owner explicitly changes visibility.

The 2026-11-09 baseline replaced the old `main` tree with a new root commit.
That one-time history replacement was intentional. Future maintenance should
be append-only on the new `main`, unless the owner explicitly requests another
history rewrite.

## Updating from upstream Android/Evolution X

1. Record the old and new upstream revisions.
2. Regenerate a full-SHA platform manifest, not branch names or short SHAs.
3. Rebase or re-export the ishtar-specific platform delta one project at a
   time.
4. Recheck every patch against the new pinned project revision.
5. Keep device, common, hardware and DTS changes in their complete trees.
6. Regenerate `kernel/patches/` from the exact maintained kernel baseline to
   the new result.
7. Update `manifests/`, `provenance/`, `releases/` and affected docs.
8. Run a clean source preflight and, when authorized, a real build.
9. Do not claim a version is installed until the device-side evidence exists.

Do not use `git apply --3way` as an automatic upgrade mechanism in
`apply.sh`. A three-way merge can silently choose a plausible but untested
resolution. Conflicts must be resolved deliberately and captured as a new
patch baseline.

## Adding a release record

Create `releases/<incremental>.json` with:

- product, ROM, Android/SDK, incremental and security-patch identity;
- source pins, kernel base and patch hash;
- ROM and target-files byte length/SHA256;
- signing status;
- installation method and result;
- exact verification scope and unverified areas;
- external asset location, or `null` while pending.

Do not put a default `false` for "tested" if testing has not occurred; use an
explicit scope list or `null` where the evidence is unavailable.

## Updating platform patches

`patches/platform/` must describe the final cumulative tree, not an
accumulation of superseded issue patches. For each project:

```bash
git -C /path/to/project format-patch --stdout <base>..<final> \
  > patches/platform/<manifest-path-with-double-underscores>.patch
```

Then verify the patch against the pinned source and the final worktree. Keep
provenance issue patches as historical evidence only.

## External assets and SourceForge

When the owner creates a SourceForge project:

1. add only the approved binaries and proprietary inputs;
2. publish hashes and a manifest alongside them;
3. record the real URL in `releases/` and `docs/EXTERNAL_INPUTS.md`;
4. never embed credentials or signing material;
5. keep the GitHub archive's source-only boundary.

Until a real URL exists, leave the field `null`.

## Archive hygiene

After changing files:

```bash
cd /path/to/archive
scripts/verify-archive.sh
find . -type f ! -path './.git/*' ! -name SHA256SUMS \
  | LC_ALL=C sort | xargs shasum -a 256 > SHA256SUMS
shasum -a 256 -c SHA256SUMS
```

The verifier rejects symlinks, nested Git repositories, forbidden binary
extensions, NUL-containing files, files over 20 MiB, non-SHA manifest
revisions, suspicious token patterns and checksum mismatches.

Do not commit generated build output or an extracted proprietary tree just to
make a local build pass.

## Release claims

Separate these states in every future note:

1. source baseline recorded;
2. build artifact produced;
3. static artifact audited;
4. preserved-data installation completed;
5. each runtime check passed;
6. owner accepted for daily use;
7. published or officially released.

A build is not published merely because it exists. A successful install is not
proof of every hardware feature.
