/*
 * File: port_uart_stm32.h
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Declares the STM32 UART backend used by the generic port layer.
 */

#ifndef MCU_STM32_PORT_UART_STM32_H
#define MCU_STM32_PORT_UART_STM32_H

#include "port_uart.h"

const port_uart_ops_t *port_uart_stm32_get_ops(void);

#endif /* MCU_STM32_PORT_UART_STM32_H */
