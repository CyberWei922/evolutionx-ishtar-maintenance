#!/system/bin/sh
# Copyright (C) 2026 CyberWei
# SPDX-License-Identifier: Apache-2.0
# Protocol 2: only tmpfs staging; the native broker owns all mounts and commits.
[ "$BACKUPTOOL_BROKER_PROTOCOL" = 2 ] || exit 2
export S=/system C=/postinstall/tmp/backupdir V=16.0 ADDOND_VERSION=3
export all_V3_partitions="vendor product system_ext" backuptool_ab=true
result=0
checked() {
  "$@"
  local rc="$?"
  if [ "$rc" -ne 0 ]; then
    [ "$result" -ne 0 ] || result="$rc"
    echo "backuptool staged: $* failed ($rc)" >&2
  fi
  return "$rc"
}
version() {
  local value
  value=$(grep '^# ADDOND_VERSION=' "$1" | cut -d= -f2)
  [ -n "$value" ] || value=1
  case "$value" in 1|2|3) echo "$value" ;; *) return 1 ;; esac
}
# No direct target write, mount, blockdev, relabel, or security-context copy.
checked mkdir -p "$C" /postinstall/tmp/backupmeta /postinstall/tmp/restore /postinstall/tmp/restoremeta || exit "$result"
checked cp -p /system/bin/backuptool_ab.functions /postinstall/tmp/backuptool.functions || exit "$result"
if ! grep -q '^ro.evolution.version=16\.0' /product/etc/build.prop /system/build.prop; then
  echo 'backuptool staged: incompatible source ROM' >&2
  exit 2
fi
if [ ! -d /system/addon.d ]; then exit 0; fi
checked mkdir -p /postinstall/tmp/addon.d || exit "$result"
# Only preserve mode/ownership/timestamps and non-security xattrs for scripts.
# Data file security metadata is encoded by backuptool_store and committed offline.
checked cp -RP --preserve=motx /system/addon.d/. /postinstall/tmp/addon.d/ || exit "$result"
checked find /postinstall/tmp/addon.d -name '*.sh' > /postinstall/tmp/addon-list || exit "$result"
checked sort -n /postinstall/tmp/addon-list -o /postinstall/tmp/addon-list || exit "$result"
while IFS= read -r script; do
  if [ -L "$script" ] || [ ! -f "$script" ]; then
    echo "backuptool staged: addon entry is not a regular script: $script" >&2
    exit 2
  fi
  n=$(version "$script") || exit 2
  if [ "$n" = 1 ]; then checked rm "$script" || exit "$result"; continue; fi
  checked chmod 755 "$script" || exit "$result"
  checked sed -i '0,/#!\/sbin\/sh/{s|#!/sbin/sh|#!/system/bin/sh|}' "$script" || exit "$result"
  checked sed -i 's|. /tmp/backuptool.functions|. /postinstall/tmp/backuptool.functions|g' "$script" || exit "$result"
done < /postinstall/tmp/addon-list
run_stages() {
  local script n stage
  while IFS= read -r script; do
    [ -f "$script" ] || continue
    n=$(version "$script") || { [ "$result" -ne 0 ] || result=2; continue; }
    for stage in "$@"; do
      if [ "$n" = 2 ]; then ADDOND_VERSION=2 checked /system/bin/sh "$script" "$stage"
      else ADDOND_VERSION=3 checked /system/bin/sh "$script" "$stage"; fi
    done
  done < /postinstall/tmp/addon-list
}
run_stages pre-backup backup post-backup
# A failed backup must not publish a partial restore or start an offline commit.
[ "$result" -eq 0 ] || exit "$result"
[ ! -s /postinstall/tmp/store-failures ] || exit 2
run_stages pre-restore restore post-restore
[ "$result" -eq 0 ] || exit "$result"
[ ! -s /postinstall/tmp/store-failures ] || exit 2
checked mkdir -p /postinstall/tmp/restore/system/addon.d || exit "$result"
checked cp -RP --preserve=motx /postinstall/tmp/addon.d/. /postinstall/tmp/restore/system/addon.d/ || exit "$result"
checked sync
exit "$result"
