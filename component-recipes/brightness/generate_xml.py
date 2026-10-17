#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 CyberWei922
# SPDX-License-Identifier: Apache-2.0
"""Generate accepted XMLs from separately obtained, exact baseline files into a NEW directory."""
from pathlib import Path
import argparse, hashlib, json
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--input-dir',type=Path,required=True)
p.add_argument('--output-dir',type=Path,required=True)
a=p.parse_args()
spec=json.loads(Path(__file__).with_name('ramp-parameters.json').read_text())
block=('    <!-- HyperOS legacy rate units are 1..16383; AOSP requires normalized floats.\n'
       '         This preserves only the stock native-DDC scalar route, not MIUI dynamic ramps. -->\n'
       +''.join('    <'+k+'>'+format(v,'.12f')+'</'+k+'>\n' for k,v in spec['rates'].items()))
pending=[]
for row in spec['inputs']:
 b=(a.input_dir/row['name']).read_bytes()
 h=hashlib.sha256(b).hexdigest()
 if h==row['accepted_sha256']: out=b
 else:
  assert h==row['baseline_sha256'], 'Unexpected input: '+row['name']
  anchor=b'    <ambientLightHorizonLong>'
  assert b.count(anchor)==1
  out=b.replace(anchor,block.encode()+anchor,1)
 assert hashlib.sha256(out).hexdigest()==row['accepted_sha256'], row['name']
 pending.append((row['name'],out))
a.output_dir.mkdir(parents=True,exist_ok=False)
for name,b in pending:(a.output_dir/name).write_bytes(b)
print('Generated',len(pending),'accepted XML files. No source inputs or phone files changed.')
