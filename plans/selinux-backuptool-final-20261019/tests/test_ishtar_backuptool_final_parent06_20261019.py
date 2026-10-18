from pathlib import Path
import subprocess,hashlib,json
C=Path('/mnt/android-download/control/ishtar-selinux-backuptool-final-20261019-01');D=C/'source-candidate06/parent-tests01';D.mkdir(exist_ok=False)
source=(C/'source-candidate06/after/system/update_engine/payload_consumer/postinstall_runner_action.cc').read_text();old=(C/'source-candidate06/before/system/update_engine/payload_consumer/postinstall_runner_action.cc').read_text()
def method(s,start,next_name):
 a=s.index(start);b=s.index(next_name,a);return s[a:b]
mount=method(source,'bool PostinstallRunnerAction::MountPartition(','\nvoid PostinstallRunnerAction::')
optional=method(source,'void PostinstallRunnerAction::CompletePartitionPostinstall(','\nPostinstallRunnerAction::~')
assert optional==method(old,'void PostinstallRunnerAction::CompletePartitionPostinstall(','\nPostinstallRunnerAction::~')
perform=method(source,'void PostinstallRunnerAction::PerformPartitionPostinstall()','\nvoid PostinstallRunnerAction::OnProgressFdReady()')
assert perform==method(old,'void PostinstallRunnerAction::PerformPartitionPostinstall()','\nvoid PostinstallRunnerAction::OnProgressFdReady()')
assert 'CompletePostinstall(ErrorCode::kPostInstallMountError)' in perform
prefix=r'''#include <sys/mount.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using std::string;
struct Log{template<class T>Log&operator<<(const T&){return *this;}};
#define LOG(x) Log()
struct State {bool exists=true,read=true,resolve=true,mount=true;string empty,fail_resolve;int status=0;std::set<string>ro_fail;std::vector<unsigned char>chunk={1,0,0,0,0x53,0xef};std::vector<string>calls,args;int completed=-1;}s;
namespace brillo{using Blob=std::vector<unsigned char>;struct ProcessImpl{std::vector<string>a;void AddArg(const string&v){a.push_back(v);}void SetSearchPath(bool b){assert(!b);}void SetCloseUnusedFileDescriptors(bool b){assert(b);}int Run(){s.calls.push_back("broker");s.args=a;return s.status;}};}
struct BootControlInterface{static constexpr int kInvalidSlot=-1;bool GetPartitionDevice(const string&name,int,string*out){s.calls.push_back("resolve:"+name);if(!s.resolve||s.fail_resolve==name)return false;*out=s.empty==name?"":name=="system"?"source":"source_"+name;return true;}};
struct Hardware{string GetPartitionMountOptions(const string&){return "opts";}};
struct InstallPlan {struct Partition{string name="system",readonly_target_path="target",filesystem_type="ext4";bool postinstall_optional=false;};int source_slot=0;std::vector<Partition>partitions;};
namespace utils{bool FileExists(const char*){return s.exists;}bool ReadFileChunk(const string&,int64_t,size_t,brillo::Blob*out){s.calls.push_back("read");*out=s.chunk;return s.read;}bool SetBlockDeviceReadOnly(const string&p,bool value){assert(value);s.calls.push_back("ro:"+p);return !s.ro_fail.count(p);}bool MountFilesystem(const string&,const string&,int flags,const string&,const string&){assert(flags==MS_RDONLY);s.calls.push_back("mount_ro");return s.mount;}}
enum class ErrorCode{kPostinstallRunnerError=1,kPostinstallBootedFromFirmwareB=3,kPostinstallFirmwareRONotUpdatable=4};
class PostinstallRunnerAction{public:InstallPlan install_plan_;BootControlInterface boot;BootControlInterface*boot_control_=&boot;Hardware hw;Hardware*hardware_=&hw;string fs_mount_dir_="/postinstall";int current_command_=1;size_t current_partition_=0;double accumulated_weight_=0;std::vector<double>partition_weight_={1};void EnsureUnmounted(){s.calls.push_back("ensure");}bool MountPartition(const InstallPlan::Partition&)noexcept;void Cleanup(){s.calls.push_back("cleanup");}void CompletePartitionPostinstall(int,const string&);void CompletePostinstall(ErrorCode e){s.completed=int(e);s.calls.push_back("finish");}void ReportProgress(int){s.calls.push_back("progress");}void PerformPartitionPostinstall(){s.calls.push_back("next");}};
'''
suffix=r'''
void check(const string&name,bool wanted,const std::vector<string>&calls,InstallPlan::Partition part={},std::vector<InstallPlan::Partition>extras={},int slot=0){PostinstallRunnerAction a;a.install_plan_.source_slot=slot;a.install_plan_.partitions=extras;assert(a.MountPartition(part)==wanted);assert(s.calls==calls);if(!s.args.empty()){assert(s.args.size()>=4);assert((std::vector<string>(s.args.begin(),s.args.begin()+4)==std::vector<string>{"/system/bin/backuptool_broker","--v2","target","source"}));}std::cout<<name<<" PASS\n";}
int main(){
 const std::vector<string>normal={"ensure","resolve:system","read","broker","ro:target","ro:target","mount_ro"};
 check("primary_success_no_parent_rw_ioctl",true,normal);
 for(int code:{1,-1,126,127,137}){s=State{};s.status=code;check("worker_failure_restores_ro_"+std::to_string(code),false,{"ensure","resolve:system","read","broker","ro:target"});}
 s=State{};check("extra_devices_passed_and_all_restored_ro",true,{"ensure","resolve:system","read","resolve:vendor","resolve:product","broker","ro:target","ro:target_vendor","ro:target_product","ro:target","mount_ro"},{},{{"vendor","target_vendor","erofs"},{"product","target_product",""}});assert((std::vector<string>(s.args.begin()+4,s.args.end())==std::vector<string>{"vendor","target_vendor","source_vendor","product","target_product","source_product"}));
 s=State{};s.ro_fail.insert("target_vendor");check("extra_ro_failure_does_not_skip_other_cleanup",false,{"ensure","resolve:system","read","resolve:vendor","resolve:product","broker","ro:target","ro:target_vendor","ro:target_product"},{},{{"vendor","target_vendor","erofs"},{"product","target_product",""}});
 s=State{};s.fail_resolve="vendor";check("extra_identity_failure_before_broker",false,{"ensure","resolve:system","read","resolve:vendor"},{},{{"vendor","target_vendor","erofs"}});
 s=State{};check("empty_extra_target_before_broker",false,{"ensure","resolve:system","read"},{},{{"vendor","","erofs"}});
 for(auto part:std::vector<InstallPlan::Partition>{{"vendor","target","erofs"},{"system","target","erofs"},{"product","target","ext4"}}){s=State{};check("normal_postinstall_"+part.name+part.filesystem_type,true,{"ensure","ro:target","mount_ro"},part);}
 s=State{};s.chunk={0,0,0,0,0x53,0xef};check("unmodified_source_skips_retention",true,{"ensure","resolve:system","read","ro:target","mount_ro"});
 s=State{};s.chunk={1,0,0,0,0,0};check("source_not_ext4_skips_retention",true,{"ensure","resolve:system","read","ro:target","mount_ro"});
 s=State{};s.resolve=false;check("source_resolution_failure",false,{"ensure","resolve:system"});
 s=State{};s.empty="system";check("empty_source_path",false,{"ensure","resolve:system"});
 s=State{};s.read=false;check("source_read_failure",false,{"ensure","resolve:system","read"});
 for(auto bytes:std::vector<std::vector<unsigned char>>{{},{1},{1,0},{1,0,0,0,0x53},{1,0,0,0,0x53,0xef,0}}){s=State{};s.chunk=bytes;check("read_size_"+std::to_string(bytes.size()),false,{"ensure","resolve:system","read"});}
 s=State{};check("no_source_slot",true,{"ensure","ro:target","mount_ro"},{},{},-1);
 s=State{};s.mount=false;check("normal_mount_failure",false,normal);
 for(bool optional:{false,true})for(int code:{0,1,3,4}){s=State{};PostinstallRunnerAction a;a.install_plan_.partitions={{"system","target","ext4",optional}};a.CompletePartitionPostinstall(code,"");assert(s.calls.front()=="cleanup");if(code&&!optional){assert(s.completed==code);assert(a.current_partition_==0);}else{assert(s.completed==-1);assert(a.current_partition_==1&&s.calls.back()=="next");}std::cout<<"optional_"<<optional<<"_status_"<<code<<" PASS\n";}
}
'''
(D/'parent.cpp').write_text(prefix+mount+optional+suffix)
q=subprocess.run(['taskset','-c','4-7','nice','-n','15','c++','-std=c++17','-Wall','-Wextra','-Werror','-Wno-unused-parameter','-D__ANDROID__','-DRUN_BACKUPTOOL',str(D/'parent.cpp'),'-o',str(D/'parent')],capture_output=True,text=True,timeout=40);(D/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stderr
q=subprocess.run([str(D/'parent')],capture_output=True,text=True,timeout=15);(D/'cases.log').write_text(q.stdout+q.stderr);assert q.returncode==0,(q.stdout,q.stderr)
r={'state':'ACTUAL_PARENT_METHODS_AND_OPTIONAL_CONTRACT_PASSED','cases':q.stdout.splitlines(),'count':len(q.stdout.splitlines()),'optional_handler_and_real_callers_byte_identical_to_baseline':True,'actual_mount_method_sha256':hashlib.sha256(mount.encode()).hexdigest(),'dependency_io_mocked':True,'real_ota_or_android_tested':False,'phone_accessed':False};(D/'report.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps({k:v for k,v in r.items() if k!='cases'}))
