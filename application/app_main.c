/*
 * File: app_main.c
 * Author: Phat Nguyen
 * Date: 2026-04-29
 * Description: Provides a simple STM32 application used to validate the boot
 * flow.
 */

#include "app_update.h"

#include "boot_config.h"
#include "board_config.h"
#include "port_hal.h"

static UART_HandleTypeDef huart1;

static void app_clock_init(void) {
  board_system_clock_init();
}

static void app_gpio_init(void) {
  board_led_init();
}

static void app_uart_init(void) {
  board_uart_init_pins();

  huart1.Instance = BOARD_UART_INSTANCE;
  huart1.Init.BaudRate = BOARD_UART_BAUDRATE;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  (void)HAL_UART_Init(&huart1);
}

static void app_send_text(const char *text) {
  const char *ptr = text;
  uint16_t len = 0;

  while (*ptr != '\0') {
    ++ptr;
    ++len;
  }

  (void)HAL_UART_Transmit(&huart1, (uint8_t *)text, len, HAL_MAX_DELAY);
}

int main(void) {
  uint8_t rx_byte;
  uint32_t last_tick = 0u;

  SCB->VTOR = APP_ADDR;

  HAL_Init();
  app_clock_init();
  app_gpio_init();
  app_uart_init();

  app_send_text("\r\n=== APP RUNNING ===\r\n");
  app_send_text("Board: " BOARD_NAME "\r\n");
  app_send_text("UART: " BOARD_UART_LABEL " @ " BOARD_UART_BAUDRATE_TEXT
                " 8N1\r\n");
  app_send_text("Command: send 'u' to enter programmer\r\n\r\n");

  for (;;) {
    if ((HAL_GetTick() - last_tick) >= 100u) {
      HAL_GPIO_TogglePin(BOARD_LED_PORT, BOARD_LED_PIN);
      last_tick = HAL_GetTick();
    }

    if (HAL_UART_Receive(&huart1, &rx_byte, 1u, 10u) == HAL_OK) {
      if ((rx_byte == 'u') || (rx_byte == 'U')) {
        app_send_text("GO PROGRAMMER\r\n");
        HAL_Delay(50u);
        app_request_programmer_and_reset();
      }
    }
  }
}
