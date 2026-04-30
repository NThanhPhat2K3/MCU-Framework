# STM32F103 Bring-up Checklist

Tai lieu nay de em lam tung buoc cho 3 image:

- `BootManager`
- `Programmer`
- `App`

Muc tieu:

- build duoc tung image
- flash xuong board
- xac nhan flow boot hoat dong dung

## 1. Chuan bi

Can co:

- STM32CubeIDE hoac build system GCC tuong duong
- HAL/CMSIS cho STM32F103
- ST-Link hoac debug probe de nap firmware
- UART terminal tren `USART1` 115200 8N1
- `arm-none-eabi-gcc`

So do UART trong mau hien tai:

- `PA9`  = `USART1_TX`
- `PA10` = `USART1_RX`

## 2. Tao 3 image rieng

Nen tao 3 project hoac 3 build target rieng:

1. `bootmanager`
2. `programmer`
3. `app`

Moi image can:

- startup file
- linker script dung cho image do
- code chung tu `bootloader/common/`, `port/`

Neu build bang command line:

- dung [../Makefile](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/Makefile)
- copy [../make/config.mk.example](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/make/config.mk.example) thanh `make/config.mk`
- sua `STM32CUBE_F1_DIR`

## 3. Gan linker script

### BootManager

Dung:

- [ld/bootmanager.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/bootmanager.ld)

### Programmer

Dung:

- [ld/programmer.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/programmer.ld)

### App

Dung:

- [ld/app.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/app.ld)

## 4. Gan startup

Nen uu tien startup C:

- [startup/startup_stm32f103xb.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/startup/startup_stm32f103xb.c)
- [startup/startup_portable_cortexm.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/startup/startup_portable_cortexm.c)

Neu project da co startup rieng cua CubeIDE thi:

- giu startup cua CubeIDE
- chi can dam bao vector table/linker dung dia chi image

## 5. Build order de thu nhanh

Thu theo dung thu tu nay:

1. Build `BootManager`
2. Build `Programmer`
3. Flash `BootManager`
4. Flash `Programmer`
5. Reset board
6. Kiem tra board vao `Programmer`
7. Build `App`
8. Gui `App` qua UART

Neu build bang Makefile:

```bash
make bootmanager
make programmer
make app
```

## 6. Ket qua mong doi

### Case A: chi co BootManager + Programmer

Sau reset:

```text
BootManager
-> App invalid
-> jump Programmer
-> UART in ra "PROGRAMMER"
```

Neu khong thay `PROGRAMMER`:

- kiem tra `port_uart.c`
- kiem tra clock
- kiem tra startup file
- kiem tra linker cua `Programmer`

### Case B: da nap them App

Sau reset:

```text
BootManager
-> App valid
-> jump App
-> UART in ra "APP RUNNING"
```

### Case C: App yeu cau update

Khi gui `'u'` cho app:

```text
App
-> app_request_programmer_and_reset()
-> BootManager
-> jump Programmer
-> UART in ra "PROGRAMMER"
```

## 7. Debug theo tung tang

### Tang 1: image placement

Kiem tra file `.map` cua tung image:

- `BootManager` phai bat dau tai `0x08000000`
- `Programmer` phai bat dau tai `0x08004000`
- `App` phai bat dau tai `0x08008000`

### Tang 2: startup

Kiem tra:

- `.isr_vector` o dau image
- `Reset_Handler` ton tai
- `_sidata`, `_sdata`, `_edata`, `_sbss`, `_ebss` hop le

### Tang 3: jump logic

Kiem tra:

- `boot_jump_to_image(PROGRAMMER_ADDR)`
- `boot_jump_to_image(APP_ADDR)`

### Tang 4: UART / Flash

Kiem tra:

- `port_uart_init()`
- `port_flash_erase_app()`
- `port_flash_write()`

## 8. Tinh huong loi hay gap

### Jump sang image roi treo ngay

Thuong do:

- linker sai
- vector table sai
- startup file sai
- `VTOR` sai
- image dich khong dung dia chi

### Khong vao duoc App

Thuong do:

- `boot_image_check(APP_ADDR)` fail
- app build sai offset

### UART im lang

Thuong do:

- clock chua dung
- UART init chua dung
- wiring PA9/PA10 sai

## 9. Muc tiep theo sau bring-up

Sau khi flow co ban chay duoc, nen lam tiep:

1. them image header
2. them CRC32
3. them verify image truoc khi jump app
4. them fault handling / error code ro hon
