# Startup Notes

The startup code is split into two parts.

## Portable Cortex-M Startup

Shared files:

- `startup/startup_portable_cortexm.h`
- `startup/startup_portable_cortexm.c`

This part handles:

- copying `.data`
- clearing `.bss`
- calling `SystemInit()`
- calling `__libc_init_array()`
- calling `main()`

## Target-Specific Vector File

STM32F103 file:

- `mcu/stm32f1/targets/stm32f103/startup/startup_stm32f103xb.c`

This file mainly contains:

- `.isr_vector`
- the IRQ handler list for the target

## Porting Rule

When moving to another Cortex-M target:

1. keep `startup_portable_cortexm.*`
2. add a new vector file for the new MCU
3. update the IRQ list
4. provide the correct `SystemInit()`
5. keep the linker symbols consistent
