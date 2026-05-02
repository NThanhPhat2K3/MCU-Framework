/*
 * File: boot_config.h
 * Author: Phat Nguyen
 * Date: 2026-04-29
 * Description: Defines flash layout constants and shared boot configuration
 * values.
 */

#ifndef PORTABLE_BOOT_EXAMPLE_BOOT_CONFIG_H
#define PORTABLE_BOOT_EXAMPLE_BOOT_CONFIG_H

#include <stdint.h>

#if defined(STM32F103xB)
#define BOOT_MANAGER_ADDR 0x08000000u
#define PROGRAMMER_ADDR 0x08004000u
#define APP_ADDR 0x08008000u
#define FLASH_BASE_ADDR 0x08000000u
#define FLASH_END_ADDR 0x08020000u
#define FLASH_WRITE_ALIGNMENT 2u /* STM32F1: halfword (16-bit) */
#elif defined(STM32F411xE)
#define BOOT_MANAGER_ADDR 0x08000000u
#define PROGRAMMER_ADDR 0x08008000u
#define APP_ADDR 0x08010000u
#define FLASH_BASE_ADDR 0x08000000u
#define FLASH_END_ADDR 0x08080000u
#define FLASH_WRITE_ALIGNMENT 4u /* STM32F4: word (32-bit) */
#else
#error Unsupported MCU for boot_config.h
#endif

#define APP_VECTOR_ADDR APP_ADDR

/* Shared memory magic number
0x53 = 'S'
0x48 = 'H'
0x44 = 'D'
0x30 = '0'
'S' 'H' 'D' '0'
SHD = shared
0 = version 0
*/
#define SHARED_MAGIC 0x53484430u

#endif
