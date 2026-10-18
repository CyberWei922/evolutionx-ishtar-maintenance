from pathlib import Path
import hashlib,json,sys
r=Path(sys.argv[1]);m=json.loads((Path(__file__).parent/'external-inputs.json').read_text())
for x in m['full_source_omissions']:
 p=r/x['path'];b=p.read_bytes();assert hashlib.sha256(b).hexdigest()==x['before_sha256'],x['path']
 if p.name=='vendor.prop':
  for s in m['property_lines_removed']:
   line=(s+'\n').encode();assert b.count(line)==1;b=b.replace(line,b'')
 else:b+=m['file_contexts_appended'].encode()
 assert hashlib.sha256(b).hexdigest()==x['after_sha256'],x['path'];p.write_bytes(b)
for x in m['owner_images']:
 p=r/x['path'];assert p.is_file() and hashlib.sha256(p.read_bytes()).hexdigest()==x['after_sha256'],x['path']
print('External configuration and owner-image hashes match')
