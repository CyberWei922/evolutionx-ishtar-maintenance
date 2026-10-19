#!/bin/bash
# Copyright (C) 2026 CyberWei922
# SPDX-License-Identifier: Apache-2.0
set -o pipefail
ROOT=/mnt/android-download/git-cache/evolution
CONTROL=/mnt/android-download/control/ishtar-selinux-enforcing-boot-20261019-01
PREFIX=enforcing-vendor-boot-build-01
export OUT_STORAGE=/mnt/android-build/Android/out/evolution-bka OUT_DIR=out
export TMPDIR="$OUT_STORAGE/tmp" DIST_DIR="$OUT_STORAGE/dist"
export ROOMSERVICE_DRYRUN=true EVO_BUILD_TYPE=Unofficial WITH_GMS=true USE_CCACHE=0
export BUILD_DATETIME=1792357362 GOMAXPROCS=4 NINJA_HIGHMEM_NUM_JOBS=1
export ENVSETUP_NO_COMPLETION=adb:fastboot:asuite
export GIT_TERMINAL_PROMPT=0 GIT_CONFIG_NOSYSTEM=1 GIT_CONFIG_GLOBAL=/dev/null
export CUDA_VISIBLE_DEVICES= HIP_VISIBLE_DEVICES= ROCR_VISIBLE_DEVICES=
[[ ! -e "$CONTROL/$PREFIX.started" ]] || exit 2
exec >"$CONTROL/$PREFIX.log" 2>&1
date --iso-8601=seconds >"$CONTROL/$PREFIX.started"
finish() { code=$?; printf '%s\n' "$code" >"$CONTROL/$PREFIX.exit"; date --iso-8601=seconds >"$CONTROL/$PREFIX.finished"; }
trap finish EXIT
[[ -z ${NINJA_ARGS:-} && -z ${NINJA_EXTRA_ARGS:-} && $EUID != 0 ]] || exit 2
unset NINJA_EXTRA_ARGS
exec 9>/mnt/android-download/control/ishtar-build.lock
flock -n 9 || exit 1
[[ $(findmnt -n -o UUID -T "$ROOT") == c7d419fe-06d9-4508-9762-17b207e8be35 ]] || exit 1
[[ $(findmnt -n -o UUID -T "$OUT_STORAGE") == 8769c13b-ae01-408c-a755-2647391b2f28 ]] || exit 1
[[ -L "$ROOT/out" && $(readlink "$ROOT/out") == "$OUT_STORAGE" ]] || exit 1
python3 - "$CONTROL" "$ROOT" "$OUT_STORAGE" <<'PY'
from pathlib import Path
import sys,hashlib,json,shutil
c,r,o=map(Path,sys.argv[1:]);assert shutil.disk_usage(o).free>=40*2**30
expected=json.loads((c/'expected-source-files.json').read_text());assert len(expected)==104
for n,v in expected.items():assert hashlib.sha256((r/n).read_bytes()).hexdigest()==v['sha256'],n
assert (o/'build_date.txt').read_text().strip()=='1792357362'
(c/'enforcing-vendor-boot-build-01-start-proof.json').open('x').write(json.dumps({'source_files_verified':104,'target':'vendorbootimage','build_datetime_pin':'1792357362','jobs':4,'highmem_jobs':1,'phone_accessed':False,'system_or_kernel_rebuild_requested':False},indent=2)+'\n')
PY
[[ $? == 0 ]] || exit 1
cd "$ROOT" || exit 1
source build/envsetup.sh || exit 1
adb() { echo 'Phone commands disabled in boot image build shell.' >&2; return 125; }
fastboot() { echo 'Phone commands disabled in boot image build shell.' >&2; return 125; }
export -f adb fastboot
check_product lineage_ishtar bp4a || exit 1
lunch lineage_ishtar-bp4a-userdebug || exit 1
m -j4 vendorbootimage || exit 1
