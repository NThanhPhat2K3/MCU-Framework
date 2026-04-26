# Portable Boot Example

> Human + AI friendly project note.
> This file is intended to be readable by both developers and coding assistants.

## Quick Context

Portable boot/update framework skeleton for Cortex-M MCUs.

Current concrete target:

- STM32F103 example included

Primary goals:

- easy to learn
- easy to port
- clear separation between boot logic and MCU-specific code
- no dependency on the large Bosch-style repo logic

## AI Summary

```yaml
project:
  name: portable_boot_example
  purpose: portable_mcu_boot_framework_skeleton
  style: educational_framework_oriented
  status: prototype

architecture:
  images:
    - bootmanager
    - programmer
    - app
  layers:
    - bootloader_common
    - bootloader
    - application
    - linker
    - port
    - startup
    - stm32f103

boot_flow:
  reset_enters: bootmanager
  bootmanager_decides_between:
    - programmer
    - app
  app_can_request: programmer
  programmer_can_update: app

porting:
  keep:
    - bootloader/common/*
    - bootloader/*
    - application/*
    - linker/*
  replace_or_adjust:
    - port/port_system.*
    - port/port_flash.*
    - port/port_uart.*
    - startup_vector_file
    - linker_scripts
    - SystemInit

current_target:
  family: stm32f103
  has_example_linkers: true
  has_startup_example: true
  has_uart_programmer_example: true
  has_makefile_example: true

missing_features:
  - image_header
  - crc32
  - rollback
  - power_fail_safety
  - second_mcu_family_example
```

Mau kien truc boot co 3 image:

- `bootmanager`: quyet dinh boot vao `app` hay `programmer`
- `programmer`: nhan lenh update firmware
- `app`: firmware chinh, co the yeu cau boot vao `programmer`

Muc tieu cua thu muc nay:

- de doc
- de port sang nhieu dong STM32
- tach boot domain, application domain, va phan phu thuoc chip
- khong phu thuoc vao code firmware update cua repo lon

## Progress Log

### Current status

- Da tach kien truc thanh `bootloader/common/`, `bootloader/`, `application/`, `port/`
- Da co startup portable cho Cortex-M va vector table rieng cho STM32F103
- Da co linker script mau cho `BootManager`, `Programmer`, `App`
- Da co `Makefile` doc lap IDE va output tach rieng theo tung image
- Da tach linker thanh `linker common` + memory map tung image
- Da co app test don gian cho STM32F103
- Da co script Python gui image qua UART cho `Programmer`
- Da refactor API theo huong framework hon bang `boot_status_t`
- Da them tai lieu:
  - `ARCHITECTURE.md`
  - `PORTING.md`
  - `BOOT_FLOW.md`
  - `stm32f103/BRINGUP.md`
  - `stm32f103/LINKER_GUIDE.md`

### What is still missing

- Chua co CRC32 / image header / version header
- Chua co co che rollback hoac power-fail safety
- Chua build va verify thuc te trong workspace nay
- Chua co vi du port sang dong chip thu hai nhu STM32G0/F4
- Chua co huong dan CubeIDE tung buoc cho 3 image
- Chua co tool pack image nhieu phan vung thanh 1 goi chung

### Collaboration note

Tu dong ghi chu nay tro di, moi lan co thay doi dang ke trong `portable_boot_example`,
muc `Progress Log` se duoc cap nhat de de ban giao repo va theo doi tien do.

## Read This First

Neu em la nguoi moi, doc theo thu tu:

1. `README.md`
2. `ARCHITECTURE.md`
3. `BOOT_FLOW.md`
4. `PORTING.md`
5. `bootloader/bootmanager/main.c`
6. `bootloader/programmer/main.c`
7. `bootloader/common/`
8. `application/app_main.c`
9. `stm32f103/`
10. `Makefile`

## Thu muc

```text
portable_boot_example/
  README.md
  ARCHITECTURE.md
  PORTING.md
  BOOT_FLOW.md
  Makefile
  make/
  startup/
  linker/
  port/
  bootloader/
  application/
  stm32f103/
```

## Directory Intent

- `startup/`
  - startup flow chung va startup-related code
- `linker/`
  - linker common theo toolchain, khong gan cung 1 MCU cu the
- `make/`
  - config build local, khong commit duong dan may ca nhan
- `port/`
  - abstraction layer theo MCU / board
