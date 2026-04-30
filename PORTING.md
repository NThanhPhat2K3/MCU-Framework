# Porting Guide

## 1. Copy core folders

Copy:

- `bootloader/common/`
- `port/`
- `mcu/`
- `bootloader/`
- `application/`
- `startup/`
- `linker/`

## 2. Create MCU-family startup vector file

Giu nguyen:

- `startup/startup_portable_cortexm.c`

Tao moi:

- startup vector file rieng cho dong chip

Viec can sua:

- danh sach IRQ
- `.isr_vector`

## 3. Provide linker scripts

Nen tach thanh 2 lop:

- 1 file linker common dung chung cho cung toolchain
- 3 file memory map rieng:
  - `bootmanager`
  - `programmer`
  - `app`

Can co 3 memory map linker:

- `bootmanager`
- `programmer`
- `app`

Phai export:

- `_estack`
- `_sidata`
- `_sdata`
- `_edata`
- `_sbss`
- `_ebss`

Phai co section:

- `.noinit`

Neu doi sang:

- STM32 variant khac
  - thuong chi doi memory map + startup vector file
  - nhung neu khac generation nhu F1 -> F4 thi thuong phai sua them:
    - `mcu/stm32f1/*` hoac `mcu/stm32f4/*`
    - `targets/<target>/config/board_config.h`
- MSP / Renesas / toolchain khac
  - van nen giu y tuong tach `common sections` va `target memory`
  - nhung file cu the co the khong con la GNU `.ld`

## 4. Port `port/` and `mcu/`

### `port/`

- giu interface chung
- han che code vendor-specific

### `mcu/`

- dat backend theo family
- vi du:
  - `mcu/stm32/port_system_stm32.c`
  - `mcu/stm32/port_uart_stm32.c`
  - `mcu/stm32f1/port_flash_stm32f1.c`
  - `mcu/stm32f4/port_flash_stm32f4.c`

## 5. Add target folder

Nen tao 1 target folder rieng theo ten MCU / board, vi du:

- `targets/stm32f103/`
- `targets/stm32f411ce/`

Trong do nen co:

- `config/`
- `startup/`
- `system/`
- `ld/`
- `README.md`
- `BRINGUP.md`

Trong `config/`, nen co:

- `board_config.h`
  - LED pin
  - UART pin
  - board clock source

## 6. Update build system

Neu dung `Makefile`, can them:

- block `ifeq ($(TARGET),...)`
- include path HAL/CMSIS dung target
- startup source dung target
- linker script dung target
- quy uoc noi dat vendor package, vi du `vendor/STM32CubeF1`, `vendor/STM32CubeF4`
- bien config local dung target, vi du:
  - `STM32CUBE_F1_DIR`
  - `STM32CUBE_F4_DIR`

Khong nen dat nguyen STM32Cube package vao `bsp/`.
Nen tach:

- `vendor/`
  - HAL/CMSIS/Middleware tu ST
- `bsp/`
  - pin map, LED, button, UART wiring rieng cua board

## 7. Verify flow

1. Build `BootManager`
2. Build `Programmer`
3. Reset
4. Neu chua co app -> vao `Programmer`
5. Nap `App`
6. Reset -> vao `App`

## 8. Practical note after F103 -> F411 port

Khi them `STM32F411CE`, co 3 bai hoc thuc te:

1. `bootloader/common/` gan nhu giu nguyen
2. phan startup / linker / flash erase khac nhau ro rang
3. phan test app de bring-up thuong bi lo ra la chua tach BSP sach

Neu em port tiep sang NXP / Renesas, nen xem dot them F411 nhu mot buoc trung gian:

- F103 -> F411
  - kiem tra abstraction da du cho khac generation chua
- F411 -> NXP/Renesas
  - moi bat dau danh gia can tach them `mcal/hal/bsp` hay chua
