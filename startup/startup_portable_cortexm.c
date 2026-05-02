/*
 * File: startup_portable_cortexm.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Implements portable Cortex-M reset flow, data copy, and BSS initialization.
 */

#include "startup_portable_cortexm.h"

#include "boot_config.h"

#if defined(STM32F103xB) || defined(STM32F411xE)
#include "port_stm32.h"
#endif

__attribute__((weak)) void startup_low_level_init(void)
{
    /*
     * Generic startup keeps image-specific vector-table selection here so
     * application code can stay unaware of VTOR details.
     *
     * For the normal boot flow, BootManager already programs VTOR before
     * jumping into Programmer or App. Reprogramming it here is harmless.
     *
     * For standalone bring-up, each image also becomes self-contained because
     * its own Reset_Handler points VTOR back at that image's vector table.
     */
#if defined(STM32F103xB) || defined(STM32F411xE)
#if defined(IMAGE_BOOTMANAGER)
    SCB->VTOR = BOOT_MANAGER_ADDR;
#elif defined(IMAGE_PROGRAMMER)
    SCB->VTOR = PROGRAMMER_ADDR;
#elif defined(IMAGE_APP)
    SCB->VTOR = APP_ADDR;
#endif
#endif
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
