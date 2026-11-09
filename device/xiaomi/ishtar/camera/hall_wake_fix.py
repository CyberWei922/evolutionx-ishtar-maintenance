#!/usr/bin/env python3
"""Apply the exact verified ishtar CamX Hall-wake candidate to a separate output.
No phone actions; no in-place changes; refuse any unrecognized source artifact.
"""
import argparse, hashlib, pathlib, struct
ORIGINAL_SHA = "8b960e8e7004af175770f03c5f11dd9ddb115703ae4376c76abeb24c9a7d9106"
PATCHED_SHA = "8dd04166117042d1315910e944acc027f52022a65373d0ecb10924a9b89017e6"
STUB = bytes.fromhex("e103182a08dc40b91f190071e1010054081840b9a8010035080840f9680100b408a140f9280100b4093d40b93f050071c1000054094941b98900003409a940f9490000b401048052f318e017")

def patch(original):
    if hashlib.sha256(original).hexdigest() != ORIGINAL_SHA:
        raise ValueError("Refusing unrecognized input SHA256; this patch is only for the verified .307 CamX artifact")
    b = bytearray(original)
    assert struct.unpack_from("<I", b, 0x11749f0)[0] == 0x2a1803e1
    assert b[0x11749f4:0x11749f8] == bytes.fromhex("c3e8ff97")
    assert b[0x196e5e0:0x196e5e0+len(STUB)] == bytes(len(STUB))
    phoff = struct.unpack_from("<Q", b, 32)[0]
    phentsize, phnum = struct.unpack_from("<HH", b, 54)
    assert (phoff, phentsize, phnum) == (64, 56, 12)
    assert struct.unpack_from("<II6Q", b, phoff+2*phentsize) == (1,5,0x4f8000,0x4f8000,0x4f8000,0x14765d8,0x14765d8,0x1000)
    delta = 0x196e5e0-0x11749f0
    struct.pack_into("<I", b, 0x11749f0, 0x14000000 | ((delta//4)&0x3ffffff))
    b[0x196e5e0:0x196e5e0+len(STUB)] = STUB
    struct.pack_into("<QQ", b, phoff+2*phentsize+32, 0x147662c, 0x147662c)
    b[0x308:0x318] = bytes.fromhex("1996c17ea709da74dd8d4af0da779ce5")
    if hashlib.sha256(b).hexdigest() != PATCHED_SHA:
        raise ValueError("Output failed exact verified candidate SHA256")
    return bytes(b)


def extract_fixup(ctx, file, file_path, *args, **kwargs):
    """Extract-utils callback, accepting only the pinned original or patched blob."""
    import os
    import tempfile
    target = pathlib.Path(file_path)
    original = target.read_bytes()
    if hashlib.sha256(original).hexdigest() == PATCHED_SHA:
        return
    result = patch(original)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(dir=target.parent, prefix=target.name + '.hall-wake-', delete=False) as stream:
            temporary = pathlib.Path(stream.name)
            stream.write(result)
        temporary.chmod(target.stat().st_mode & 0o777)
        os.replace(temporary, target)
    finally:
        if temporary is not None and temporary.exists():
            temporary.unlink()

if __name__ == "__main__":
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("input", type=pathlib.Path)
    ap.add_argument("output", type=pathlib.Path)
    args=ap.parse_args()
    if args.input.resolve() == args.output.resolve():
        ap.error("Output must be separate from input")
    result=patch(args.input.read_bytes())
    with args.output.open("xb") as f:
        f.write(result)
    print(PATCHED_SHA, args.output)
