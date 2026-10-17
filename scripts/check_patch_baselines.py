#!/usr/bin/env python3
"""Check all exported text patches against recorded HEADs using temporary indexes."""
import argparse,json,os,subprocess,tempfile
from pathlib import Path

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--source-tree',type=Path,required=True);a=p.parse_args()
 root=Path(__file__).resolve().parents[1];tree=a.source_tree.resolve()
 rows=json.loads((root/'provenance/local-projects.json').read_text());checked=0
 with tempfile.TemporaryDirectory(prefix='ishtar-patch-check-') as temp:
  for i,row in enumerate(rows):
   if not row['patch']:continue
   repo=tree/row['project']
   head=subprocess.check_output(['git','-C',str(repo),'rev-parse','HEAD'],text=True).strip()
   assert head==row['head'],row['project']
   env=dict(os.environ,GIT_OPTIONAL_LOCKS='0',GIT_INDEX_FILE=str(Path(temp)/('index-'+str(i))))
   for args in [['read-tree','HEAD'],['apply','--cached','--check',str(root/row['patch'])]]:
    subprocess.run(['git','-C',str(repo),*args],env=env,check=True)
   checked+=1
 print(json.dumps({'status':'PATCH_BASELINES_OK','patches':checked,'source_worktree_and_real_index_modified':False}))
if __name__=='__main__':main()
