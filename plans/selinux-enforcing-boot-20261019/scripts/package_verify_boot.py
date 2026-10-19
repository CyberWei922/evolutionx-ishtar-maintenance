# Copyright (C) 2026 CyberWei922
# SPDX-License-Identifier: Apache-2.0
"""Package and verify only vendor_boot + matching root AVB metadata from installed baseline."""
from pathlib import Path
import hashlib,json,subprocess,struct,importlib.util,sys,shutil,time,fcntl,tarfile
R=Path('/mnt/android-download/git-cache/evolution');O=Path('/mnt/android-build/Android/out/evolution-bka');P=O/'target/product/ishtar';H=O/'host/linux-x86/bin';C=Path('/mnt/android-download/control/ishtar-selinux-enforcing-boot-20261019-01');B=C/'baseline-images';N=C/'candidate-images'
assert (C/'enforcing-vendor-boot-build-01.exit').read_text().strip()=='0'
assert not (C/'actual-image-verification.json').exists()
lock=(C.parent/'ishtar-build.lock').open('a');fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
sha=lambda b:hashlib.sha256(b).hexdigest();load=lambda p:json.loads(p.read_text())
expected=load(C/'expected-source-files.json');assert len(expected)==104
for n,v in expected.items():assert sha((R/n).read_bytes())==v['sha256'],n
for n,v in load(C/'baseline-image-manifest.json').items():assert sha((B/(n+'.img')).read_bytes())==v['sha256'],n
assert sha((P/'odm/etc/selinux/precompiled_sepolicy').read_bytes())=='a1152f338c5f9f012206e166aadbc7eb280c4b6fd0725ee2eaaa48e5cead283f'
spec=importlib.util.spec_from_file_location('ishtar_avb',R/'external/avb/avbtool.py');M=importlib.util.module_from_spec(spec);sys.modules[spec.name]=M;spec.loader.exec_module(M);avb=M.Avb()
def parse(p):return avb._parse_image(M.ImageHandler(str(p),read_only=True))
def blob(p):return avb._load_vbmeta_blob(M.ImageHandler(str(p),read_only=True))
def text(v):return v.decode() if isinstance(v,bytes) else v
def run(args,name):
 p=subprocess.run([str(v) for v in args],capture_output=True,text=True,timeout=45);(C/(name+'.log')).open('x').write(p.stdout+p.stderr);assert p.returncode==0,(name,p.stderr[-1500:]);return p.stdout