- `bootloader/`
  - chua boot domain:
    - `common/`
    - `bootmanager/`
    - `programmer/`
- `application/`
  - firmware chinh va entry `app_main.c`
- `stm32f103/`
  - example cu the cho dong STM32F103

## Y tuong

Flash map vi du:

- `0x08000000 - 0x08003FFF`: `BootManager`
- `0x08004000 - 0x08007FFF`: `Programmer`
- `0x08008000 - ...`: `App`

Flow:

1. Reset -> `BootManager`
2. `BootManager` doc `shared data`
3. Neu co yeu cau update -> jump `Programmer`
4. Neu khong -> check `App`
5. `App` valid -> jump `App`
6. `App` invalid -> jump `Programmer`

## Stable Decisions

Nhung quyet dinh kien truc dang duoc giu on dinh:

- reset vao `BootManager` truoc
- `BootManager` cang nho cang tot
- `Programmer` tach rieng khoi `App`
- `shared data` nam trong RAM `.noinit`
- `bootloader/common/` khong duoc goi logic tu repo lon
- startup duoc tach thanh:
  - startup flow chung cho Cortex-M
  - vector table rieng theo MCU family

## Porting

Khi doi chip, em thuong chi sua:

- `bootloader/common/boot_config.h`
- `port/port_system.c`
- `port/port_flash.c`
- `port/port_uart.c`

Logic boot trong `bootloader/common/`, `bootloader/`, `application/` co the giu nguyen hoac sua rat it.

## Build khong phu thuoc IDE

Da co `Makefile` de build tung image rieng:

- `make bootmanager`
- `make programmer`
- `make app`
- `make all`

Output duoc tach rieng tai:

- `out/stm32f103/bootmanager/`
- `out/stm32f103/programmer/`
- `out/stm32f103/app/`

Muoi chot de em hoc:

- moi target build = mot firmware image rieng
- moi image dung linker rieng
- moi image chi giu `MEMORY`, con section layout dung chung neu cung toolchain
- moi image sinh ra `.elf`, `.bin`, `.hex`, `.map`, `.lst` rieng

## AI Update Rule

Neu mot coding assistant tiep tuc phat trien thu muc nay, nen cap nhat lai:

- `AI Summary`
- `Progress Log`
- `Stable Decisions` neu co thay doi kien truc
- cac file `.md` lien quan neu flow hoac porting thay doi

## Do doc lap voi repo lon

Thu muc nay chi muon "muon y tuong", khong goi toi code cua:

- `buildcp420/sal/...`
- `application/...`
- `board/...`
- `platform/...`

No van con phu thuoc vao:

- header HAL/CMSIS cua project STM32 dich
- linker script co section `.noinit`
- startup file / vector table cua tung image

Noi cach khac, em co the copy nguyen `portable_boot_example/` sang mot project STM32 trang va chi can port lai lop `port/` cung linker.

## Startup da duoc tach de de port

Startup gio chia 2 lop:

- `startup/startup_portable_cortexm.*`: reset flow chung cho Cortex-M
- `stm32f103/startup/startup_stm32f103xb.c`: vector table rieng cho STM32F103

Khi doi sang dong chip khac, em thuong khong can viet lai toan bo startup.
Em chi can thay file vector table va `SystemInit()`.

## Tai lieu framework

- [ARCHITECTURE.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/ARCHITECTURE.md)
- [PORTING.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/PORTING.md)
- [BOOT_FLOW.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/BOOT_FLOW.md)

## Vi du cu the cho STM32F103

Da co bo linker va huong dan rieng tai:

- [stm32f103/README.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/README.md)
- [stm32f103/LINKER_GUIDE.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/LINKER_GUIDE.md)
- [linker/gnu/sections_common.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/linker/gnu/sections_common.ld)
- [make/config.mk.example](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/make/config.mk.example)
- [stm32f103/ld/bootmanager.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/bootmanager.ld)
- [stm32f103/ld/programmer.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/programmer.ld)
- [stm32f103/ld/app.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/app.ld)

## Shared data

`shared data` la vung RAM `.noinit` dung chung giua:

- `BootManager`
- `Programmer`
- `App`

No dung de truyen:

- yeu cau boot vao `Programmer`
- trang thai update
- ma loi tam thoi

## Luu y

Day la skeleton de hoc kien truc va de port.
No chua bao gom:

- CRC32 image
- rollback
- secure boot
- version header
- protocol update day du
