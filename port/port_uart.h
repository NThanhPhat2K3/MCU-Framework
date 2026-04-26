#ifndef PORTABLE_BOOT_EXAMPLE_PORT_UART_H
#define PORTABLE_BOOT_EXAMPLE_PORT_UART_H

#include <stdint.h>

#define PORT_UART_TIMEOUT_FOREVER 0xFFFFFFFFu

void port_uart_init(void);
int port_uart_read(uint8_t *buf, uint32_t len, uint32_t timeout_ms);
int port_uart_write(const uint8_t *buf, uint32_t len);

#endif
