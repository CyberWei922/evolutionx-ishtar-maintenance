"""Audit rebuilt signed full OTA against all accepted partition bytes; no phone I/O."""
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
import json,time,hashlib,shutil,subprocess,sys,zipfile,struct,base64,fcntl
C=Path('/mnt/android-download/control/ishtar-selinux-enforcing-final-20261019-01');B=C.parent/'ishtar-selinux-backuptool-final-20261019-01';O=Path('/mnt/android-build/Android/out/evolution-bka');R=Path('/mnt/android-download/git-cache/evolution');H=O/'host/linux-x86/bin';W=O/'tmp/ishtar-selinux-enforcing-final-20261019-01';D=Path('/mnt/android-download/delivery/ishtar-enforcing-final-1792357362-20261019-01')
s={'state':'waiting-for-package','phase':'waiting','started':time.time(),'phone_accessed':False,'other_tasks_changed':False}
def save():p=C/'audit-status.tmp';p.write_text(json.dumps(s,indent=2)+'\n');p.replace(C/'audit-status.json')
def sha(p):return hashlib.file_digest(p.open('rb'),'sha256').hexdigest()
def run(label,args,timeout=180):
 s['phase']=label;save()
 with (C/(label+'.log')).open('xb') as f:p=subprocess.run([str(x) for x in args],stdout=f,stderr=subprocess.STDOUT,timeout=timeout)
 assert p.returncode==0,label
