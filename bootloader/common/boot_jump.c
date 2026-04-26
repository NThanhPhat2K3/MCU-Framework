#include "boot_jump.h"

#include "port_system.h"

typedef void (*boot_entry_fn_t)(void);

boot_status_t boot_image_check(uint32_t image_addr)
{
    uint32_t stack_ptr = *(volatile uint32_t *)image_addr;
    uint32_t reset_vector = *(volatile uint32_t *)(image_addr + 4u);

    if (!port_system_is_ram_addr(stack_ptr))
    {
        return BOOT_STATUS_INVALID_IMAGE;
    }

    if (!port_system_is_flash_addr(reset_vector))
    {
        return BOOT_STATUS_INVALID_IMAGE;
    }

    return BOOT_STATUS_OK;
}

void boot_jump_to_image(uint32_t image_addr)
{
    uint32_t stack_ptr = *(volatile uint32_t *)image_addr;
    uint32_t reset_vector = *(volatile uint32_t *)(image_addr + 4u);
    boot_entry_fn_t entry = (boot_entry_fn_t)reset_vector;

    port_system_prepare_jump(image_addr, stack_ptr);
    entry();
}
