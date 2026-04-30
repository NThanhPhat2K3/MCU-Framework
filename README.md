# MCU-Framework

Small bootloader and firmware-update framework for Cortex-M MCUs.

This repository is built for learning, porting, and sharing.  
The current examples focus on:

- `STM32F103`
- `STM32F411CE Black Pill`

## Main Idea

The framework uses three images:

- `BootManager`
- `Programmer`
- `App`

Basic boot flow:

1. Reset starts `BootManager`
2. `BootManager` checks shared boot state
3. If update is requested, it jumps to `Programmer`
4. Otherwise it tries to jump to `App`
5. If `App` is invalid, it falls back to `Programmer`

## Project Goals

- easy to read
- easy to port
- clear separation between common boot logic and target-specific code
- simple enough for study projects

## Directory Layout

```text
MCU-Framework/
  README.md
  ARCHITECTURE.md
  BOOT_FLOW.md
  PORTING.md
  Makefile
  make/
  vendor/
  linker/
  startup/
  port/
  bootloader/
  application/
  stm32f103/
  stm32f411ce/
  tools/
```

## Important Folders

- `bootloader/common/`
  Common boot code shared by boot images
- `bootloader/bootmanager/`
  Boot decision logic
- `bootloader/programmer/`
  UART firmware update image
- `application/`
  Main firmware example
- `port/`
  MCU-specific flash, UART, and system helpers
- `stm32f103/`
  STM32F103 target files
- `stm32f411ce/`
  STM32F411CE target files
- `tools/`
  Host-side update tools
- `vendor/`
  HAL and CMSIS third-party code

## Build

From the repository root:

```bash
make TARGET=stm32f411ce all
make TARGET=stm32f103 all
```

Build output goes to:

```text
out/<target>/bootmanager/
out/<target>/programmer/
out/<target>/app/
```

Each image produces:

- `.elf`
- `.bin`
- `.hex`
- `.map`
- `.lst`

## Flash Layout

### STM32F103 example

- `0x08000000` BootManager
- `0x08004000` Programmer
- `0x08008000` App

### STM32F411CE example

- `0x08000000` BootManager
- `0x08008000` Programmer
- `0x08010000` App

## Current Tool Flow

The host-side UART tool is now:

```bash
python3 tools/flasher.py
```

What it does:

- scans serial ports
- selects target MCU
- auto-fills app address
- flashes `.bin` images through the `Programmer`
- can erase the app region
- includes a serial monitor tab

If the board is still in `App` mode, the update client tries to send `u`
first so the app can reset into `Programmer` mode.

## Current STM32F411 Note

The STM32F411 example now uses:

- `HSE = 25 MHz`
- `USART1 = 115200 8N1`

This was important for making UART behavior stable on the tested board.

## Suggested Reading Order

If you are new to the repo, read in this order:

1. `README.md`
2. `ARCHITECTURE.md`
3. `BOOT_FLOW.md`
4. `PORTING.md`
5. `bootloader/bootmanager/main.c`
6. `bootloader/programmer/main.c`
7. `application/app_main.c`
8. `stm32f103/README.md`
9. `stm32f411ce/README.md`

## Known Gaps

This project is still a learning framework, not a finished production bootloader.

Missing or incomplete items:

- image header
- CRC32
- rollback
- power-fail safety
- stronger update verification
- dedicated BSP layer

## AI Note

This repository is also written to be easy for coding assistants to read.

High-level summary:

```yaml
project:
  name: MCU-Framework
  type: educational_bootloader_framework
  targets:
    - stm32f103
    - stm32f411ce

images:
  - bootmanager
  - programmer
  - app

host_tools:
  - tools/flasher.py
  - tools/update_client.py

important_boot_flow:
  reset: bootmanager
  app_request_update: send 'u' or set shared boot request
  update_transport: uart

current_f411_uart:
  clock: hse_25mhz
  baud: 115200
```
