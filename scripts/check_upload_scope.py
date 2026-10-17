#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 CyberWei922
# SPDX-License-Identifier: Apache-2.0
"""Verify the license-scoped content manifest; never contact a device or network."""
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
external=json.loads((r/'provenance/external-text-dependencies.json').read_text())
for x in external:assert not (r/'source-overrides'/x['path']).exists(),x['path']
for x in json.loads((r/'provenance/excluded-camera-recipes.json').read_text()):assert not (r/x['path']).exists(),x['path']
for p in list((r/'source-files').rglob('*'))+list((r/'source-overrides').rglob('*')):
 if not p.is_file() or p.name.endswith('.license'):continue
 assert not p.is_symlink(),p
 b=p.read_bytes();assert b'Confidential and Proprietary' not in b,p
 assert (b'-----BEGIN '+b'PRIVATE KEY-----') not in b,p
 assert Path(str(p)+'.license').is_file(),p
for name,row in json.loads((r/'provenance/source-files.json').read_text()).items():
 p=r/'source-files'/name
 assert hashlib.sha256(p.read_bytes()).hexdigest()==row['sha256'],name
for p in r.rglob('*'):
 if not p.is_file() or '.git' in p.relative_to(r).parts:continue
 assert p.suffix.lower() not in ['.apk','.img','.zip','.ko','.so','.dex','.pk8','.key','.jks','.keystore','.p12','.pfx'],p
 assert (b'-----BEGIN '+b'PRIVATE KEY-----') not in p.read_bytes(),p
print('UPLOAD_SCOPE_OK: exact licensed sources, notices, no excluded content or compiled payloads')
