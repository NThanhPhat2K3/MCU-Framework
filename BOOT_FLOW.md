# Boot Flow

## Normal boot

```text
Reset
-> BootManager
-> shared init
-> no programmer request
-> App valid
-> jump App
```

## Update request from App

```text
App
-> app_request_programmer_and_reset()
-> shared.boot_request = PROGRAMMER
-> reset
-> BootManager
-> jump Programmer
```

## First bring-up without App

```text
Reset
-> BootManager
-> App invalid
-> Programmer valid
-> jump Programmer
```

## Jump mechanics

```text
boot_jump_to_image(addr)
-> read MSP at addr + 0
-> read Reset_Handler at addr + 4
-> prepare CPU state
-> jump Reset_Handler
-> startup
-> main()
```

## Target-specific note

Flow boot logic giua `stm32f103` va `stm32f411ce` la giong nhau.

Khac biet hien tai nam o:

- memory map
- startup vector table
- flash erase/program implementation
- UART / GPIO init cho app bring-up

Vi vay khi move repo sang workspace moi, neu flow sai o 1 target nhung dung o target kia,
uu tien kiem tra:

1. linker script cua target
2. `bootloader/common/boot_config.h`
3. startup file cua target
4. `mcu/stm32f1/port_flash_stm32f1.c` hoac `mcu/stm32f4/port_flash_stm32f4.c`
5. `mcu/stm32/port_uart_stm32.c`
6. `targets/<target>/config/board_config.h`
