#!/usr/bin/env python3
"""CLI wrapper around the shared UART firmware update client."""

import argparse
import os
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "tools"))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)

from update_client import UpdateConfig
from update_client import flash_image
from update_client import query_programmer_info


def _print_info(info: dict[str, int | str]) -> None:
    if not info:
        print("No programmer info returned.")
        return

    print("Programmer info:")
    for key, value in info.items():
        print(f"  {key}: {value}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Send app image to portable programmer example")
    parser.add_argument("--port", required=True, help="Serial port, e.g. COM5 or /dev/ttyUSB0")
    parser.add_argument("--baud", type=int, default=115200, help="UART baud rate")
    parser.add_argument("--app-bin", help="Path to app binary")
    parser.add_argument("--app-addr", type=lambda x: int(x, 0), default=0x08008000, help="App start address")
    parser.add_argument("--chunk", type=int, default=128, help="Chunk size in bytes")
    parser.add_argument("--info-only", action="store_true", help="Only query programmer info, do not flash")
    args = parser.parse_args()

    cfg = UpdateConfig(
        port=args.port,
        baud=args.baud,
        app_bin=args.app_bin or "",
        app_addr=args.app_addr,
        chunk_size=args.chunk,
    )

    if args.info_only:
        _print_info(query_programmer_info(cfg, log=print))
        return

    if not args.app_bin:
        parser.error("--app-bin is required unless --info-only is used")

    flash_image(
        cfg,
        log=print,
    )

    print("")
    _print_info(query_programmer_info(cfg, log=print))


if __name__ == "__main__":
    main()
