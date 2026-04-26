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

## Design goals

- de hoc
- de doc
- de port
- it phu thuoc
- tach logic boot voi logic chip
- tach `common section layout` khoi `per-image memory map`
