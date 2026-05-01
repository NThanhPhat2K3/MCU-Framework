/*
 * File: port_uart_stm32.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Implements the UART backend for STM32 targets. This file is the
 * only layer that knows about STM32 LL/CMSIS UART details.
 */

#include "port_uart_stm32.h"

#include "board_config.h"
#include "port_stm32.h"
#include "port_system.h"

static int port_uart_stm32_init(void) {
  board_uart_init_pins();

  LL_USART_Disable(BOARD_UART_INSTANCE);
  LL_USART_SetTransferDirection(BOARD_UART_INSTANCE, LL_USART_DIRECTION_TX_RX);
  LL_USART_ConfigCharacter(BOARD_UART_INSTANCE, LL_USART_DATAWIDTH_8B,
                           LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
  LL_USART_SetHWFlowCtrl(BOARD_UART_INSTANCE, LL_USART_HWCONTROL_NONE);
#if defined(USART_CR1_OVER8)
  LL_USART_SetOverSampling(BOARD_UART_INSTANCE, LL_USART_OVERSAMPLING_16);
#endif
  PORT_USART_SET_BAUD(BOARD_UART_INSTANCE, SystemCoreClock,
                      BOARD_UART_BAUDRATE);
  LL_USART_Enable(BOARD_UART_INSTANCE);
#if defined(USART_ISR_TEACK)
  while (LL_USART_IsActiveFlag_TEACK(BOARD_UART_INSTANCE) == 0U) {
  }
#endif
  board_uart_connect_tx_pin();

  return 0;
}

static int port_uart_stm32_read(uint8_t *buf, uint32_t len,
                                uint32_t timeout_ms) {
  uint32_t i;
  uint32_t start = port_system_get_tick();

  for (i = 0; i < len; ++i) {
    while (LL_USART_IsActiveFlag_RXNE(BOARD_UART_INSTANCE) == 0U) {
      if (LL_USART_IsActiveFlag_ORE(BOARD_UART_INSTANCE) != 0U) {
        LL_USART_ClearFlag_ORE(BOARD_UART_INSTANCE);
        return -1;
      }

      if ((timeout_ms != PORT_UART_TIMEOUT_FOREVER) &&
          ((port_system_get_tick() - start) >= timeout_ms)) {
        return -1;
      }
    }

    buf[i] = LL_USART_ReceiveData8(BOARD_UART_INSTANCE);
  }

  return 0;
}

static int port_uart_stm32_write(const uint8_t *buf, uint32_t len) {
  uint32_t i;

  for (i = 0; i < len; ++i) {
    while (LL_USART_IsActiveFlag_TXE(BOARD_UART_INSTANCE) == 0U) {
    }

    LL_USART_TransmitData8(BOARD_UART_INSTANCE, buf[i]);
  }

  while (LL_USART_IsActiveFlag_TC(BOARD_UART_INSTANCE) == 0U) {
  }

  return 0;
}

const port_uart_ops_t *port_uart_stm32_get_ops(void) {
  static const port_uart_ops_t ops = {
      .init = port_uart_stm32_init,
      .read = port_uart_stm32_read,
      .write = port_uart_stm32_write,
  };

  return &ops;
}
