/*
 * File: boot_jump.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Implements minimal image validation and jump-to-image runtime
 * handover.
 */

#include "boot_jump.h"

#include "boot_config.h"
#include "boot_image.h"
#include "port_system.h"

typedef void (*boot_entry_fn_t)(void);

static uint32_t boot_image_expected_type(uint32_t image_addr) {
  uint32_t ret = 0u;
  if (image_addr == BOOT_MANAGER_ADDR) {
    ret = BOOT_IMAGE_TYPE_BOOT_MANAGER;
  } else if (image_addr == PROGRAMMER_ADDR) {
    ret = BOOT_IMAGE_TYPE_PROGRAMMER;
  } else if (image_addr == APP_ADDR) {
    ret = BOOT_IMAGE_TYPE_APP;
  }

  return ret;
}

boot_status_t boot_image_check(uint32_t image_addr) {
  uint32_t expected_type = boot_image_expected_type(image_addr);

  if (expected_type == 0u) {
    return BOOT_STATUS_INVALID_ARGUMENT;
  }

  return boot_image_check_typed(image_addr, expected_type);
}

void boot_jump_to_image(uint32_t image_addr) {
  uint32_t vector_addr = image_addr;
  uint32_t stack_ptr = *(volatile uint32_t *)vector_addr;
  uint32_t reset_vector = *(volatile uint32_t *)(vector_addr + 4u);
  boot_entry_fn_t entry = (boot_entry_fn_t)reset_vector;

  port_system_prepare_jump(vector_addr, stack_ptr);
  entry();
}
