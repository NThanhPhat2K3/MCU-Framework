#!/usr/bin/env python3
"""
Patch MCU-Framework image headers and generate Intel HEX from the patched BIN.
"""

from __future__ import annotations

import argparse
import struct
from pathlib import Path


BOOT_IMAGE_HEADER_OFFSET = 0x200
BOOT_IMAGE_HEADER_SIZE = 64
BOOT_IMAGE_MAGIC = 0x494D4731
BOOT_IMAGE_HEADER_VERSION = 1
HEADER_FORMAT = "<IHHIIIIIIII6I"


def crc32_update(crc: int, data: bytes) -> int:
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xEDB88320
            else:
                crc >>= 1
    return crc & 0xFFFFFFFF


def crc32_image(image: bytes, header_size: int) -> int:
    crc = 0xFFFFFFFF
    crc = crc32_update(crc, image[:BOOT_IMAGE_HEADER_OFFSET])
    crc = crc32_update(crc, image[BOOT_IMAGE_HEADER_OFFSET + header_size :])
    return crc ^ 0xFFFFFFFF


def patch_image(path: Path) -> None:
    data = bytearray(path.read_bytes())

    if len(data) < BOOT_IMAGE_HEADER_OFFSET + BOOT_IMAGE_HEADER_SIZE:
        raise ValueError("Image is smaller than the reserved header area")

    raw_header = data[
        BOOT_IMAGE_HEADER_OFFSET : BOOT_IMAGE_HEADER_OFFSET + BOOT_IMAGE_HEADER_SIZE
    ]
    header = list(struct.unpack(HEADER_FORMAT, raw_header))

    magic = header[0]
    header_version = header[1]
    header_size = header[2]

    if magic != BOOT_IMAGE_MAGIC:
        raise ValueError("Unexpected image magic in binary header")

    if header_version != BOOT_IMAGE_HEADER_VERSION:
        raise ValueError("Unexpected image header version in binary header")

    if header_size != BOOT_IMAGE_HEADER_SIZE:
        raise ValueError("Unexpected image header size in binary header")

    header[5] = len(data)
    header[7] = 0

    packed = struct.pack(HEADER_FORMAT, *header)
    data[
        BOOT_IMAGE_HEADER_OFFSET : BOOT_IMAGE_HEADER_OFFSET + BOOT_IMAGE_HEADER_SIZE
    ] = packed

    header[7] = crc32_image(bytes(data), header_size)
    packed = struct.pack(HEADER_FORMAT, *header)
    data[
        BOOT_IMAGE_HEADER_OFFSET : BOOT_IMAGE_HEADER_OFFSET + BOOT_IMAGE_HEADER_SIZE
    ] = packed

    path.write_bytes(data)


def intel_hex_record(record_type: int, address: int, payload: bytes) -> str:
    count = len(payload)
    record = bytearray([count, (address >> 8) & 0xFF, address & 0xFF, record_type])
    record.extend(payload)
    checksum = ((~sum(record) + 1) & 0xFF)
    return ":" + "".join(f"{byte:02X}" for byte in record) + f"{checksum:02X}"


def write_hex(bin_path: Path, hex_path: Path, base_addr: int) -> None:
    data = bin_path.read_bytes()
    lines: list[str] = []
    current_upper = None

    for offset in range(0, len(data), 16):
        chunk = data[offset : offset + 16]
        address = base_addr + offset
        upper = (address >> 16) & 0xFFFF

        if upper != current_upper:
            current_upper = upper
            lines.append(intel_hex_record(0x04, 0x0000, struct.pack(">H", upper)))

        lines.append(intel_hex_record(0x00, address & 0xFFFF, chunk))

    lines.append(intel_hex_record(0x01, 0x0000, b""))
    hex_path.write_text("\n".join(lines) + "\n", encoding="ascii")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--bin", required=True, help="Patched binary image path")
    parser.add_argument("--hex", help="Optional Intel HEX output path")
    parser.add_argument(
        "--base-addr",
        type=lambda value: int(value, 0),
        help="Flash base address used for Intel HEX generation",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    bin_path = Path(args.bin)

    patch_image(bin_path)

    if args.hex is not None:
        if args.base_addr is None:
            raise ValueError("--base-addr is required when --hex is used")
        write_hex(bin_path, Path(args.hex), args.base_addr)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
