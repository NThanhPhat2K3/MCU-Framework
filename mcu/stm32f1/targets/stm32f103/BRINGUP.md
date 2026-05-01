# STM32F103 Bring-Up Checklist

Use this checklist to bring up the three images on the STM32F103 target.

## Goal

- build all images
- flash the board
- confirm the boot flow works

## 1. Prerequisites

You need:

- `arm-none-eabi-gcc`
- an ST-Link or similar debug probe
- a UART terminal
- access to `USART1` at `115200 8N1`

Current UART wiring:

- `PA9`: `USART1_TX`
- `PA10`: `USART1_RX`

## 2. Build the Images

From the repository root:

```bash
make TARGET=stm32f103 bootmanager
make TARGET=stm32f103 programmer
make TARGET=stm32f103 app
```

## 3. Flash Order

Recommended order:

1. flash `BootManager`
2. flash `Programmer`
3. reset the board
4. confirm the board enters `Programmer`
5. build and flash `App`, or upload it through UART

## 4. Expected Results

### Case A: only `BootManager` and `Programmer`

After reset:

```text
BootManager
-> App invalid
-> jump Programmer
-> UART prints "PROGRAMMER"
```

### Case B: `App` is present

After reset:

```text
BootManager
-> App valid
-> jump App
-> UART prints "APP RUNNING"
```

### Case C: app requests update

After sending `u` to the app:

```text
App
-> app_request_programmer_and_reset()
-> BootManager
-> jump Programmer
-> UART prints "PROGRAMMER"
```

## 5. Quick Debug Checks

Check the `.map` files:

- `BootManager` should start at `0x08000000`
- `Programmer` should start at `0x08004000`
- `App` should start at `0x08008000`

Check the startup path:

- `.isr_vector` is at the start of the image
- `Reset_Handler` exists
- `_sidata`, `_sdata`, `_edata`, `_sbss`, `_ebss` look correct

Check the jump path:

- `boot_jump_to_image(PROGRAMMER_ADDR)`
- `boot_jump_to_image(APP_ADDR)`

Check the backends:

- `port_uart_init()`
- `port_flash_erase_app()`
- `port_flash_write()`

## 6. Common Problems

### The jump hangs immediately

Usually this means:

- wrong linker script
- wrong vector table
- wrong startup file
- wrong `VTOR`
- wrong flash base address

### The board never enters `App`

Usually this means:

- `boot_image_check(APP_ADDR)` fails
- the app was linked for the wrong address

### UART is silent

Usually this means:

- wrong board wiring
- wrong clock setup
- wrong baud rate
- wrong UART instance

- clock chua dung
- UART init chua dung
- wiring PA9/PA10 sai

## 9. Muc tiep theo sau bring-up

Sau khi flow co ban chay duoc, nen lam tiep:

1. them image header
2. them CRC32
3. them verify image truoc khi jump app
4. them fault handling / error code ro hon
