/*
 * File: port_flash.h
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Declares MCU-specific flash erase and write services for App programming.
 */

#ifndef PORTABLE_BOOT_EXAMPLE_PORT_FLASH_H
#define PORTABLE_BOOT_EXAMPLE_PORT_FLASH_H

#include "boot_types.h"

#include <stdint.h>

boot_status_t port_flash_erase_app(void);
boot_status_t port_flash_write(uint32_t addr, const uint8_t *data, uint32_t len);

#endif
