from pathlib import Path
import hashlib,json,os,shutil,subprocess,re
C=Path('/mnt/android-download/control/ishtar-selinux-backuptool-final-20261019-01');D=C/'source-candidate08/script-tests01';D.mkdir(exist_ok=False);A=C/'source-candidate08/after';R=Path('/mnt/android-download/git-cache/evolution');B=C/'source-candidate08/host-compile01/store'
script=(A/'system/update_engine/backuptool_broker/backuptool_staged.sh').read_text();functions=(A/'vendor/lineage/prebuilt/common/bin/backuptool_ab.functions').read_text();stock=(R/'vendor/lineage/prebuilt/common/bin/50-lineage.sh').read_text()
cp='''#!/usr/bin/python3
import os,subprocess,sys
args=[x.replace('--preserve=motx','--preserve=mode,ownership,timestamps,xattr').replace('--preserve=a','--preserve=all') for x in sys.argv[1:]]
p=subprocess.run(['/usr/bin/cp',*args])
if os.environ.get('FAIL_ADDON_COPY')=='1' and any('/system/addon.d/.' in x for x in args):sys.exit(7)
sys.exit(p.returncode)
'''
helper='''#!/usr/bin/python3
import json,os,subprocess,sys
from pathlib import Path
root=Path(os.environ['CASE_ROOT']);args=sys.argv[1:]
with (root/'native-calls.jsonl').open('a') as f:f.write(json.dumps(args)+'\\n')
p=subprocess.run([BIN,'--host-root',str(root),*args])
if p.returncode==0 and args[0]==os.environ.get('FAIL_NATIVE_ACTION'):sys.exit(7)
sys.exit(p.returncode)
'''.replace('BIN',repr(str(B)))
addon='''#!/sbin/sh
# ADDOND_VERSION=3
. /tmp/backuptool.functions
echo "$1" >> "$CASE_ROOT/stages.log"
case "$1" in
 backup) backup_file "$S/product/etc/custom" ;;
 restore) [ -f "$C/$S/product/etc/custom" ] || exit 9; restore_file "$S/product/etc/custom" ;;
esac
'''
cases=[]
def relocated(text,root):
 text=re.sub(r'/(?:postinstall|system|product|vendor)(?:/[A-Za-z0-9_.-]+)*',lambda m:str(root)+m.group(),text)
 # S is the logical Android path used by standard addon APIs/cache probes;
 # native source I/O is redirected by the host-only fixture-root entry point.
 text=text.replace('export S='+str(root)+'/system','export S=/system')
 return text

def setup(name,mode='normal'):
 root=D/name
 for p in ['system/bin','system/addon.d','system/etc','product/etc','postinstall/tmp','postinstall/system/etc','postinstall/product/etc','fake-bin']:(root/p).mkdir(parents=True,exist_ok=True)
 (root/'system/bin/sh').symlink_to('/bin/sh')
 (root/'system/bin/backuptool_store').write_text(helper);(root/'system/bin/backuptool_store').chmod(0o755)
 (root/'system/bin/backuptool_ab.functions').write_text(relocated(functions,root))
 (root/'system/etc/hosts').write_text('owner_modified_hosts\n');(root/'product/etc/custom').write_text('owner_product_file\n')
 (root/'postinstall/system/etc/hosts').write_text('new_target_hosts\n');(root/'postinstall/product/etc/custom').write_text('new_target_product\n')
 for p in ['system/build.prop','product/etc/build.prop']:(root/p).write_text('ro.evolution.version=16.0-fixture\n')
 (root/'postinstall/partitions').write_text('system ext4\nproduct ext4\nvendor erofs\nsystem_ext erofs\nodm erofs\n')
 (root/'system/addon.d/50-lineage.sh').write_text(stock)
 extra=addon
 if mode=='bad_version':extra=extra.replace('ADDOND_VERSION=3','ADDOND_VERSION=9')
 if mode=='ignored_native_error':extra=extra.replace('backup_file "$S/product/etc/custom"','/system/bin/backuptool_store --backup /data/private/file || true')
 if mode=='erofs_write':extra=extra.replace('backup_file "$S/product/etc/custom"','/system/bin/backuptool_store --output /vendor/etc/private || true')
 # Literal /system/bin helper in a synthetic addon is fixture-local; $S stays logical.
 extra=extra.replace('/system/bin/backuptool_store',str(root/'system/bin/backuptool_store'))
 extra_path=root/'system/addon.d/60-v3 space.sh'
 if mode=='symlink_script':extra_path.symlink_to(str(root/'system/etc/hosts'))
 else:extra_path.write_text(extra)
 p=root/'fake-bin/cp';p.write_text(cp);p.chmod(0o755)
 p=root/'fake-bin/sync';p.write_text('#!/bin/sh\nexit 0\n');p.chmod(0o755)
 runner=root/'staged.sh';runner.write_text(relocated(script,root))
 env={**os.environ,'PATH':str(root/'fake-bin')+':/usr/bin:/bin','BACKUPTOOL_BROKER_PROTOCOL':'2','CASE_ROOT':str(root)}
 if mode=='copy_fail':env['FAIL_ADDON_COPY']='1'
 if mode=='backup_fail':env['FAIL_NATIVE_ACTION']='--backup'
 if mode=='restore_fail':env['FAIL_NATIVE_ACTION']='--restore'
 if mode=='no_protocol':env.pop('BACKUPTOOL_BROKER_PROTOCOL')
 return root,runner,env

