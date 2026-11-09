package vendor.qti.iwlan;

import android.os.Message;
import java.util.ArrayList;
import java.util.IdentityHashMap;
import vendor.qti.hardware.data.iwlan.V1_0.IWlanResponseInfo;

/**
 * Request ownership repair for the exact IWlanService 13.5 prebuilt.
 *
 * The same source is compiled into the injected DEX and exercised by host tests.
 * State is attached to each proxy through one added volatile field. All pending
 * membership, lease ownership, counters and timer validation use State's monitor.
 * No internal lock crosses a HAL call or application callback. The original
 * service acquisition and wire-call lifecycle are intentionally preserved.
 */
public final class IWlanWakeLockFix {
    private static final int REQUEST = 0, RESPONSE = 1;
    private static final int REQUEST_TIMEOUT = 2, RESPONSE_TIMEOUT = 4;
    private static final long REQUEST_MS = 60000L, RESPONSE_MS = 200L;

    static final class Lease {
        final long epoch;
        Lease(long epoch) { this.epoch = epoch; }
    }

    public static final class State {
        final IdentityHashMap<IWlanRequest, Lease> owners = new IdentityHashMap<>();
        long ownershipEpoch;
    }


    private IWlanWakeLockFix() {}

    static State state(IWlanProxy proxy) {
        State result = proxy.mLeaseState;
        if (result == null) {
            synchronized (proxy) {
                result = proxy.mLeaseState;
                if (result == null) proxy.mLeaseState = result = new State();
            }
        }
        return result;
    }

    private static void log(IWlanProxy p, String tag, String message) {
        p.iwlanLog.w(tag, message);
    }

    private static void dispatch(Runnable action) { action.run(); }

    private static void suppress(Throwable first, Throwable next) {
        if (first != next) first.addSuppressed(next);
    }

    public static IWlanRequest obtain(IWlanProxy proxy, int request, Message result) {
        IWlanRequest req = new IWlanRequest(request, result);
        State s = state(proxy);
        RuntimeException failure = null;
        synchronized (s) {
            proxy.mRequestList.append(req.mSerial, req);
            try { acquireLocked(proxy, s, req, REQUEST); }
            catch (RuntimeException error) {
                // Claim terminal membership before State is released; reset cannot notify twice.
                proxy.mRequestList.remove(req.mSerial);
                releaseLocked(proxy, s, req);
                failure = error;
            }
        }
        if (failure != null) {
            try { failureCallback(proxy, req); }
            catch (RuntimeException | Error secondary) { suppress(failure, secondary); }
            throw failure;
        }
        return req;
    }

    public static void acquire(IWlanProxy proxy, IWlanRequest req, int type) {
        State s = state(proxy);
        synchronized (s) { acquireLocked(proxy, s, req, type); }
    }

    private static void acquireLocked(IWlanProxy p, State s, IWlanRequest req, int type) {
        if (req.mWakeLockType != -1) {
            throw new IllegalStateException("Request already owns a wake lock");
        }
        if (type != REQUEST && type != RESPONSE) {
            throw new IllegalArgumentException("Invalid wake lock type " + type);
        }
        if (type == RESPONSE) {
            boolean wasHeld = p.mRespWakeLock.isHeld();
            int next = p.mRespWlSequenceNum + 1;
            try {
                if (!wasHeld) p.mRespWakeLock.acquire();
                Message timer = p.mHandler.obtainMessage(RESPONSE_TIMEOUT);
                timer.arg1 = next;
                if (!p.mHandler.sendMessageDelayed(timer, RESPONSE_MS)) {
                    throw new IllegalStateException("Response watchdog rejected");
                }
                p.mRespWlSequenceNum = next;
                req.mWakeLockType = RESPONSE;
            } catch (RuntimeException error) {
                if (!wasHeld && p.mRespWakeLock.isHeld()) p.mRespWakeLock.release();
                throw error;
            }
            return;
        }
        boolean wasHeld = p.mWakeLock.isHeld();
        int next = p.mWlSequenceNum + 1;
        try {
            if (!wasHeld) p.mWakeLock.acquire();
            s.owners.put(req, new Lease(s.ownershipEpoch));
            req.mWakeLockType = REQUEST;
            p.mWakeLockCount = s.owners.size();
            Message timer = p.mHandler.obtainMessage(REQUEST_TIMEOUT);
            timer.arg1 = next;
            if (!p.mHandler.sendMessageDelayed(timer, REQUEST_MS)) {
                throw new IllegalStateException("Request watchdog rejected");
            }
            p.mWlSequenceNum = next;
        } catch (RuntimeException error) {
            s.owners.remove(req);
            req.mWakeLockType = -1;
            p.mWakeLockCount = s.owners.size();
            if (!wasHeld && p.mWakeLock.isHeld()) p.mWakeLock.release();
            throw error;
        }
    }

