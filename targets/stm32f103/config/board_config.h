/*
 * File: board_config.h
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Board-level wiring and clock setup for the STM32F103 target.
 */

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "port_hal.h"

#define BOARD_NAME "STM32F103 BluePill"

#define BOARD_LED_PORT GPIOC
#define BOARD_LED_PIN GPIO_PIN_13

#define BOARD_UART_INSTANCE USART1
#define BOARD_UART_GPIO_PORT GPIOA
#define BOARD_UART_TX_PIN GPIO_PIN_9
#define BOARD_UART_RX_PIN GPIO_PIN_10
#define BOARD_UART_BAUDRATE 115200u
#define BOARD_UART_BAUDRATE_TEXT "115200"
#define BOARD_UART_LABEL "USART1"

static inline void board_system_clock_init(void) {
  RCC_OscInitTypeDef osc = {0};
  RCC_ClkInitTypeDef clk = {0};

  osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  osc.HSIState = RCC_HSI_ON;
  osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  osc.PLL.PLLState = RCC_PLL_NONE;
  (void)HAL_RCC_OscConfig(&osc);

  clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clk.APB1CLKDivider = RCC_HCLK_DIV1;
  clk.APB2CLKDivider = RCC_HCLK_DIV1;
  (void)HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_0);
}

static inline void board_led_init(void) {
  GPIO_InitTypeDef gpio = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();

  gpio.Pin = BOARD_LED_PIN;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BOARD_LED_PORT, &gpio);
}

static inline void board_uart_init_pins(void) {
  GPIO_InitTypeDef gpio = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_USART1_CLK_ENABLE();

  gpio.Pin = BOARD_UART_TX_PIN;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BOARD_UART_GPIO_PORT, &gpio);

  gpio.Pin = BOARD_UART_RX_PIN;
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOARD_UART_GPIO_PORT, &gpio);
}

#endif /* BOARD_CONFIG_H */
