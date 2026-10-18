from pathlib import Path
import subprocess,json,os,time,signal
C=Path('/mnt/android-download/control/ishtar-selinux-backuptool-final-20261019-01')
S=C/'source-candidate06'; B=S/'after/system/update_engine/backuptool_broker'; D=S/'worker-tests02';D.mkdir(exist_ok=False)
fixture=r'''#include "worker.h"
#include <linux/audit.h>
#include <linux/seccomp.h>
#include <linux/sched.h>
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>
using namespace backuptool_broker;
uint32_t evaluate(uint32_t arch,uint32_t number,uint64_t flags){
 seccomp_data data{};data.arch=arch;data.nr=number;data.args[0]=flags;
 auto code=WorkerFilter();uint32_t a=0;
 for(size_t i=0;i<code.size();i++){auto f=code[i];switch(f.code){
 case BPF_LD|BPF_W|BPF_ABS:std::memcpy(&a,reinterpret_cast<char*>(&data)+f.k,4);break;
 case BPF_ALU|BPF_AND|BPF_K:a&=f.k;break;
 case BPF_JMP|BPF_JEQ|BPF_K:i+=a==f.k?f.jt:f.jf;break;
 case BPF_RET|BPF_K:return f.k;
 default:assert(false);
 }}assert(false);return 0;
}
int main(int argc,char**argv){
 if(argc==2&&std::string(argv[1])=="bpf"){
  struct Arch{uint32_t a,pgid,sid,clone,unshare,setns;};
  int count=0;
  for(auto x:{Arch{AUDIT_ARCH_AARCH64,154,157,220,97,268},Arch{AUDIT_ARCH_ARM,57,66,120,337,375},Arch{AUDIT_ARCH_X86_64,109,112,56,272,308},Arch{AUDIT_ARCH_I386,57,66,120,310,346}}){
   for(auto n:{x.pgid,x.sid,x.unshare,x.setns}){assert(evaluate(x.a,n,0)==(SECCOMP_RET_ERRNO|EPERM));count++;}
   assert(evaluate(x.a,435,0)==(SECCOMP_RET_ERRNO|ENOSYS));count++;
   assert(evaluate(x.a,x.clone,17)==SECCOMP_RET_ALLOW);count++;
   for(uint32_t f:{CLONE_NEWNS,CLONE_NEWCGROUP,CLONE_NEWUTS,CLONE_NEWIPC,CLONE_NEWUSER,CLONE_NEWPID,CLONE_NEWNET}){assert(evaluate(x.a,x.clone,f|17)==(SECCOMP_RET_ERRNO|EPERM));count++;}
   assert(evaluate(x.a,1,0)==SECCOMP_RET_ALLOW);count++;
  }
  assert(evaluate(AUDIT_ARCH_ARM,0x900000|66,0)==(SECCOMP_RET_ERRNO|EPERM));count++;
  assert(evaluate(AUDIT_ARCH_X86_64,0x40000000|112,0)==(SECCOMP_RET_ERRNO|EPERM));count++;
  assert(evaluate(0xffffffff,1,0)==SECCOMP_RET_KILL_PROCESS);count++;
  std::cout<<count<<"\n";return 0;
 }
 assert(argc==4);WorkerFixture f{argv[1],argv[2],"/usr/bin:/bin"};
 f.fail_filter=std::string(argv[3])=="filter";f.fail_context=std::string(argv[3])=="context";
 return RunStagedWorkerForTest(f);
}
'''
(D/'driver.cpp').write_text(fixture)
lib='/mnt/android-build/Android/out/evolution-bka/host/linux-x86/lib64'
q=subprocess.run(['c++','-std=c++17','-Wall','-Wextra','-Werror','-I'+str(B),'-I/mnt/android-download/git-cache/evolution/external/selinux/libselinux/include',str(B/'worker.cpp'),str(D/'driver.cpp'),'-L'+lib,'-Wl,-rpath,'+lib,'-l:libselinux.so','-o',str(D/'driver')],capture_output=True,text=True,timeout=60)
(D/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stderr
q=subprocess.run([str(D/'driver'),'bpf'],capture_output=True,text=True,timeout=10);assert q.returncode==0,q.stderr
bpf_count=int(q.stdout);cases=[]
def script(name,body):
 p=D/(name+'.sh');p.write_text('#!/bin/sh\n'+body+'\n');p.chmod(0o755);return p
def run(name,body,want,mode='normal'):
 p=script(name,body);q=subprocess.run([str(D/'driver'),str(p),str(D),mode],capture_output=True,text=True,timeout=15)
 (D/(name+'.log')).write_text(q.stdout+q.stderr);assert q.returncode==want,(name,q.returncode,q.stdout,q.stderr);cases.append(name)
run('success','test "$BACKUPTOOL_BROKER_PROTOCOL" = 2 && test "$BACKUPTOOL_BROKER_WORKSPACE" = 1',0)
run('preserve-error','exit 37',37)
run('preserve-signal','kill -TERM $$',143)
run('filter-failure','exit 0',125,'filter')
run('context-failure','exit 0',126,'context')
# The inherited real host filter must survive both exec and ordinary fork.
probe=D/'probe.py';probe.write_text('''import os,errno,ctypes
for fn in [lambda:os.setsid(),lambda:os.setpgid(0,0)]:
 try: fn();raise AssertionError("escaped process group")
 except OSError as e: assert e.errno==errno.EPERM
lib=ctypes.CDLL(None,use_errno=True)
for number in [272,308]:
 assert lib.syscall(number,0,0)==-1 and ctypes.get_errno()==errno.EPERM
assert lib.syscall(435,0,0)==-1 and ctypes.get_errno()==errno.ENOSYS
p=os.fork()
if not p:
 try:os.setsid();os._exit(2)
 except OSError as e:os._exit(0 if e.errno==errno.EPERM else 3)
assert os.waitpid(p,0)[1]==0
''')
run('inherited-seccomp-fork-exec','exec /usr/bin/python3 '+str(probe),0)
def running(pid):
 try:return Path('/proc/'+str(pid)+'/stat').read_text().split(') ',1)[1][0] not in 'ZX'
 except FileNotFoundError:return False
bg=D/'background.pid'
run('background-descendant-drained','sleep 120 &\necho $! > '+str(bg)+'\nexit 0',0)
assert not running(int(bg.read_text()))
# Kill only the fresh fixture broker PID; parent-death cleanup targets its own PGID.
leaf=D/'parent-death.pid';p=script('parent-death','sleep 120 &\necho $! > '+str(leaf)+'\nwait')
q=subprocess.Popen([str(D/'driver'),str(p),str(D),'normal'],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
for _ in range(300):
 if leaf.exists():break
 time.sleep(.01)
assert leaf.exists();pid=int(leaf.read_text());assert running(pid)
q.kill();q.wait(timeout=5)
for _ in range(300):
 if not running(pid):break
 time.sleep(.01)
assert not running(pid);cases.append('broker-parent-death-private-group-cleanup')
r={'state':'ACTUAL_SECCOMP_PRIVATE_GROUP_HOST_TESTS_PASSED','bpf_semantic_cases':bpf_count,'lifecycle_cases':cases,'count':bpf_count+len(cases),'real_host_seccomp_fork_exec_background_and_parent_death':True,'android_selinux_transition_tested':False,'host_only_no_new_privs':True,'kernel_pid_namespace_required':False,'phone_accessed':False}
(D/'report.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r))
