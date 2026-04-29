# STM32F411CE Bring-up Checklist

## 1. Chuan bi

Can co:

- STM32CubeF4 package
- `arm-none-eabi-gcc`
- ST-Link hoac probe tuong duong
- UART terminal 115200 8N1

## 2. Build

Them vao `make/config.mk`:

```make
STM32CUBE_F4_DIR := /absolute/path/to/STM32Cube_FW_F4
```

Sau do build:

```bash
make TARGET=stm32f411ce bootmanager
make TARGET=stm32f411ce programmer
make TARGET=stm32f411ce app
```

## 3. Flash thu tu

1. flash `BootManager`
2. flash `Programmer`
3. reset board
4. neu chua co `App` thi ky vong vao `Programmer`
5. build va nap `App`

## 4. Ket qua mong doi

### Case A: moi co BootManager + Programmer

Sau reset:

```text
BootManager
-> App invalid
-> jump Programmer
-> UART in ra "PROGRAMMER"
```

### Case B: da co App

Sau reset:

```text
BootManager
-> App valid
-> jump App
-> UART in ra "APP RUNNING"
```

## 5. Luu y quan trong

Target F411 nay dang dung layout flash theo huong de cho `App` nam tren vung sector lon.
Neu em doi layout, nho sua dong bo:

- `bootloader/common/boot_config.h`
- `stm32f411ce/ld/*.ld`
- `port/port_flash.c`
