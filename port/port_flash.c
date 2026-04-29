/*
 * File: port_flash.c
 * Author: Phat Nguyen
 * Date: 2026-04-29
 * Description: Implements STM32 flash erase and write services for the App region.
 */

#include "port_flash.h"

#include "boot_config.h"
#include "port_hal.h"

#if defined(STM32F103xB)
#define PORT_FLASH_PAGE_SIZE 0x400u
#endif

boot_status_t port_flash_erase_app(void)
{
    HAL_StatusTypeDef status;

    HAL_FLASH_Unlock();

#if defined(STM32F103xB)
    FLASH_EraseInitTypeDef erase;
    uint32_t page_error = 0u;

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = APP_ADDR;
    erase.NbPages = (FLASH_END_ADDR - APP_ADDR) / PORT_FLASH_PAGE_SIZE;

    status = HAL_FLASHEx_Erase(&erase, &page_error);
#elif defined(STM32F411xE)
    FLASH_EraseInitTypeDef erase;
    uint32_t sector_error = 0u;

    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    erase.Sector = FLASH_SECTOR_4;
    erase.NbSectors = 4u;

    status = HAL_FLASHEx_Erase(&erase, &sector_error);
#else
#error Unsupported STM32 family for port_flash.c
#endif

    HAL_FLASH_Lock();

    return (status == HAL_OK) ? BOOT_STATUS_OK : BOOT_STATUS_IO_ERROR;
}

boot_status_t port_flash_write(uint32_t addr, const uint8_t *data, uint32_t len)
{
    uint32_t i;

    HAL_FLASH_Unlock();

#if defined(STM32F103xB)
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
#elif defined(STM32F411xE)
    for (i = 0; i < len; ++i)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr + i, data[i]) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return BOOT_STATUS_IO_ERROR;
        }
    }
#else
#error Unsupported STM32 family for port_flash.c
#endif

    HAL_FLASH_Lock();
    return BOOT_STATUS_OK;
}
