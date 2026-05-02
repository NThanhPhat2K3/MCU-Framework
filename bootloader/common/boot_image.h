/*
 * File: boot_image.h
 * Author: Phat Nguyen
 * Date: 2026-05-02
 * Description: Declares the on-flash image header format and validation
 * helpers.
 */

#ifndef PORTABLE_BOOT_EXAMPLE_BOOT_IMAGE_H
#define PORTABLE_BOOT_EXAMPLE_BOOT_IMAGE_H

#include "boot_types.h"

#include <stdint.h>

#define BOOT_IMAGE_MAGIC 0x494D4731u
#define BOOT_IMAGE_HEADER_VERSION 1u
#define BOOT_IMAGE_HEADER_OFFSET 0x200u
#define BOOT_IMAGE_STATE_EMPTY 0xFFFFFFFFu
#define BOOT_IMAGE_STATE_PENDING 0xFFFFFFFEu
#define BOOT_IMAGE_STATE_VALID 0xFFFFFFFCu
#define BOOT_IMAGE_STATE_INVALID 0xFFFFFFF8u

typedef struct {
  uint32_t magic;
  uint16_t header_version;
  uint16_t header_size;
  uint32_t image_type;
  uint32_t image_state;
  uint32_t image_size;
  uint32_t vector_addr;
  uint32_t crc32;
  uint32_t version_major;
  uint32_t version_minor;
  uint32_t version_patch;
  uint32_t reserved[6];
} boot_image_header_t;

const boot_image_header_t *boot_image_get_header(uint32_t image_addr);
boot_status_t boot_image_check_typed(uint32_t image_addr,
                                     uint32_t expected_type);
boot_status_t boot_image_mark_valid_if_pending(uint32_t image_addr);
const char *boot_image_state_name(uint32_t state);
const char *boot_status_name(boot_status_t status);

#endif
