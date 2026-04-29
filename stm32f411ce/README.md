# STM32F411CE Black Pill Example

Target nay them support cho `STM32F411CEU6 Black Pill` theo cung flow 3 image:

- `BootManager`
- `Programmer`
- `App`

## Memory Map

```text
0x08000000 - 0x08007FFF   BootManager   32KB
0x08008000 - 0x0800FFFF   Programmer    32KB
0x08010000 - 0x0807BFFF   App           432KB
0x0807C000 - 0x0807FFFF   reserved      16KB
```

Layout nay co y de bat dau `App` tai `0x08010000`, trung ranh gioi sector lon cua STM32F411,
nen viec erase update app don gian hon tren dong F4.

## Bring-up assumptions

Ban scaffold hien tai dang gia dinh:

- LED user o `PC13`
- debug UART dung `USART1`
- `PA9  = USART1_TX`
- `PA10 = USART1_RX`
- clock toi thieu dung `HSI 16MHz`

Neu board cua em khac wiring nay, sua lai trong:

- `application/app_main.c`
- `port/port_uart.c`

## Build

Can them `STM32CUBE_F4_DIR` trong `make/config.mk`.
Khuyen nghi clone package vao `vendor/STM32CubeF4/` cua repo nay.

Vi du:

```make
STM32CUBE_F4_DIR := /mnt/hdd1/project/MCU-Framework/vendor/STM32CubeF4
```

Build:

```bash
make TARGET=stm32f411ce bootmanager
make TARGET=stm32f411ce programmer
make TARGET=stm32f411ce app
```
