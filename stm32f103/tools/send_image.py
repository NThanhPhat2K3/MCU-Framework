#!/usr/bin/env python3
import argparse
import struct
import time

import serial


def crc8(cmd: int, data: bytes) -> int:
    length = len(data)
    value = cmd ^ (length & 0xFF) ^ ((length >> 8) & 0xFF)
    for byte in data:
        value ^= byte
    return value & 0xFF


def send_packet(port: serial.Serial, cmd: bytes, data: bytes = b"") -> bytes:
    packet = cmd + struct.pack("<H", len(data)) + data + bytes([crc8(cmd[0], data)])
    port.write(packet)
    port.flush()
    time.sleep(0.05)
    return port.read_all()


def main() -> None:
    parser = argparse.ArgumentParser(description="Send app image to portable programmer example")
    parser.add_argument("--port", required=True, help="Serial port, e.g. COM5 or /dev/ttyUSB0")
    parser.add_argument("--baud", type=int, default=115200, help="UART baud rate")
    parser.add_argument("--app-bin", required=True, help="Path to app binary")
    parser.add_argument("--app-addr", type=lambda x: int(x, 0), default=0x08008000, help="App start address")
    parser.add_argument("--chunk", type=int, default=128, help="Chunk size in bytes")
    args = parser.parse_args()

    with serial.Serial(args.port, args.baud, timeout=0.3) as port:
        time.sleep(0.3)
        print(port.read_all().decode(errors="ignore"), end="")

        print(send_packet(port, b"I").decode(errors="ignore"), end="")
        print(send_packet(port, b"E").decode(errors="ignore"), end="")

        address = args.app_addr
        with open(args.app_bin, "rb") as image:
            while True:
                chunk = image.read(args.chunk)
                if not chunk:
                    break
                payload = struct.pack("<I", address) + chunk
                print(send_packet(port, b"W", payload).decode(errors="ignore"), end="")
                address += len(chunk)

        print(send_packet(port, b"J").decode(errors="ignore"), end="")


if __name__ == "__main__":
    main()
