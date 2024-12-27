#!/usr/bin/env python3

"""
Chain together parts of compiled binaries into a complete ROM file.
"""

import sys
from pathlib import Path
from typing import List, Tuple


def read_binary_file(file_path: Path, start: int, length: int) -> bytes:
    """
    Read a slice of binary data from a file.

    Args:
        file_path: Path to the binary file.
        start: Starting position (1-based, inclusive).
        length: Number of bytes to read.

    Returns:
        The binary data as bytes.
    """
    if not file_path.exists():
        raise FileNotFoundError(f"File not found: {file_path}")
    with file_path.open("rb") as file:
        return file.read()[start - 1 : start - 1 + length]


def parse_chain_argument(arg: str) -> Tuple[Path, int, int, int]:
    """
    Parse a chain argument in the format 'file,start,length,end'.

    Args:
        arg: A string representing the chain data, e.g., 'file,0x100,0x200,0x300'.

    Returns:
        A tuple containing the file path, start address, length, and end position.
    """
    try:
        parts = arg.split(",")
        file_path = Path(parts[0])
        start, length, end = (int(x, 16) for x in parts[1:])
    except (ValueError, IndexError) as e:
        raise ValueError(
            f"Invalid chain argument '{arg}'. Expected format 'file,start,length,end'."
        ) from e

    return file_path, start, length, end


def build_rom(rom_length: int, chain_args: List[str]) -> bytearray:
    """
    Build the ROM data by chaining binary files.

    Args:
        rom_length: The total length of the ROM.
        chain_args: A list of arguments specifying file parts to chain.

    Returns:
        A bytearray containing the complete ROM data.
    """
    rom_data = bytearray([0xFF] * rom_length)

    for arg in chain_args:
        file_path, start, length, end = parse_chain_argument(arg)
        binary_data = read_binary_file(file_path, start, length)

        if end + len(binary_data) > len(rom_data):
            raise ValueError(f"Data from {file_path} at {end:#x} exceeds ROM size.")
        rom_data[end : end + len(binary_data)] = binary_data

    return rom_data


def write_rom(rom_path: Path, rom_data: bytearray) -> None:
    """Write the complete ROM data to a file."""
    rom_path.write_bytes(rom_data)


def main(args: List[str]) -> None:
    if len(args) < 3:
        print(
            "Usage: rom_chain.py <output_rom> <rom_length> <file,start,length,end>..."
        )
        sys.exit(1)

    rom_path = Path(args[0])
    try:
        rom_length = int(args[1], 16)
        chain_args = args[2:]
        rom_data = build_rom(rom_length, chain_args)
        write_rom(rom_path, rom_data)
        print(f"Successfully created ROM: {rom_path}")
    except (FileNotFoundError, ValueError) as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main(sys.argv[1:])
