#include "port_flash.h"

#include "boot_config.h"

#include "stm32f1xx_hal.h"

#define PORT_FLASH_PAGE_SIZE 0x400u

boot_status_t port_flash_erase_app(void)
{
    FLASH_EraseInitTypeDef erase;
    uint32_t page_error = 0u;
    HAL_StatusTypeDef status;

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = APP_ADDR;
    erase.NbPages = (FLASH_END_ADDR - APP_ADDR) / PORT_FLASH_PAGE_SIZE;

    HAL_FLASH_Unlock();
    status = HAL_FLASHEx_Erase(&erase, &page_error);
    HAL_FLASH_Lock();

    return (status == HAL_OK) ? BOOT_STATUS_OK : BOOT_STATUS_IO_ERROR;
}

boot_status_t port_flash_write(uint32_t addr, const uint8_t *data, uint32_t len)
{
    uint32_t i;

    HAL_FLASH_Unlock();

    for (i = 0; i < len; i += 2u)
    {
        uint16_t halfword = data[i];

        if (i + 1u < len)
        {
            halfword |= (uint16_t)data[i + 1u] << 8;
        }
        else
        {
            halfword |= 0xFF00u;
        }

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + i, halfword) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return BOOT_STATUS_IO_ERROR;
        }
    }

    HAL_FLASH_Lock();
    return BOOT_STATUS_OK;
}
