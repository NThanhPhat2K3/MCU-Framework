# Portable Boot Example

> Human + AI friendly project note.
> This file is intended to be readable by both developers and coding assistants.

## Quick Context

Portable boot/update framework skeleton for Cortex-M MCUs.

Current concrete targets:

- STM32F103 example included
- STM32F411CE Black Pill example included

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
    - stm32f411ce

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
  families:
    - stm32f103
    - stm32f411ce
  has_example_linkers: true
  has_startup_example: true
  has_uart_programmer_example: true
  has_makefile_example: true

missing_features:
  - image_header
  - crc32
  - rollback
  - power_fail_safety
  - vendor_neutral_hal_layer
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
- Da co startup portable cho Cortex-M va vector table rieng cho STM32F103 / STM32F411CE
- Da co linker script mau cho `BootManager`, `Programmer`, `App`
- Da co `Makefile` doc lap IDE va output tach rieng theo tung image
- Da tach linker thanh `linker common` + memory map tung image
- Da co app test don gian cho STM32F103 va STM32F411CE
- Da co script Python gui image qua UART cho `Programmer`
- Da them header comment thong nhat cho code/config/linker/build files
- Da refactor API theo huong framework hon bang `boot_status_t`
- Da them tai lieu:
  - `ARCHITECTURE.md`
  - `PORTING.md`
  - `BOOT_FLOW.md`
  - `stm32f103/BRINGUP.md`
  - `stm32f103/LINKER_GUIDE.md`
  - `stm32f411ce/BRINGUP.md`

### What is still missing

- Chua co CRC32 / image header / version header
- Chua co co che rollback hoac power-fail safety
- Chua build va verify thuc te trong workspace nay
- Chua tach board config / BSP rieng khoi `application/app_main.c`
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
10. `stm32f411ce/`
11. `Makefile`

## Thu muc

```text
portable_boot_example/
  README.md
  ARCHITECTURE.md
  PORTING.md
  BOOT_FLOW.md
  Makefile
  make/
  vendor/
  startup/
  linker/
  port/
  bootloader/
  application/
  stm32f103/
  stm32f411ce/
```

## Directory Intent

- `startup/`
  - startup flow chung va startup-related code
- `linker/`
  - linker common theo toolchain, khong gan cung 1 MCU cu the
- `make/`
  - config build local, khong commit duong dan may ca nhan
- `vendor/`
  - noi dat STM32Cube package local nhu `STM32CubeF1/`, `STM32CubeF4/`
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
- `stm32f411ce/`
  - example cu the cho dong STM32F411CE Black Pill

## Y tuong

Flash map vi du:

- `0x08000000 - 0x08003FFF`: `BootManager`
- `0x08004000 - 0x08007FFF`: `Programmer`
- `0x08008000 - ...`: `App`

Flash map F411CE example:

- `0x08000000 - 0x08007FFF`: `BootManager`
- `0x08008000 - 0x0800FFFF`: `Programmer`
- `0x08010000 - ...`: `App`

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
- `boot_config.h` co the chua flash map theo target, nhung linker script van la noi quyet dinh memory map cuoi cung
- F4 layout uu tien canh theo sector erase de de xu ly flash hon F1

## Porting

Khi doi chip, em thuong chi sua:

- `bootloader/common/boot_config.h`
- `port/port_system.c`
- `port/port_flash.c`
- `port/port_uart.c`

Logic boot trong `bootloader/common/`, `bootloader/`, `application/` co the giu nguyen hoac sua rat it.

## Target Matrix

### `stm32f103`

- core: Cortex-M3
- HAL package: `STM32CubeF1`
- flash map hien tai:
  - `BootManager` 16KB
  - `Programmer` 16KB
  - `App` bat dau tai `0x08008000`

### `stm32f411ce`

- board: WeAct Black Pill STM32F411CEU6
- core: Cortex-M4
- HAL package: `STM32CubeF4`
- flash map hien tai:
  - `BootManager` 32KB
  - `Programmer` 32KB
  - `App` bat dau tai `0x08010000`
- ly do layout:
  - giu `App` tren ranh gioi sector lon de erase update don gian hon tren F4

## Build khong phu thuoc IDE

## Build khong phu thuoc IDE

Da co `Makefile` de build tung image rieng:

- `make bootmanager`
- `make programmer`
- `make app`
- `make all`

Build theo target:

- `make TARGET=stm32f103 all`
- `make TARGET=stm32f411ce all`

Output duoc tach rieng tai:

- `out/stm32f103/bootmanager/`
- `out/stm32f103/programmer/`
- `out/stm32f103/app/`
- `out/stm32f411ce/bootmanager/`
- `out/stm32f411ce/programmer/`
- `out/stm32f411ce/app/`

Can cau hinh `make/config.mk`:

- `STM32CUBE_F1_DIR` cho target `stm32f103`
- `STM32CUBE_F4_DIR` cho target `stm32f411ce`

Khuyen nghi layout local:

```text
vendor/
  STM32CubeF1/
  STM32CubeF4/
```

