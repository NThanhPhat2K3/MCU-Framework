/*
 * File: app.ld
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Defines the STM32F103 memory map for the application image.
 */

/*
 * STM32F103 App memory map.
 * Common section layout is shared from sections_common.ld.
 */

PORTABLE_HEAP_SIZE = 0x400;
PORTABLE_STACK_RESERVE_SIZE = 0x400;

MEMORY
{
  FLASH  (rx)  : ORIGIN = 0x08008000, LENGTH = 95K
  RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 0x4FE0
  NOINIT (xrw) : ORIGIN = 0x20004FE0, LENGTH = 32
}

INCLUDE linker/gnu/sections_common.ld
