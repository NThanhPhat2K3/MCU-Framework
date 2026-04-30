# STM32F103 Example

This folder contains the STM32F103 example target for the three-image boot flow:

- `BootManager`
- `Programmer`
- `App`

## Memory Map

Typical layout for the current example:

```text
0x08000000 - 0x08003FFF   BootManager   16 KB
0x08004000 - 0x08007FFF   Programmer    16 KB
0x08008000 - 0x0801FFFF   App
```

If you use a smaller flash variant, update:

- `bootloader/common/boot_config.h`
- linker scripts in `stm32f103/ld/`

## Important Files

- [bootmanager.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/stm32f103/ld/bootmanager.ld)
- [programmer.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/stm32f103/ld/programmer.ld)
- [app.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/stm32f103/ld/app.ld)
- [stm32f1xx_hal_conf.h](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/stm32f103/config/stm32f1xx_hal_conf.h)
- [system_stm32f1xx_min.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/stm32f103/system/system_stm32f1xx_min.c)
- [BRINGUP.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/stm32f103/BRINGUP.md)
- [LINKER_GUIDE.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/stm32f103/LINKER_GUIDE.md)

## Build

```bash
make TARGET=stm32f103 all
```

Or build images one by one:

```bash
make TARGET=stm32f103 bootmanager
make TARGET=stm32f103 programmer
make TARGET=stm32f103 app
```

## UART Assumptions

Current example:

- `USART1`
- `PA9  = TX`
- `PA10 = RX`

If your board uses different pins, update:

- `application/app_main.c`
- `port/port_uart.c`

## Quick Test

1. Flash `BootManager`
2. Flash `Programmer`
3. Flash `App` or update `App` through UART
4. Open serial monitor
5. If needed, send `u` from the app to enter `Programmer`

The desktop tool can also be used:

```bash
python3 tools/flasher.py
```
