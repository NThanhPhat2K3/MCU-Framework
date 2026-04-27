/*
 * File: startup_portable_cortexm.h
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Declares portable Cortex-M startup symbols and reset flow entry points.
 */

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
 * 4. startup_low_level_init() runs before .data copy and .bss clear.
 *    It must not rely on initialized global variables or zeroed BSS.
 * 5. SystemInit() is called after .data/.bss initialization by the generic
 *    reset flow. Targets that need earlier chip-specific work should override
 *    startup_low_level_init() and keep that hook RAM-init safe.
 * 6. Generic startup does not enforce a global IRQ disable policy.
 *    Targets that need strict interrupt control must handle it explicitly.
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

void startup_low_level_init(void);
void startup_copy_data_init(void);
void startup_zero_bss(void);
void startup_run(void);
void Default_Handler(void);
void Reset_Handler(void);

#endif