    public static void release(IWlanProxy proxy, IWlanRequest req) {
        State s = state(proxy);
        synchronized (s) { releaseLocked(proxy, s, req); }
    }

    private static void releaseLocked(IWlanProxy p, State s, IWlanRequest req) {
        Lease owner = s.owners.remove(req);
        req.mWakeLockType = -1;
        if (owner == null || owner.epoch != s.ownershipEpoch) return;
        p.mWakeLockCount = s.owners.size();
        if (s.owners.isEmpty() && p.mWakeLock.isHeld()) p.mWakeLock.release();
    }

    public static IWlanRequest take(IWlanProxy proxy, int serial) {
        State s = state(proxy);
        synchronized (s) { return takeLocked(proxy, s, serial); }
    }

    private static IWlanRequest takeLocked(IWlanProxy p, State s, int serial) {
        IWlanRequest req = (IWlanRequest) p.mRequestList.get(serial);
        if (req != null) {
            p.mRequestList.remove(serial);
        }
        return req;
    }

    public static void earlyAck(IWlanProxy proxy, int serial) {
        State s = state(proxy);
        boolean found;
        synchronized (s) {
            IWlanRequest req = (IWlanRequest) proxy.mRequestList.get(serial);
            found = req != null;
            if (found) releaseLocked(proxy, s, req);
        }
        log(proxy, IWlanProxy.TAG, found ? serial + " Recieved ACK for serial = " + serial
                : "processRequestAck: request not found for serial = " + serial);
    }

    private static void responseAck(IWlanProxy proxy, IWlanRequest req, boolean ack, String tag) {
        log(proxy, tag, req.mSerial + " > Response Processed");
        if (ack) {
            log(proxy, tag, req.mSerial + " > send ACK for serial");
            proxy.sendAck(tag);
        }
    }

    private static IWlanRequest responseRequest(IWlanProxy p, IWlanResponseInfo info, String tag) {
        IWlanRequest req = take(p, info.serial);
        if (req == null) log(p, tag, info.serial + " > processResponse: Unexpected response! error: " + info.error);
        return req;
    }

    /** All five production final helpers call this, including setup and void. */
    public static void complete(final IWlanProxy proxy, IWlanResponseInfo info,
            final Object payload, final boolean ack, final String tag) {
        final IWlanRequest req = responseRequest(proxy, info, tag);
        if (req == null) return;
        dispatch(new Runnable() {
            public void run() {
                try {
                    responseAck(proxy, req, ack, tag);
                    if (req.mResult != null) {
                        AsyncResult.forMessage(req.mResult, payload, null);
                        req.mResult.sendToTarget();
                    }
                } finally { release(proxy, req); }
            }
        });
    }

    /** Safe fallback for any original direct caller outside the five helpers. */
    public static IWlanRequest processResponse(final IWlanProxy proxy, IWlanResponseInfo info,
            final boolean ack, final String tag) {
        final IWlanRequest req = responseRequest(proxy, info, tag);
        if (req != null) dispatch(new Runnable() {
            public void run() {
                try { responseAck(proxy, req, ack, tag); }
                finally { release(proxy, req); }
            }
        });
        return req;
    }

    public static boolean clear(IWlanProxy proxy, int type) {
        State s = state(proxy);
        synchronized (s) { return clearLocked(proxy, s, type); }
    }

