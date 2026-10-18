from pathlib import Path
import hashlib,json,sys
r=Path(sys.argv[1]);meta=json.loads((Path(__file__).parent/'external-property-file.json').read_text());p=r/meta['path'];b=p.read_bytes()
assert hashlib.sha256(b).hexdigest()==meta['before_sha256']
line=(meta['removed_exact_line']+'\n').encode();assert b.count(line)==1;a=b.replace(line,b'');assert hashlib.sha256(a).hexdigest()==meta['after_sha256'];p.write_bytes(a)
