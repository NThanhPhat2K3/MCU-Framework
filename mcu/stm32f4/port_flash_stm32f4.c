/*
 * File: port_flash_stm32f4.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Implements flash erase/write services for STM32F4 targets.
 */

#include "port_flash_stm32f4.h"

#include "port_hal.h"

static boot_status_t port_flash_stm32f4_erase_app(void) {
  FLASH_EraseInitTypeDef erase;
  uint32_t sector_error = 0u;
  HAL_StatusTypeDef status;

  HAL_FLASH_Unlock();

  erase.TypeErase = FLASH_TYPEERASE_SECTORS;
  erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  erase.Sector = FLASH_SECTOR_4;
  erase.NbSectors = 4u;

  status = HAL_FLASHEx_Erase(&erase, &sector_error);

  HAL_FLASH_Lock();

  return (status == HAL_OK) ? BOOT_STATUS_OK : BOOT_STATUS_IO_ERROR;
}

static boot_status_t port_flash_stm32f4_write(uint32_t addr,
                                              const uint8_t *data,
                                              uint32_t len) {
  uint32_t i;

  HAL_FLASH_Unlock();

  for (i = 0; i < len; ++i) {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr + i, data[i]) !=
        HAL_OK) {
      HAL_FLASH_Lock();
      return BOOT_STATUS_IO_ERROR;
    }
  }

  HAL_FLASH_Lock();
  return BOOT_STATUS_OK;
}

const port_flash_ops_t *port_flash_stm32f4_get_ops(void) {
  static const port_flash_ops_t ops = {
      .erase_app = port_flash_stm32f4_erase_app,
      .write = port_flash_stm32f4_write,
  };

  return &ops;
}