    private static boolean clearLocked(IWlanProxy p, State s, int type) {
        if (type == RESPONSE) {
            if (!p.mRespWakeLock.isHeld()) return false;
            p.mRespWakeLock.release();
            return true;
        }
        if (type != REQUEST) throw new IllegalArgumentException("Invalid wake lock type");
        boolean held = p.mWakeLock.isHeld() || !s.owners.isEmpty();
        if (!held) return false;
        for (IWlanRequest req : s.owners.keySet()) req.mWakeLockType = -1;
        s.owners.clear();
        s.ownershipEpoch++;
        p.mWakeLockCount = 0;
        if (p.mWakeLock.isHeld()) p.mWakeLock.release();
        return true;
    }

    /** Called before the original handler body; only the two timer events are consumed. */
    public static boolean handleEvent(IWlanProxy proxy, Message timer) {
        if (timer.what != REQUEST_TIMEOUT && timer.what != RESPONSE_TIMEOUT) return false;
        State s = state(proxy);
        boolean cleared = false;
        int previousCount = 0, pending = 0;
        synchronized (s) {
            if (timer.what == REQUEST_TIMEOUT && timer.arg1 == proxy.mWlSequenceNum) {
                previousCount = proxy.mWakeLockCount;
                cleared = clearLocked(proxy, s, REQUEST);
                pending = proxy.mRequestList.size();
            } else if (timer.what == RESPONSE_TIMEOUT && timer.arg1 == proxy.mRespWlSequenceNum) {
                cleared = clearLocked(proxy, s, RESPONSE);
            }
        }
        if (cleared) {
            if (timer.what == RESPONSE_TIMEOUT) log(proxy, IWlanProxy.TAG, "RESP_WAKE_LOCK_TIMEOUT");
            else {
                log(proxy, IWlanProxy.TAG, "NOTE: mWakeLockCount is " + previousCount + "at time of clearing");
                log(proxy, IWlanProxy.TAG, "WAKE_LOCK_TIMEOUT  mRequestList = " + pending);
            }
        }
        return true;
    }

    private static void failureCallback(final IWlanProxy proxy, final IWlanRequest req) {
        dispatch(new Runnable() {
            public void run() {
                try {
                    if (req.mResult != null) {
                        AsyncResult.forMessage(req.mResult, null,
                                new CommandException(CommandException.Error.RADIO_NOT_AVAILABLE));
                        req.mResult.sendToTarget();
                    }
                } finally { release(proxy, req); }
            }
        });
    }

    /** Inserted in the existing six HAL exception handlers, after the original HAL call unwinds. */
    public static void failRequest(IWlanProxy proxy, IWlanRequest req, Throwable cause) {
        State s = state(proxy);
        boolean detached;
        synchronized (s) {
            detached = proxy.mRequestList.get(req.mSerial) == req;
            if (detached) takeLocked(proxy, s, req.mSerial);
        }
        if (detached) {
            try { failureCallback(proxy, req); }
            catch (RuntimeException | Error callbackError) {
                suppress(cause, callbackError);
                log(proxy, IWlanProxy.TAG, "Failure callback dispatch failed for serial " + req.mSerial);
            }
        }
    }

    /** Original reset wrapper reconnects using unchanged getService() after this returns. */
    public static void reset(IWlanProxy proxy) {
        State s = state(proxy);
        ArrayList<IWlanRequest> pending = new ArrayList<>();
        synchronized (s) {
            proxy.mService = null;
            proxy.mServiceCookie.incrementAndGet();
            for (int i = 0; i < proxy.mRequestList.size(); i++) {
                pending.add((IWlanRequest) proxy.mRequestList.valueAt(i));
            }
            proxy.mRequestList.clear();
            if (!clearLocked(proxy, s, REQUEST)) s.ownershipEpoch++;
            proxy.mWlSequenceNum++;
        }
        Throwable first = null;
        for (IWlanRequest req : pending) {
            try { failureCallback(proxy, req); }
            catch (RuntimeException | Error error) {
                if (first == null) first = error;
                else suppress(first, error);
            }
        }
        if (first != null) log(proxy, IWlanProxy.TAG, "Reset callback dispatch failed");
    }
}
