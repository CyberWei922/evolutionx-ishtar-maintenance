#!/sbin/sh
#
# ADDOND_VERSION=2
#
# /system/addon.d/50-lineage.sh
# During a LineageOS upgrade, this script backs up /system/etc/hosts,
# /system is formatted and reinstalled, then the file is restored.
#

. /tmp/backuptool.functions

list_files() {
cat <<EOF
etc/hosts
EOF
}

case "$1" in
  backup)
    list_files | (
      result=0
      while read -r FILE DUMMY; do
        backup_file "$S/$FILE"
        status="$?"
        [ "$status" -eq 0 ] || { [ "$result" -ne 0 ] || result="$status"; }
      done
      exit "$result"
    )
  ;;
  restore)
    list_files | (
      result=0
      while read -r FILE REPLACEMENT; do
        R=""
        [ -z "$REPLACEMENT" ] || R="$S/$REPLACEMENT"
        if [ -f "$C/$S/$FILE" ]; then
          restore_file "$S/$FILE" "$R"
          status="$?"
          [ "$status" -eq 0 ] || { [ "$result" -ne 0 ] || result="$status"; }
        fi
      done
      exit "$result"
    )
  ;;
  pre-backup)
    # Stub
  ;;
  post-backup)
    # Stub
  ;;
  pre-restore)
    # Stub
  ;;
  post-restore)
    # Stub
  ;;
esac
