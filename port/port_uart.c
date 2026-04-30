/*
 * File: port_uart.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Provides the portable UART interface and dispatches calls to
 * the active MCU backend through a small ops table.
 */

#include "port_uart.h"

#include <stddef.h>

#if defined(STM32F103xB) || defined(STM32F411xE)
#include "port_uart_stm32.h"
#else
#error Unsupported MCU backend for port_uart.c
#endif

static const port_uart_ops_t *g_uart_ops;

static const port_uart_ops_t *port_uart_get_ops(void) {
  if (g_uart_ops != NULL) {
    return g_uart_ops;
  }

  g_uart_ops = port_uart_stm32_get_ops();
  return g_uart_ops;
}

void port_uart_init(void) {
  const port_uart_ops_t *ops = port_uart_get_ops();

  if ((ops != NULL) && (ops->init != NULL)) {
    (void)ops->init();
  }
}

int port_uart_read(uint8_t *buf, uint32_t len, uint32_t timeout_ms) {
  const port_uart_ops_t *ops = port_uart_get_ops();

  if ((ops == NULL) || (ops->read == NULL)) {
    return -1;
  }

  return ops->read(buf, len, timeout_ms);
}

int port_uart_write(const uint8_t *buf, uint32_t len) {
  const port_uart_ops_t *ops = port_uart_get_ops();

  if ((ops == NULL) || (ops->write == NULL)) {
    return -1;
  }

  return ops->write(buf, len);
}
