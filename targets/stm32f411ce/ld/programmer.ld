/*
 * File: programmer.ld
 * Author: Phat Nguyen
 * Date: 2026-04-29
 * Description: Defines the STM32F411CE memory map for the Programmer image.
 */

PORTABLE_HEAP_SIZE = 0x800;
PORTABLE_STACK_RESERVE_SIZE = 0x400;

MEMORY
{
  FLASH  (rx)  : ORIGIN = 0x08008000, LENGTH = 32K
  RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 0x1FFE0
  NOINIT (xrw) : ORIGIN = 0x2001FFE0, LENGTH = 32
}

INCLUDE linker/gnu/sections_common.ld