try:
 save();deadline=time.monotonic()+10800
 while True:
  p=json.loads((C/'package-status.json').read_text())
  if p['state']=='failed':raise RuntimeError('Package job failed; no audit/flash')
  if p['state']=='package-complete-audit-required':break
  assert time.monotonic()<deadline;time.sleep(10)
 lock=(C.parent/'ishtar-build.lock').open('a');fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
 zpath=Path(p['output']);assert sha(zpath)==p['package_sha256'];expected=json.loads((C/'expected-payload-partitions.json').read_text());assert len(expected)==34
 prior_dir=Path('/mnt/android-download/delivery/ishtar-selinux-backuptool-final-permissive-candidate-20261019-01');prior_manifest=json.loads((prior_dir/'manifest.json').read_text());prior=prior_dir/prior_manifest['rom']['file']
 A=C/'package-audit01';A.mkdir(exist_ok=False);sys.path.insert(0,str(B/'rom-candidate01/ota-proto'));import update_metadata_pb2
 def kv(b):
  rows=b.decode().splitlines();pairs=[x.split('=',1) for x in rows if x.strip()];assert all(len(x)==2 for x in pairs);d=dict(pairs);assert len(d)==len(pairs);return d
 s.update(state='auditing',phase='zip-crc-and-metadata');save()
 with zipfile.ZipFile(zpath) as z,zipfile.ZipFile(prior) as old:
  assert len(z.namelist())==len(set(z.namelist())) and set(z.namelist())==set(old.namelist());assert z.testzip() is None
  metadata=kv(z.read('META-INF/com/android/metadata'));oldmeta=kv(old.read('META-INF/com/android/metadata'))
  for k,v in oldmeta.items():
   if k not in ['ota-property-files','ota-streaming-property-files']:assert metadata[k]==v,k
  assert metadata['post-build-incremental']=='1792357362' and metadata['pre-device']=='ishtar' and metadata['ota-type']=='AB' and metadata['post-sdk-level']=='36'
  assert all(metadata.get(n,'no').lower() in ['no','false','0'] for n in ['ota-wipe','ota-downgrade','spl-downgrade'])
  for n in ['apex_info.pb','care_map.pb','META-INF/com/android/otacert']:assert z.read(n)==old.read(n),n
  cert=z.read('META-INF/com/android/otacert');(A/'ota-certificate.pem').write_bytes(cert)
  payload=W/'audit-signed-payload.bin'
  with z.open('payload.bin') as inp,payload.open('xb') as out:shutil.copyfileobj(inp,out,16*2**20)
  props=kv(z.read('payload_properties.txt'))
 with payload.open('rb') as f:
  prefix=f.read(24);magic,version,msize,ssize=struct.unpack('>4sQQI',prefix);assert magic==b'CrAU' and version==2 and 0<msize<64*2**20;mb=f.read(msize);assert len(mb)==msize and len(f.read(ssize))==ssize
  meta_bytes=prefix+mb
 m=update_metadata_pb2.DeltaArchiveManifest();m.ParseFromString(mb);assert m.IsInitialized() and m.minor_version==0 and not m.partial_update
 actual={x.partition_name:{'bytes':x.new_partition_info.size,'sha256':x.new_partition_info.hash.hex()} for x in m.partitions};assert len(actual)==len(m.partitions)==34 and actual==expected and not {'userdata','metadata'}&actual.keys()
 for k,v in {'FILE_SIZE':str(payload.stat().st_size),'FILE_HASH':base64.b64encode(bytes.fromhex(sha(payload))).decode(),'METADATA_SIZE':str(len(meta_bytes)),'METADATA_HASH':base64.b64encode(hashlib.sha256(meta_bytes).digest()).decode()}.items():assert props[k]==v,k
 # Verify OTA whole-file CMS signature over the exact AOSP-defined byte range.
 s['phase']='whole-file-cms-signature';save();length=zpath.stat().st_size
 with zpath.open('rb') as f:f.seek(-6,2);footer=f.read(6);assert footer[2:4]==b'\xff\xff';sigstart=struct.unpack_from('<H',footer,0)[0];comment=struct.unpack_from('<H',footer,4)[0];assert 6<sigstart<=comment;f.seek(-(22+comment),2);eocd=f.read(22+comment)
 assert eocd[:4]==b'PK\x05\x06' and b'PK\x05\x06' not in eocd[4:];signed_len=length-comment-2;signature=eocd[-sigstart:-6];(A/'ota-whole-file-signature.der').write_bytes(signature)
 with (C/'whole-file-signature.log').open('xb') as log:
  p=subprocess.Popen(['openssl','cms','-verify','-binary','-inform','DER','-in',str(A/'ota-whole-file-signature.der'),'-noverify','-nointern','-certfile',str(A/'ota-certificate.pem'),'-content','/dev/stdin','-out','/dev/null'],stdin=subprocess.PIPE,stdout=log,stderr=log)
  with zpath.open('rb') as f:
   remaining=signed_len
   while remaining:
    b=f.read(min(8*2**20,remaining));assert b;p.stdin.write(b);remaining-=len(b)
  p.stdin.close();assert p.wait(timeout=120)==0,'Whole-file CMS signature'
 # Verify payload RSA signature independently using the certificate's public key.
 with (A/'payload-public-key.pem').open('xb') as out:subprocess.run(['openssl','x509','-in',str(A/'ota-certificate.pem'),'-pubkey','-noout'],stdout=out,check=True)
 run('payload-rsa-signature',[H/'delta_generator','--in_file='+str(payload),'--public_key='+str(A/'payload-public-key.pem')],180)
 s['phase']='reconstruct-all-34-payload-partitions';save();images=A/'images';images.mkdir();extract=R/'prebuilts/extract-tools/linux-x86/bin/ota_extractor';assert sha(extract)=='7cf65d6c557cc6e761082e88aa225123d445be26dd1a18f618f87c142afaff8c'
 assert shutil.disk_usage(A).free>sum(v['bytes'] for v in expected.values())+20*2**30
 groups=[[],[]];weights=[0,0]
 for n,v in sorted(expected.items(),key=lambda x:x[1]['bytes'],reverse=True):i=min(range(2),key=lambda a:weights[a]);groups[i].append(n);weights[i]+=v['bytes']
 def extract_group(item):
  i,names=item
  with (C/('payload-extract-group'+str(i)+'.log')).open('xb') as log:p=subprocess.run([str(extract),'--payload='+str(payload),'--output_dir='+str(images),'--partitions='+','.join(names),'--single_thread=true','--operation_threads=1','--verity_threads=1'],stdout=log,stderr=subprocess.STDOUT,timeout=1800)
  assert p.returncode==0,('extract',i,p.returncode);return i
 with ThreadPoolExecutor(max_workers=2) as pool:list(pool.map(extract_group,enumerate(groups)))
 for n,v in expected.items():p=images/(n+'.img');assert p.stat().st_size==v['bytes'] and sha(p)==v['sha256'],n
 prior_payload=json.loads((B/'rom-candidate01/actual-payload-verification.json').read_text())['partitions'];assert sorted(n for n,v in expected.items() if prior_payload[n]!=v)==['vbmeta','vendor_boot']
 for n in ['vendor_boot','vbmeta']:assert sha(images/(n+'.img'))==sha(Path('/mnt/android-download/delivery/ishtar-selinux-enforcing-boot-candidate-20261019-01/candidate')/(n+'.img'))
 # The other 32 images are byte identical; prior file/label/Recovery policy audits remain applicable.
 report={'state':'FULL_ENFORCING_OTA_ACTUAL_PAYLOAD_AND_SIGNATURES_VERIFIED','incremental':'1792357362','zip':str(zpath),'zip_bytes':zpath.stat().st_size,'zip_sha256':sha(zpath),'partitions':expected,'actual_images':str(images),'partition_count':34,'changed_partitions':['vbmeta','vendor_boot'],'unchanged_partitions':32,'zip_crc_passed':True,'whole_file_signature_verified_against_matching_installed_certificate':True,'payload_rsa_signature_verified':True,'payload_properties_verified':True,'full_nonincremental_payload':True,'static_permissive_domains':[],'global_boot_mode':'Enforcing','kernel_ksu_and_recovery_retained':True,'data_wipe_not_embedded_in_ota':True,'clean_flash_still_required':True,'real_ota_device_install_not_tested':True,'phone_accessed':False,'finished':time.time()};(C/'actual-full-package-verification.json').open('x').write(json.dumps(report,indent=2)+'\n')
 D.mkdir(exist_ok=False);shutil.copy2(zpath,D/zpath.name)
 assert sha(D/zpath.name)==report['zip_sha256'];(D/'verification.json').write_text(json.dumps(report,indent=2)+'\n');(D/'README.md').write_text('# Ishtar Enforcing 完整候选\n\n内部版本1792357362，完整signed OTA已重新生成，实际34分区已回解匹配；仅vendor_boot/vbmeta相对最后P整包变化，且与已验收E组件逐字匹配。完整OTA与payload签名已独立验证，证书与原包保持。\n\n无内置强制wipe；clean flash由另行已授权过程执行。相机/亮度/刷新率/Recovery/内核保持；待干净首次初始化和机主功能验收。此包为Unofficial，KSU动态例外保持；不冒称真实在线OTA或长时验收。\n')
 (D/'manifest.json').write_text(json.dumps({'state':report['state'],'incremental':'1792357362','mode':'Enforcing','rom':{'file':zpath.name,'bytes':report['zip_bytes'],'sha256':report['zip_sha256']},'clean_flash_accepted':False,'owner_scope':json.loads((C/'owner-scope.json').read_text())},ensure_ascii=False,indent=2)+'\n')
 for p in D.iterdir():p.chmod(0o444)
 payload.unlink();s.update(state='complete',phase='formal-delivery-ready',delivery=str(D),rom_sha256=report['zip_sha256'],rom_bytes=report['zip_bytes'],finished=time.time());save();print(json.dumps(s))
except BaseException as e:s.update(state='failed',error=repr(e),finished=time.time());save();raise
