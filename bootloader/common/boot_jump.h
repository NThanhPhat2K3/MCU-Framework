#ifndef PORTABLE_BOOT_EXAMPLE_BOOT_JUMP_H
#define PORTABLE_BOOT_EXAMPLE_BOOT_JUMP_H

#include "boot_types.h"

#include <stdint.h>

boot_status_t boot_image_check(uint32_t image_addr);
void boot_jump_to_image(uint32_t image_addr);

#endif
