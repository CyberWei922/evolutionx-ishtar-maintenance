from pathlib import Path
import hashlib,json,os,shutil,struct,subprocess
C=Path('/mnt/android-download/control/ishtar-selinux-backuptool-final-20261019-01');T=C/'store-image-tests08';T.mkdir(exist_ok=False)
B=C/'source-candidate08/host-compile01/store';H=Path('/mnt/android-build/Android/out/evolution-bka/host/linux-x86/bin')
mk=shutil.which('mke2fs') or '/usr/sbin/mke2fs';ck=shutil.which('e2fsck') or '/usr/sbin/e2fsck';dbg=str(H/'debugfs')
root=T/'case';(root/'system/etc').mkdir(parents=True);(root/'system/bin').mkdir();(root/'postinstall/tmp').mkdir(parents=True);ctx=root/'postinstall/.broker/contexts';ctx.mkdir(parents=True)
(root/'postinstall/partitions').write_text('system ext4\nproduct erofs\n')
(ctx/'plat_file_contexts').write_text('/system(/.*)? u:object_r:test_system_file:s0\n/system/etc/hosts u:object_r:test_NEW_hosts:s0\n');(ctx/'plat_file_contexts').chmod(0o444)
source=T/'source';(source/'system/etc').mkdir(parents=True);(source/'system/bin').mkdir()
(source/'system/etc/hosts').write_text('old_target\n');(source/'system/etc/obsolete').write_text('obsolete\n');os.link(source/'system/etc/hosts',source/'system/etc/untouched_hardlink')
image=T/'target.img';image.open('wb').truncate(64*2**20)
def cmd(name,args,ok=True):
 q=subprocess.run(args,capture_output=True,text=True,timeout=35);(T/(name+'.log')).write_text(q.stdout+q.stderr)
 if ok:assert q.returncode==0,(name,q.returncode,q.stdout,q.stderr)
 else:assert q.returncode!=0,(name,'unexpected success')
 return q
cmd('mke2fs',[mk,'-q','-t','ext4','-F','-b','4096','-I','256','-d',str(source),str(image)])
active=T/'active.img';shutil.copy2(image,active)
ident=lambda p:str(p.stat().st_dev)+':'+str(p.stat().st_ino)
(root/'postinstall/.broker/devices').write_text('system '+ident(image)+' '+ident(active)+'\n');(root/'postinstall/.broker/devices').chmod(0o444)
(root/'system/etc/hosts').write_text('new_preserved_hosts\n');(root/'system/etc/hosts').chmod(0o640);os.utime(root/'system/etc/hosts',ns=(1700000000*10**9+123456789,1700000000*10**9+987654321));os.setxattr(root/'system/etc/hosts','user.test-preserve',b'value\x00blob')
(root/'system/bin/asset_link').symlink_to('/system/etc/hosts')
base=[str(B),'--host-root',str(root)];cases=[]
def call(name,args,ok=True):
 q=cmd(name,base+args,ok);cases.append(name);return q
call('backup-hosts',['--backup','/system/etc/hosts']);call('restore-hosts',['--restore','/system/etc/hosts','/system/etc/obsolete'])
call('backup-link',['--backup','/system/bin/asset_link']);call('restore-link',['--restore','/system/bin/asset_link'])
# Inject a valid Linux file-capability blob into the staged opaque metadata.
# Rootless host cannot set security.capability on its VFS source; this exercises
# the REAL offline xattr writer, not a mocked kernel privilege grant.
m=root/'postinstall/tmp/restoremeta/system/etc/hosts';b=m.read_bytes();linklen=struct.unpack_from('<Q',b,60)[0];at=68+linklen;count=struct.unpack_from('<Q',b,at)[0];cap=struct.pack('<IIIII',0x02000001,1<<13,0,0,0);key=b'security.capability';m.write_bytes(b[:at]+struct.pack('<Q',count+1)+b[at+8:]+struct.pack('<Q',len(key))+key+struct.pack('<Q',len(cap))+cap)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();before=sha(image)
call('whole-stage-preflight-no-write',['--check','system',str(image),str(active)]);assert sha(image)==before
call('offline-restore',['--apply','system',str(image),str(active)]);assert sha(active)==before
cmd('fsck',[ck,'-fn',str(image)])
for name,internal,wanted in [('hosts','/system/etc/hosts',b'new_preserved_hosts\n'),('hardlink','/system/etc/untouched_hardlink',b'old_target\n')]:
 p=T/(name+'.readback');cmd('dump-'+name,[dbg,'-R','dump '+internal+' '+str(p),str(image)]);assert p.read_bytes()==wanted
stat=cmd('stat-hosts',[dbg,'-R','stat /system/etc/hosts',str(image)]);assert '0640' in stat.stdout and 'eb79a2c4' in stat.stdout.lower(),stat.stdout
xattr=cmd('labels-hosts',[dbg,'-R','ea_list /system/etc/hosts',str(image)]);assert 'u:object_r:test_NEW_hosts:s0' in xattr.stdout and 'user.test-preserve' in xattr.stdout and 'security.capability' in xattr.stdout
for attr,wanted in [('user.test-preserve',b'value\x00blob'),('security.capability',cap),('security.selinux',b'u:object_r:test_NEW_hosts:s0\x00')]:
 p=T/(attr+'.raw');cmd('raw-'+attr,[dbg,'-R','ea_get -f '+str(p)+' /system/etc/hosts '+attr,str(image)]);assert p.read_bytes()==wanted,(attr,p.read_bytes(),wanted)
