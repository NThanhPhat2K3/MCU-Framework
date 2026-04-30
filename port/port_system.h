/*
 * File: port_system.h
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Declares MCU-specific system init, address checks, reset, and jump helpers.
 */

#ifndef PORTABLE_BOOT_EXAMPLE_PORT_SYSTEM_H
#define PORTABLE_BOOT_EXAMPLE_PORT_SYSTEM_H

#include <stdint.h>

typedef struct {
  void (*init)(void);
  void (*reset)(void);
  int (*is_flash_addr)(uint32_t addr);
  int (*is_ram_addr)(uint32_t addr);
  void (*prepare_jump)(uint32_t vector_addr, uint32_t stack_ptr);
} port_system_ops_t;

void port_system_init(void);
void port_system_reset(void);
int port_system_is_flash_addr(uint32_t addr);
int port_system_is_ram_addr(uint32_t addr);
void port_system_prepare_jump(uint32_t vector_addr, uint32_t stack_ptr);

#endif
