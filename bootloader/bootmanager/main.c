/*
 * File: main.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Entry point for the BootManager image that selects App or
 * Programmer.
 */

#include "boot_config.h"
#include "boot_image.h"
#include "boot_jump.h"
#include "boot_shared.h"
#include "port_system.h"
#include "port_uart.h"

static void bootmanager_send_text(const char *text) {
  const char *ptr = text;
  uint16_t len = 0u;

  while (*ptr != '\0') {
    ++ptr;
    ++len;
  }

  (void)port_uart_write((const uint8_t *)text, len);
}

static void bootmanager_send_labeled_text(const char *label, const char *value) {
  bootmanager_send_text(label);
  bootmanager_send_text(value);
  bootmanager_send_text("\r\n");
}

static void bootmanager_print_banner(void) {
  bootmanager_send_text("\r\n[ boot manager entry ]\r\n");
}

static boot_status_t bootmanager_try_programmer_request(boot_shared_t *shared) {
  boot_status_t status;

  if (shared->boot_request != BOOT_REQ_PROGRAMMER) {
    return BOOT_STATUS_INVALID_ARGUMENT;
  }

  status = boot_image_check(PROGRAMMER_ADDR);
  if (status == BOOT_STATUS_OK) {
    boot_shared_clear();
    boot_jump_to_image(PROGRAMMER_ADDR);
  }

  shared->error_code = (uint32_t)status;
  bootmanager_send_labeled_text("programmer request failed: ",
                                boot_status_name(status));
  return status;
}

static boot_status_t bootmanager_try_application(boot_shared_t *shared) {
  boot_status_t status = boot_image_check(APP_ADDR);

  if (status == BOOT_STATUS_OK) {
    status = boot_image_mark_valid_if_pending(APP_ADDR);
    if (status != BOOT_STATUS_OK) {
      shared->error_code = (uint32_t)status;
      bootmanager_send_labeled_text("app promote failed: ",
                                    boot_status_name(status));
      return status;
    }

    shared->error_code = (uint32_t)BOOT_STATUS_OK;
    boot_jump_to_image(APP_ADDR);
  }

  shared->error_code = (uint32_t)status;
  bootmanager_send_labeled_text("app check failed: ", boot_status_name(status));
  return status;
}

static boot_status_t bootmanager_try_programmer_fallback(boot_shared_t *shared) {
  boot_status_t status = boot_image_check(PROGRAMMER_ADDR);

  if (status == BOOT_STATUS_OK) {
    boot_jump_to_image(PROGRAMMER_ADDR);
  }

  shared->error_code = (uint32_t)status;
  bootmanager_send_labeled_text("programmer fallback failed: ",
                                boot_status_name(status));
  return status;
}

int main(void) {
  boot_shared_t *shared;

  port_system_init();
  boot_shared_init();
  port_uart_init();

  bootmanager_print_banner();

  shared = boot_shared_get();

  if (shared->boot_request == BOOT_REQ_PROGRAMMER) {
    (void)bootmanager_try_programmer_request(shared);
  }

  (void)bootmanager_try_application(shared);
  (void)bootmanager_try_programmer_fallback(shared);

  while (1) {
  }
}
