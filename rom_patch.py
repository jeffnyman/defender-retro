#!/usr/bin/env python3

"""
Patch a ROM file with the given bytes at specified positions.
"""

import sys
from pathlib import Path
from typing import List


def read_rom(rom_path: Path) -> bytearray:
    """Read the ROM file into a bytearray."""
    if not rom_path.exists():
        raise FileNotFoundError(f"The ROM file {rom_path} does not exist.")
    return bytearray(rom_path.read_bytes())


def parse_patch_argument(arg: str) -> tuple[int, bytearray]:
    """
    Parse a patch argument in the form 'offset,hexbytes'.

    Args:
        arg: A string representing the patch, e.g., '0x10,deadbeef'.

    Returns:
        A tuple of the offset (as an integer) and the bytes (as a bytearray).
    """
    try:
        offset_str, hex_data = arg.split(",")
        offset = int(offset_str, 16)
        patch_bytes = bytearray.fromhex(hex_data)
    except (ValueError, IndexError) as e:
        raise ValueError(
            f"Invalid patch argument '{arg}'. Expected format 'offset,hexbytes'."
        ) from e

    return offset, patch_bytes


def apply_patch(rom_data: bytearray, patches: List[tuple[int, bytearray]]) -> bytearray:
    """
    Apply patches to the ROM data.

    Args:
        rom_data: The original ROM data as a bytearray.
        patches: A list of tuples containing offset and patch bytes.

    Returns:
        The patched ROM data as a bytearray.
    """
    for offset, patch_bytes in patches:
        if offset + len(patch_bytes) > len(rom_data):
            raise ValueError(f"Patch at offset {offset:#x} exceeds ROM size.")
        rom_data[offset : offset + len(patch_bytes)] = patch_bytes
    return rom_data


def write_rom(rom_path: Path, rom_data: bytearray) -> None:
    """Write the patched ROM data back to the file."""
    rom_path.write_bytes(rom_data)


def main(args: List[str]) -> None:
    if len(args) < 2:
        print("Usage: rom_paych.py <rom_file> <offset,hexbytes>...")
        sys.exit(1)

    rom_path = Path(args[0])
    patches = [parse_patch_argument(arg) for arg in args[1:]]

    try:
        rom_data = read_rom(rom_path)
        patched_rom = apply_patch(rom_data, patches)
        write_rom(rom_path, patched_rom)
        print(f"Successfully patched {rom_path}")
    except (FileNotFoundError, ValueError) as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main(sys.argv[1:])
