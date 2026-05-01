# STM32F411CE Bring-Up Checklist

## 1. Prerequisites

You need:

- `arm-none-eabi-gcc`
- an ST-Link or similar debug probe
- a UART terminal at `115200 8N1`

## 2. Build

From the repository root:

```bash
make TARGET=stm32f411ce bootmanager
make TARGET=stm32f411ce programmer
make TARGET=stm32f411ce app
```

## 3. Flash Order

1. flash `BootManager`
2. flash `Programmer`
3. reset the board
4. confirm the board enters `Programmer` if no app is present
5. flash or upload `App`

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

## 5. Important Layout Note

This target places the app at `0x08010000` so the app region sits on larger
STM32F4 sectors.

If you change that layout, update all of these together:

- `bootloader/common/boot_config.h`
- `mcu/stm32f4/targets/stm32f411ce/ld/*.ld`
- `mcu/stm32f4/port_flash_stm32f4.c`
