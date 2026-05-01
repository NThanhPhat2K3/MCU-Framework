# Porting Guide

This guide explains the simplest way to move the framework to a new board or a
new MCU family.

## 1. Keep the Core Folders

These folders are the reusable core:

- `application/`
- `bootloader/`
- `linker/`
- `mcu/`
- `port/`
- `startup/`

## 2. Add a New Target Folder

Create a new folder under the matching MCU family, for example:

- `mcu/stm32f1/targets/my_board/`

Recommended layout:

```text
mcu/stm32f1/targets/my_board/
  config/
  ld/
  startup/
  system/
  README.md
  BRINGUP.md
```

Required files:

- `config/board_config.h`
- one linker script for each image
- one startup vector file
- one minimal `SystemInit()` file

## 3. Create `board_config.h`

This file should contain the board-level choices:

- LED pin
- UART pins
- UART instance
- baud rate
- board clock source

Try to keep board wiring here instead of scattering it across the codebase.

## 4. Create the Startup Vector File

Keep:

- `startup/startup_portable_cortexm.c`
- `startup/startup_portable_cortexm.h`

Add:

- a new target-specific vector table file under
  `mcu/<family>/targets/<target>/startup/`

This target-specific startup file should mainly describe:

- `.isr_vector`
- IRQ names for the MCU

## 5. Create Three Linker Scripts

Each image needs its own linker script:

- `bootmanager.ld`
- `programmer.ld`
- `app.ld`

The common GNU section layout already lives in:

- `linker/gnu/sections_common.ld`

Each target linker script should define:

- flash origin and size
- RAM origin and size
- `_estack`
- `_sidata`
- `_sdata`
- `_edata`
- `_sbss`
- `_ebss`

It should also include a `.noinit` section.

## 6. Update the MCU Backend Only When Needed

If the new board uses the same MCU family:

- you may only need new linker scripts and a new `board_config.h`

If the new board uses a different MCU family:

- update or add files under `mcu/`

Current examples:

- `mcu/stm32/port_system_stm32.c`
- `mcu/stm32/port_uart_stm32.c`
- `mcu/stm32f1/port_flash_stm32f1.c`
- `mcu/stm32f4/port_flash_stm32f4.c`

## 7. Update the Makefile Target Block

To add a new target, update the build system with:

- a new `ifeq ($(TARGET),...)` block
- target-specific include paths
- target-specific startup and system files
- target-specific linker scripts
- target-specific flash layout constants

## 8. Verify in Small Steps

Recommended order:

1. build `BootManager`
2. build `Programmer`
3. build `App`
4. flash `BootManager`
5. flash `Programmer`
6. reset and confirm the board enters `Programmer`
7. flash or upload `App`
8. reset and confirm the board enters `App`

## 9. Practical Lessons From F103 and F411

The current two STM32 targets show a useful split:

- shared boot logic stays almost unchanged
- linker layout changes per target
- startup vectors change per target
- flash backend changes per MCU family
- board clock and UART wiring change per board

That is the main portability rule of this repo:

- keep boot logic generic
- keep board details in the target folder inside the MCU family
- keep MCU-family details in `mcu/`
