# STM32F411CE Black Pill Example

This folder contains the STM32F411CE target example for the three-image boot flow:

- `BootManager`
- `Programmer`
- `App`

## Memory Map

```text
0x08000000 - 0x08007FFF   BootManager   32 KB
0x08008000 - 0x0800FFFF   Programmer    32 KB
0x08010000 - 0x0807FFFF   App / remaining flash
```

The app starts at `0x08010000` so erase and update are easier on STM32F4 sector
layout.

## Current Board Assumptions

- board: `STM32F411CEU6 Black Pill`
- LED: `PC13`
- debug/update UART: `USART1`
- `PA9  = USART1_TX`
- `PA10 = USART1_RX`
- clock: `HSE 25 MHz`

If your board wiring is different, update:

- `application/app_main.c`
- `port/port_uart.c`

## Build

```bash
make TARGET=stm32f411ce all
```

Or image by image:

```bash
make TARGET=stm32f411ce bootmanager
make TARGET=stm32f411ce programmer
make TARGET=stm32f411ce app
```

## UART Note

The current STM32F411 example now uses:

- `HSE 25 MHz`
- `USART1 @ 115200 8N1`

This change was important to make UART behavior stable on the tested board.

## Update Flow

1. Boot into `App`
2. Send `u` if you want to enter `Programmer`
3. Use the host tool:

```bash
python3 tools/flasher.py
```

Target app address in the tool:

```text
0x08010000
```
