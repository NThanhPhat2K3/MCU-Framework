# Vendor Components

This folder stores third-party MCU support code used by the project.

## Why These Files Are Included

The goal of this repository is simple setup:

- clone
- build
- learn

Because of that, the needed vendor components are stored inside the repository
instead of forcing new users to clone many extra packages first.

## Current Components

- `CMSIS_5/`
- `cmsis-device-f1/`
- `cmsis-device-f4/`
- `stm32f1xx-hal-driver/`
- `stm32f4xx-hal-driver/`

## Source

- `CMSIS_5/` comes from ARM
- `cmsis-device-*` comes from STMicroelectronics
- `stm32*hal-driver/` comes from STMicroelectronics

## Why Component Repos Are Used

This project uses smaller component repositories instead of full STM32Cube
packages because they are:

- lighter
- easier to share
- easier to understand
- better for both Linux and Windows setups

## Notes About the STM32 Driver Packages

The STM32 folders still use the upstream package names
`stm32f1xx-hal-driver` and `stm32f4xx-hal-driver`.

That is normal.

This project currently uses:

- CMSIS device headers
- STM32 LL headers from those packages
- direct register access where needed

The project-owned code no longer depends on STM32 `HAL_*` runtime APIs.

## Layer Split

- `vendor/`
  third-party CMSIS and STM32 driver code
- `mcu/stm32f1/targets/stm32f103/`, `mcu/stm32f4/targets/stm32f411ce/`
  target-specific startup, linker, and system files
- `mcu/`
  family-specific backend code owned by this project
- future `bsp/`
  board-specific code written by the project itself

## Scope

Files in this folder are upstream third-party sources.

Project cleanup may remove obvious local leftovers such as `.bak` files, but
the actual vendor documentation and source layout should stay close to upstream.
