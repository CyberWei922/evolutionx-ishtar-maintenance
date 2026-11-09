#!/usr/bin/env python3
"""Reproduce the IWLAN lifecycle fix from one exact prebuilt. No device commands."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import zipfile

HERE = Path(__file__).resolve().parent
ORIGINAL_SHA256 = "909d14bff0bc85943ead69f08bb795b56736256502466b522c80b30a70dad2ef"
ORIGINAL_DEX_SHA256 = "626a72021bb02388bf1f7e667887957241970eea52d02edc5d4bd65ff34cc22b"
PATCHED_SHA256 = "c66f190fd4aa8f010a8f7d03a157e57bd5d379b125e24d1b4282d987a5047a68"
SIGNATURE_MEMBERS = {"META-INF/CERT.SF", "META-INF/CERT.RSA", "META-INF/MANIFEST.MF"}


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def run(argv, log, **kwargs):
    result = subprocess.run([str(x) for x in argv], stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, text=True, **kwargs)
    Path(log).write_text(result.stdout)
    if result.returncode:
        raise RuntimeError(f"exit {result.returncode}: {argv[0]}; see {log}")
    return result.stdout


def zip_classes(root, destination, helper_only=False):
    with zipfile.ZipFile(destination, "w", compression=zipfile.ZIP_STORED) as z:
        for path in sorted(root.rglob("*.class")):
            name = path.relative_to(root).as_posix()
            if helper_only and not name.startswith("vendor/qti/iwlan/IWlanWakeLockFix"):
                continue
            info = zipfile.ZipInfo(name, (1980, 1, 1, 0, 0, 0))
            info.external_attr = 0o100644 << 16
            z.writestr(info, path.read_bytes())


def verify_members(original, patched):
    with zipfile.ZipFile(original) as a, zipfile.ZipFile(patched) as b:
        before, after = set(a.namelist()), set(b.namelist())
        if before - after != SIGNATURE_MEMBERS or after - before != {"classes2.dex"}:
            raise RuntimeError("Unexpected APK member additions/deletions")
        unchanged = []
        for name in sorted(before - SIGNATURE_MEMBERS - {"classes.dex"}):
            if a.read(name) != b.read(name):
                raise RuntimeError("Unrelated APK member changed: " + name)
            unchanged.append({"name": name, "sha256": hashlib.sha256(a.read(name)).hexdigest()})
        return {"non_dex_content_bytes_unchanged": unchanged,
                "removed_old_signature_members": sorted(SIGNATURE_MEMBERS),
                "added_dex_members": ["classes2.dex"]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, default=HERE / "out/IWlanService-unsigned.apk")
    parser.add_argument("--java-home", type=Path, default=os.environ.get("IWLAN_JAVA_HOME", os.environ.get("JAVA_HOME", "/Users/wei/Library/Java/JavaVirtualMachines/jbr-21.0.11/Contents/Home")))
    parser.add_argument("--build-tools", type=Path, default=os.environ.get("IWLAN_BUILD_TOOLS", "/Users/wei/Library/Android/sdk/build-tools/37.0.0"))
    parser.add_argument("--android-jar", type=Path, default=os.environ.get("IWLAN_ANDROID_JAR", "/Users/wei/Library/Android/sdk/platforms/android-37.0/android.jar"))
    parser.add_argument("--dexlib2", type=Path, default=os.environ.get("IWLAN_DEXLIB2", "/Users/wei/Tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/FileFormats/lib/dexlib2-2.5.2.jar"))
    parser.add_argument("--guava", type=Path, default=os.environ.get("IWLAN_GUAVA", "/Users/wei/Tools/ghidra_12.1.4_PUBLIC/Ghidra/Framework/Generic/lib/guava-32.1.3-jre.jar"))
    args = parser.parse_args()
    args.input, args.output = args.input.resolve(), args.output.resolve()
    input_hash = sha(args.input)
    if input_hash == PATCHED_SHA256:
        if args.input != args.output:
            args.output.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(args.input, args.output)
        print(json.dumps({"status": "already-patched-exact-hash", "sha256": input_hash}))
        return
    if input_hash != ORIGINAL_SHA256:
        raise RuntimeError("Refusing unknown APK SHA256: " + input_hash)
    java, javac = args.java_home / "bin/java", args.java_home / "bin/javac"
    for path in (java, javac, args.dexlib2, args.guava, args.android_jar,
                 args.build_tools / "d8", args.build_tools / "dexdump", args.build_tools / "zipalign"):
        if not path.is_file():
            raise RuntimeError("Required existing dependency missing: " + str(path))
    out = HERE / "out"
    out.mkdir(exist_ok=True)
    for sub in ("host", "tools", "helper-dex"):
        target = out / sub
        if target.exists(): shutil.rmtree(target)
        target.mkdir()
    with zipfile.ZipFile(args.input) as z:
        dex = z.read("classes.dex")
    if hashlib.sha256(dex).hexdigest() != ORIGINAL_DEX_SHA256:
        raise RuntimeError("Original DEX hash mismatch")
    (out / "original.dex").write_bytes(dex)
    sources = sorted([p for folder in ("src", "host-stubs", "tests")
                      for p in (HERE / folder).rglob("*.java")])
    run([javac, "--release", "8", "-g", "-Xlint:-options", "-d", out / "host", *sources], out / "javac-helper.log")
    test_log = run([java, "-ea", "-cp", out / "host", "vendor.qti.iwlan.ProductionHelperTest"], out / "host-tests.log", timeout=60)
    result_lines = [x for x in test_log.splitlines() if x.startswith("RESULT ")]
    if len(result_lines) != 1: raise RuntimeError("Missing host test result")
    tests = json.loads(result_lines[0][7:])
    if tests["failed"] or not tests["productionSourceTested"]: raise RuntimeError("Host tests failed")
    zip_classes(out / "host", out / "helper.jar", helper_only=True)
    zip_classes(out / "host", out / "host-classpath.jar")
    environment = dict(os.environ, JAVA_HOME=str(args.java_home))
    run([args.build_tools / "d8", "--min-api", "33", "--release", "--lib", args.android_jar,
         "--classpath", out / "host-classpath.jar", "--output", out / "helper-dex", out / "helper.jar"], out / "d8.log", env=environment)
    classpath = os.pathsep.join(map(str, (args.dexlib2, args.guava)))
    run([javac, "--release", "8", "-Xlint:-options", "-cp", classpath, "-d", out / "tools", HERE / "tools/PatchIwlan.java"], out / "javac-patcher.log")
    runtime_cp = str(out / "tools") + os.pathsep + classpath
    run([java, "-cp", runtime_cp, "PatchIwlan", out / "original.dex", out / "patched.dex", out / "dex-whitelist.json"], out / "patcher.log")
    for name, path in (("patched", out / "patched.dex"), ("helper", out / "helper-dex/classes.dex")):
        run([args.build_tools / "dexdump", "-d", path], out / (name + ".dexdump.txt"))
    unaligned = out / "unaligned-unsigned.apk"
    with zipfile.ZipFile(args.input) as original, zipfile.ZipFile(unaligned, "w", compression=zipfile.ZIP_STORED) as z:
        for old in original.infolist():
            if old.filename in SIGNATURE_MEMBERS: continue
            info = zipfile.ZipInfo(old.filename, (1980, 1, 1, 0, 0, 0))
            info.external_attr = 0o100644 << 16
            z.writestr(info, (out / "patched.dex").read_bytes() if old.filename == "classes.dex" else original.read(old))
        z.writestr(zipfile.ZipInfo("classes2.dex", (1980, 1, 1, 0, 0, 0)), (out / "helper-dex/classes.dex").read_bytes())
    args.output.parent.mkdir(parents=True, exist_ok=True)
    run([args.build_tools / "zipalign", "-P", "16", "-f", "4", unaligned, args.output], out / "zipalign.log")
    run([args.build_tools / "zipalign", "-P", "16", "-c", "4", args.output], out / "zipalign-check.log")
    if sha(args.output) != PATCHED_SHA256:
        raise RuntimeError("Rebuild differs from reviewed unsigned SHA256: " + sha(args.output))
    members = verify_members(args.input, args.output)
    (out / "apk-members-verification.json").write_text(json.dumps(members, indent=2) + "\n")
    ledger = {"status": "built-and-host-tests-passed", "input_sha256": input_hash,
              "unsigned_output": str(args.output), "unsigned_sha256": sha(args.output),
              "original_dex_sha256": ORIGINAL_DEX_SHA256, "patched_dex_sha256": sha(out / "patched.dex"),
              "helper_dex_sha256": sha(out / "helper-dex/classes.dex"),
              "helper_source_sha256": sha(HERE / "src/vendor/qti/iwlan/IWlanWakeLockFix.java"),
              "patcher_source_sha256": sha(HERE / "tools/PatchIwlan.java"),
              "host_tests_source_sha256": sha(HERE / "tests/ProductionHelperTest.java"),
              "host_tests_log_sha256": sha(out / "host-tests.log"), "tests": tests,
              "dex_whitelist": json.loads((out / "dex-whitelist.json").read_text()),
              "dependencies": {str(path): sha(path) for path in (args.dexlib2, args.guava, args.android_jar, java, javac, args.build_tools / "lib/d8.jar")},
              "runtime_limits": ["No phone tests or battery improvement claim", "Original service acquisition and reset/IPC window intentionally preserved; stale results are idempotent", "Requires ROM platform signing; this APK is unsigned"]}
    (out / "build-ledger.json").write_text(json.dumps(ledger, indent=2) + "\n")
    print(json.dumps({k: ledger[k] for k in ("status", "unsigned_output", "unsigned_sha256", "tests")}))


if __name__ == "__main__":
    main()
