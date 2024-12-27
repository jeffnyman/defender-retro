#!/usr/bin/env python3

"""
Patch a ROM file with the given bytes at specified positions.
"""

import sys
from typing import List

if len(sys.argv) < 2:
    print("ROM patching info must be provided.")
    exit()

rom_name: str = sys.argv[1]
binary_data: bytearray = bytearray(open(rom_name, "rb").read())

for a in sys.argv[2:]:
    args: List[str] = a.split(",")
    to: int = int(args[0], 16)
    position: bytearray = bytearray.fromhex(args[1])
    binary_data[to : to + len(position)] = position

rom_file = open(rom_name, "wb")
rom_file.write(binary_data)
