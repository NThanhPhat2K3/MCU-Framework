/*
 * File: port_uart_stm32.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Implements the UART backend for STM32 targets. This file is the
 * only layer that knows about STM32 HAL UART types and init fields.
 */

#include "port_uart_stm32.h"

#include "board_config.h"
#include "port_hal.h"

static UART_HandleTypeDef g_uart_handle;

static int port_uart_stm32_init(void) {
  board_uart_init_pins();

  g_uart_handle.Instance = BOARD_UART_INSTANCE;
  g_uart_handle.Init.BaudRate = BOARD_UART_BAUDRATE;
  g_uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
  g_uart_handle.Init.StopBits = UART_STOPBITS_1;
  g_uart_handle.Init.Parity = UART_PARITY_NONE;
  g_uart_handle.Init.Mode = UART_MODE_TX_RX;
  g_uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  g_uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;

  return (HAL_UART_Init(&g_uart_handle) == HAL_OK) ? 0 : -1;
}

static int port_uart_stm32_read(uint8_t *buf, uint32_t len,
                                uint32_t timeout_ms) {
  return (HAL_UART_Receive(&g_uart_handle, buf, (uint16_t)len, timeout_ms) ==
          HAL_OK)
             ? 0
             : -1;
}

static int port_uart_stm32_write(const uint8_t *buf, uint32_t len) {
  return (HAL_UART_Transmit(&g_uart_handle, (uint8_t *)buf, (uint16_t)len,
                            HAL_MAX_DELAY) == HAL_OK)
             ? 0
             : -1;
}

const port_uart_ops_t *port_uart_stm32_get_ops(void) {
  static const port_uart_ops_t ops = {
      .init = port_uart_stm32_init,
      .read = port_uart_stm32_read,
      .write = port_uart_stm32_write,
  };

  return &ops;
}
