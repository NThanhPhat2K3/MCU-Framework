/*
 * File: main.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Entry point for the BootManager image that selects App or Programmer.
 */

#include "boot_config.h"
#include "boot_jump.h"
#include "boot_shared.h"
#include "port_system.h"

static boot_status_t bootmanager_try_programmer_request(boot_shared_t *shared)
{
    if (shared->boot_request != BOOT_REQ_PROGRAMMER)
    {
        return BOOT_STATUS_INVALID_ARGUMENT;
    }

    boot_shared_clear();

    if (boot_image_check(PROGRAMMER_ADDR) == BOOT_STATUS_OK)
    {
        boot_jump_to_image(PROGRAMMER_ADDR);
    }

    return BOOT_STATUS_INVALID_IMAGE;
}

static boot_status_t bootmanager_try_application(void)
{
    if (boot_image_check(APP_ADDR) == BOOT_STATUS_OK)
    {
        boot_jump_to_image(APP_ADDR);
    }

    return BOOT_STATUS_INVALID_IMAGE;
}

static boot_status_t bootmanager_try_programmer_fallback(void)
{
    if (boot_image_check(PROGRAMMER_ADDR) == BOOT_STATUS_OK)
    {
        boot_jump_to_image(PROGRAMMER_ADDR);
    }

    return BOOT_STATUS_INVALID_IMAGE;
}

int main(void)
{
    boot_shared_t *shared;

    port_system_init();
    boot_shared_init();

    shared = boot_shared_get();

    if (shared->boot_request == BOOT_REQ_PROGRAMMER)
    {
        (void)bootmanager_try_programmer_request(shared);
    }

    (void)bootmanager_try_application();
    (void)bootmanager_try_programmer_fallback();

    while (1)
    {
    }
}
