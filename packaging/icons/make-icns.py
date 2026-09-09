#!/usr/bin/env python3
"""Assemble a macOS .icns from a set of square PNGs.

macOS' own iconutil only exists on macOS, and libicns' png2icns is rarely
installed; this reproduces the container format with the standard library so the
icon can be regenerated from any machine.

Usage: make-icns.py OUT.icns IN-16.png IN-32.png ... (any subset of the sizes below)
"""

import struct
import sys

# ICNS chunk type per pixel size. Modern macOS reads PNG payloads directly.
# The @2x entries carry the same pixels as their 1x counterpart of equal size.
TYPES = {
    16:   [b"icp4"],
    32:   [b"icp5", b"ic11"],   # ic11 = 16x16@2x
    64:   [b"icp6", b"ic12"],   # ic12 = 32x32@2x
    128:  [b"ic07"],
    256:  [b"ic08", b"ic13"],   # ic13 = 128x128@2x
    512:  [b"ic09", b"ic14"],   # ic14 = 256x256@2x
    1024: [b"ic10"],            # 512x512@2x
}


def png_size(data: bytes) -> int:
    if data[:8] != b"\x89PNG\r\n\x1a\n" or data[12:16] != b"IHDR":
        raise ValueError("not a PNG file")
    width, height = struct.unpack(">II", data[16:24])
    if width != height:
        raise ValueError(f"icon must be square, got {width}x{height}")
    return width


def main(argv):
    if len(argv) < 3:
        sys.exit(__doc__)

    out_path, png_paths = argv[1], argv[2:]

    chunks = []
    for path in png_paths:
        with open(path, "rb") as handle:
            data = handle.read()
        size = png_size(data)
        if size not in TYPES:
            sys.exit(f"{path}: {size}px has no ICNS chunk type")
        for chunk_type in TYPES[size]:
            chunks.append(chunk_type + struct.pack(">I", len(data) + 8) + data)

    if not chunks:
        sys.exit("no input icons")

    body = b"".join(chunks)
    with open(out_path, "wb") as handle:
        handle.write(b"icns" + struct.pack(">I", len(body) + 8) + body)

    print(f"{out_path}: {len(chunks)} entries, {len(body) + 8} bytes")


if __name__ == "__main__":
    main(sys.argv)
