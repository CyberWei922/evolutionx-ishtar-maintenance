# IWLAN request wake-lock repair

This directory patches one exact Qualcomm IWlanService 13.5 prebuilt. It changes
request ownership and error completion, and preserves package/shared UID,
manifest/resources, HIDL service names and every original HAL wire invocation,
60,000 ms request watchdog, and 200 ms response ACK watchdog. ROM platform
signing is a later build step.

The original APK SHA256 is
`909d14bff0bc85943ead69f08bb795b56736256502466b522c80b30a70dad2ef`.
Its only DEX SHA256 is
`626a72021bb02388bf1f7e667887957241970eea52d02edc5d4bd65ff34cc22b`.
The reviewed unsigned payload SHA256 is
`c66f190fd4aa8f010a8f7d03a157e57bd5d379b125e24d1b4282d987a5047a68`.

## Fast extraction fixup

Keep `apply.py` and `payload/IWlanService-unsigned.apk` together. It uses only
Python's standard library and can run on Linux without Mac paths or Java:

```sh
python3 apply.py /path/to/extracted/IWlanService.apk
```

Only the exact original input is replaced, atomically. The exact reviewed
patched input is a no-op; unknown APK hashes fail closed. `--output FILE`
creates an independent output for review.

## Rebuild from source

`build.py` compiles the production Java helper and host tests, runs the tests,
compiles the helper with D8, rewrites the original DEX with dexlib2 without
optimizing unrelated methods, and assembles/alines the unsigned APK. Set these
paths for the existing host tool installation:

```sh
python3 build.py --input /path/to/original/IWlanService.apk \
  --java-home /path/to/jdk21 \
  --build-tools /path/to/android/build-tools/37.0.0 \
  --android-jar /path/to/android-37/android.jar \
  --dexlib2 /path/to/dexlib2-2.5.2.jar \
  --guava /path/to/guava-32.1.3-jre.jar
```

Equivalent `IWLAN_JAVA_HOME`, `IWLAN_BUILD_TOOLS`, `IWLAN_ANDROID_JAR`,
`IWLAN_DEXLIB2`, and `IWLAN_GUAVA` environment variables are supported. Default
paths describe the reviewed Mac installation; no downloads/installers run.
Exact dependency/source/result hashes are in `out/build-ledger.json` and
`artifact-lock.json`. Rebuild output must match the reviewed unsigned SHA.

`verify.py --original ORIGINAL --patched OUTPUT` is a fast standard-library
check for the reviewed DEX hashes, DEX checksums/class inventory, and all
unrelated ZIP member content. It accepts later platform signing metadata but
does not replace signature verification. The detailed symbolic DEX whitelist
check can also be rerun using compiled `PatchIwlan` with
`original.dex patched.dex report.json --verify`.

## Changes and invariants

- One volatile per-proxy `mLeaseState` field and six helper classes are added.
  Twenty original methods are changed. The other 7,585 original methods,
  including the complete original `getService`, retain their symbolic code.
- Pending membership, leases, epoch and watchdog checks share one State monitor.
  Acquire and append are atomic relative to timeout. A watchdog validates its
  sequence and clears leases in the same critical section.
- Every request lease is consumed at most once. Aggregate clear/reset advances
  an independent ownership epoch and invalidates all active owners, including
  requests already removed for callback dispatch. A late ACK/final/failure
  cannot consume a newer request's lease.
- All five final response helpers use the same production completion function.
  Its `finally` covers response ACK and result dispatch. Early ACK remains an
  optional early release; final completion is the fallback. Response ACK keeps
  its separate 200 ms lock and original HAL call.
- The six send methods preserve original HAL instructions/arguments. Existing
  catches first claim terminal membership and dispatch RADIO_NOT_AVAILABLE;
  added catch-all coverage handles preparation/conversion errors after obtain,
  cleans up, and rethrows the original preparation error.
- Reset claims queued requests and clears ownership inside State, then notifies
  every caller outside it, even if an earlier callback throws. A callback cannot
  cause another terminal claimant to notify the same request twice.
- No new lock crosses HAL IPC, original service acquisition, or external
  callbacks. The pre-existing service acquisition/reset/in-flight RPC window is
  intentionally preserved; this patch does not promise to cancel a wire call
  already crossing that window. Late results are harmless to ownership and
  terminal notification state. It does not change WFC or retry configuration.

The original watchdog sequence fields are **volatile**. The earlier 287 review's
plain-int description was inaccurate; volatility does not make check-then-clear
atomic and does not provide per-request ownership epochs.

All non-DEX ZIP content is byte-identical except removal of the three obsolete
v1 signing members `META-INF/CERT.SF`, `CERT.RSA`, and `MANIFEST.MF`. ZIP container
metadata/compression/alignment is rebuilt. No host stub is injected.

## Validation boundary

Host tests execute the exact production helper source with host Android/HIDL
stubs. They cover normal/absent/duplicate/late ACKs, concurrent final/reset,
timeouts, stale owners, callback and scheduling errors, and cross-thread final
response ACK through `getService`. They do not execute the APK or emulate radio
transport. Actual bytecode catch ranges/registers are checked separately in the
DEX readback. ROM integration must still verify platform signing, ART against
the matching boot classpath, and normal telephony behavior before claiming a
device or battery-life repair.

`tools/NegativeControl.java` and `out/negative-control` deliberately corrupt a
helper return opcode solely to validate the offline ART verifier. They are
excluded from `payload` and every normal build output.
