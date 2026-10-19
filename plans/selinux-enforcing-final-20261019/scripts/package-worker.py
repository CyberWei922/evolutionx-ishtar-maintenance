from pathlib import Path
import json,time,hashlib,shutil,os,subprocess,fcntl,fnmatch
R=Path('/mnt/android-download/git-cache/evolution');O=Path('/mnt/android-build/Android/out/evolution-bka');H=O/'host/linux-x86';T=O/'target/product/ishtar/obj/PACKAGING/target_files_intermediates/lineage_ishtar-target_files'
C=Path('/mnt/android-download/control/ishtar-selinux-enforcing-final-20261019-01');B=C.parent/'ishtar-selinux-backuptool-final-20261019-01';E=C.parent/'ishtar-selinux-enforcing-boot-20261019-01';D=Path('/mnt/android-download/delivery/ishtar-selinux-enforcing-boot-candidate-20261019-01');W=O/'tmp/ishtar-selinux-enforcing-final-20261019-01';S=W/'target-files';OUT=W/'EvolutionX-16.0-20261019-ishtar-11.12-Unofficial-Enforcing.zip'
s={'state':'preparing','phase':'source-and-baseline','started':time.time(),'phone_accessed':False,'other_tasks_changed':False}
def save():p=C/'package-status.tmp';p.write_text(json.dumps(s,indent=2)+'\n');p.replace(C/'package-status.json')
def sha(p):return hashlib.file_digest(p.open('rb'),'sha256').hexdigest()
try:
 save();lock=(C.parent/'ishtar-build.lock').open('a');fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
 assert shutil.disk_usage(O).free>55*2**30 and shutil.disk_usage(C).free>60*2**30
 expected=json.loads((E/'expected-source-files.json').read_text());assert len(expected)==104
 for n,v in expected.items():assert sha(R/n)==v['sha256'],n
 (C/'expected-source-files.json').open('x').write(json.dumps(expected,indent=2)+'\n')
 base=json.loads((B/'rom-candidate01/actual-payload-verification.json').read_text());images=base['partitions'];assert len(images)==34 and base['zip_sha256']=='a2c7945bcd847a9109c409313d13723e55d2d148d00701c29c7f4b53eea48ebc'
 source={}
 for n,v in images.items():
  paths=[T/sub/(n+'.img') for sub in ['IMAGES','RADIO'] if (T/sub/(n+'.img')).is_file()];assert len(paths)==1,n;p=paths[0];assert p.stat().st_size==v['bytes'] and sha(p)==v['sha256'],n;source[n]={'path':str(p),**v}
 W.mkdir(exist_ok=False);S.mkdir()
 # Work from a private metadata copy; image hardlinks are read-only inputs.
 for sub in ['IMAGES','RADIO','PREBUILT_IMAGES']:
  if (T/sub).is_dir():shutil.copytree(T/sub,S/sub,copy_function=os.link,symlinks=True)
 shutil.copytree(T/'META',S/'META',symlinks=True)
 patterns=['INSTALL/*','OTA/*','*/build.prop','*/default.prop','*/build.default','*/etc/vintf/*']
 for root,dirs,files in os.walk(T):
  relroot=Path(root).relative_to(T)
  if not relroot.parts:dirs[:]=[x for x in dirs if x not in ['IMAGES','RADIO','PREBUILT_IMAGES','META']]
  for n in files:
   p=Path(root)/n;rel=p.relative_to(T)
   if any(fnmatch.fnmatch(str(rel),pattern) for pattern in patterns):
    dst=S/rel;dst.parent.mkdir(parents=True,exist_ok=True)
    if p.is_symlink():dst.symlink_to(os.readlink(p))
    else:shutil.copy2(p,dst)
 for n in ['vendor_bootconfig','vendor_cmdline']:
  src=T/'VENDOR_BOOT'/n;text=src.read_text();assert 'androidboot.selinux=permissive' in text
  text=text.replace('androidboot.selinux=permissive','androidboot.selinux=enforcing').replace('enforcing=0','enforcing=1');dst=S/'VENDOR_BOOT'/n;dst.parent.mkdir(parents=True,exist_ok=True);dst.write_text(text)
 final=dict(images)
 for n,v in {'vendor_boot':{'bytes':100663296,'sha256':'7df0dde9214d37f5a081fd5f972e051c2c026f7b03abcd2415a3a4fd52a17670'},'vbmeta':{'bytes':8192,'sha256':'8704c186b031a83383c60be6bcaeed304b8a4f33d58ed129ba1a125191c06963'}}.items():
  src=D/'candidate'/(n+'.img');assert src.stat().st_size==v['bytes'] and sha(src)==v['sha256'];dst=S/'IMAGES'/(n+'.img');dst.unlink();shutil.copy2(src,dst);final[n]=v
 assert set(n for n in images if final[n]!=images[n])=={'vendor_boot','vbmeta'}
 (C/'expected-payload-partitions.json').open('x').write(json.dumps(final,indent=2)+'\n');(C/'input-proof.json').open('x').write(json.dumps({'source_files':104,'baseline_incremental':'1792357362','original_full_rom_sha256':base['zip_sha256'],'baseline_partition_sources':source,'replaced_partitions':['vendor_boot','vbmeta'],'internal_version_unchanged':True,'payload_wipe_flag_requested':False,'clean_flash_is_separately_authorized':True,'private_target_files':str(S),'source_target_files_not_modified':True},indent=2)+'\n')
 env=os.environ.copy();java=R/'prebuilts/jdk/jdk21/linux-x86';assert (java/'bin/java').is_file();env.update(PATH=str(H/'bin')+':'+str(java/'bin')+':'+env['PATH'],JAVA_HOME=str(java),ANDROID_JAVA_HOME=str(java),TMPDIR=str(W/'ota-tmp'),GOMAXPROCS='4',CUDA_VISIBLE_DEVICES='',HIP_VISIBLE_DEVICES='',ROCR_VISIBLE_DEVICES='');(W/'ota-tmp').mkdir()
 args=[str(H/'bin/ota_from_target_files'),'--verbose','--path',str(H),'--backup=true','--max_threads=4','-k',str(R/'build/make/target/product/security/testkey'),'--output_metadata_path',str(C/'ota-metadata'),str(S),str(OUT)]
 (C/'package-command.json').open('x').write(json.dumps({'args':args,'cpu_affinity':'4-7','max_threads':4,'gpu':False},indent=2)+'\n');s.update(state='packaging',phase='signed-full-payload-generation',output=str(OUT));save()
 with (C/'package.log').open('x') as f:p=subprocess.run(args,cwd=R,env=env,stdout=f,stderr=subprocess.STDOUT,timeout=10800)
 assert p.returncode==0,'ota_from_target_files exit '+str(p.returncode)
 s.update(state='package-complete-audit-required',phase='awaiting-output-audit',package_bytes=OUT.stat().st_size,package_sha256=sha(OUT),finished=time.time());save()
except BaseException as e:s.update(state='failed',error=repr(e),finished=time.time());save();raise
