/*
 * File: app_main.c
 * Author: Phat Nguyen
 * Date: 2026-04-29
 * Description: Provides a simple STM32 application used to validate the boot
 * flow.
 */

#include "app_update.h"

#include "board_config.h"
#include "boot_config.h"
#include "port_system.h"
#include "port_uart.h"

static void app_send_text(const char *text) {
  const char *ptr = text;
  uint16_t len = 0;

  while (*ptr != '\0') {
    ++ptr;
    ++len;
  }

  (void)port_uart_write((const uint8_t *)text, len);
}

static void app_print_banner(void) {
  app_send_text("\r\n[ app entry ]\r\n");
  app_send_text("board: " BOARD_NAME "\r\n");
  app_send_text("uart: " BOARD_UART_LABEL " @ " BOARD_UART_BAUDRATE_TEXT
                " 8N1\r\n");
  app_send_text("clock: " BOARD_SYSCLK_HZ_TEXT " Hz\r\n");
  app_send_text("Command: send 'u' for programmer\r\n\r\n");
}

int main(void) {
  uint8_t rx_byte;
  uint32_t last_tick = 0u;

  port_system_init();
  board_led_init();
  port_uart_init();

  app_print_banner();

  for (;;) {
    if ((port_system_get_tick() - last_tick) >= 1000u) {
      board_led_toggle();
      last_tick = port_system_get_tick();
    }

    if (port_uart_read(&rx_byte, 1u, 10u) == 0) {
      if ((rx_byte == 'u') || (rx_byte == 'U')) {
        app_send_text("GO PROGRAMMER\r\n");
        port_system_delay_ms(50u);
        app_request_programmer_and_reset();
      }
    }
  }
}
