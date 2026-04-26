#ifndef PORTABLE_BOOT_EXAMPLE_BOOT_SHARED_H
#define PORTABLE_BOOT_EXAMPLE_BOOT_SHARED_H

#include "boot_types.h"

void boot_shared_init(void);
boot_shared_t *boot_shared_get(void);
void boot_shared_clear(void);

#endif
