#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)"
cd "$ROOT"

failures=0
fail() {
    printf 'FAIL: %s\n' "$*" >&2
    failures=$((failures + 1))
}

required_files=(
    README.md
    README.zh-CN.md
    AGENTS.md
    LICENSE.md
    NOTICE
    .gitignore
    manifests/platform-pinned.xml
    manifests/ishtar-system-lock.xml
    patches/platform/bionic.patch
    patches/platform/build__soong.patch
    patches/platform/device__lineage__sepolicy.patch
    patches/platform/device__qcom__sepolicy.patch
    patches/platform/device__qcom__sepolicy_vndr__sm8550.patch
    patches/platform/frameworks__base.patch
    patches/platform/frameworks__native.patch
    patches/platform/frameworks__opt__telephony.patch
    patches/platform/hardware__qcom-caf__thermal.patch
    patches/platform/packages__apps__GameSpace.patch
    patches/platform/system__netd.patch
    patches/platform/system__sepolicy.patch
    patches/platform/system__update_engine.patch
    patches/platform/vendor__gms.patch
    patches/platform/vendor__lineage.patch
    patches/platform/vendor__qcom__opensource__vibrator.patch
    patches/platform/vendor__unbundled_google__packages__SystemUIGoogle.patch
    kernel/patches/linux-5.15.224-ishtar.patch
    kernel/patches/sm8550-modules.patch
    docs/BUILD.md
    docs/SOURCE_LAYOUT.md
    docs/ARCHITECTURE.md
    docs/PLATFORM_PATCHES.md
    docs/KERNEL.md
    docs/EXTERNAL_INPUTS.md
    docs/VERIFICATION.md
    docs/MAINTENANCE.md
    docs/LICENSE_AND_DISTRIBUTION.md
    provenance/README.md
    releases/305.json
    licenses/Apache-2.0.txt
    licenses/GPL-2.0-only.txt
    licenses/upstream/frameworks/base/NOTICE
    licenses/upstream/frameworks/native/NOTICE
    licenses/upstream/system/sepolicy/NOTICE
    licenses/upstream/system/update_engine/NOTICE
    licenses/upstream/vendor/lineage/LICENSE
)

for path in "${required_files[@]}"; do
    [[ -f "$path" ]] || fail "missing required file: $path"
done

if find . -type l -print -quit | grep -q .; then
    find . -type l -print >&2
    fail "symlinks are not allowed"
fi

if find . -mindepth 1 -name .git -print -quit | grep -q .; then
    find . -mindepth 1 -name .git -print >&2
    fail "nested .git entries are not allowed"
fi

if [[ -e kernel/xiaomi/sm8550 ]]; then
    fail "complete kernel source tree must remain external"
fi
if [[ -e kernel/xiaomi/sm8550-modules ]]; then
    fail "complete module source tree must remain external"
fi
if [[ -e device/xiaomi/ishtar/root ]]; then
    fail "device/xiaomi/ishtar/root external-image directory must remain absent"
fi

python3 - "$ROOT" <<'PY'
import json
import os
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

root = Path(sys.argv[1])
full_sha = re.compile(r"^[0-9a-f]{40}$")
forbidden_suffixes = {
    ".img", ".apk", ".jar", ".zip", ".so", ".ko", ".o", ".a", ".elf",
    ".dtbo", ".dex", ".odex", ".vdex", ".oat", ".art", ".bin",
}
secret_patterns = [
    (re.compile(rb"-----BEGIN (?:RSA |EC |DSA |OPENSSH )?PRIVATE KEY-----"),
     "private-key marker"),
    (re.compile(rb"gh[pousr]_[A-Za-z0-9]{30,}"), "GitHub token"),
    (re.compile(rb"github_pat_[A-Za-z0-9_]{30,}"), "GitHub fine-grained token"),
    (re.compile(rb"AKIA[0-9A-Z]{16}"), "AWS access key"),
    (re.compile(rb"xox[baprs]-[A-Za-z0-9-]{10,}"), "Slack token"),
    (re.compile(rb"1e8459c[0-9]"), "known ADB serial"),
]

def validate_manifest(path, kinds):
    tree = ET.parse(path)
    manifest = tree.getroot()
    if manifest.tag != "manifest":
        raise ValueError("root is not <manifest>")
    records = [e for e in manifest if e.tag in kinds]
    if not records:
        raise ValueError("no project records")
    for element in records:
        revision = element.get("revision") or ""
        if not full_sha.fullmatch(revision):
            raise ValueError(
                f"{element.tag} {element.get('name')} has non-40-character revision"
            )
    return len(records)

try:
    platform_count = validate_manifest(root / "manifests/platform-pinned.xml", {"project"})
    lock_count = validate_manifest(
        root / "manifests/ishtar-system-lock.xml", {"extend-project"}
    )
except (ET.ParseError, ValueError) as error:
    print(f"FAIL: manifest validation: {error}", file=sys.stderr)
    sys.exit(1)

files = []
total_bytes = 0
max_file = (0, None)
problems = []

for current, dirnames, filenames in os.walk(root):
    dirnames[:] = [name for name in dirnames if name != ".git"]
    for filename in filenames:
        path = Path(current) / filename
        relative = path.relative_to(root)
        if path.is_symlink():
            continue
        try:
            size = path.stat().st_size
        except OSError as error:
            problems.append(f"cannot stat {relative}: {error}")
            continue
        files.append(relative)
        total_bytes += size
        if size > max_file[0]:
            max_file = (size, relative)
        if size > 20 * 1024 * 1024:
            problems.append(f"file exceeds 20 MiB: {relative} ({size} bytes)")
        if path.suffix.lower() in forbidden_suffixes:
            problems.append(f"forbidden binary suffix: {relative}")

        try:
            with path.open("rb") as stream:
                head = stream.read(65536)
                if b"\x00" in head:
                    problems.append(f"NUL-containing binary data: {relative}")
                stream.seek(0)
                sample = stream.read()
        except OSError as error:
            problems.append(f"cannot read {relative}: {error}")
            continue

        for pattern, label in secret_patterns:
            match = pattern.search(sample)
            if match:
                problems.append(f"{label} pattern in {relative}")

for path in sorted(root.glob("**/*.json")):
    try:
        json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        problems.append(f"invalid JSON {path.relative_to(root)}: {error}")

if problems:
    for problem in problems:
        print(f"FAIL: {problem}", file=sys.stderr)
    sys.exit(1)

print(
    f"OK: {len(files)} files, {total_bytes} bytes; "
    f"manifests {platform_count} project / {lock_count} extend-project"
)
print(f"OK: largest file {max_file[0]} bytes ({max_file[1]})")
PY
if (( $? != 0 )); then
    failures=$((failures + 1))
fi

if [[ -f SHA256SUMS ]]; then
    if command -v shasum >/dev/null 2>&1; then
        if ! shasum -a 256 -c SHA256SUMS >/dev/null; then
            fail "SHA256SUMS verification failed"
        fi
    elif command -v sha256sum >/dev/null 2>&1; then
        if ! sha256sum -c SHA256SUMS >/dev/null; then
            fail "SHA256SUMS verification failed"
        fi
    else
        fail "no SHA256 checker found"
    fi
    printf 'OK: SHA256SUMS verified\n'
else
    printf 'NOTE: SHA256SUMS not present yet\n'
fi

if (( failures > 0 )); then
    printf 'Archive verification failed with %d issue(s).\n' "$failures" >&2
    exit 1
fi

printf 'Archive verification passed.\n'
