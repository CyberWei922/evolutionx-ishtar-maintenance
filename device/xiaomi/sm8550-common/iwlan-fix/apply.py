#!/usr/bin/env python3
"""Fast, exact-hash, atomic extraction fixup; uses only the Python standard library."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import stat
import tempfile

HERE = Path(__file__).resolve().parent
ORIGINAL = "909d14bff0bc85943ead69f08bb795b56736256502466b522c80b30a70dad2ef"
PATCHED = "c66f190fd4aa8f010a8f7d03a157e57bd5d379b125e24d1b4282d987a5047a68"


def apply(source, destination=None):
    source = Path(source).resolve()
    destination = source if destination is None else Path(destination).resolve()
    current = source.read_bytes()
    digest = hashlib.sha256(current).hexdigest()
    if digest == PATCHED:
        data, status = current, "already-patched"
    elif digest == ORIGINAL:
        data = (HERE / "payload/IWlanService-unsigned.apk").read_bytes()
        if hashlib.sha256(data).hexdigest() != PATCHED:
            raise RuntimeError("Reviewed payload hash mismatch")
        status = "patched"
    else:
        raise RuntimeError("Refusing unknown IWLAN APK SHA256: " + digest)
    if source == destination and digest == PATCHED:
        return {"status": status, "sha256": PATCHED}
    destination.parent.mkdir(parents=True, exist_ok=True)
    mode = stat.S_IMODE(source.stat().st_mode)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(prefix=".iwlan-fix-", dir=destination.parent, delete=False) as f:
            temporary = Path(f.name)
            f.write(data)
            f.flush()
            os.fsync(f.fileno())
        os.chmod(temporary, mode)
        os.replace(temporary, destination)
        temporary = None
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)
    return {"status": status, "sha256": PATCHED}


if __name__ == "__main__":
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("input", type=Path)
    p.add_argument("--output", type=Path)
    a = p.parse_args()
    print(json.dumps(apply(a.input, a.output)))
