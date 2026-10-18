from pathlib import Path
import subprocess,json,hashlib
C=Path('/mnt/android-download/control/ishtar-selinux-backuptool-final-20261019-01');S=C/'source-candidate06';D=S/'method-tests01';D.mkdir(exist_ok=False);B=S/'after/system/update_engine/backuptool_broker'
fixture=r'''#include "broker.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using std::string;
struct Fake : backuptool_broker::Operations {
 string fail; int worker=0, commit=0; std::vector<string> calls;
 bool op(const string& n){calls.push_back(n);return n!=fail;}
 bool Validate()override{return op("validate");} bool IsolateMounts()override{return op("isolate");}
 bool MountWorkspace()override{return op("workspace");} bool MakeDirectory(const string&p)override{return op("mkdir:"+p);}
 bool MountTarget(const string&p)override{return op("target:"+p);} bool BindSystem(const string&,const string&)override{return op("bind");}
 bool MountExtras()override{return op("extras");} bool SnapshotContexts()override{return op("contexts");}
 int RunWorker()override{calls.push_back("worker");return worker;} int Commit()override{calls.push_back("commit");return commit;}
 bool UnmountIfMounted(const string&p)override{return op("unmount:"+p);}
};
const std::vector<string> views={"/postinstall/odm","/postinstall/inactive-odm","/postinstall/system_ext","/postinstall/inactive-system_ext","/postinstall/product","/postinstall/inactive-product","/postinstall/vendor","/postinstall/inactive-vendor","/postinstall/system","/postinstall/inactive-root"};
void check(const string& name,Fake f,int expected,bool commit){int out=backuptool_broker::Run(f);assert(out==expected);auto at=std::find(f.calls.begin(),f.calls.end(),"commit");assert((at!=f.calls.end())==commit);if(commit){assert(at!=f.calls.begin());for(size_t i=0;i<views.size();i++)assert(*(at-views.size()+i)=="unmount:"+views[i]);}if(f.fail!="validate"&&f.fail!="isolate"&&f.fail!="workspace"){assert(f.calls.back()=="unmount:/postinstall");for(auto& v:views)assert(std::find(f.calls.begin(),f.calls.end(),"unmount:"+v)!=f.calls.end());}std::cout<<name<<" PASS\n";}
int main(){
 check("successful_worker_unmounts_before_offline_commit",Fake{},0,true);
 for(auto x:std::vector<std::pair<string,int>>{{"validate",20},{"isolate",21},{"workspace",22},{"target:/postinstall/inactive-root",24},{"bind",25},{"extras",30},{"contexts",31}}){Fake f;f.fail=x.first;check("setup_"+x.first,f,x.second,false);}
 for(auto p:{".broker",".broker/contexts","inactive-root","system","tmp","vendor","inactive-vendor","product","inactive-product","system_ext","inactive-system_ext","odm","inactive-odm"}){Fake f;f.fail=string("mkdir:/postinstall/")+p;check("mkdir_"+string(p),f,23,false);}
 for(int code:{1,26,27,28,29,126,127,137}){Fake f;f.worker=code;check("worker_failure_no_commit_"+std::to_string(code),f,code,false);}
 for(auto& p:views){Fake f;f.fail="unmount:"+p;check("busy_target_never_offline_writes_"+p,f,32,false);}
 {Fake f;f.fail="unmount:/postinstall";check("workspace_cleanup_failure",f,70,true);}
 for(int code:{1,40,41,42,43,127,137}){Fake f;f.commit=code;check("commit_failure_cleanup_"+std::to_string(code),f,code,true);}
 {Fake f;f.worker=9;f.fail="unmount:"+views[0];check("preserve_worker_error_with_failed_cleanup",f,9,false);}
}
'''
(D/'sequence.cpp').write_text(fixture)
def run(name,args):
 q=subprocess.run(['taskset','-c','4-7','nice','-n','15',*args],capture_output=True,text=True,timeout=45);(D/(name+'.log')).write_text(q.stdout+q.stderr);assert q.returncode==0,(name,q.stderr,q.stdout);return q.stdout.splitlines()
