/*
 * File: port_flash_stm32f1.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Implements flash erase/write services for STM32F1 targets.
 */

#include "port_flash_stm32f1.h"

#include "boot_config.h"
#include "port_hal.h"

#define PORT_FLASH_STM32F1_PAGE_SIZE 0x400u

static boot_status_t port_flash_stm32f1_erase_app(void) {
  FLASH_EraseInitTypeDef erase;
  uint32_t page_error = 0u;
  HAL_StatusTypeDef status;

  HAL_FLASH_Unlock();

  erase.TypeErase = FLASH_TYPEERASE_PAGES;
  erase.PageAddress = APP_ADDR;
  erase.NbPages = (FLASH_END_ADDR - APP_ADDR) / PORT_FLASH_STM32F1_PAGE_SIZE;

  status = HAL_FLASHEx_Erase(&erase, &page_error);

  HAL_FLASH_Lock();

  return (status == HAL_OK) ? BOOT_STATUS_OK : BOOT_STATUS_IO_ERROR;
}

static boot_status_t port_flash_stm32f1_write(uint32_t addr,
                                              const uint8_t *data,
                                              uint32_t len) {
  uint32_t i;

  HAL_FLASH_Unlock();

  for (i = 0; i < len; i += 2u) {
    uint16_t halfword = data[i];

    if (i + 1u < len) {
      halfword |= (uint16_t)data[i + 1u] << 8;
    } else {
      halfword |= 0xFF00u;
    }

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + i, halfword) !=
        HAL_OK) {
      HAL_FLASH_Lock();
      return BOOT_STATUS_IO_ERROR;
    }
  }

  HAL_FLASH_Lock();
  return BOOT_STATUS_OK;
}

const port_flash_ops_t *port_flash_stm32f1_get_ops(void) {
  static const port_flash_ops_t ops = {
      .erase_app = port_flash_stm32f1_erase_app,
      .write = port_flash_stm32f1_write,
  };

  return &ops;
}