def vendor(b):
 assert b[:8]==b'VNDRBOOT';v,page=struct.unpack_from('<II',b,8);assert v==4 and page==4096
 rs=struct.unpack_from('<I',b,24)[0];hs,ds=struct.unpack_from('<II',b,2096);ts,tn,te,bs=struct.unpack_from('<IIII',b,2112);assert hs==2128 and tn==1 and te==108
 off=((hs+page-1)//page)*page;parts={};ranges={}
 for n,size in [('vendor_ramdisk',rs),('dtb',ds),('table',ts),('bootconfig',bs)]:
  parts[n]=b[off:off+size];ranges[n]=[off,size];off+=((size+page-1)//page)*page
 return b[28:2076].split(b'\0')[0].decode(),parts,ranges

old=(B/'vendor_boot.img').read_bytes();built=(P/'vendor_boot.img').read_bytes();oldcmd,oldparts,oldranges=vendor(old);newcmd,newparts,newranges=vendor(built)
assert newcmd==oldcmd.replace('androidboot.selinux=permissive','androidboot.selinux=enforcing').replace('enforcing=0','enforcing=1')
assert newcmd.split().count('androidboot.selinux=enforcing')==1 and newcmd.split().count('enforcing=1')==1
assert newparts['bootconfig']==oldparts['bootconfig'].replace(b'androidboot.selinux=permissive',b'androidboot.selinux=enforcing')
assert newparts['bootconfig'].count(b'androidboot.selinux=')==1
for n in ['vendor_ramdisk','dtb','table']:assert oldparts[n]==newparts[n],n
# Every other header byte remains the installed one.
oldheader=bytearray(old[:4096]);newheader=bytearray(built[:4096])
for a,z in [(28,2076),(2124,2128)]:oldheader[a:z]=bytes(z-a);newheader[a:z]=bytes(z-a)
assert oldheader==newheader
(C/'raw-build').mkdir(exist_ok=False);shutil.copy2(P/'vendor_boot.img',C/'raw-build/vendor_boot.img');shutil.copy2(P/'vendor_boot.img',N/'vendor_boot.img')
of,oh,od,_=parse(B/'vendor_boot.img');assert of and oh.algorithm_type==0 and oh.flags==0
hashes=[d for d in od if isinstance(d,M.AvbHashDescriptor)];props=[d for d in od if isinstance(d,M.AvbPropertyDescriptor)];assert len(hashes)==1 and len(props)==1 and len(od)==2
h=hashes[0];assert h.partition_name=='vendor_boot' and h.hash_algorithm=='sha256' and h.flags==0
run([H/'avbtool','erase_footer','--image',N/'vendor_boot.img'],'normalize-vendor-remove-footer')
args=[H/'avbtool','add_hash_footer','--image',N/'vendor_boot.img','--partition_size',len(old),'--partition_name','vendor_boot','--algorithm','NONE','--hash_algorithm','sha256','--salt',h.salt.hex(),'--flags',oh.flags,'--rollback_index',oh.rollback_index,'--rollback_index_location',oh.rollback_index_location]
for d in props:args+=['--prop',text(d.key)+':'+text(d.value)]
run(args,'normalize-vendor-add-footer')
nf,nh,nd,_=parse(N/'vendor_boot.img');newhash=next(d for d in nd if isinstance(d,M.AvbHashDescriptor))
assert nf.original_image_size==of.original_image_size and nh.algorithm_type==oh.algorithm_type and nh.flags==oh.flags and nh.release_string==oh.release_string
assert [d.encode() for d in nd if not isinstance(d,M.AvbHashDescriptor)]==[d.encode() for d in od if not isinstance(d,M.AvbHashDescriptor)]
assert newhash.salt==h.salt and newhash.image_size==h.image_size and newhash.flags==h.flags and newhash.partition_name==h.partition_name
newbytes=(N/'vendor_boot.img').read_bytes();assert hashlib.sha256(h.salt+newbytes[:newhash.image_size]).digest()==newhash.digest
assert vendor(newbytes)[0]==newcmd and vendor(newbytes)[1]==newparts
# Root VBMeta is rebuilt using only installed descriptors, replacing one vendor_boot hash.
rf,rh,rd,_=parse(B/'vbmeta.img');assert rf is None and rh.flags==3 and rh.rollback_index==0 and rh.public_key_metadata_size==0
oldblob=blob(B/'vbmeta.img');assert M.verify_vbmeta_signature(rh,oldblob) is True
key=R/'external/avb/test/data/testkey_rsa4096.pem';alg='SHA256_RSA4096';assert rh.algorithm_type==M.ALGORITHMS[alg].algorithm_type
kwargs=dict(algorithm_name=alg,key_path=str(key),public_key_metadata_path=None,chain_partitions_use_ab=None,chain_partitions_do_not_use_ab=None,rollback_index=rh.rollback_index,flags=rh.flags,rollback_index_location=rh.rollback_index_location,props=None,props_from_file=None,kernel_cmdlines=None,setup_rootfs_from_kernel=None,ht_desc_to_setup=None,include_descriptors_from_image=None,signing_helper=None,signing_helper_with_files=None,release_string=text(rh.release_string),append_to_release_string=None,required_libavb_version_minor=rh.required_libavb_version_minor)
canonical=avb._generate_vbmeta_blob(descriptors=rd,**kwargs);assert canonical==oldblob,'Installed root metadata was not reproduced byte-for-byte'
replace_count=sum(isinstance(d,M.AvbHashDescriptor) and d.partition_name=='vendor_boot' for d in rd);assert replace_count==1
replacement=[newhash if isinstance(d,M.AvbHashDescriptor) and d.partition_name=='vendor_boot' else d for d in rd]
newblob=avb._generate_vbmeta_blob(descriptors=replacement,**kwargs);assert len(newblob)<=len((B/'vbmeta.img').read_bytes())
(N/'vbmeta.img').open('xb').write(newblob+bytes(len((B/'vbmeta.img').read_bytes())-len(newblob)))
_,vh,vd,_=parse(N/'vbmeta.img');assert M.verify_vbmeta_signature(vh,newblob) is True
for field in ['required_libavb_version_major','required_libavb_version_minor','algorithm_type','rollback_index','rollback_index_location','flags','public_key_size','public_key_metadata_size','release_string']:assert getattr(vh,field)==getattr(rh,field),field
assert len(vd)==len(rd)
for a,b in zip(rd,vd):
 if isinstance(a,M.AvbHashDescriptor) and a.partition_name=='vendor_boot':assert b.encode()==newhash.encode()
 else:assert a.encode()==b.encode(),type(a).__name__
run([H/'avbtool','verify_image','--image',N/'vendor_boot.img'],'verify-vendor-self-hash')
for n in ['vendor_boot','vbmeta']:run([H/'avbtool','info_image','--image',N/(n+'.img')],'candidate-'+n+'-avb-info')
report={'state':'STATICALLY_VERIFIED_BOOT_ENFORCING_COMPONENT_CANDIDATE','baseline_system_incremental':'1792357362','source_files_verified':104,'changed_source_paths':['device/xiaomi/ishtar/BoardConfig.mk'],'candidate_mode':'Enforcing','kernel_and_all_ramdisks_dtb_table_preserved_in_candidate':True,'vendor_boot_non_mode_header_bytes_preserved':True,'vendor_boot_cmdline':newcmd,'vendor_bootconfig':newparts['bootconfig'].decode(),'vendor_boot_avb_props_salt_and_security_parameters_preserved':True,'installed_vbmeta_reproduced_byte_for_byte_with_native_avb_api':True,'new_vbmeta_signature_verified':True,'installed_vbmeta_signature_verified':True,'root_descriptors_changed_only_vendor_boot_hash':True,'root_descriptor_count':len(rd),'avb_root_flags':3,'avb_root_rollback_index':vh.rollback_index,'avb_policy_not_strengthened_or_weakened_by_this_change':True,'on_disk_selinux_policy_sha256':'a1152f338c5f9f012206e166aadbc7eb280c4b6fd0725ee2eaaa48e5cead283f','static_permissive_domains':[],'ksu_dynamic_exception_retained':True,'candidate_images':{n:{'bytes':(N/(n+'.img')).stat().st_size,'sha256':sha((N/(n+'.img')).read_bytes())} for n in ['vendor_boot','vbmeta']},'ramdisk_dtb_table':{n:{'bytes':len(newparts[n]),'sha256':sha(newparts[n])} for n in ['vendor_ramdisk','dtb','table']},'possible_write_partitions':['vendor_boot_a','vbmeta_a'],'baseline_slot_a_only_system_layout_retained':True,'userdata_metadata_or_low_level_firmware_operations':False,'phone_accessed':False,'phone_installed':False,'enforcing_boot_tested':False,'flash_authorized':False,'reboot_authorized':False,'user_requires_notice_before_any_reboot':True,'own_build_pid':json.loads((C/'build-launch01.json').read_text())['pid'],'build_exit':0,'time':time.time()}
(C/'actual-image-verification.json').open('x').write(json.dumps(report,ensure_ascii=False,indent=2)+'\n');print(json.dumps(report,ensure_ascii=False))
