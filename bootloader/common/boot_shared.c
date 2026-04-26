/*
 * File: boot_shared.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Implements shared RAM state initialization, access, and reset helpers.
 */

#include "boot_shared.h"

#include "boot_config.h"

__attribute__((section(".noinit"))) static boot_shared_t g_boot_shared;

void boot_shared_init(void)
{
    unsigned i;

    if (g_boot_shared.magic == SHARED_MAGIC)
    {
        return;
    }

    g_boot_shared.magic = SHARED_MAGIC;
    g_boot_shared.boot_request = BOOT_REQ_NONE;
    g_boot_shared.update_status = UPDATE_IDLE;
    g_boot_shared.error_code = 0u;

    for (i = 0; i < 4; ++i)
    {
        g_boot_shared.reserved[i] = 0u;
    }
}

boot_shared_t *boot_shared_get(void)
{
    return &g_boot_shared;
}

void boot_shared_clear(void)
{
    g_boot_shared.magic = SHARED_MAGIC;
    g_boot_shared.boot_request = BOOT_REQ_NONE;
    g_boot_shared.update_status = UPDATE_IDLE;
    g_boot_shared.error_code = 0u;
}
