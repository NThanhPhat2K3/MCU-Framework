#!/usr/bin/env python3
"""
UART firmware update client shared by CLI and GUI tools.
"""

from __future__ import annotations

from dataclasses import dataclass
import struct
import time
from typing import Callable

import serial


@dataclass(frozen=True)
class UpdateConfig:
    port: str
    baud: int = 115200
    app_bin: str = ""
    app_addr: int = 0x08008000
    chunk_size: int = 128
    timeout: float = 0.3


def _normalize_response(response: bytes) -> str:
    return response.decode(errors="ignore").replace("\r", "").strip()


def _check_response(text: str, expected_prefix: str | None = None) -> None:
    if not text:
        if expected_prefix is None:
            return
        raise RuntimeError("No response from programmer")

    lines = [line.strip() for line in text.split("\n") if line.strip()]
    for line in lines:
        if line.startswith("ERR"):
            raise RuntimeError(f"Programmer error: {line}")

    if expected_prefix is not None and not any(line.startswith(expected_prefix) for line in lines):
        raise RuntimeError(f"Unexpected programmer response: {text}")


def _read_text(port: serial.Serial) -> str:
    return _normalize_response(port.read_all())


def _request_programmer_from_app(port: serial.Serial, log_fn: Callable[[str], None], timeout_s: float = 3.0) -> bool:
    deadline = time.time() + timeout_s

    # Flush any leftover bytes from binary probe packets so the 'u'
    # command reaches the App cleanly and the response is not corrupted.
    port.reset_input_buffer()

    log_fn("Requesting Programmer mode from App...")
    port.write(b"u")
    port.flush()

    accumulated = ""
    while time.time() < deadline:
        text = _read_text(port)
        if text:
            # Accumulate text across reads so a split "GO PROGRA" / "M"
            # still matches "PROGRAMMER" on the next iteration.
            accumulated += ("\n" + text) if accumulated else text
            log_fn(text)
            if "PROGRAMMER" in accumulated:
                return True
        time.sleep(0.1)

    return False


def _wait_for_programmer(port: serial.Serial, log_fn: Callable[[str], None], timeout_s: float = 4.0) -> None:
    deadline = time.time() + timeout_s
    app_mode_seen = False

    while time.time() < deadline:
        text = _read_text(port)
        if text:
            log_fn(text)

            if "PROGRAMMER" in text:
                return

            if "APP RUNNING" in text:
                app_mode_seen = True
                if _request_programmer_from_app(port, log_fn):
                    return

        response = _normalize_response(send_packet(port, b"I"))
        if response:
            log_fn(response)
            if "OK INFO" in response:
                return
            if "PROGRAMMER" in response:
                return
            if "APP RUNNING" in response:
                app_mode_seen = True
                if _request_programmer_from_app(port, log_fn):
                    return
            if "ERR" in response:
                raise RuntimeError(f"Programmer error while probing: {response}")

        time.sleep(0.15)

    if app_mode_seen:
        raise RuntimeError("Failed to switch App into Programmer mode")

    if _request_programmer_from_app(port, log_fn):
        return

    raise RuntimeError("No response from programmer")


def crc8(cmd: int, data: bytes) -> int:
    length = len(data)
    value = cmd ^ (length & 0xFF) ^ ((length >> 8) & 0xFF)
    for byte in data:
        value ^= byte
    return value & 0xFF


def send_packet(port: serial.Serial, cmd: bytes, data: bytes = b"", timeout_s: float = 1.0) -> bytes:
    packet = cmd + struct.pack("<H", len(data)) + data + bytes([crc8(cmd[0], data)])

    port.write(packet)
    port.flush()

    # Read with a deadline instead of a fixed 50ms sleep.  The MCU
    # programmer blocks until the erase/write completes, which on
    # STM32F4 can be several seconds for a full sector erase.
    deadline = time.time() + timeout_s
    buf = b""
    while time.time() < deadline:
        chunk = port.read(port.in_waiting or 1)
        if chunk:
            buf += chunk
        # If we already have a complete response line (ending with \n)
        # we can stop early.
        if b"\n" in buf:
            break
    return buf


def flash_image(
    config: UpdateConfig,
    log: Callable[[str], None] | None = None,
    progress: Callable[[int, int], None] | None = None,
) -> None:
    log_fn = log or (lambda message: None)
    progress_fn = progress or (lambda done, total: None)

    with open(config.app_bin, "rb") as image:
        image_data = image.read()

    total_size = len(image_data)
    address = config.app_addr
    sent_size = 0

    if total_size == 0:
        raise ValueError("Firmware image is empty")

    with serial.Serial(config.port, config.baud, timeout=config.timeout) as port:
        time.sleep(0.8)
        _wait_for_programmer(port, log_fn)

        # Drain any leftover text (e.g. "PROGRAMMER\r\n" banner) that
        # arrived after the handshake completed, so the erase command
        # gets a clean response.
        port.reset_input_buffer()

        # Flash erase can take several seconds on STM32F4 (up to ~4s
        # for a 128 KB sector), so use a generous timeout.
        response = _normalize_response(send_packet(port, b"E", timeout_s=30.0))
        if response:
            log_fn(response)
        _check_response(response, "OK ERASE")

        while sent_size < total_size:
            chunk = image_data[sent_size : sent_size + config.chunk_size]
            payload = struct.pack("<I", address) + chunk
            response = _normalize_response(send_packet(port, b"W", payload))
            if response:
                log_fn(response)
            _check_response(response, "OK WRITE")

            sent_size += len(chunk)
            address += len(chunk)
            progress_fn(sent_size, total_size)

        response = _normalize_response(send_packet(port, b"J"))
        if response:
            log_fn(response)
        _check_response(response, None)
