#!/usr/bin/env python3
# Copyright (C) 2026 CyberWei922
# SPDX-License-Identifier: Apache-2.0
"""Reproduce two minimal external-file edits with complete before/after SHA gates."""
from pathlib import Path
import argparse,hashlib,json
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source-tree',required=True,type=Path)
parser.add_argument('--apply',action='store_true')
args=parser.parse_args();root=args.source_tree.resolve()
manifest=json.loads((Path(__file__).parent/'external-source-transformations.json').read_text())
changes=[]
for row in manifest['files']:
    p=root/row['path'];assert p.resolve().is_relative_to(root) and not p.is_symlink()
    before=p.read_bytes();assert hashlib.sha256(before).hexdigest()==row['before_sha256'],row['path']
    if p.name=='vendor.prop':
        after=before
        for line in [b'bluetooth.profile.vc.server.enabled=true\n',b'ro.mediaserver.64b.enable=true\n']:
            assert after.count(line)==1
            after=after.replace(line,b'')
    else:
        marker=b'on property:persist.radio.restart_rild=1\n'
        assert before.count(marker)==1
        index=before.index(marker);tail=before[index:]
        assert tail.count(b'on property:')==2 and b'on property:persist.radio.restart_rild=2\n' in tail
        after=before[:index]
    assert hashlib.sha256(after).hexdigest()==row['after_sha256'],row['path']
    changes.append((p,after))
if args.apply:
    for p,data in changes:p.write_bytes(data)
print(json.dumps({'state':'EXTERNAL_TRANSFORMATIONS_APPLIED' if args.apply else 'EXTERNAL_TRANSFORMATIONS_CHECKED','files':len(changes),'phone_accessed':False}))
