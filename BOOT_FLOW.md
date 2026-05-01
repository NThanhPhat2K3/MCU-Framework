# Boot Flow

## Normal Boot

```text
Reset
-> BootManager
-> initialize shared state
-> no programmer request
-> App is valid
-> jump to App
```

## Update Request From App

```text
App
-> app_request_programmer_and_reset()
-> shared boot request = PROGRAMMER
-> reset
-> BootManager
-> jump to Programmer
```

## Bring-Up Without App

```text
Reset
-> BootManager
-> App is not valid
-> Programmer is valid
-> jump to Programmer
```

## Image Jump Sequence

`BootManager` does not jump directly to `main()`.

It jumps to the start of the target image:

```text
boot_jump_to_image(image_addr)
-> read initial MSP from image_addr + 0
-> read Reset_Handler from image_addr + 4
-> prepare CPU state
-> set MSP
-> set VTOR
-> jump to Reset_Handler
-> startup code runs
-> main() runs
```

## Shared Logic vs Target Logic

The high-level boot logic is the same for `stm32f103` and `stm32f411ce`.

The target-specific differences are:

- flash memory layout
- startup vector file
- board clock setup
- UART and GPIO pin setup
- flash erase and program backend

## Debug Checklist

If one target works and another does not, check these files first:

1. `bootloader/common/boot_config.h`
2. `targets/<target>/ld/*.ld`
3. `targets/<target>/startup/*`
4. `mcu/<family>/targets/<target>/config/board_config.h`
5. `mcu/stm32/port_system_stm32.c`
6. `mcu/stm32/port_uart_stm32.c`
7. the target flash backend under `mcu/stm32f1/` or `mcu/stm32f4/`
