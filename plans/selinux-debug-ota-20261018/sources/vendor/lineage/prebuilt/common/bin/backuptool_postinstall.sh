#!/system/bin/sh
#
# LineageOS A/B OTA Postinstall Script
#

# Preserve the established backup -> restore -> sync order. Propagate the
# first failure instead of unconditionally returning success.
/postinstall/system/bin/backuptool_ab.sh backup
backup_status=$?
/postinstall/system/bin/backuptool_ab.sh restore
restore_status=$?

sync
sync_status=$?

[ "$backup_status" -eq 0 ] || exit "$backup_status"
[ "$restore_status" -eq 0 ] || exit "$restore_status"
exit "$sync_status"
