#!/usr/bin/env python3
"""Quick content verifier for unsigned or platform-resigned outputs; no Android runtime."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile
import zlib

HERE = Path(__file__).resolve().parent
SIGNATURES = {"META-INF/CERT.SF", "META-INF/CERT.RSA", "META-INF/MANIFEST.MF"}


def hash_bytes(data):
    return hashlib.sha256(data).hexdigest()


def classes(data):
    if not data.startswith(b"dex\n"):
        raise RuntimeError("Invalid DEX magic")
    if struct.unpack_from("<I", data, 8)[0] != zlib.adler32(data[12:]) & 0xffffffff:
        raise RuntimeError("DEX Adler32 mismatch")
    if data[12:32] != hashlib.sha1(data[32:]).digest():
        raise RuntimeError("DEX SHA1 header mismatch")
    strings_count, strings_off = struct.unpack_from("<II", data, 56)
    types_count, types_off = struct.unpack_from("<II", data, 64)
    class_count, class_off = struct.unpack_from("<II", data, 96)
    strings = []
    for i in range(strings_count):
        off = struct.unpack_from("<I", data, strings_off + 4 * i)[0]
        while data[off] & 128: off += 1
        off += 1
        strings.append(data[off:data.index(b"\0", off)].decode("utf-8", "replace"))
    types = [strings[struct.unpack_from("<I", data, types_off + 4 * i)[0]] for i in range(types_count)]
    return {types[struct.unpack_from("<I", data, class_off + 32 * i)[0]] for i in range(class_count)}


def verify(original, patched):
    lock = json.loads((HERE / "artifact-lock.json").read_text())
    if hash_bytes(Path(original).read_bytes()) != lock["original_apk_sha256"]:
        raise RuntimeError("Unknown original APK")
    with zipfile.ZipFile(original) as a, zipfile.ZipFile(patched) as b:
        expected = set(a.namelist()) - SIGNATURES
        actual = set(b.namelist()) - SIGNATURES
        if actual != expected | {"classes2.dex"}: raise RuntimeError("Unexpected APK member inventory")
        count = 0
        for name in sorted(expected - {"classes.dex"}):
            if a.read(name) != b.read(name): raise RuntimeError("Unrelated member changed: " + name)
            count += 1
        main, helper = b.read("classes.dex"), b.read("classes2.dex")
        if hash_bytes(main) != lock["patched_dex_sha256"]: raise RuntimeError("Unreviewed main DEX")
        if hash_bytes(helper) != lock["helper_dex_sha256"]: raise RuntimeError("Unreviewed helper DEX")
        original_classes = classes(a.read("classes.dex"))
        if classes(main) != original_classes: raise RuntimeError("Original class inventory changed")
        injected = classes(helper)
        if "Lvendor/qti/iwlan/IWlanWakeLockFix;" not in injected: raise RuntimeError("Missing production helper")
        if any(not n.startswith("Lvendor/qti/iwlan/IWlanWakeLockFix") for n in injected): raise RuntimeError("Host stub or unrelated class injected")
    return {"status": "verified", "original_classes": len(original_classes), "helper_classes": len(injected),
            "unchanged_non_dex_members": count, "reviewed_dex_hashes_match": True,
            "apk_sha256": hash_bytes(Path(patched).read_bytes()),
            "signature_verification": "Separate ROM/platform apksigner gate required"}


if __name__ == "__main__":
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original", type=Path, required=True)
    p.add_argument("--patched", type=Path, required=True)
    a = p.parse_args()
    print(json.dumps(verify(a.original, a.patched)))
