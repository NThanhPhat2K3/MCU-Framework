/*
 * STM32F103 Programmer memory map.
 * Common section layout is shared from sections_common.ld.
 */

#define PORTABLE_HEAP_SIZE 0x400
#define PORTABLE_STACK_RESERVE_SIZE 0x200

MEMORY
{
  FLASH  (rx)  : ORIGIN = 0x08004000, LENGTH = 16K
  RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 0x4FE0
  NOINIT (xrw) : ORIGIN = 0x20004FE0, LENGTH = 32
}

INCLUDE ../../linker/gnu/sections_common.ld
