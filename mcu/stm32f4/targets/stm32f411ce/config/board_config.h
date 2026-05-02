/*
 * File: board_config.h
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Board-level wiring and clock setup for the STM32F411CE target.
 */

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "port_stm32.h"

#define BOARD_NAME "STM32F411CE BlackPill"

#define BOARD_LED_PORT GPIOC
#define BOARD_LED_PIN LL_GPIO_PIN_13

#define BOARD_UART_INSTANCE USART1
#define BOARD_UART_GPIO_PORT GPIOA
#define BOARD_UART_TX_PIN LL_GPIO_PIN_9
#define BOARD_UART_RX_PIN LL_GPIO_PIN_10
#define BOARD_UART_BAUDRATE 115200u
#define BOARD_UART_BAUDRATE_TEXT "115200"
#define BOARD_UART_LABEL "USART1"
#define BOARD_SYSCLK_HZ 25000000u
#define BOARD_SYSCLK_HZ_TEXT "25000000"
#define BOARD_FLASH_LATENCY LL_FLASH_LATENCY_0

static inline void board_system_clock_init(void) {
  /*
   * This target is configured to run directly from HSE without a PLL step.
   * Flash latency here controls instruction/data reads from internal flash.
   * It is not related to firmware erase/program operations.
   */
  LL_FLASH_SetLatency(BOARD_FLASH_LATENCY);
  while (LL_FLASH_GetLatency() != BOARD_FLASH_LATENCY) {
  }

  /*
   * Prefetch lets the flash interface fetch upcoming instructions early,
   * which helps sequential code execution run more smoothly.
   */
  LL_FLASH_EnablePrefetch();

  LL_RCC_HSE_Enable();
  while (LL_RCC_HSE_IsReady() != 1U) {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);

  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE) {
  }

  SystemCoreClock = BOARD_SYSCLK_HZ;
}

static inline void board_led_init(void) {
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_GPIO_SetPinMode(BOARD_LED_PORT, BOARD_LED_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(BOARD_LED_PORT, BOARD_LED_PIN,
                           LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(BOARD_LED_PORT, BOARD_LED_PIN, LL_GPIO_PULL_NO);
  LL_GPIO_SetPinSpeed(BOARD_LED_PORT, BOARD_LED_PIN, LL_GPIO_SPEED_FREQ_LOW);
}

static inline void board_led_toggle(void) {
  LL_GPIO_TogglePin(BOARD_LED_PORT, BOARD_LED_PIN);
}

static inline void board_uart_init_pins(void) {
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

  /*
   * Keep TX at the UART idle-high level while USART is still being configured.
   * On the tested board this avoids a stray start bit and a garbage byte at
   * boot.
   */
  LL_GPIO_SetOutputPin(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN);
  LL_GPIO_SetPinMode(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN,
                     LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN,
                           LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN, LL_GPIO_PULL_UP);
  LL_GPIO_SetPinSpeed(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN,
                      LL_GPIO_SPEED_FREQ_VERY_HIGH);

  LL_GPIO_SetPinMode(BOARD_UART_GPIO_PORT, BOARD_UART_RX_PIN,
                     LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinOutputType(BOARD_UART_GPIO_PORT, BOARD_UART_RX_PIN,
                           LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(BOARD_UART_GPIO_PORT, BOARD_UART_RX_PIN, LL_GPIO_PULL_UP);
  LL_GPIO_SetPinSpeed(BOARD_UART_GPIO_PORT, BOARD_UART_RX_PIN,
                      LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetAFPin_8_15(BOARD_UART_GPIO_PORT, BOARD_UART_RX_PIN, LL_GPIO_AF_7);
}

static inline void board_uart_connect_tx_pin(void) {
  LL_GPIO_SetPinMode(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN,
                     LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_8_15(BOARD_UART_GPIO_PORT, BOARD_UART_TX_PIN, LL_GPIO_AF_7);
}

#endif /* BOARD_CONFIG_H */
