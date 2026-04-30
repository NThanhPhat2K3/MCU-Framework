/*
 * File: port_uart.h
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Declares the MCU-specific UART abstraction used by the programmer transport.
 */

#ifndef PORTABLE_BOOT_EXAMPLE_PORT_UART_H
#define PORTABLE_BOOT_EXAMPLE_PORT_UART_H

#include <stdint.h>

#define PORT_UART_TIMEOUT_FOREVER 0xFFFFFFFFu

typedef struct {
  int (*init)(void);
  int (*read)(uint8_t *buf, uint32_t len, uint32_t timeout_ms);
  int (*write)(const uint8_t *buf, uint32_t len);
} port_uart_ops_t;

void port_uart_init(void);
int port_uart_read(uint8_t *buf, uint32_t len, uint32_t timeout_ms);
int port_uart_write(const uint8_t *buf, uint32_t len);

#endif
