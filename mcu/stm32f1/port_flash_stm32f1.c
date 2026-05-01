/*
 * File: port_flash_stm32f1.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Implements flash erase/write services for STM32F1 targets.
 */

#include "port_flash_stm32f1.h"

#include "boot_config.h"
#include "port_stm32.h"

#define PORT_FLASH_STM32F1_PAGE_SIZE 0x400u
#define PORT_FLASH_STM32F1_ERROR_FLAGS (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)

static void port_flash_stm32f1_unlock(void) {
  if ((FLASH->CR & FLASH_CR_LOCK) != 0u) {
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
  }
}

static void port_flash_stm32f1_lock(void) { FLASH->CR |= FLASH_CR_LOCK; }

static int port_flash_stm32f1_wait_ready(void) {
  while ((FLASH->SR & FLASH_SR_BSY) != 0u) {
  }

  if ((FLASH->SR & PORT_FLASH_STM32F1_ERROR_FLAGS) != 0u) {
    FLASH->SR = PORT_FLASH_STM32F1_ERROR_FLAGS;
    return -1;
  }

  FLASH->SR = FLASH_SR_EOP;
  return 0;
}

static boot_status_t port_flash_stm32f1_erase_app(void) {
  uint32_t page_addr;
  uint32_t page_count = (FLASH_END_ADDR - APP_ADDR) / PORT_FLASH_STM32F1_PAGE_SIZE;

  port_flash_stm32f1_unlock();
  FLASH->SR = PORT_FLASH_STM32F1_ERROR_FLAGS;

  for (page_addr = 0u; page_addr < page_count; ++page_addr) {
    if (port_flash_stm32f1_wait_ready() != 0) {
      port_flash_stm32f1_lock();
      return BOOT_STATUS_IO_ERROR;
    }

    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = APP_ADDR + (page_addr * PORT_FLASH_STM32F1_PAGE_SIZE);
    FLASH->CR |= FLASH_CR_STRT;

    if (port_flash_stm32f1_wait_ready() != 0) {
      FLASH->CR &= ~FLASH_CR_PER;
      port_flash_stm32f1_lock();
      return BOOT_STATUS_IO_ERROR;
    }

    FLASH->CR &= ~FLASH_CR_PER;
  }

  port_flash_stm32f1_lock();
  return BOOT_STATUS_OK;
}

static boot_status_t port_flash_stm32f1_write(uint32_t addr,
                                              const uint8_t *data,
                                              uint32_t len) {
  uint32_t i;

  port_flash_stm32f1_unlock();
  FLASH->SR = PORT_FLASH_STM32F1_ERROR_FLAGS;
  FLASH->CR |= FLASH_CR_PG;

  for (i = 0; i < len; i += 2u) {
    uint16_t halfword = data[i];

    if (i + 1u < len) {
      halfword |= (uint16_t)data[i + 1u] << 8;
    } else {
      halfword |= 0xFF00u;
    }

    *(__IO uint16_t *)(addr + i) = halfword;

    if (port_flash_stm32f1_wait_ready() != 0) {
      FLASH->CR &= ~FLASH_CR_PG;
      port_flash_stm32f1_lock();
      return BOOT_STATUS_IO_ERROR;
    }
  }

  FLASH->CR &= ~FLASH_CR_PG;
  port_flash_stm32f1_lock();
  return BOOT_STATUS_OK;
}

const port_flash_ops_t *port_flash_stm32f1_get_ops(void) {
  static const port_flash_ops_t ops = {
      .erase_app = port_flash_stm32f1_erase_app,
      .write = port_flash_stm32f1_write,
  };

  return &ops;
}
