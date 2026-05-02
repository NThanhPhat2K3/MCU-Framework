/*
 * File: port_flash_stm32f4.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Implements flash erase/write services for STM32F4 targets.
 */

#include "port_flash_stm32f4.h"

#include "port_stm32.h"

#define PORT_FLASH_STM32F4_KEY1 0x45670123u
#define PORT_FLASH_STM32F4_KEY2 0xCDEF89ABu
#define PORT_FLASH_STM32F4_ERROR_FLAGS \
  (FLASH_SR_OPERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | \
   FLASH_SR_PGSERR | FLASH_SR_RDERR)
#define PORT_FLASH_STM32F4_PSIZE_WORD FLASH_CR_PSIZE_1

static uint32_t g_port_flash_stm32f4_last_error_detail;
static uint32_t g_port_flash_stm32f4_last_error_addr;

static uint32_t port_flash_stm32f4_irq_save(void) {
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  return primask;
}

static void port_flash_stm32f4_irq_restore(uint32_t primask) {
  if (primask == 0u) {
    __enable_irq();
  }
}

static void port_flash_stm32f4_unlock(void) {
  if ((FLASH->CR & FLASH_CR_LOCK) != 0u) {
    FLASH->KEYR = PORT_FLASH_STM32F4_KEY1;
    FLASH->KEYR = PORT_FLASH_STM32F4_KEY2;
  }
}

static void port_flash_stm32f4_lock(void) { FLASH->CR |= FLASH_CR_LOCK; }

static int port_flash_stm32f4_wait_ready(void) {
  while ((FLASH->SR & FLASH_SR_BSY) != 0u) {
  }

  if ((FLASH->SR & PORT_FLASH_STM32F4_ERROR_FLAGS) != 0u) {
    g_port_flash_stm32f4_last_error_detail = FLASH->SR & PORT_FLASH_STM32F4_ERROR_FLAGS;
    FLASH->SR = PORT_FLASH_STM32F4_ERROR_FLAGS;
    return -1;
  }

  FLASH->SR = FLASH_SR_EOP;
  return 0;
}

static boot_status_t port_flash_stm32f4_erase_app(void) {
  uint32_t sector;
  uint32_t primask = port_flash_stm32f4_irq_save();

  g_port_flash_stm32f4_last_error_detail = 0u;
  g_port_flash_stm32f4_last_error_addr = 0u;
  port_flash_stm32f4_unlock();
  FLASH->SR = PORT_FLASH_STM32F4_ERROR_FLAGS;

  for (sector = 4u; sector <= 7u; ++sector) {
    if (port_flash_stm32f4_wait_ready() != 0) {
      port_flash_stm32f4_lock();
      port_flash_stm32f4_irq_restore(primask);
      return BOOT_STATUS_IO_ERROR;
    }

    FLASH->CR &= ~(FLASH_CR_SNB | FLASH_CR_PSIZE);
    FLASH->CR |= FLASH_CR_SER | PORT_FLASH_STM32F4_PSIZE_WORD |
                 (sector << FLASH_CR_SNB_Pos);
    FLASH->CR |= FLASH_CR_STRT;

    if (port_flash_stm32f4_wait_ready() != 0) {
      FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB | FLASH_CR_PSIZE);
      port_flash_stm32f4_lock();
      port_flash_stm32f4_irq_restore(primask);
      return BOOT_STATUS_IO_ERROR;
    }

    FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB | FLASH_CR_PSIZE);
  }

  port_flash_stm32f4_lock();
  port_flash_stm32f4_irq_restore(primask);
  return BOOT_STATUS_OK;
}

static boot_status_t port_flash_stm32f4_write(uint32_t addr,
                                              const uint8_t *data,
                                              uint32_t len) {
  uint32_t i;
  uint32_t primask = port_flash_stm32f4_irq_save();

  g_port_flash_stm32f4_last_error_detail = 0u;
  g_port_flash_stm32f4_last_error_addr = 0u;
  port_flash_stm32f4_unlock();
  FLASH->SR = PORT_FLASH_STM32F4_ERROR_FLAGS;
  FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB | FLASH_CR_PSIZE);
  FLASH->CR |= FLASH_CR_PG | PORT_FLASH_STM32F4_PSIZE_WORD;

  for (i = 0; i < len; i += 4u) {
    uint32_t word = 0xFFFFFFFFu;
    uint8_t *word_bytes = (uint8_t *)&word;
    uint32_t j;

    for (j = 0u; (j < 4u) && ((i + j) < len); ++j) {
      word_bytes[j] = data[i + j];
    }

    g_port_flash_stm32f4_last_error_addr = addr + i;
    *(__IO uint32_t *)(addr + i) = word;

    if (port_flash_stm32f4_wait_ready() != 0) {
      FLASH->CR &= ~(FLASH_CR_PG | FLASH_CR_PSIZE);
      port_flash_stm32f4_lock();
      port_flash_stm32f4_irq_restore(primask);
      return BOOT_STATUS_IO_ERROR;
    }
  }

  FLASH->CR &= ~(FLASH_CR_PG | FLASH_CR_PSIZE);
  port_flash_stm32f4_lock();
  port_flash_stm32f4_irq_restore(primask);
  return BOOT_STATUS_OK;
}

static uint32_t port_flash_stm32f4_get_last_error_detail(void) {
  return g_port_flash_stm32f4_last_error_detail;
}

static uint32_t port_flash_stm32f4_get_last_error_addr(void) {
  return g_port_flash_stm32f4_last_error_addr;
}

const port_flash_ops_t *port_flash_stm32f4_get_ops(void) {
  static const port_flash_ops_t ops = {
      .erase_app = port_flash_stm32f4_erase_app,
      .write = port_flash_stm32f4_write,
      .get_last_error_detail = port_flash_stm32f4_get_last_error_detail,
      .get_last_error_addr = port_flash_stm32f4_get_last_error_addr,
  };

  return &ops;
}