run('sequence-compile',['c++','-std=c++17','-Wall','-Wextra','-Werror','-I'+str(B),str(B/'broker.cpp'),str(D/'sequence.cpp'),'-o',str(D/'sequence')]);sequence=run('sequence-cases',[str(D/'sequence')])
source=(B/'main.cpp').read_text();a=source.index('  bool Validate() override {');b=source.index('  bool IsolateMounts()',a);method=source[a:b].replace('Validate() override','Validate()')
validation=r'''#include <sys/types.h>
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using std::string;
constexpr char kRoot[]="/postinstall";
struct Part{string name,target,source,filesystem;dev_t target_id=0,source_id=0;};
struct State{const char* context="u:r:update_engine:s0";bool getcon_ok=true,dir_ok=true,workspace_mounted=false;dev_t mounted=0;int io=0;std::map<string,dev_t>ids={{"target",1},{"source",2},{"target_product",3},{"source_product",4}};std::map<string,string>fs={{"target","ext4"},{"source","ext4"},{"target_product","erofs"},{"source_product","erofs"}};string bad;}s;
int getcon(char**v){if(!s.getcon_ok)return -1;*v=s.context?strdup(s.context):nullptr;return 0;}void freecon(char*p){free(p);}bool Error(const string&){return false;}
bool Directory(const string&){return s.dir_ok;}bool Mounted(const string&p,bool*v,dev_t dev=0){*v=p.empty()?(dev==s.mounted):s.workspace_mounted;return true;}
bool Block(const string&p,dev_t*id,string*fs){s.io++;if(p==s.bad)return false;*id=s.ids.at(p);*fs=s.fs.at(p);return true;}
class Test{public:std::vector<Part>parts_={{"system","target","source",""},{"product","target_product","source_product",""}};
'''+method+r'''};
void good(const string&n,bool expected,bool io=true){Test t;assert(t.Validate()==expected);if(!io)assert(s.io==0);std::cout<<n<<" PASS\n";}
int main(){
 good("valid_system_ext4_and_readonly_erofs_extra",true);
 for(auto p:{"u:r:shell:s0","u:r:backuptool:s0","u:r:recovery:s0","u:r:update_engine:s0_extra","unconfined"}){s=State{};s.context=p;good(string("reject_domain_before_io_")+p,false,false);}
 s=State{};s.context=nullptr;good("null_context_no_io",false,false);s=State{};s.getcon_ok=false;good("getcon_failed_no_io",false,false);
 s=State{};s.ids["target"]=2;good("system_target_aliases_source",false);
 s=State{};s.ids["target_product"]=2;good("extra_target_aliases_primary_source",false);
 s=State{};s.ids["target_product"]=1;good("duplicate_target_device",false);
 s=State{};s.ids["source_product"]=2;good("duplicate_source_device",false);
 s=State{};s.fs["target"]="erofs";good("primary_target_not_ext4",false);
 s=State{};s.fs["source"]="erofs";good("primary_source_not_ext4",false);
 for(auto p:{"target","source","target_product","source_product"}){s=State{};s.bad=p;good(string("bad_block_")+p,false);}
 s=State{};s.mounted=1;good("primary_target_already_mounted",false);s=State{};s.mounted=3;good("extra_target_already_mounted",false);
 s=State{};s.workspace_mounted=true;good("workspace_already_mounted",false);s=State{};s.dir_ok=false;good("workspace_symlink_or_not_directory",false);
}
'''
(D/'validate.cpp').write_text(validation);run('validate-compile',['c++','-std=c++17','-Wall','-Wextra','-Werror',str(D/'validate.cpp'),'-o',str(D/'validate')]);valid=run('validate-cases',[str(D/'validate')])
r={'state':'BROKER_REAL_SEQUENCE_AND_VALIDATE_TESTS_PASSED','sequence_cases':sequence,'validate_cases':valid,'count':len(sequence)+len(valid),'real_validate_body_sha256':hashlib.sha256(method.encode()).hexdigest(),'io_dependencies_are_explicit_doubles':True,'real_kernel_namespace_mount_or_selinux_tested':False,'phone_accessed':False};(D/'report.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps({k:v for k,v in r.items() if not isinstance(v,list)}))
