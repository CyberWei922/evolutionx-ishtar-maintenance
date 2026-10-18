#!/system/bin/sh
#
# Backup and restore addon /system files
#
# Modified 2026 CyberWei: retain the first received failure while completing
# addon stages and cleanup. Do not use set -e: cleanup must still run.
#

export S=/system
export C=/postinstall/tmp/backupdir
export V=16.0
export ADDOND_VERSION=3
export all_V3_partitions="vendor product system_ext"

result=0
workspace_ready=0
record_failure() {
  local status="$1"
  shift
  if [ "$status" -ne 0 ]; then
    [ "$result" -ne 0 ] || result="$status"
    echo "backuptool: $* failed (status $status)" >&2
  fi
  return 0
}

run_checked() {
  "$@"
  local status="$?"
  record_failure "$status" "$@"
  return "$status"
}

get_script_version() {
  local version
  version=$(grep "^# ADDOND_VERSION=" "$1" | cut -d= -f2)
  [ -n "$version" ] || version=1
  case "$version" in
    *[!0-9]*) return 1 ;;
  esac
  echo "$version"
}

preserve_addon_d() {
  if [ -d /system/addon.d/ ]; then
    # Publish a script list only after the full copy succeeds. cp -a can copy
    # some files and then fail (for example while preserving metadata).
    local pending=/postinstall/tmp/addon.d.pending
    run_checked rm -rf "$pending" /postinstall/tmp/addon.d/ || return "$?"
    run_checked mkdir -p "$pending" || return "$?"
    if run_checked cp -a /system/addon.d/. "$pending/"; then
      :
    else
      local status="$?"
      run_checked rm -rf "$pending"
      return "$status"
    fi
    # Version 1 scripts are not compatible with A/B. Empty lists are normal.
    for f in "$pending"/*.sh; do
      [ -f "$f" ] || continue
      local version
      if version=$(get_script_version "$f"); then
        if [ "$version" = 1 ]; then
          run_checked rm "$f"
        else
          run_checked chmod 755 "$f"
        fi
      else
        record_failure 1 "version metadata for $f"
      fi
    done
    run_checked mv "$pending" /postinstall/tmp/addon.d/ || return "$?"
  fi
  return 0
}

restore_addon_d() {
  if [ -d /postinstall/tmp/addon.d/ ]; then
    run_checked mkdir -p /postinstall/system/addon.d/ || return "$?"
    run_checked cp -a /postinstall/tmp/addon.d/. /postinstall/system/addon.d/ || return "$?"
    run_checked rm -rf /postinstall/tmp/addon.d/
  fi
}

check_prereq() {
  if [ ! -r /product/etc/build.prop ] && [ ! -r /system/build.prop ]; then
    echo "Backup/restore is not possible. Partition is probably empty" >&2
    return 1
  fi
  if ! grep -q "^ro.evolution.version=$V.*" /product/etc/build.prop /system/build.prop; then
    echo "Backup/restore is not possible. Incompatible ROM version: $V" >&2
    return 2
  fi
  return 0
}

run_stages() {
  [ -d /postinstall/tmp/addon.d/ ] || return 0
  # A redirected loop retains failures in this shell and handles spaces in names.
  run_checked find /postinstall/tmp/addon.d/ -name '*.sh' > /postinstall/tmp/addon-script-list || return "$?"
  run_checked sort -n /postinstall/tmp/addon-script-list -o /postinstall/tmp/addon-script-list || return "$?"
  local script version stage
  while IFS= read -r script; do
    run_checked sed -i '0,/#!\/sbin\/sh/{s|#!/sbin/sh|#!/system/bin/sh|}' "$script" || continue
    run_checked sed -i 's|. /tmp/backuptool.functions|. /postinstall/tmp/backuptool.functions|g' "$script" || continue
    if ! version=$(get_script_version "$script"); then
      record_failure 1 "version metadata for $script"
      continue
    fi
    if [ "$version" -ge 3 ]; then
      # Do not run a V3 writer against an unavailable target mount.
      mount_extra $all_V3_partitions || continue
    else
      umount_extra $all_V3_partitions || continue
    fi
    for stage in "$@"; do
      if [ "$version" -ge 3 ]; then
        run_checked "$script" "$stage"
      else
        ADDOND_VERSION=2 run_checked "$script" "$stage"
      fi
    done
  done < /postinstall/tmp/addon-script-list
  return 0
}

get_block_for_mount_point() {
  grep -v "^#" /vendor/etc/fstab.$(getprop ro.boot.hardware) | grep "[[:blank:]]$1[[:blank:]]" | tail -n1 | tr -s '[:blank:]' ' ' | cut -d' ' -f1
}

find_block() {
  local name="$1"
  local fstab_entry=$(get_block_for_mount_point "/$name")
  [ -z "$fstab_entry" ] && [ "$name" = "system" ] && fstab_entry=$(get_block_for_mount_point "/")
  [ -z "$fstab_entry" ] && [ "$name" = "system" ] && fstab_entry=$(get_block_for_mount_point "/system_root")
  local dev
  if [ "$DYNAMIC_PARTITIONS" = "true" ]; then
    dev="${BLK_PATH}/${fstab_entry:-$name}${SLOT_SUFFIX}"
  else
    dev="${fstab_entry:-${BLK_PATH}/${name}}${SLOT_SUFFIX}"
  fi
  [ -b "$dev" ] && echo "$dev"
}

mount_extra() {
  local partition mnt_point blk_dev failed=0
  for partition in "$@"; do
    mnt_point="/postinstall/$partition"
    [ -L "$mnt_point" ] && continue
    mountpoint "$mnt_point" >/dev/null 2>&1 && continue
    blk_dev=$(find_block "$partition")
    if [ -z "$blk_dev" ]; then
      record_failure 1 "inactive block device for $partition"
      failed=1
      continue
    fi
    if [ "$DYNAMIC_PARTITIONS" = true ]; then
      if ! run_checked blockdev --setrw "$blk_dev"; then
        failed=1
        continue
      fi
    fi
    run_checked mount -o rw "$blk_dev" "$mnt_point" || failed=1
  done
  return "$failed"
}

umount_extra() {
  local partition mnt_point failed=0
  for partition in "$@"; do
    mnt_point="/postinstall/$partition"
    # Never follow a target symlink to the active slot's mountpoint.
    [ -L "$mnt_point" ] && continue
    mountpoint "$mnt_point" >/dev/null 2>&1 || continue
    run_checked umount -l "$mnt_point" || failed=1
  done
  return "$failed"
}

cleanup() {
  umount_extra $all_V3_partitions
  if [ "$BACKUPTOOL_BROKER_WORKSPACE" != 1 ]; then
    # Transitional ABI for an older updater running the new top-level script.
    # A failed unmount must never be followed by deleting a mounted tree.
    if mountpoint /postinstall/tmp >/dev/null 2>&1; then
      run_checked umount /postinstall/tmp || return "$?"
    fi
  fi
  run_checked rm -rf /postinstall/tmp
}

case "$1" in backup|restore) ;; *) echo "Usage: $0 {backup|restore}" >&2; exit 1 ;; esac

DYNAMIC_PARTITIONS=$(getprop ro.boot.dynamic_partitions)
if [ "$DYNAMIC_PARTITIONS" = true ]; then
  BLK_PATH=/dev/block/mapper
else
  BLK_PATH=/dev/block/bootdevice/by-name
fi
CURRENTSLOT=$(getprop ro.boot.slot_suffix)
case "$CURRENTSLOT" in
  _a) SLOT_SUFFIX=_b ;;
  _b) SLOT_SUFFIX=_a ;;
  *) record_failure 1 "valid active slot"; exit "$result" ;;
esac

# Scripts expect a temporary copy of the helper functions. Stop before addon
# execution if that workspace cannot be prepared, then still attempt cleanup.
setup_workspace() {
  if [ "$BACKUPTOOL_BROKER_WORKSPACE" = 1 ]; then
    if [ ! -d /postinstall/tmp ] || [ -L /postinstall/tmp ]; then
      record_failure 1 "broker-owned workspace required"
      return 1
    fi
  elif [ "$BACKUPTOOL_LEGACY_WORKSPACE" = 1 ]; then
    # Only the new top-level wrapper selects compatibility for older callers.
    # Require an existing postinstall mount before creating any legacy path.
    if [ -L /postinstall/tmp ] || ! mountpoint /postinstall >/dev/null 2>&1; then
      record_failure 1 "legacy postinstall mount required"
      return 1
    fi
    run_checked mkdir -p /postinstall/tmp || return "$?"
    if ! mountpoint /postinstall/tmp >/dev/null 2>&1; then
      run_checked mount -t tmpfs tmpfs /postinstall/tmp || return "$?"
    fi
  else
    record_failure 1 "broker or top-level legacy workspace required"
    return 1
  fi
  workspace_ready=1
  run_checked cp -f /postinstall/system/bin/backuptool_ab.functions /postinstall/tmp/backuptool.functions
}

if ! setup_workspace; then
  [ "$workspace_ready" -ne 1 ] || cleanup
  exit "$result"
fi
if ! run_checked check_prereq; then
  cleanup
  exit "$result"
fi

case "$1" in
  backup)
    if run_checked mkdir -p "$C"; then
      if preserve_addon_d; then
        run_stages pre-backup backup post-backup
      fi
    fi
    umount_extra $all_V3_partitions
    ;;
  restore)
    run_stages pre-restore restore post-restore
    restore_addon_d
    cleanup
    run_checked sync
    ;;
esac
exit "$result"
