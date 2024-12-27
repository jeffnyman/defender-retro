#!/usr/bin/env python3

"""
Chain together parts of compiled binaries into a complete ROM file.
"""

import sys
from typing import List, IO

if len(sys.argv) < 2:
    print("ROM files must be provided.")
    exit()

rom_name: str = sys.argv[1]
rom_file: IO[bytes] = open(rom_name, "wb")
rom_length: int = int(sys.argv[2], 16)
rom_data: bytearray = bytearray([0xFF] * rom_length)

for a in sys.argv[3:]:
    args: List[str] = a.split(",")
    fn: str = args[0]
    nm: List[str] = args[-1:]
    fr, to, ln = [int(x, 16) for x in args[1:-1]]

    byte_data: bytes = open(fn, "rb").read()[fr - 1 : fr + ln - 1]
    rom_data[to : to + len(byte_data)] = byte_data

rom_file.write(rom_data)
