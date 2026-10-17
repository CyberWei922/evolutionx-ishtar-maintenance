#!/usr/bin/env python3
"""Verify this maintenance snapshot without network, source writes or device access."""
from pathlib import Path
import argparse,hashlib,json

def main():
 parser=argparse.ArgumentParser(description=__doc__)
 parser.add_argument('--source-tree',type=Path,help='Optionally compare the known accepted files in an existing Android checkout')
 args=parser.parse_args();root=Path(__file__).resolve().parents[1]
 m=json.loads((root/'provenance/bundle-manifest.json').read_text())
 for name,row in m['files'].items():
  p=root/name
  assert p.is_file() and not p.is_symlink() and p.resolve().is_relative_to(root),name
  assert p.stat().st_size==row['bytes'],name
  with p.open('rb') as f:assert hashlib.file_digest(f,'sha256').hexdigest()==row['sha256'],name
 source_files=0
 if args.source_tree:
  tree=args.source_tree.resolve();expected=json.loads((root/'provenance/accepted-source-files.json').read_text())
  for name,row in expected.items():
   p=tree/name;assert p.is_file(),name
   assert p.stat().st_size==row['bytes'],name
   with p.open('rb') as f:assert hashlib.file_digest(f,'sha256').hexdigest()==row['sha256'],name
   source_files+=1
 print(json.dumps({'status':'MAINTENANCE_BUNDLE_OK','files':len(m['files']),'payload_bytes':sum(x['bytes'] for x in m['files'].values()),'accepted_checkout_files_verified':source_files,'phone_accessed':False,'compiled_artifacts_in_repository':False}))
if __name__=='__main__':main()
