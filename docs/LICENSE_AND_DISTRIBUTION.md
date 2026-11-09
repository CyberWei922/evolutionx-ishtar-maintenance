# Licensing and distribution

## No blanket license

This repository is a maintenance aggregation. A single repository-level
license cannot replace the terms of AOSP, Evolution X, LineageOS, Linux,
Qualcomm, Xiaomi and other imported projects.

Apply the narrowest correct rule:

- read per-file SPDX identifiers and copyright notices;
- preserve component `NOTICE` and license files;
- keep GPL obligations for kernel/module material;
- keep Apache-2.0 notices for Android and device code;
- do not apply Apache-2.0 to GPL-only code;
- do not treat proprietary payload licenses as open source.

The copied texts are under `licenses/` and `licenses/upstream/`.

## Patches

A patch does not create a new standalone license. Its rights and obligations
follow the target project and the source lines it changes. Preserve the
applicable copyright and attribution when moving or rebasing a patch.

## External binaries

The repository does not include:

- Xiaomi or Qualcomm proprietary libraries and firmware;
- Google applications/APEX;
- MIUI Camera payloads;
- Recovery or `init_boot` images;
- OTA or target-files packages;
- signing keys and credentials.

Their absence is intentional. SourceForge distribution must follow the
original terms and must not imply that this repository grants a redistribution
right.

## Private GitHub archive

The repository is private. Keep it private unless the owner explicitly
authorizes publication. A private repository does not remove copyright,
license or third-party distribution obligations; it only limits access.

## Signing

Build 305 used test keys. Test keys are not suitable for a public release
with meaningful update security. A future official or public build requires
the appropriate signing process and must not commit private keys.

## Attribution

See:

- `NOTICE`
- `LICENSE.md`
- `licenses/upstream/`
- `provenance/`
- `manifests/platform-pinned.xml`

This documentation is an engineering inventory, not legal advice. Obtain
legal review if a distribution decision depends on a specific component's
terms.
