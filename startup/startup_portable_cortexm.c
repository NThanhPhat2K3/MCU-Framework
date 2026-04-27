/*
 * File: startup_portable_cortexm.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Implements portable Cortex-M reset flow, data copy, and BSS initialization.
 */

#include "startup_portable_cortexm.h"

__attribute__((weak)) void startup_low_level_init(void)
{
    /*
     * Default early hook intentionally does nothing.
     * Generic startup cannot assume that SystemInit() is safe before .data/.bss
     * have been initialized on every target.
     */
}

void startup_copy_data_init(void)
{
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;

    while (dst < &_edata)
    {
        *dst++ = *src++;
    }
}

void startup_zero_bss(void)
{
    uint32_t *dst = &_sbss;

    while (dst < &_ebss)
    {
        *dst++ = 0u;
    }
}

static void startup_run_constructors(void)
{
    __libc_init_array();
}

void startup_run(void)
{
    /*
     * Follow the classic split described by Miro Samek:
     * keep the reset flow generic, but let each MCU family override
     * the early low-level initialization before .data/.bss handling.
     *
     * Interrupt policy:
     * - generic startup does not force IRQs disabled across the handoff to main()
     * - if a target needs stricter interrupt control during bring-up, it should
     *   do so inside its low-level or platform initialization code
     */
    startup_low_level_init();
    startup_copy_data_init();
    startup_zero_bss();
    SystemInit();
    startup_run_constructors();
    (void)main();

    while (1)
    {
    }
}

void Reset_Handler(void)
{
    startup_run();
}

void Default_Handler(void)
{
    while (1)
    {
    }
}
