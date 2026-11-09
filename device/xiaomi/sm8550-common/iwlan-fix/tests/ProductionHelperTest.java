package vendor.qti.iwlan;

import android.os.Message;
import android.os.RemoteException;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import vendor.qti.hardware.data.iwlan.V1_0.IWlanResponseInfo;
import vendor.qti.hardware.data.iwlan.V1_1.IIWlan;

/** Executes the production source, not a second implementation of its state machine. */
public final class ProductionHelperTest {
    interface Checked { void run() throws Exception; }
    static final List<String> passed = new ArrayList<>();
    static void check(boolean ok, String why) { if (!ok) throw new AssertionError(why); }
    static void test(String name, Checked body) throws Exception {
        body.run(); passed.add(name); System.out.println("PASS " + name);
    }
    static void await(CountDownLatch latch) {
        try { check(latch.await(3, TimeUnit.SECONDS), "barrier deadline"); }
        catch (InterruptedException e) { throw new AssertionError(e); }
    }
    static final class Task {
        final AtomicReference<Throwable> error = new AtomicReference<>();
        final Thread thread;
        Task(Runnable body) { thread = new Thread(() -> {try{body.run();}catch(Throwable t){error.set(t);}}); thread.setDaemon(true);thread.start(); }
        void done() throws Exception {thread.join(4000);check(!thread.isAlive(),"thread deadlock");if(error.get()!=null)throw new AssertionError(error.get());}
    }
    static final class Hal implements IIWlan {
        volatile int calls, acks;
        Runnable action;
        RemoteException remoteError;
        Object[] wire;
        void called(Object... args) throws RemoteException { calls++;wire=args;if(action!=null)action.run();if(remoteError!=null)throw remoteError; }
        public void deactivateDataCall(int s,int c,int r)throws RemoteException{called(s,c,r);}
        public void getDataCallList(int s)throws RemoteException{called(s);}
        public void getDataRegistrationState(int s)throws RemoteException{called(s);}
        public void getAllQualifiedNetworks(int s)throws RemoteException{called(s);}
        public void setupDataCall(int s,android.hardware.radio.V1_4.DataProfileInfo p,boolean r,int reason,ArrayList<String>a,ArrayList<String>d)throws RemoteException{called(s,p,r,reason,a,d);}
        public void setupDataCall_1_1(int s,android.hardware.radio.V1_5.DataProfileInfo p,boolean r,int reason,ArrayList<String>a,ArrayList<String>d,android.hardware.radio.V1_6.OptionalTrafficDescriptor t,boolean m)throws RemoteException{called(s,p,r,reason,a,d,t,m);}
        public void responseAcknowledgement(){acks++;}
    }
    static IWlanProxy proxy() {IWlanProxy p=new IWlanProxy();p.mService=new Hal();return p;}
    static Message callback(IWlanProxy p, Runnable body) {
        Message m=new Message();m.onSend=ignored->{
            IWlanWakeLockFix.State s=IWlanWakeLockFix.state(p);
            check(!Thread.holdsLock(s),"callback under state lock");
            if(body!=null)body.run();
        };return m;
    }
    static IWlanRequest obtain(IWlanProxy p,Message m){return IWlanWakeLockFix.obtain(p,1,m);}
    static IWlanResponseInfo info(IWlanRequest r){IWlanResponseInfo i=new IWlanResponseInfo();i.serial=r.mSerial;return i;}
    static Message timer(IWlanProxy p,int what){Message m=new Message();m.what=what;m.arg1=what==2?p.mWlSequenceNum:p.mRespWlSequenceNum;return m;}
    static void invariant(IWlanProxy p,int owners,int pending){
        IWlanWakeLockFix.State s=IWlanWakeLockFix.state(p);
        synchronized(s){check(p.mWakeLockCount==owners,"count");check(s.owners.size()==owners,"owners");check(p.mRequestList.size()==pending,"pending");check(p.mWakeLock.isHeld()==(owners>0),"physical request lock");}
    }
    static void finish(IWlanProxy p,IWlanRequest r,boolean ack){IWlanWakeLockFix.complete(p,info(r),new Object(),ack,"test");}
    static void cancelled(Checked action){try{action.run();throw new AssertionError("expected cancellation");}catch(IllegalStateException expected){}catch(Exception e){throw new AssertionError(e);}}

