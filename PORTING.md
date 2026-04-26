# Porting Guide

## 1. Copy core folders

Copy:

- `bootloader/common/`
- `port/`
- `bootloader/`
- `application/`

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
- MSP / Renesas / toolchain khac
  - van nen giu y tuong tach `common sections` va `target memory`
  - nhung file cu the co the khong con la GNU `.ld`

## 4. Port `port/`

### `port_system.c`

- `HAL_Init()` / CMSIS init
- `SystemClock_Config()` toi thieu
- reset API
- flash addr check
- ram addr check
- jump preparation

### `port_uart.c`

- UART init
- UART read/write

### `port_flash.c`

- page/sector erase
- program unit

## 5. Verify flow

1. Nap `BootManager`
2. Nap `Programmer`
3. Reset
4. Neu chua co app -> vao `Programmer`
5. Nap `App`
6. Reset -> vao `App`
