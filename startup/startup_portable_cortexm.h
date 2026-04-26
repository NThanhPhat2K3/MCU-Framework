#ifndef PORTABLE_BOOT_EXAMPLE_STARTUP_PORTABLE_CORTEXM_H
#define PORTABLE_BOOT_EXAMPLE_STARTUP_PORTABLE_CORTEXM_H

#include <stdint.h>

/*
 * Portable Cortex-M startup notes:
 *
 * 1. This file only provides the common reset flow.
 * 2. Each MCU family should provide its own vector table file because
 *    external interrupt lists differ across chips.
 * 3. Linker scripts must export:
 *      _estack, _sidata, _sdata, _edata, _sbss, _ebss
 */

extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void SystemInit(void);
void __libc_init_array(void);
int main(void);

void startup_copy_data_init(void);
void startup_zero_bss(void);
void startup_run(void);
void Default_Handler(void);
void Reset_Handler(void);

#endif
