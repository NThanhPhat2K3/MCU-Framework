/*
 * File: main.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Entry point for the Programmer image that receives and writes
 * App data.
 */

#include "boot_config.h"
#include "boot_image.h"
#include "boot_jump.h"
#include "boot_proto.h"
#include "boot_shared.h"
#include "boot_types.h"
#include "port_flash.h"
#include "port_system.h"
#include "port_uart.h"

#include <stdint.h>

static void programmer_send_status(const char *text) {
  (void)boot_proto_send_text(text);
}

static void programmer_send_labeled_text(const char *label, const char *value) {
  programmer_send_status(label);
  programmer_send_status(value);
  programmer_send_status("\r\n");
}

static void programmer_u32_to_dec(uint32_t value, char *buf) {
  char scratch[11];
  uint32_t i = 0u;
  uint32_t j;

  if (value == 0u) {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }

  while (value > 0u) {
    scratch[i] = (char)('0' + (value % 10u));
    value /= 10u;
    ++i;
  }

  for (j = 0u; j < i; ++j) {
    buf[j] = scratch[i - 1u - j];
  }
  buf[i] = '\0';
}

static void programmer_u32_to_hex(uint32_t value, char *buf) {
  static const char hex_digits[] = "0123456789ABCDEF";
  uint32_t shift;

  buf[0] = '0';
  buf[1] = 'x';

  for (shift = 0u; shift < 8u; ++shift) {
    uint32_t nibble_shift = (7u - shift) * 4u;
    buf[2u + shift] = hex_digits[(value >> nibble_shift) & 0xFu];
  }

  buf[10] = '\0';
}

static void programmer_send_u32_dec(const char *label, uint32_t value) {
  char buf[11];

  programmer_u32_to_dec(value, buf);
  programmer_send_labeled_text(label, buf);
}

static void programmer_send_u32_hex(const char *label, uint32_t value) {
  char buf[11];

  programmer_u32_to_hex(value, buf);
  programmer_send_labeled_text(label, buf);
}

static boot_status_t programmer_handle_erase(boot_shared_t *shared) {
  boot_status_t status = port_flash_erase_app();

  if (status == BOOT_STATUS_OK) {
    shared->error_code = (uint32_t)BOOT_STATUS_OK;
    programmer_send_status("OK ERASE\r\n");
  } else {
    shared->error_code = (uint32_t)status;
    programmer_send_status("ERR ERASE\r\n");
    programmer_send_labeled_text("reason: ", boot_status_name(status));
  }

  return status;
}

static boot_status_t programmer_handle_write(const boot_packet_t *pkt,
                                             boot_shared_t *shared) {
  uint32_t addr;
  uint32_t data_len;
  boot_status_t status;

  if (pkt->len < 4u) {
    shared->error_code = (uint32_t)BOOT_STATUS_INVALID_ARGUMENT;
    programmer_send_status("ERR WLEN\r\n");
    return BOOT_STATUS_INVALID_ARGUMENT;
  }

  addr = ((uint32_t)pkt->data[0]) | ((uint32_t)pkt->data[1] << 8) |
         ((uint32_t)pkt->data[2] << 16) | ((uint32_t)pkt->data[3] << 24);

  data_len = pkt->len - 4u;

  if ((addr < APP_ADDR) || ((addr + data_len) > FLASH_END_ADDR)) {
    shared->error_code = (uint32_t)BOOT_STATUS_INVALID_ARGUMENT;
    programmer_send_status("ERR ADDR\r\n");
    return BOOT_STATUS_INVALID_ARGUMENT;
  }

  if ((addr % FLASH_WRITE_ALIGNMENT) != 0u) {
    shared->error_code = (uint32_t)BOOT_STATUS_INVALID_ARGUMENT;
    programmer_send_status("ERR ALIGN\r\n");
    return BOOT_STATUS_INVALID_ARGUMENT;
  }

  status = port_flash_write(addr, &pkt->data[4], data_len);
  if (status == BOOT_STATUS_OK) {
    shared->error_code = (uint32_t)BOOT_STATUS_OK;
    programmer_send_status("OK WRITE\r\n");
    return BOOT_STATUS_OK;
  }

  shared->error_code = (uint32_t)status;
  programmer_send_status("ERR WRITE\r\n");
  programmer_send_labeled_text("reason: ", boot_status_name(status));
  programmer_send_u32_hex("flash_err: ", port_flash_get_last_error_detail());
  programmer_send_u32_hex("flash_addr: ", port_flash_get_last_error_addr());
  return status;
}

static boot_status_t programmer_handle_jump(boot_shared_t *shared) {
  boot_status_t status = boot_image_check(APP_ADDR);

  if (status == BOOT_STATUS_OK) {
    shared->update_status = UPDATE_OK;
    shared->boot_request = BOOT_REQ_NONE;
    shared->error_code = (uint32_t)BOOT_STATUS_OK;
    port_system_reset();
    /* Should never reach here. Halt if reset somehow fails. */
    while (1) {
    }
  }

  shared->error_code = (uint32_t)status;
  shared->update_status = UPDATE_FAIL;
  programmer_send_status("ERR NOAPP\r\n");
  programmer_send_labeled_text("reason: ", boot_status_name(status));
  return status;
}

static void programmer_handle_info(boot_shared_t *shared) {
  const boot_image_header_t *header = boot_image_get_header(APP_ADDR);
  boot_status_t status = boot_image_check(APP_ADDR);

  programmer_send_status("OK INFO\r\n");
  programmer_send_u32_hex("app_magic: ", header->magic);
  programmer_send_u32_dec("app_header_version: ", header->header_version);
  programmer_send_u32_dec("app_header_size: ", header->header_size);
  programmer_send_u32_dec("app_type: ", header->image_type);
  programmer_send_labeled_text("app_state: ",
                               boot_image_state_name(header->image_state));
  programmer_send_u32_dec("app_size: ", header->image_size);
  programmer_send_u32_hex("app_vector: ", header->vector_addr);
  programmer_send_u32_hex("app_crc32: ", header->crc32);
  programmer_send_u32_dec("app_version_major: ", header->version_major);
  programmer_send_u32_dec("app_version_minor: ", header->version_minor);
  programmer_send_u32_dec("app_version_patch: ", header->version_patch);
  programmer_send_labeled_text("app_check: ", boot_status_name(status));
  programmer_send_u32_dec("update_status: ", shared->update_status);
  programmer_send_u32_dec("last_error: ", shared->error_code);
  programmer_send_labeled_text(
      "last_error_name: ", boot_status_name((boot_status_t)shared->error_code));
}

static void programmer_process_packet(const boot_packet_t *pkt,
                                      boot_shared_t *shared) {
  switch (pkt->cmd) {
  case 'I':
    programmer_handle_info(shared);
    break;

  case 'E':
    (void)programmer_handle_erase(shared);
    break;

  case 'W':
    (void)programmer_handle_write(pkt, shared);
    break;

  case 'J':
    (void)programmer_handle_jump(shared);
    break;

  default:
    programmer_send_status("ERR CMD\r\n");
    break;
  }
}

int main(void) {
  boot_packet_t pkt;
  boot_shared_t *shared;

  port_system_init();
  port_uart_init();
  boot_shared_init();

  shared = boot_shared_get();
  shared->update_status = UPDATE_RUNNING;

  programmer_send_status("\r\n[ programmer entry ]\r\n");

  for (;;) {
    if (boot_proto_recv(&pkt) != BOOT_STATUS_OK) {
      programmer_send_status("ERR PKT\r\n");
      continue;
    }
    programmer_process_packet(&pkt, shared);
  }
}
