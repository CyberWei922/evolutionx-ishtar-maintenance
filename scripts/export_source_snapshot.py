#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 CyberWei922
# SPDX-License-Identifier: Apache-2.0
"""Export ONLY the reviewed source path allowlist to a new local directory; no network or device access."""
from pathlib import Path
import argparse,json,shutil
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--source-root',type=Path,required=True)
p.add_argument('--destination',type=Path,required=True)
a=p.parse_args();repo=Path(__file__).resolve().parents[1]
names=list(json.loads((repo/'provenance/source-files.json').read_text()))
names += [x['path'] for x in json.loads((repo/'provenance/source-overrides.json').read_text())]
root=a.source_root.resolve();pending=[]
for name in names:
 src=root/name
 assert src.is_file() and not src.is_symlink() and src.resolve().is_relative_to(root),name
 pending.append((name,src))
a.destination.mkdir(parents=True,exist_ok=False)
for name,src in pending:
 dst=a.destination/name;dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dst)
print('Exported',len(pending),'reviewed paths. Review changes/licenses before uploading. No automatic new-file discovery.')
