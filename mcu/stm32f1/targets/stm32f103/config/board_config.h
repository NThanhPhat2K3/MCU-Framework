/*
 * File: board_config.h
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Board-level wiring and clock setup for the STM32F103 target.
 */

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "port_stm32.h"

#define BOARD_NAME "STM32F103 BluePill"

#define BOARD_LED_PORT GPIOC
#define BOARD_LED_PIN LL_GPIO_PIN_13

#define BOARD_UART_INSTANCE USART1
#define BOARD_UART_GPIO_PORT GPIOA
#define BOARD_UART_TX_PIN LL_GPIO_PIN_9
#define BOARD_UART_RX_PIN LL_GPIO_PIN_10
#define BOARD_UART_BAUDRATE 115200u
#define BOARD_UART_BAUDRATE_TEXT "115200"
#define BOARD_UART_LABEL "USART1"
#define BOARD_SYSCLK_HZ 8000000u

static inline void board_system_clock_init(void) {
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  LL_FLASH_EnablePrefetch();

  LL_RCC_HSI_Enable();
  while (LL_RCC_HSI_IsReady() != 1U) {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {
  }

  SystemCoreClock = BOARD_SYSCLK_HZ;
}

static inline void board_led_init(void) {
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
  LL_GPIO_SetPinMode(BOARD_LED_PORT, BOARD_LED_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(BOARD_LED_PORT, BOARD_LED_PIN,
                           LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinSpeed(BOARD_LED_PORT, BOARD_LED_PIN, LL_GPIO_SPEED_FREQ_LOW);
}

static inline void board_led_toggle(void) {
  LL_GPIO_TogglePin(BOARD_LED_PORT, BOARD_LED_PIN);
}

static inline void board_uart_init_pins(void) {
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA |
                           LL_APB2_GRP1_PERIPH_USART1);

  /*
   * Keep TX high while USART is still being configured to avoid a spurious
   * start bit during bring-up.
   */
  LL_GPIO_SetPinMode(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN,
                     LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN,
                           LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinSpeed(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN,
                      LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetOutputPin(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN);

  LL_GPIO_SetPinMode(BOARD_UART_GPIO_PORT, BOARD_UART_RX_PIN,
                     LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(BOARD_UART_GPIO_PORT, BOARD_UART_RX_PIN, LL_GPIO_PULL_UP);
}

static inline void board_uart_connect_tx_pin(void) {
  LL_GPIO_SetPinMode(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN,
                     LL_GPIO_MODE_ALTERNATE);
}

#endif /* BOARD_CONFIG_H */
