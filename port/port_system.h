#ifndef PORTABLE_BOOT_EXAMPLE_PORT_SYSTEM_H
#define PORTABLE_BOOT_EXAMPLE_PORT_SYSTEM_H

#include <stdint.h>

void port_system_init(void);
void port_system_reset(void);
int port_system_is_flash_addr(uint32_t addr);
int port_system_is_ram_addr(uint32_t addr);
void port_system_prepare_jump(uint32_t vector_addr, uint32_t stack_ptr);

#endif
