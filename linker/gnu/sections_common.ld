/*
 * Common GNU ld section layout for portable_boot_example.
 *
 * This file is intentionally split from per-image MEMORY maps so that:
 * - BootManager / Programmer / App share one section layout
 * - per-image linker files only describe FLASH/RAM placement
 * - future STM32 variants can reuse the same structure
 *
 * Toolchain note:
 * - This file is written for GNU ld
 * - other toolchains such as Keil/IAR/MSP/Renesas usually need an equivalent
 *   scatter/icf/xcl style file, but the split between "common sections" and
 *   "target memory map" should remain the same
 */

ENTRY(Reset_Handler)

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
    KEEP(*(.init))
    KEEP(*(.fini))
    . = ALIGN(4);
    _etext = .;
  } > FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > FLASH

  _sidata = LOADADDR(.data);

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > RAM AT > FLASH

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  .noinit (NOLOAD) :
  {
    . = ALIGN(4);
    *(.noinit)
    *(.noinit*)
    . = ALIGN(4);
  } > NOINIT

  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + PORTABLE_HEAP_SIZE;
    . = . + PORTABLE_STACK_RESERVE_SIZE;
    . = ALIGN(8);
  } > RAM

  .ARM.attributes 0 :
  {
    *(.ARM.attributes)
  }
}
