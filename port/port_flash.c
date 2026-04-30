/*
 * File: port_flash.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Provides the portable flash interface and dispatches calls to
 * the active MCU backend through a small ops table.
 */

#include "port_flash.h"

#include <stddef.h>

#if defined(STM32F103xB)
#include "port_flash_stm32f1.h"
#elif defined(STM32F411xE)
#include "port_flash_stm32f4.h"
#else
#error Unsupported MCU backend for port_flash.c
#endif

static const port_flash_ops_t *g_flash_ops;

static const port_flash_ops_t *port_flash_get_ops(void) {
  if (g_flash_ops != NULL) {
    return g_flash_ops;
  }

#if defined(STM32F103xB)
  g_flash_ops = port_flash_stm32f1_get_ops();
#elif defined(STM32F411xE)
  g_flash_ops = port_flash_stm32f4_get_ops();
#endif

  return g_flash_ops;
}

boot_status_t port_flash_erase_app(void) {
  const port_flash_ops_t *ops = port_flash_get_ops();

  if ((ops == NULL) || (ops->erase_app == NULL)) {
    return BOOT_STATUS_IO_ERROR;
  }

  return ops->erase_app();
}

boot_status_t port_flash_write(uint32_t addr, const uint8_t *data,
                               uint32_t len) {
  const port_flash_ops_t *ops = port_flash_get_ops();

  if ((ops == NULL) || (ops->write == NULL)) {
    return BOOT_STATUS_IO_ERROR;
  }

  return ops->write(addr, data, len);
}
