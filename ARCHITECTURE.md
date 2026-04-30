# Architecture

## Images

- `BootManager`
  - image chay dau tien sau reset
  - quyet dinh boot vao `App` hay `Programmer`

- `Programmer`
  - image chuyen de update firmware
  - nhan command qua UART
  - erase / write image `App`

- `App`
  - firmware chinh cua san pham
  - co the yeu cau quay lai `Programmer`

## Layers

### `bootloader/common/`

Chua logic dung chung cua boot domain:

- `boot_shared.*`
- `boot_jump.*`
- `boot_proto.*`

### `startup/`

Chua startup flow dung chung:

- `startup_portable_cortexm.*`

### `port/`

Chua interface portable:

- `port_system.*`
- `port_flash.*`
- `port_uart.*`

### `mcu/`

Chua implementation theo MCU family:

- `mcu/stm32/`
- `mcu/stm32f1/`
- `mcu/stm32f4/`

### `linker/`

Chua linker common theo toolchain:

- `gnu/sections_common.ld`

### `bootloader/`

Chua boot domain:

- `bootloader/common/*`
- `bootloader/bootmanager/main.c`
- `bootloader/programmer/main.c`

### `application/`

Chua firmware chinh:

- `application/app_main.c`
- `application/app_update.*`

### `targets/stm32f103/`

Chua:

- linker scripts
- memory map linker rieng cho F103
- startup vector table rieng cho F103
- system init toi thieu
- tool script thu nghiem

### `targets/stm32f411ce/`

Chua:

- linker scripts
- memory map linker rieng cho STM32F411CE
- startup vector table rieng cho STM32F411xE
- system init toi thieu cho dong F4
- bring-up note cho Black Pill

## Design goals

- de hoc
- de doc
- de port
- it phu thuoc
- tach logic boot voi logic chip
- tach `common section layout` khoi `per-image memory map`

## Current portability shape

- `bootloader/common/`
  - dang kha portable giua cac dong Cortex-M
- `startup/startup_portable_cortexm.*`
  - portable o muc reset flow chung
- `port/*.c`
  - gio chi nen giu interface va dispatch nhe
- `mcu/*`
  - la noi chua vendor-specific backend
- `application/app_main.c`
  - da giam hardcode nho `board_config.h`, nhung van chua thanh BSP that su

## What changed after adding STM32F411CE

- Framework da co 2 target song song:
  - `stm32f103`
  - `stm32f411ce`
- `boot_config.h` hien tai map flash theo macro target
- `mcu/stm32f1/port_flash_stm32f1.c` da cho thay su khac nhau giua:
  - F1: `page erase`
- `mcu/stm32f4/port_flash_stm32f4.c` da cho thay su khac nhau giua:
  - F4: `sector erase`
- `board_config.h` giu wiring / clock theo board
- `mcu/stm32/port_uart_stm32.c` giu HAL UART setup theo family

## Known architecture limitation

- `bsp/` van chua tach thanh folder rieng
- `board_config.h` van dang nam trong target package, chua thanh BSP package doc lap
- layout hien tai phu hop hoc tap va bring-up, chua phai final production architecture
