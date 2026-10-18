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
for inventory in ['candidate-source-files.json','next-source-files.json','framework-api-source-files.json']:
 candidate=r/'provenance'/inventory
 if not candidate.exists():continue
 for row in json.loads(candidate.read_text())['files']:
  p=r/row['repository_path']
  assert p.resolve().is_relative_to(r/'plans') and not p.is_symlink(),p
  assert p.stat().st_size==row['bytes'] and hashlib.sha256(p.read_bytes()).hexdigest()==row['sha256'],p
  assert Path(str(p)+'.license').is_file() and row['license']=='Apache-2.0',p
boot=r/'provenance/boot-contracts-source-files.json'
if boot.exists():
 for row in json.loads(boot.read_text())['files']:
  p=r/row['repository_path']
  assert p.resolve().is_relative_to(r/'plans/selinux-boot-contracts-20261018/sources') and not p.is_symlink(),p
  b=p.read_bytes();assert len(b)==row['bytes'] and hashlib.sha256(b).hexdigest()==row['sha256'],p
  assert Path(str(p)+'.license').is_file() and row['license'] in ['Apache-2.0','BSD-3-Clause'],p
  if row['license']=='BSD-3-Clause':
   assert b'The Linux Foundation' in b and b'Redistribution and use in source and binary forms' in b and b'THIS SOFTWARE IS PROVIDED' in b,p
debug=r/'provenance/debug-ota-source-files.json'
if debug.exists():
 for row in json.loads(debug.read_text())['files']:
  p=r/row['repository_path']
  assert p.resolve().is_relative_to(r/'plans/selinux-debug-ota-20261018/sources') and not p.is_symlink(),p
  b=p.read_bytes();assert len(b)==row['bytes'] and hashlib.sha256(b).hexdigest()==row['after_sha256'],p
  assert Path(str(p)+'.license').is_file(),p
  assert row['license'] in ['Apache-2.0','BSD-3-Clause','Apache-2.0 AND LicenseRef-AOSP-sepolicy-public-domain'],p
  if row['license']=='BSD-3-Clause':
   assert b'The Linux Foundation' in b and b'Redistribution and use in source and binary forms' in b and b'THIS SOFTWARE IS PROVIDED' in b,p
  if 'public-domain' in row['license']:
   assert 'public domain' in (r/'plans/selinux-debug-ota-20261018/upstream-notices/system-sepolicy-NOTICE').read_text(),p
ota=r/'provenance/ota-contracts-source-files.json'
if ota.exists():
 for row in json.loads(ota.read_text())['files']:
  p=r/row['repository_path']
  assert p.resolve().is_relative_to(r/'plans/selinux-ota-contracts-20261018/sources') and not p.is_symlink(),p
  b=p.read_bytes();assert len(b)==row['bytes'] and hashlib.sha256(b).hexdigest()==row['after_sha256'],p
  assert row['license']=='Apache-2.0' and Path(str(p)+'.license').is_file(),p
for p in r.rglob('*'):
 if not p.is_file() or '.git' in p.relative_to(r).parts:continue
 assert p.suffix.lower() not in ['.apk','.img','.zip','.ko','.so','.dex','.pk8','.key','.jks','.keystore','.p12','.pfx'],p
 assert (b'-----BEGIN '+b'PRIVATE KEY-----') not in p.read_bytes(),p
print('UPLOAD_SCOPE_OK: exact licensed sources, notices, no excluded content or compiled payloads')
