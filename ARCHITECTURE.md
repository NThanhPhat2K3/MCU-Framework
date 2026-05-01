# Architecture

## Images

The framework is split into three firmware images:

- `BootManager`: starts first after reset and decides where to jump
- `Programmer`: receives update data over UART and writes the app region
- `App`: normal user firmware that can request an update

Each image has its own:

- flash address
- vector table
- linker script
- output directory

## Layer Model

The code is organized in layers so board-specific changes stay local.

### `bootloader/common/`

Shared boot logic:

- `boot_shared.*`
- `boot_jump.*`
- `boot_proto.*`

### `application/`

Example app logic:

- `app_main.c`
- `app_update.*`

### `port/`

Portable interfaces:

- `port_system.*`
- `port_uart.*`
- `port_flash.*`

This layer describes what the framework needs, but not how a specific MCU does
it.

### `mcu/`

MCU-family backends:

- `mcu/stm32/`: shared STM32 system and UART backends
- `mcu/stm32f1/`: STM32F1 flash backend
- `mcu/stm32f4/`: STM32F4 flash backend

This is the only layer that should know STM32 register details.

### `startup/`

Portable Cortex-M startup flow:

- `startup_portable_cortexm.c`
- `startup_portable_cortexm.h`

### `mcu/<family>/targets/<target>/`

Target-specific files:

- `config/board_config.h`
- `startup/`
- `system/`
- `ld/`
- target notes and bring-up docs

### `linker/`

Common linker building blocks:

- `linker/gnu/sections_common.ld`

## Porting Strategy

When moving to a new board in the same MCU family:

- update `mcu/<family>/targets/<target>/config/board_config.h`
- update linker scripts if the flash layout changes
- keep most of `bootloader/common/` and `port/` unchanged

When moving to a different MCU family:

- keep `bootloader/common/`, `application/`, and `port/`
- add or update the backend files under `mcu/`
- add a new target folder under the matching MCU family

## Current STM32 Notes

The framework currently supports:

- `stm32f103`
- `stm32f411ce`

Important differences between them:

- flash erase model
  - F1 uses page erase
  - F4 uses sector erase
- startup vector file
- linker memory map
- board clock setup

Shared STM32 behavior:

- SysTick-based timekeeping
- board-level GPIO and UART setup through `board_config.h`
- LL/CMSIS based system code

## Design Goals

- simple structure
- clear ownership per folder
- easy bring-up on a new target
- minimal direct dependency between boot logic and chip details

## Current Limits

The project still has a few intentional simplifications:

- no separate `bsp/` folder yet
- board config still lives inside each target folder
- no image authentication or rollback layer
- documentation and code focus on learning and bring-up first
