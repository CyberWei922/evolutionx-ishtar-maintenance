#!/system/bin/sh
# Copyright (C) 2026 CyberWei922
# SPDX-License-Identifier: Apache-2.0
# This guard never enables Enforcing. Non-dry use requires separate owner approval.
set -u
MODE=${1-}
LIMIT=${2-}
TOKEN=${3-}
case "$MODE" in dry-run|restore-permissive) ;; *) exit 64 ;; esac
case "$LIMIT" in ''|*[!0-9]*) exit 64 ;; esac
[ "$LIMIT" -ge 10 ] && [ "$LIMIT" -le 120 ] || exit 64
case "$TOKEN" in ''|*[!a-f0-9]*) exit 64 ;; esac
[ "${#TOKEN}" -eq 32 ] || exit 64
[ "$(/system/bin/id -u)" = 0 ] || exit 65
[ "$(/system/bin/id -Z)" = u:r:ksu:s0 ] || exit 65
[ "$(/system/bin/getenforce)" = Permissive ] || exit 65
umask 077
BASE=${0%/*}
RUN=$BASE/$TOKEN
/system/bin/mkdir "$RUN" || exit 66
read UP REST < /proc/uptime
START=${UP%%.*}
DEADLINE=$((START + LIMIT))
printf '%s\n' "$$" > "$RUN/pid"
/system/bin/id -Z > "$RUN/context"
/system/bin/cat /proc/$$/status > "$RUN/process-status"
/system/bin/cat /proc/sys/kernel/random/boot_id > "$RUN/boot-id"
printf '%s\n' "$MODE" > "$RUN/mode"
printf '%s\n' "$START $DEADLINE" > "$RUN/window"
state() {
    printf '%s\n' "$1" > "$RUN/status.tmp"
    /system/bin/mv "$RUN/status.tmp" "$RUN/status"
}
finish() {
    trap '' HUP INT TERM
    if [ "$MODE" = dry-run ]; then
        /system/bin/getenforce > "$RUN/final-mode"
        state DRY_RUN_DEADLINE_REACHED
        exit 0
    fi
    state RESTORE_REQUESTED
    ATTEMPT=0
    while [ "$ATTEMPT" -lt 3 ]; do
        ATTEMPT=$((ATTEMPT + 1))
        /system/bin/setenforce 0 >> "$RUN/restore.log" 2>&1
        if [ "$(/system/bin/getenforce)" = Permissive ] \
                && [ "$(/system/bin/cat /sys/fs/selinux/enforce)" = 0 ]; then
            printf '%s\n' Permissive > "$RUN/final-mode"
            state RESTORED_PERMISSIVE
            exit 0
        fi
        /system/bin/sleep 1
    done
    state RESTORE_FAILED
    exit 67
}
trap 'finish' HUP INT TERM
state READY
while :; do
    read UP REST < /proc/uptime
    NOW=${UP%%.*}
    printf '%s\n' "$NOW" > "$RUN/heartbeat"
    [ "$NOW" -lt "$DEADLINE" ] || break
    [ ! -e "$RUN/restore-now" ] || break
    /system/bin/sleep 1
done
finish
