/*
 * File: port_system.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Provides the portable system interface and dispatches calls to
 * the active MCU backend through a small ops table.
 */

#include "port_system.h"

#include <stddef.h>

#if defined(STM32F103xB) || defined(STM32F411xE)
#include "port_system_stm32.h"
#else
#error Unsupported MCU backend for port_system.c
#endif

static const port_system_ops_t *g_system_ops;

static const port_system_ops_t *port_system_get_ops(void) {
  if (g_system_ops != NULL) {
    return g_system_ops;
  }

  g_system_ops = port_system_stm32_get_ops();
  return g_system_ops;
}

void port_system_init(void) {
  const port_system_ops_t *ops = port_system_get_ops();

  if ((ops != NULL) && (ops->init != NULL)) {
    ops->init();
  }
}

void port_system_reset(void) {
  const port_system_ops_t *ops = port_system_get_ops();

  if ((ops != NULL) && (ops->reset != NULL)) {
    ops->reset();
  }
}

uint32_t port_system_get_tick(void) {
  const port_system_ops_t *ops = port_system_get_ops();

  if ((ops == NULL) || (ops->get_tick == NULL)) {
    return 0u;
  }

  return ops->get_tick();
}

void port_system_delay_ms(uint32_t delay_ms) {
  const port_system_ops_t *ops = port_system_get_ops();

  if ((ops != NULL) && (ops->delay_ms != NULL)) {
    ops->delay_ms(delay_ms);
  }
}

int port_system_is_flash_addr(uint32_t addr) {
  const port_system_ops_t *ops = port_system_get_ops();

  if ((ops == NULL) || (ops->is_flash_addr == NULL)) {
    return 0;
  }

  return ops->is_flash_addr(addr);
}

int port_system_is_ram_addr(uint32_t addr) {
  const port_system_ops_t *ops = port_system_get_ops();

  if ((ops == NULL) || (ops->is_ram_addr == NULL)) {
    return 0;
  }

  return ops->is_ram_addr(addr);
}

void port_system_prepare_jump(uint32_t vector_addr, uint32_t stack_ptr) {
  const port_system_ops_t *ops = port_system_get_ops();

  if ((ops != NULL) && (ops->prepare_jump != NULL)) {
    ops->prepare_jump(vector_addr, stack_ptr);
  }
}
