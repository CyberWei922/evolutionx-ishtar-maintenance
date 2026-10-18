#!/system/bin/sh
#
# LineageOS A/B OTA Postinstall Script
#

# Preserve the established backup -> restore -> sync order. Propagate the
# first failure instead of unconditionally returning success.

# Older installed update_engine versions directly execute this new script and
# cannot provide a broker workspace. Keep that upgrade ABI until those callers
# have migrated; new broker callers never use the legacy shell mount path.
if [ "$BACKUPTOOL_BROKER_WORKSPACE" = 1 ]; then
  unset BACKUPTOOL_LEGACY_WORKSPACE
else
  export BACKUPTOOL_LEGACY_WORKSPACE=1
fi
/postinstall/system/bin/backuptool_ab.sh backup
backup_status=$?
/postinstall/system/bin/backuptool_ab.sh restore
restore_status=$?

sync
sync_status=$?

[ "$backup_status" -eq 0 ] || exit "$backup_status"
[ "$restore_status" -eq 0 ] || exit "$restore_status"
exit "$sync_status"
