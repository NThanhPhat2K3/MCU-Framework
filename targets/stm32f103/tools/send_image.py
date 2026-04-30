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


def main() -> None:
    parser = argparse.ArgumentParser(description="Send app image to portable programmer example")
    parser.add_argument("--port", required=True, help="Serial port, e.g. COM5 or /dev/ttyUSB0")
    parser.add_argument("--baud", type=int, default=115200, help="UART baud rate")
    parser.add_argument("--app-bin", required=True, help="Path to app binary")
    parser.add_argument("--app-addr", type=lambda x: int(x, 0), default=0x08008000, help="App start address")
    parser.add_argument("--chunk", type=int, default=128, help="Chunk size in bytes")
    args = parser.parse_args()

    flash_image(
        UpdateConfig(
            port=args.port,
            baud=args.baud,
            app_bin=args.app_bin,
            app_addr=args.app_addr,
            chunk_size=args.chunk,
        ),
        log=print,
    )


if __name__ == "__main__":
    main()
