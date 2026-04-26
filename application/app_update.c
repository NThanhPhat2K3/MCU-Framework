#include "app_update.h"

#include "boot_shared.h"
#include "boot_types.h"
#include "port_system.h"

void app_request_programmer_and_reset(void)
{
    boot_shared_t *shared;

    boot_shared_init();
    shared = boot_shared_get();

    shared->boot_request = BOOT_REQ_PROGRAMMER;
    shared->update_status = UPDATE_IDLE;
    shared->error_code = 0u;

    port_system_reset();
}
