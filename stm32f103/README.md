# STM32F103 Example

Thu muc nay la bo vi du cu the cho `STM32F103C8/CB` theo huong:

- `BootManager` o dau flash
- `Programmer` o giua flash
- `App` o offset cao hon

Muc dich:

- cho em nhin ro memory map
- co linker script tach rieng tung image
- de copy sang project CubeIDE trang
- co startup file toi thieu va app test UART

## Memory Map

Vi du cho MCU 128KB flash:

```text
0x08000000 - 0x08003FFF   BootManager   (16KB)
0x08004000 - 0x08007FFF   Programmer    (16KB)
0x08008000 - 0x0801FBFF   App           (95KB)
0x0801FC00 - 0x0801FFFF   reserved      (1KB)
```

Neu em dung ban 64KB flash thi phai giam `FLASH_END_ADDR` trong `bootloader/common/boot_config.h`
va sua lai linker script cho dung kich thuoc that.

## Linker Scripts

Co 3 file:

- [ld/bootmanager.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/bootmanager.ld)
- [ld/programmer.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/programmer.ld)
- [ld/app.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/app.ld)

Va 1 file section dung chung:

- [../linker/gnu/sections_common.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/linker/gnu/sections_common.ld)

Co them:

- [../Makefile](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/Makefile)
- [../make/config.mk.example](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/make/config.mk.example)
- [config/stm32f1xx_hal_conf.h](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/config/stm32f1xx_hal_conf.h)
- [startup/startup_stm32f103xb.s](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/startup/startup_stm32f103xb.s)
- [startup/startup_stm32f103xb.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/startup/startup_stm32f103xb.c)
- [startup_notes.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/startup/startup_notes.md)
- [BRINGUP.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/BRINGUP.md)
- [LINKER_GUIDE.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/LINKER_GUIDE.md)
- [system/system_stm32f1xx_min.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/system/system_stm32f1xx_min.c)
- [tools/send_image.py](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/tools/send_image.py)

## Cach dung trong CubeIDE

Lam 3 project rieng:

1. `bootmanager`
2. `programmer`
3. `app`

Moi project:

- dung startup file cua STM32F103
- dung HAL/CMSIS cua STM32Cube
- copy code tu `portable_boot_example/`
- doi linker script thanh file phu hop

Neu em khong muon dung startup asm cua CubeIDE, em co the dung file startup C tai `startup/startup_stm32f103xb.c`.
Ban `.s` duoc giu lai chi de tham khao.
Reset flow chung da duoc dua vao `startup/startup_portable_cortexm.c` de de port sang chip khac.

## Cach dung khong phu thuoc IDE

Neu em muon hoc theo huong framework va build bang command line, thu theo flow:

1. copy `make/config.mk.example` thanh `make/config.mk`
2. sua `STM32CUBE_F1_DIR` tro toi STM32CubeF1 package tren may em
3. chay:

```bash
make bootmanager
make programmer
make app
```

Hoac:

```bash
make all
```

Output se nam tai:

```text
out/stm32f103/bootmanager/
out/stm32f103/programmer/
out/stm32f103/app/
```

Moi image se co:

- `.elf`
- `.bin`
- `.hex`
- `.map`
- `.lst`

Lien ket linker hien tai duoc tach theo huong:

- `linker/gnu/sections_common.ld`: bo cuc section dung chung
- `bootmanager.ld`: memory map BootManager
- `programmer.ld`: memory map Programmer
- `app.ld`: memory map App

## Cac diem can sua trong project moi

### 1. `port/port_system.c`

Em them:

- `SystemClock_Config()`
- neu can thi init watchdog / gpio co ban

### 2. `port/port_uart.c`

Em phai init that `USART1`

Ban hien tai da co init toi thieu cho:

- PA9  = USART1_TX
- PA10 = USART1_RX
- 115200 8N1

### 3. `port/port_flash.c`

Bo nay dang theo style `STM32F1`:

- page erase
- halfword program

Nen rat hop voi F103.

### 4. `.noinit`

Phai co section `.noinit` trong linker script de giu `g_boot_shared`
qua reset mem.

Ban linker trong thu muc nay da co roi.

## Flow thuc te

### Lan dau tien

Neu chip moi va em nap:

- `BootManager`
- `Programmer`

ma chua nap `App`, thi:

```text
Reset
-> BootManager
-> App invalid
-> jump Programmer
-> Programmer cho UART de nap App
```

App test trong [application/app_main.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/application/app_main.c) se:

- in `APP RUNNING`
- chớp LED PC13
- nhan `'u'` tren USART1 de quay lai `Programmer`

### Sau khi da co App

```text
Reset
-> BootManager
-> App valid
-> jump App
```

### Khi App muon update

```text
App
-> app_request_programmer_and_reset()
-> BootManager
-> jump Programmer
```

## Luu y

Day la vi du kien truc va linker.
No da gan hon muc "flash xuong chay thu", nhung em van phai:

- noi startup file cua CubeIDE
- noi clock init
- dua dung include path HAL

Neu em dung startup C cua thu muc nay thi bo dong "noi startup file cua CubeIDE".

## Cach thu nhanh

1. Build va nap `BootManager`
2. Build va nap `Programmer`
3. Mo serial terminal 115200 tren USART1
4. Reset board
5. Neu chua co App, board se vao `Programmer`
6. Build `App` ra file `.bin`
7. Dung script:

```bash
python portable_boot_example/stm32f103/tools/send_image.py \
  --port COM5 \
  --app-bin app.bin
```

8. Programmer se erase/write/jump sang App

Neu em muon lam tung buoc ro rang hon theo checklist, doc:

- [BRINGUP.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/BRINGUP.md)

Neu em dang bi mo ho o linker, symbol startup, hoac quan he giua `PROGRAMMER_ADDR`,
`.isr_vector`, `Reset_Handler`, `main()`, doc them:

- [LINKER_GUIDE.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/LINKER_GUIDE.md)