link=cmd('stat-link',[dbg,'-R','stat /system/bin/asset_link',str(image)]);assert 'symlink' in link.stdout and '/system/etc/hosts' in link.stdout
obsolete=cmd('obsolete',[dbg,'-R','stat /system/etc/obsolete',str(image)]);assert 'File not found' in obsolete.stderr
cases+=['source-unchanged','checked-target-unchanged','valid-ext4-after-commit','restored-content','hardlinked-peer-unchanged','restored-mode-and-nanosecond-mtime','target-policy-label','user-xattr-and-capability-exact-bytes-preserved','symlink-content','obsolete-file-removed']
# Real program rejects wrong scope and directory traversal before writes.
for i,path in enumerate(['/data/system/secret','/system/../data/file','/system//etc/file','/system/etc/','relative','/system/etc/line\nbreak']):
 call('unsafe-path-'+str(i),['--backup',path],False)
(root/'system/etc/escape').symlink_to(str(source/'system/etc'),target_is_directory=True)
call('source-parent-symlink',['--backup','/system/etc/escape/hosts'],False)
call('missing-backup',['--backup','/system/nonexistent/file'])
call('missing-restore',['--restore','/system/nonexistent/file'])
call('erofs-output-refused',['--output','/product/etc/test'],False)
wrong=T/'wrong.img';shutil.copy2(active,wrong);before=sha(image)
call('wrong-inventory',['--apply','system',str(wrong),str(active)],False);assert sha(image)==before
# A malformed sidecar must not silently fall back to untrusted stat metadata.
m.write_bytes(b'bad metadata');call('malformed-metadata-no-write',['--apply','system',str(image),str(active)],False);assert sha(image)==before
root2=T/'v3';(root2/'product/etc').mkdir(parents=True);(root2/'postinstall/tmp').mkdir(parents=True);cx=root2/'postinstall/.broker/contexts';cx.mkdir(parents=True)
(root2/'postinstall/partitions').write_text('product ext4\n');(cx/'plat_file_contexts').write_text('/product(/.*)? u:object_r:test_NEW_product:s0\n');(cx/'plat_file_contexts').chmod(0o444)
tree=T/'product-tree';(tree/'etc').mkdir(parents=True);(tree/'etc/config').write_text('target_template\n');(root2/'postinstall/product/etc').mkdir(parents=True);(root2/'postinstall/product/etc/config').write_text('target_template\n')
prod=T/'product.img';prod.open('wb').truncate(64*2**20);cmd('mk-product',[mk,'-q','-t','ext4','-F','-b','4096','-I','256','-d',str(tree),str(prod)]);src=T/'source-product.img';shutil.copy2(prod,src)
(root2/'postinstall/.broker/devices').write_text('product '+ident(prod)+' '+ident(src)+'\n');(root2/'postinstall/.broker/devices').chmod(0o444)
(root2/'product/etc/custom').write_text('preserved_v3\n');oldbase=base;base=[str(B),'--host-root',str(root2)]
call('v3-alias-backup',['--backup','/system/product/etc/custom']);assert (root2/'postinstall/tmp/backupdir/system/product/etc/custom').is_file()
assert (root2/'postinstall/tmp/backupdir/system/product/etc/custom').stat().st_ino==(root2/'postinstall/tmp/backupdir/product/etc/custom').stat().st_ino
call('v3-alias-restore',['--restore','/system/product/etc/custom'])
q=call('v3-output-template',['--output','/product/etc/config']);Path(q.stdout.strip()).write_text('edited_template\n')
q=call('v3-new-output-parent',['--output','/product/new_dir/added']);Path(q.stdout.strip()).write_text('added\n')
source_sha=sha(src);call('non-sar-product-preflight',['--check','product',str(prod),str(src)]);call('non-sar-product-commit',['--apply','product',str(prod),str(src)]);cmd('product-fsck',[ck,'-fn',str(prod)]);assert sha(src)==source_sha
for n,v in [('etc/custom',b'preserved_v3\n'),('etc/config',b'edited_template\n'),('new_dir/added',b'added\n')]:
 p=T/('prod-'+n.replace('/','_'));cmd('product-dump-'+n.replace('/','_'),[dbg,'-R','dump /'+n+' '+str(p),str(prod)]);assert p.read_bytes()==v
cases+=['v3-old-cache-path-regular-hardlink-alias','non-SAR-layout-restored','get-output-path-edits-ro-template-copy','new-parent-created-with-target-label','product-source-unchanged','product-e2fsck-clean']
base=oldbase
r={'state':'REAL_EXT4_BACKUP_STAGE_RESTORE_TESTS_PASSED','cases':cases,'count':len(cases),'real_libext2fs_images':True,'e2fsck_clean':True,'target_labels_from_fixture_NEW_contexts':True,'opaque_capability_fixture_note':'Rootless host: capability bytes injected in staged metadata, then verified through actual ext4 xattr writer. No host CAP_SETFCAP grant claimed.','source_image_unchanged':True,'phone_accessed':False,'real_android_mount_or_selinux_enforcement_tested':False}
(T/'report.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r))
