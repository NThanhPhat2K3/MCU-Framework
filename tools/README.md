# MCU-Framework Tools

Host-side tools for UART firmware update and serial monitoring.

## Files

| File | Purpose |
|---|---|
| `flasher.py` | PyQt5 desktop flasher and serial monitor |
| `update_client.py` | Shared UART update logic used by the flasher |

## Install

```bash
python3 -m pip install pyserial PyQt5
```

## Run

From the repository root:

```bash
python3 tools/flasher.py
```

## Features

- serial port scan
- target selector
- auto-filled app address
- `.bin` file picker
- flash app over UART
- erase app region
- serial monitor tab
- optional `CRLF` append for monitor send
- live log and progress bar

## Supported App Addresses

| Target | App Address |
|---|---|
| `STM32F411CE (BlackPill)` | `0x08010000` |
| `STM32F103 (BluePill)` | `0x08008000` |

## Typical Update Flow

1. Connect the board through a USB-UART adapter
2. Open `flasher.py`
3. Select target MCU
4. Select serial port
5. Select the app `.bin` file
6. Click `Flash Firmware`

If the board is still in `App` mode, the tool tries to send `u` first to
switch into `Programmer` mode.

## Monitor Tips

The monitor tab is useful for:

- reading boot logs
- sending `u` manually
- checking if the board is in `App` or `Programmer`
- testing UART wiring

By default, monitor send does **not** append `CRLF`.  
Enable `Append CRLF` only when the device side really needs it.
