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

## Layer Split

- `vendor/`
  third-party HAL and CMSIS code
- `targets/stm32f103/`, `targets/stm32f411ce/`
  target-specific startup, linker, and system files
- `mcu/`
  family-specific backend code owned by this project
- future `bsp/`
  board-specific code written by the project itself
