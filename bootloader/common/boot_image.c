/*
 * File: boot_image.c
 * Author: Phat Nguyen
 * Date: 2026-05-02
 * Description: Implements image header parsing, CRC32 validation, and
 * boot-state helpers.
 */

#include "boot_image.h"

#include "boot_config.h"
#include "port_system.h"

static uint32_t boot_image_crc32_update(uint32_t crc, const uint8_t *data,
                                        uint32_t len) {
  uint32_t i;
  uint32_t j;

  for (i = 0u; i < len; ++i) {
    crc ^= (uint32_t)data[i];
    for (j = 0u; j < 8u; ++j) {
      if ((crc & 1u) != 0u) {
        crc = (crc >> 1u) ^ 0xEDB88320u;
      } else {
        crc >>= 1u;
      }
    }
  }

  return crc;
}

static uint32_t boot_image_crc32_calc(uint32_t image_addr,
                                      const boot_image_header_t *header) {
  uint32_t crc = 0xFFFFFFFFu;
  uint32_t header_addr = image_addr + BOOT_IMAGE_HEADER_OFFSET;
  uint32_t prefix_len = BOOT_IMAGE_HEADER_OFFSET;
  uint32_t suffix_addr = header_addr + header->header_size;
  uint32_t suffix_len = header->image_size - prefix_len - header->header_size;

  crc = boot_image_crc32_update(crc, (const uint8_t *)image_addr, prefix_len);
  crc = boot_image_crc32_update(crc, (const uint8_t *)suffix_addr, suffix_len);

  return crc ^ 0xFFFFFFFFu;
}

static int boot_image_is_state_bootable(uint32_t state) {
  return (state == BOOT_IMAGE_STATE_PENDING) ||
         (state == BOOT_IMAGE_STATE_VALID);
}

static int boot_image_is_range_in_flash(uint32_t start, uint32_t size) {
  uint32_t end;

  if (size == 0u) {
    return 0;
  }

  end = start + size;
  if (end < start) {
    return 0;
  }

  return (start >= FLASH_BASE_ADDR) && (end <= FLASH_END_ADDR);
}

const char *boot_image_state_name(uint32_t state) {
  switch (state) {
  case BOOT_IMAGE_STATE_EMPTY:
    return "EMPTY";
  case BOOT_IMAGE_STATE_PENDING:
    return "PENDING";
  case BOOT_IMAGE_STATE_VALID:
    return "VALID";
  case BOOT_IMAGE_STATE_INVALID:
    return "INVALID";
  default:
    return "UNKNOWN";
  }
}

const char *boot_status_name(boot_status_t status) {
  switch (status) {
  case BOOT_STATUS_OK:
    return "OK";
  case BOOT_STATUS_ERROR:
    return "ERROR";
  case BOOT_STATUS_TIMEOUT:
    return "TIMEOUT";
  case BOOT_STATUS_INVALID_ARGUMENT:
    return "INVALID_ARGUMENT";
  case BOOT_STATUS_INVALID_IMAGE:
    return "INVALID_IMAGE";
  case BOOT_STATUS_IO_ERROR:
    return "IO_ERROR";
  case BOOT_STATUS_CRC_MISMATCH:
    return "CRC_MISMATCH";
  case BOOT_STATUS_INVALID_STATE:
    return "INVALID_STATE";
  case BOOT_STATUS_INVALID_MAGIC:
    return "INVALID_MAGIC";
  case BOOT_STATUS_INVALID_HEADER_VERSION:
    return "INVALID_HEADER_VERSION";
  case BOOT_STATUS_INVALID_HEADER_SIZE:
    return "INVALID_HEADER_SIZE";
  case BOOT_STATUS_INVALID_IMAGE_TYPE:
    return "INVALID_IMAGE_TYPE";
  case BOOT_STATUS_INVALID_VECTOR_ADDR:
    return "INVALID_VECTOR_ADDR";
  case BOOT_STATUS_INVALID_IMAGE_SIZE:
    return "INVALID_IMAGE_SIZE";
  case BOOT_STATUS_INVALID_STACK_PTR:
    return "INVALID_STACK_PTR";
  case BOOT_STATUS_INVALID_RESET_VECTOR:
    return "INVALID_RESET_VECTOR";
  default:
    return "UNKNOWN";
  }
}

const boot_image_header_t *boot_image_get_header(uint32_t image_addr) {
  return (const boot_image_header_t *)(image_addr + BOOT_IMAGE_HEADER_OFFSET);
}

boot_status_t boot_image_check_typed(uint32_t image_addr,
                                     uint32_t expected_type) {
  const boot_image_header_t *header = boot_image_get_header(image_addr);
  uint32_t stack_ptr;
  uint32_t reset_vector;

  if (header->magic != BOOT_IMAGE_MAGIC) {
    return BOOT_STATUS_INVALID_MAGIC;
  }

  if (header->header_version != BOOT_IMAGE_HEADER_VERSION) {
    return BOOT_STATUS_INVALID_HEADER_VERSION;
  }

  if (header->header_size != sizeof(boot_image_header_t)) {
    return BOOT_STATUS_INVALID_HEADER_SIZE;
  }

  if (header->image_type != expected_type) {
    return BOOT_STATUS_INVALID_IMAGE_TYPE;
  }

  if (!boot_image_is_state_bootable(header->image_state)) {
    return BOOT_STATUS_INVALID_STATE;
  }

  if (header->vector_addr != image_addr) {
    return BOOT_STATUS_INVALID_VECTOR_ADDR;
  }

  if (header->image_size <= (BOOT_IMAGE_HEADER_OFFSET + header->header_size)) {
    return BOOT_STATUS_INVALID_IMAGE_SIZE;
  }

  if (!boot_image_is_range_in_flash(image_addr, header->image_size)) {
    return BOOT_STATUS_INVALID_IMAGE_SIZE;
  }

  stack_ptr = *(volatile uint32_t *)header->vector_addr;
  reset_vector = *(volatile uint32_t *)(header->vector_addr + 4u);

  if (!port_system_is_ram_addr(stack_ptr)) {
    return BOOT_STATUS_INVALID_STACK_PTR;
  }

  if (!port_system_is_flash_addr(reset_vector)) {
    return BOOT_STATUS_INVALID_RESET_VECTOR;
  }

  if (boot_image_crc32_calc(image_addr, header) != header->crc32) {
    return BOOT_STATUS_CRC_MISMATCH;
  }

  return BOOT_STATUS_OK;
}

boot_status_t boot_image_mark_valid_if_pending(uint32_t image_addr) {
  const boot_image_header_t *header = boot_image_get_header(image_addr);

  if (header->image_state == BOOT_IMAGE_STATE_VALID) {
    return BOOT_STATUS_OK;
  }

  if (header->image_state != BOOT_IMAGE_STATE_PENDING) {
    return BOOT_STATUS_INVALID_STATE;
  }

  /* STM32 internal flash programming is not a good fit for in-place
   * metadata promotion here. Re-programming a previously written flash
   * word can fail even when the logical state transition only clears
   * extra bits. For now we keep PENDING as a bootable state and let the
   * CRC/header validation be the real gate.
   */
  (void)image_addr;
  return BOOT_STATUS_OK;
}
