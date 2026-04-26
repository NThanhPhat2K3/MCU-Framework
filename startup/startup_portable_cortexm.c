#include "startup_portable_cortexm.h"

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

void startup_run(void)
{
    startup_copy_data_init();
    startup_zero_bss();
    SystemInit();
    __libc_init_array();
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