for name,mode,ok in [('normal','normal',True),('version_rejected','bad_version',False),('symlink_addon_rejected','symlink_script',False),('ignored_native_error_sticky','ignored_native_error',False),('erofs_api_write_rejected','erofs_write',False),('partial_copy_aborts_before_scripts','copy_fail',False),('backup_error_no_restore','backup_fail',False),('restore_error_no_publish','restore_fail',False),('protocol_required','no_protocol',False)]:
 root,runner,env=setup(name,mode)
 before={p:hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'system/etc/hosts',root/'product/etc/custom',root/'postinstall/system/etc/hosts',root/'postinstall/product/etc/custom']}
 p=subprocess.run(['taskset','-c','4-7','nice','-n','15','/bin/sh',str(runner)],capture_output=True,text=True,env=env,timeout=35);(root/'result.log').write_text(p.stdout+p.stderr)
 assert (p.returncode==0)==ok,(name,p.returncode,p.stdout,p.stderr)
 assert all(hashlib.sha256(path.read_bytes()).hexdigest()==pin for path,pin in before.items()),name
 calls=[json.loads(v) for v in (root/'native-calls.jsonl').read_text().splitlines()] if (root/'native-calls.jsonl').exists() else []
 assert not any(v[0] in ['--apply','--check'] for v in calls),name
 if ok:
  assert (root/'postinstall/tmp/restore/system/etc/hosts').read_text()=='owner_modified_hosts\n'
  assert (root/'postinstall/tmp/restore/product/etc/custom').read_text()=='owner_product_file\n'
  assert (root/'postinstall/tmp/restore/system/addon.d/50-lineage.sh').is_file()
  assert (root/'stages.log').read_text().splitlines()==['pre-backup','backup','post-backup','pre-restore','restore','post-restore']
 elif mode in ['copy_fail','bad_version','symlink_script','no_protocol']:assert not calls,name
 elif mode in ['ignored_native_error','erofs_write','backup_fail']:assert not any(v[0]=='--restore' for v in calls),name
 cases.append({'case':name,'exit':p.returncode,'active_and_target_views_unchanged':True,'offline_writer_not_invoked_by_script':True})
# Run the actual new helper functions with protocol unset to exercise the old
# copying/moving ABI. The historical top-level/ab scripts remain unchanged.
root,runner,env=setup('legacy_functions');env.pop('BACKUPTOOL_BROKER_PROTOCOL');env['ADDOND_VERSION']='2'
source=root/'system/etc/hosts';test=root/'legacy.sh';test.write_text('. '+str(root/'system/bin/backuptool_ab.functions')+'\nbackup_file '+str(source)+' || exit $?\nrestore_file '+str(source)+' || exit $?\n')
p=subprocess.run(['/bin/sh',str(test)],env=env,capture_output=True,text=True,timeout=20);(root/'legacy.log').write_text(p.stdout+p.stderr);assert p.returncode==0,(p.stdout,p.stderr)
target=root/'postinstall'/str(source).lstrip('/');assert target.read_bytes()==source.read_bytes();assert not (root/'native-calls.jsonl').exists()
cases.append({'case':'legacy_functions_copy_move_abi_preserved','exit':0,'new_native_helper_not_used':True})
r={'state':'REAL_STAGED_SHELL_AND_LEGACY_FUNCTIONS_HOST_TESTS_PASSED','cases':cases,'count':len(cases),'source_script_sha256':hashlib.sha256(script.encode()).hexdigest(),'source_functions_sha256':hashlib.sha256(functions.encode()).hexdigest(),'actual_native_store_used_for_staging':True,'host_gnu_sh_cp_translation_and_sync_stub_used':True,'android_toybox_selinux_mount_or_real_ota_tested':False,'phone_accessed':False};(D/'report.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps({k:v for k,v in r.items() if k!='cases'}))
