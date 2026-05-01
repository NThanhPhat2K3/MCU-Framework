# STM32F103 Linker Guide

This note explains the linker layout used by the STM32F103 example.

## Why There Are Three Linker Scripts

The project builds three separate images:

- `BootManager`
- `Programmer`
- `App`

Each image has a different flash address, so each image needs its own linker
script.

## What a Linker Script Does

A linker script answers questions like:

- where the image starts
- where the vector table goes
- which RAM region is used
- where the stack starts
- where startup symbols such as `_sidata` and `_sbss` live

It does not call `main()`.  
The jump decision is made at runtime by `BootManager`.

## Example: `Programmer`

If `programmer.ld` says:

```ld
FLASH (rx) : ORIGIN = 0x08004000, LENGTH = 16K
```

then the `Programmer` image starts at `0x08004000`.

If the linker places:

```ld
.isr_vector :
{
    KEEP(*(.isr_vector))
} > FLASH
```

then the vector table is placed at the start of that flash region, which means:

- `Programmer` vector table address = `0x08004000`

## Where `.isr_vector` Comes From

The linker script places `.isr_vector`, but the startup file defines it.

For the STM32F103 target, the vector table comes from:

- `mcu/stm32f1/targets/stm32f103/startup/startup_stm32f103xb.c`

## Why `PROGRAMMER_ADDR` Is Not `main()`

`PROGRAMMER_ADDR` points to the start of the firmware image.

At that address:

- word 0 = initial MSP
- word 1 = `Reset_Handler`

So the jump flow is:

```text
read MSP from image base
read Reset_Handler from image base + 4
set MSP
set VTOR
jump to Reset_Handler
startup runs
main() runs
```

## Startup Symbols

The startup code uses these linker-provided symbols:

- `_sidata`
- `_sdata`
- `_edata`
- `_sbss`
- `_ebss`
- `_estack`

Meaning:

- `_sidata`: source address in flash for initialized `.data`
- `_sdata`: start of `.data` in RAM
- `_edata`: end of `.data` in RAM
- `_sbss`: start of `.bss` in RAM
- `_ebss`: end of `.bss` in RAM
- `_estack`: initial stack top

## Why Flash Layout Changes Require Linker Changes

If you change the image flash origin, you also change:

- vector table address
- code address
- `.data` load address
- startup symbols

That is why every image layout change needs a matching linker update.

## Current Split

The current linker structure is:

- `linker/gnu/sections_common.ld`
  - shared section layout
- `mcu/stm32f1/targets/stm32f103/ld/bootmanager.ld`
  - BootManager memory map
- `mcu/stm32f1/targets/stm32f103/ld/programmer.ld`
  - Programmer memory map
- `mcu/stm32f1/targets/stm32f103/ld/app.ld`
  - App memory map

This split keeps the common GNU section layout separate from the per-image
memory map.
