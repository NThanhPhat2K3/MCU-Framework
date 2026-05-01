# MCU-Framework

MCU-Framework is a small three-image bootloader framework for Cortex-M boards.
It is designed to be easy to read, easy to port, and easy to test.

Current example targets:

- `stm32f103`
- `stm32f411ce`

## Images

The framework builds three separate firmware images:

- `BootManager`
- `Programmer`
- `App`

Basic flow:

1. Reset starts `BootManager`.
2. `BootManager` checks the shared boot state.
3. If an update is requested, it jumps to `Programmer`.
4. If no update is requested, it tries to jump to `App`.
5. If `App` is not valid, it falls back to `Programmer`.

## Project Layout

```text
MCU-Framework/
  application/
  bootloader/
  linker/
  make/
  mcu/
  port/
  startup/
  tools/
  vendor/
```

Key folders:

- `application/`: example application code
- `bootloader/bootmanager/`: boot decision logic
- `bootloader/programmer/`: UART update image
- `bootloader/common/`: shared boot code
- `port/`: portable interfaces such as system, UART, and flash
- `mcu/`: MCU-family backends plus target packages for each family
- `tools/`: host-side update tools
- `vendor/`: third-party CMSIS and STM32 driver packages

## Current Driver Style

The project now uses:

- CMSIS device headers
- STM32 LL headers where useful
- direct register access for flash operations

The STM32 vendor packages still use the upstream names
`stm32f1xx-hal-driver` and `stm32f4xx-hal-driver`, but the project code no
longer depends on `HAL_*` APIs.

## Build

From the repository root:

```bash
make TARGET=stm32f411ce all
make TARGET=stm32f103 all
```

Useful commands:

```bash
make help
make TARGET=stm32f411ce bootmanager
make TARGET=stm32f411ce programmer
make TARGET=stm32f411ce app
```

Build output:

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

## Example Flash Layout

`stm32f103`

- `0x08000000`: `BootManager`
- `0x08004000`: `Programmer`
- `0x08008000`: `App`

`stm32f411ce`

- `0x08000000`: `BootManager`
- `0x08008000`: `Programmer`
- `0x08010000`: `App`

## Host Tools

Desktop tool:

```bash
python3 tools/flasher.py
```

This tool can:

- scan serial ports
- select the target board
- auto-fill the app start address
- flash an app image through `Programmer`
- erase the app region
- open a serial monitor

CLI wrapper for direct flashing:

```bash
python3 mcu/stm32f1/targets/stm32f103/tools/send_image.py --help
```

## Recommended Reading Order

1. `README.md`
2. `ARCHITECTURE.md`
3. `BOOT_FLOW.md`
4. `PORTING.md`
5. `mcu/stm32f1/targets/stm32f103/README.md`
6. `mcu/stm32f4/targets/stm32f411ce/README.md`

## Known Limits

This is still a study and bring-up framework, not a finished production
bootloader.

Not implemented yet:

- image header format
- CRC or signature checking
- rollback support
- power-fail protection
- stronger update validation
- separate BSP layer