Repo dang uu tien dat HAL/CMSIS trong `vendor/`, khong gom chung vao `bsp/`.
`bsp/` nen de danh cho board-specific code cua rieng em.

Muoi chot de em hoc:

- moi target build = mot firmware image rieng
- moi image dung linker rieng
- moi image chi giu `MEMORY`, con section layout dung chung neu cung toolchain
- moi image sinh ra `.elf`, `.bin`, `.hex`, `.map`, `.lst` rieng

## F411 Session Notes

Buoi bring-up `stm32f411ce` trong workspace nay da di qua duoc den muc:

- build thanh cong ca `bootmanager`, `programmer`, `app`
- flash thanh cong qua ST-Link
- LED app da chay
- UART app da in log

### Local setup da dung

- toolchain: `arm-none-eabi`
- host toolchain da dung:
  - `/mnt/hdd1/toolchain/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi`
- vendor package local:
  - `vendor/STM32CubeF1`
  - `vendor/STM32CubeF4`

### Build commands da verify

```bash
make TARGET=stm32f411ce all
make TARGET=stm32f411ce app
```

### Flash commands da verify

Can `st-flash` trong `PATH`.

```bash
make TARGET=stm32f411ce flash-bootmanager
make TARGET=stm32f411ce flash-programmer
make TARGET=stm32f411ce flash-app
make TARGET=stm32f411ce flash-all
```

Dia chi flash F411 hien tai:

- `BootManager` -> `0x08000000`
- `Programmer` -> `0x08008000`
- `App` -> `0x08010000`

### UART bring-up F411 da verify

- `USART1`
- `PA9`  = `TX`
- `PA10` = `RX`
- `115200 8N1`

App log hien tai:

```text
=== APP RUNNING ===
UART: USART1 @ 115200 8N1
Command: send 'u' to enter programmer
```

Neu dang o app, gui ky tu `u` qua UART thi app se request reset vao `Programmer`.

### UART update flow da chot

Repo da co script:

- `stm32f103/tools/send_image.py`

Script nay dung duoc cho F411 neu doi `--app-addr`:

```bash
python3 stm32f103/tools/send_image.py \
  --port /dev/ttyUSB0 \
  --baud 115200 \
  --app-bin out/stm32f411ce/app/app.bin \
  --app-addr 0x08010000
```

Can co:

1. `BootManager` + `Programmer` da duoc flash truoc
2. board dang o mode `Programmer`
3. Python co `pyserial`

Tren Ubuntu/Debian moi, de tranh loi `externally-managed-environment`, uu tien:

```bash
sudo apt install python3-serial
```

### Cac loi da gap va da xu ly

1. `STM32F4 HAL UART` doi `DMA_HandleTypeDef` ngay trong header
   - da them `HAL_DMA_MODULE_ENABLED`
   - da include `stm32f4xx_hal_dma.h`
   - da them `stm32f4xx_hal_dma.c` vao build F4

2. `HAL RCC EX` can `EXTERNAL_CLOCK_VALUE`
   - da them macro nay vao `stm32f4xx_hal_conf.h`

3. `HAL RCC` can symbol `AHBPrescTable` / `APBPrescTable`
   - da them vao `stm32f411ce/system/system_stm32f4xx_min.c`
   - da them `SystemCoreClockUpdate()`

4. app in banner roi co dau hieu chay khong on dinh
   - nguyen nhan lon la thieu `SysTick_Handler()`
   - da them `SysTick_Handler()` goi `HAL_IncTick()` trong `port/port_system.c`

5. `make all` truoc do chi build image dau
   - da sua `Makefile` de build du `bootmanager`, `programmer`, `app`

### Ghi chu kien truc

- `stm32f4xx_hal_conf.h` cua project nay la ban toi gian hoa theo nhu cau, khong can copy nguyen template dai cua ST
- tuy project khong chu dong dung DMA cho UART, F4 HAL van keo dependency DMA o muc header/source, nen phai giu DMA phan toi thieu de build qua
- `vendor/` la noi dat HAL/CMSIS package
- `bsp/` neu tach them sau nay thi nen de danh cho pin map / LED / button / board wiring cua rieng board

## Workspace handoff

Neu em copy repo sang workspace khac, ben nhan nen doc va kiem tra theo thu tu nay:

1. `README.md`
2. `ARCHITECTURE.md`
3. `PORTING.md`
4. target folder dang dung:
   - `stm32f103/` hoac
   - `stm32f411ce/`
5. `make/config.mk`
6. `Makefile`

Can chuan bi lai tren workspace moi:

- duong dan `STM32CUBE_F1_DIR` neu build F103
- duong dan `STM32CUBE_F4_DIR` neu build F411
- toolchain `arm-none-eabi-*`
- probe / serial terminal neu muon flash chay that

Nhung file can xem dau tien khi co loi bring-up:

- `bootloader/common/boot_config.h`
- `port/port_system.c`
- `port/port_flash.c`
- `port/port_uart.c`
- `stm32f103/ld/*.ld` hoac `stm32f411ce/ld/*.ld`
- startup file theo target

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