    public static void main(String[] args) throws Exception {
        test("final-without-early-ack-true-and-false",()->{
            for(boolean ack:new boolean[]{false,true}){IWlanProxy p=proxy();Message m=callback(p,()->check(p.mWakeLock.isHeld(),"lease covers dispatch"));IWlanRequest r=obtain(p,m);finish(p,r,ack);invariant(p,0,0);check(m.sent==1,"completion once");check(p.ackCalls==(ack?1:0),"ACK contract");if(ack){check(p.mRespWakeLock.isHeld(),"response lock");IWlanWakeLockFix.handleEvent(p,timer(p,4));check(!p.mRespWakeLock.isHeld(),"200ms response clear");}}
        });
        test("early-duplicate-and-late-ack-final-once",()->{
            IWlanProxy p=proxy();Message m=callback(p,null);IWlanRequest r=obtain(p,m);IWlanWakeLockFix.earlyAck(p,r.mSerial);IWlanWakeLockFix.earlyAck(p,r.mSerial);invariant(p,0,1);finish(p,r,true);IWlanWakeLockFix.earlyAck(p,r.mSerial);finish(p,r,true);invariant(p,0,0);check(m.sent==1&&p.ackCalls==1,"terminal exactly once");
        });
        test("concurrent-ack-final-300-interleavings",()->{
            for(int n=0;n<300;n++){IWlanProxy p=proxy();Message m=callback(p,null);IWlanRequest r=obtain(p,m);CountDownLatch go=new CountDownLatch(1);Task a=new Task(()->{await(go);IWlanWakeLockFix.earlyAck(p,r.mSerial);});Task b=new Task(()->{await(go);finish(p,r,false);});go.countDown();a.done();b.done();invariant(p,0,0);check(m.sent==1,"one dispatch");}
        });
        test("same-epoch-two-requests-and-stale-timer",()->{
            IWlanProxy p=proxy();IWlanRequest a=obtain(p,null);Message old=timer(p,2);IWlanRequest b=obtain(p,null);long e=IWlanWakeLockFix.state(p).ownershipEpoch;IWlanWakeLockFix.handleEvent(p,old);invariant(p,2,2);IWlanWakeLockFix.earlyAck(p,a.mSerial);invariant(p,1,2);check(e==IWlanWakeLockFix.state(p).ownershipEpoch,"new acquire must not advance epoch");finish(p,a,false);finish(p,b,false);invariant(p,0,0);
        });
        test("timeout-then-old-ack-does-not-consume-new-lease",()->{
            IWlanProxy p=proxy();IWlanRequest a=obtain(p,null);long e=IWlanWakeLockFix.state(p).ownershipEpoch;IWlanWakeLockFix.handleEvent(p,timer(p,2));invariant(p,0,1);check(IWlanWakeLockFix.state(p).ownershipEpoch==e+1,"epoch advanced");IWlanRequest b=obtain(p,null);IWlanWakeLockFix.earlyAck(p,a.mSerial);invariant(p,1,2);finish(p,a,false);invariant(p,1,1);finish(p,b,false);invariant(p,0,0);
        });
        test("detached-dispatch-finally-after-timeout-cannot-release-new-epoch",()->{
            IWlanProxy p=proxy();CountDownLatch inside=new CountDownLatch(1),leave=new CountDownLatch(1);Message m=callback(p,()->{inside.countDown();await(leave);});IWlanRequest a=obtain(p,m);Task old=new Task(()->finish(p,a,false));await(inside);invariant(p,1,0);IWlanWakeLockFix.handleEvent(p,timer(p,2));IWlanRequest b=obtain(p,null);leave.countDown();old.done();invariant(p,1,1);finish(p,b,false);invariant(p,0,0);
        });
        test("timeout-check-clear-atomic-with-acquire",()->{
            IWlanProxy p=proxy();IWlanRequest a=obtain(p,null);Message old=timer(p,2);CountDownLatch scheduled=new CountDownLatch(1),allow=new CountDownLatch(1);p.mHandler.onSchedule=()->{scheduled.countDown();await(allow);};AtomicReference<IWlanRequest> b=new AtomicReference<>();Task acq=new Task(()->b.set(obtain(p,null)));await(scheduled);Task timeout=new Task(()->IWlanWakeLockFix.handleEvent(p,old));allow.countDown();acq.done();timeout.done();p.mHandler.onSchedule=null;invariant(p,2,2);finish(p,a,false);finish(p,b.get(),false);invariant(p,0,0);
        });
        test("callback-runtime-exception-finally-releases",()->{
            IWlanProxy p=proxy();RuntimeException boom=new RuntimeException("dispatch");Message m=callback(p,()->{throw boom;});IWlanRequest r=obtain(p,m);try{finish(p,r,true);throw new AssertionError("missing throw");}catch(RuntimeException e){check(e==boom,"same failure");}invariant(p,0,0);
        });
        test("response-ack-exception-finally-releases",()->{
            IWlanProxy p=proxy();p.ackFailure=new RuntimeException("ack");Message m=callback(p,null);IWlanRequest r=obtain(p,m);try{finish(p,r,true);throw new AssertionError();}catch(RuntimeException expected){}invariant(p,0,0);check(m.sent==0,"failure before dispatch");
        });
        test("request-watchdog-rejection-rolls-back-only-new-reservation",()->{
            IWlanProxy p=proxy();IWlanRequest a=obtain(p,null);Message old=timer(p,2);Message failed=callback(p,null);p.mHandler.reject=true;try{obtain(p,failed);throw new AssertionError();}catch(IllegalStateException expected){}p.mHandler.reject=false;invariant(p,1,1);check(p.mWlSequenceNum==old.arg1&&failed.sent==1,"prior timer retained/failure delivered");finish(p,a,false);invariant(p,0,0);
        });
        test("response-schedule-exception-never-releases-request-lock",()->{
            IWlanProxy p=proxy();IWlanRequest a=obtain(p,null);p.mHandler.scheduleFailure=new IllegalStateException("schedule");try{IWlanWakeLockFix.acquire(p,new IWlanRequest(50,null),1);throw new AssertionError();}catch(IllegalStateException expected){}invariant(p,1,1);check(!p.mRespWakeLock.isHeld(),"correct response rollback");p.mHandler.scheduleFailure=null;finish(p,a,false);
        });
        test("send-error-detaches-notifies-and-releases-once",()->{
            IWlanProxy p=proxy();Message m=callback(p,null);IWlanRequest r=obtain(p,m);Hal h=(Hal)p.mService;h.remoteError=new RemoteException("send");try{h.getDataCallList(r.mSerial);throw new AssertionError();}catch(RemoteException e){IWlanWakeLockFix.failRequest(p,r,e);}invariant(p,0,0);check(m.sent==1&&((AsyncResult)m.obj).exception instanceof CommandException,"terminal radio error");IWlanWakeLockFix.failRequest(p,r,new RuntimeException());check(m.sent==1,"failure once");
        });
        test("reset-all-callbacks-even-shared-error-instance",()->{
            IWlanProxy p=proxy();Error shared=new AssertionError("same error");Message a=callback(p,()->{throw shared;}),b=callback(p,()->{throw shared;}),c=callback(p,null);obtain(p,a);obtain(p,b);obtain(p,c);IWlanWakeLockFix.reset(p);invariant(p,0,0);check(a.sent==1&&b.sent==1&&c.sent==1,"reset drains every caller");
        });
        test("reset-invalidates-detached-finally-and-cookie",()->{
            IWlanProxy p=proxy();CountDownLatch entered=new CountDownLatch(1),exit=new CountDownLatch(1);Message m=callback(p,()->{entered.countDown();await(exit);});IWlanRequest a=obtain(p,m);Task finalTask=new Task(()->finish(p,a,false));await(entered);long cookie=p.mServiceCookie.get();IWlanWakeLockFix.reset(p);p.mService=new Hal();IWlanRequest b=obtain(p,null);exit.countDown();finalTask.done();invariant(p,1,1);check(p.mServiceCookie.get()==cookie+1,"reset cookie");finish(p,b,false);
        });
        test("independent-proxies-have-independent-timers-and-leases",()->{
            IWlanProxy a=proxy(),b=proxy();IWlanRequest x=obtain(a,null),y=obtain(b,null);IWlanWakeLockFix.handleEvent(a,timer(a,2));invariant(a,0,1);invariant(b,1,1);finish(a,x,false);finish(b,y,false);check(a.mHandler.delays.get(0)==60000L&&b.mHandler.delays.get(0)==60000L,"60s preserved");
        });
        test("cross-thread-final-ACK-getService-no-internal-lock-deadlock",()->{
            IWlanProxy p=proxy();Hal h=(Hal)p.mService;Message m=callback(p,null);IWlanRequest r=obtain(p,m);
            h.action=()->{Task callback=new Task(()->finish(p,r,true));try{callback.done();}catch(Exception e){throw new AssertionError(e);}};
            h.getDataCallList(r.mSerial);invariant(p,0,0);check(m.sent==1&&p.ackCalls==1&&h.acks==1,"response ACK via cached getService");
        });
        test("synchronous-final-before-send-throw-notifies-once",()->{
            IWlanProxy p=proxy();Hal h=(Hal)p.mService;Message m=callback(p,null);IWlanRequest r=obtain(p,m);h.action=()->finish(p,r,true);h.remoteError=new RemoteException("after final");
            try{h.getDataCallList(r.mSerial);}catch(RemoteException e){IWlanWakeLockFix.failRequest(p,r,e);}invariant(p,0,0);check(m.sent==1,"no duplicate failure");
        });
        test("reset-old-inflight-RPC-results-cannot-corrupt-new-epoch",()->{
            IWlanProxy p=proxy();Message old=callback(p,null);IWlanRequest a=obtain(p,old);IWlanWakeLockFix.reset(p);p.mService=new Hal();IWlanRequest b=obtain(p,null);
            finish(p,a,true);IWlanWakeLockFix.earlyAck(p,a.mSerial);IWlanWakeLockFix.failRequest(p,a,new RuntimeException("late send error"));invariant(p,1,1);check(old.sent==1&&p.ackCalls==0,"old completion ignored");finish(p,b,false);invariant(p,0,0);
        });
        test("preparation-exception-terminal-cleanup",()->{
            IWlanProxy p=proxy();Message m=callback(p,null);IWlanRequest r=obtain(p,m);RuntimeException conversion=new IllegalArgumentException("profile conversion");IWlanWakeLockFix.failRequest(p,r,conversion);invariant(p,0,0);check(m.sent==1,"preparation failure callback");
        });
        test("failed-acquire-racing-reset-notifies-once",()->{
            IWlanProxy p=proxy();AtomicInteger notifications=new AtomicInteger();Message m=callback(p,notifications::incrementAndGet);
            CountDownLatch entered=new CountDownLatch(1),leave=new CountDownLatch(1);p.mHandler.reject=true;
            p.mHandler.onSchedule=()->{entered.countDown();await(leave);};
            Task acquire=new Task(()->{try{obtain(p,m);throw new AssertionError();}catch(IllegalStateException expected){}});
            await(entered);Task reset=new Task(()->IWlanWakeLockFix.reset(p));leave.countDown();acquire.done();reset.done();
            invariant(p,0,0);check(m.sent==1&&notifications.get()==1,"failed acquire claimed terminal before reset");
        });
        test("reset-versus-final-200-interleavings-notifies-once",()->{
            for(int n=0;n<200;n++){
                IWlanProxy p=proxy();AtomicInteger notifications=new AtomicInteger();Message m=callback(p,notifications::incrementAndGet);IWlanRequest r=obtain(p,m);CountDownLatch go=new CountDownLatch(1);
                Task reset=new Task(()->{await(go);IWlanWakeLockFix.reset(p);});Task last=new Task(()->{await(go);finish(p,r,false);});go.countDown();reset.done();last.done();
                invariant(p,0,0);check(m.sent==1&&notifications.get()==1,"terminal claim unique");
            }
        });
        System.out.println("RESULT {\"passed\":"+passed.size()+",\"failed\":0,\"productionSourceTested\":true,\"raceIterations\":500}");
    }
}
