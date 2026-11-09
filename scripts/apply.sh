#!/usr/bin/env bash
set -euo pipefail

ARCHIVE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)"
PLATFORM_MANIFEST="$ARCHIVE_ROOT/manifests/platform-pinned.xml"
SYSTEM_LOCK="$ARCHIVE_ROOT/manifests/ishtar-system-lock.xml"

usage() {
    cat <<'EOF'
Usage:
  scripts/apply.sh SOURCE_ROOT
  scripts/apply.sh --apply SOURCE_ROOT

Without --apply, validate the source checkout and every maintained patch
without modifying it. With --apply, copy the maintained trees and apply
forward-applicable platform patches. Conflicts always abort before writes.
EOF
}

die() {
    printf 'error: %s\n' "$*" >&2
    exit 1
}

APPLY=0
if [[ "${1:-}" == "--apply" ]]; then
    APPLY=1
    shift
elif [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
    usage
    exit 0
fi

[[ $# -eq 1 ]] || {
    usage >&2
    exit 2
}

SOURCE_ROOT="$1"
[[ -d "$SOURCE_ROOT" ]] || die "source root does not exist: $SOURCE_ROOT"
SOURCE_ROOT="$(cd "$SOURCE_ROOT" && pwd -P)"

[[ "$SOURCE_ROOT" != "$ARCHIVE_ROOT" ]] ||
    die "source root must be a separate Android checkout"
[[ -d "$SOURCE_ROOT/.repo" ]] ||
    die "source root is not a repo checkout (missing .repo): $SOURCE_ROOT"
[[ -d "$SOURCE_ROOT/build/soong" ]] ||
    die "source root has no build/soong project: $SOURCE_ROOT"
[[ -d "$SOURCE_ROOT/frameworks/base" ]] ||
    die "source root has no frameworks/base project: $SOURCE_ROOT"

for tool in git python3 rsync; do
    command -v "$tool" >/dev/null 2>&1 || die "required tool not found: $tool"
done

[[ -f "$PLATFORM_MANIFEST" ]] || die "missing manifest: $PLATFORM_MANIFEST"
[[ -f "$SYSTEM_LOCK" ]] || die "missing manifest: $SYSTEM_LOCK"

python3 - "$PLATFORM_MANIFEST" "$SYSTEM_LOCK" <<'PY'
import re
import sys
import xml.etree.ElementTree as ET

full_sha = re.compile(r"^[0-9a-f]{40}$")

def validate(path, kinds):
    root = ET.parse(path).getroot()
    if root.tag != "manifest":
        raise SystemExit(f"{path}: root element is not <manifest>")
    records = []
    for element in root:
        if element.tag in kinds:
            name = element.get("name") or "<unnamed>"
            revision = element.get("revision") or ""
            if not full_sha.fullmatch(revision):
                raise SystemExit(
                    f"{path}: {element.tag} {name} has non-full revision {revision!r}"
                )
            records.append(element)
    if not records:
        raise SystemExit(f"{path}: no project records")
    print(f"{path}: {len(records)} full-SHA project records")

validate(sys.argv[1], {"project"})
validate(sys.argv[2], {"extend-project"})
PY

if find "$ARCHIVE_ROOT/vendor/xiaomi" -type d -name proprietary -print -quit |
    grep -q .; then
    die "archive unexpectedly contains vendor/xiaomi proprietary directories"
fi

resolve_project() {
    local patch_name="$1"
    python3 - "$PLATFORM_MANIFEST" "$patch_name" <<'PY'
import sys
import xml.etree.ElementTree as ET

manifest_path, patch_name = sys.argv[1:]
root = ET.parse(manifest_path).getroot()
matches = []
for project in root.findall("project"):
    path = project.get("path") or project.get("name") or ""
    if path.replace("/", "__") + ".patch" == patch_name:
        matches.append((path, project.get("revision") or ""))

if len(matches) != 1:
    payload = ", ".join(f"{p}@{r}" for p, r in matches) or "none"
    raise SystemExit(
        f"{patch_name}: expected exactly one manifest project, found {payload}"
    )

print(*matches[0], sep="\t")
PY
}

patch_files=()
project_paths=()
project_revisions=()
patch_states=()

printf 'Preflight source: %s\n' "$SOURCE_ROOT"
for patch in "$ARCHIVE_ROOT"/patches/platform/*.patch; do
    [[ -f "$patch" ]] || die "no platform patches found"
    patch_name="$(basename "$patch")"
    mapping="$(resolve_project "$patch_name")" ||
        die "cannot resolve manifest project for $patch_name"
    IFS=$'\t' read -r project_path manifest_revision <<<"$mapping"
    [[ -n "$project_path" && -n "$manifest_revision" ]] ||
        die "empty manifest mapping for $patch_name"

    project_dir="$SOURCE_ROOT/$project_path"
    [[ -d "$project_dir" ]] || die "missing project directory: $project_path"
    git -C "$project_dir" rev-parse --is-inside-work-tree >/dev/null 2>&1 ||
        die "not a Git worktree: $project_path"

    actual_revision="$(git -C "$project_dir" rev-parse HEAD)"
    [[ "$actual_revision" == "$manifest_revision" ]] ||
        die "revision mismatch for $project_path (expected $manifest_revision, found $actual_revision)"

    if git -C "$project_dir" apply --check "$patch" >/dev/null 2>&1; then
        state="forward-applicable"
    elif git -C "$project_dir" apply --check --reverse "$patch" >/dev/null 2>&1; then
        state="already-applied"
    else
        state="conflict"
    fi

    printf '  %-58s %-18s %s\n' "$patch_name" "$state" "$project_path"
    patch_files+=("$patch")
    project_paths+=("$project_path")
    project_revisions+=("$manifest_revision")
    patch_states+=("$state")
done

conflicts=0
for state in "${patch_states[@]}"; do
    [[ "$state" != "conflict" ]] || conflicts=$((conflicts + 1))
done

if (( conflicts > 0 )); then
    die "$conflicts platform patch conflict(s); no source files were modified"
fi

if (( APPLY == 0 )); then
    printf '\nPreflight passed. No files were modified.\n'
    printf 'Run with --apply to integrate the maintained source.\n'
    exit 0
fi

printf '\nApplying platform patches...\n'
for index in "${!patch_files[@]}"; do
    state="${patch_states[$index]}"
    if [[ "$state" == "already-applied" ]]; then
        printf '  skip %s (already applied)\n' "$(basename "${patch_files[$index]}")"
        continue
    fi
    project_dir="$SOURCE_ROOT/${project_paths[$index]}"
    git -C "$project_dir" apply "${patch_files[$index]}"
    printf '  applied %s\n' "$(basename "${patch_files[$index]}")"
done

copy_tree() {
    local relative="$1"
    printf '  sync %s\n' "$relative"
    mkdir -p "$SOURCE_ROOT/$relative"
    rsync -a \
        --exclude='.git/' \
        --exclude='__pycache__/' \
        --exclude='*.pyc' \
        "$ARCHIVE_ROOT/$relative/" "$SOURCE_ROOT/$relative/"
}

printf '\nSynchronizing maintained source trees...\n'
copy_tree "device/xiaomi/ishtar"
copy_tree "device/xiaomi/sm8550-common"
copy_tree "hardware/xiaomi"
copy_tree "kernel/xiaomi/sm8550-devicetrees"
copy_tree "vendor/xiaomi"

cat <<'EOF'

Integration completed.

Still required:
  1. Apply kernel/patches/linux-5.15.224-ishtar.patch to the matching
     kernel/xiaomi/sm8550 tree.
  2. Apply kernel/patches/sm8550-modules.patch to the matching
     kernel/xiaomi/sm8550-modules tree.
  3. Restore the external proprietary, GMS, firmware and init_boot inputs.
  4. Review git diff in every modified project before building.

See docs/KERNEL.md and docs/EXTERNAL_INPUTS.md.
EOF
