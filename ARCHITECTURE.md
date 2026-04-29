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

Chua logic phu thuoc MCU:

- `port_system.*`
- `port_flash.*`
- `port_uart.*`
- `port_hal.h`

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

### `stm32f103/`

Chua:

- linker scripts
- memory map linker rieng cho F103
- startup vector table rieng cho F103
- system init toi thieu
- tool script thu nghiem

### `stm32f411ce/`

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
  - hien tai la noi chua kha nhieu logic vendor-specific nhat
- `application/app_main.c`
  - da build duoc cho F103 va F411, nhung van chua tach thanh BSP that su

## What changed after adding STM32F411CE

- Framework da co 2 target song song:
  - `stm32f103`
  - `stm32f411ce`
- `boot_config.h` hien tai map flash theo macro target
- `port_flash.c` da cho thay su khac nhau giua:
  - F1: `page erase`
  - F4: `sector erase`
- `port_uart.c` va `application/app_main.c` da cho thay su khac nhau giua:
  - F1 GPIO config style
  - F4 alternate-function GPIO style

## Known architecture limitation

- `port/` hien tai van la mot abstraction mong, chua phan tach thanh:
  - `mcal/`
  - `hal/`
  - `bsp/`
- `application/app_main.c` van con chua pin/UART/LED mapping theo board
- layout hien tai phu hop hoc tap va bring-up, chua phai final production architecture
