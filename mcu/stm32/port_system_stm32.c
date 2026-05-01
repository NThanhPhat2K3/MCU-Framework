/*
 * File: port_system_stm32.c
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Implements the generic STM32 system backend. This file is the
 * only layer that knows about STM32 clock init, SysTick, reset, and VTOR
 * details.
 */

#include "port_system_stm32.h"

#include "boot_config.h"
#include "board_config.h"
#include "port_stm32.h"

static volatile uint32_t g_tick_ms;

static void port_system_stm32_init(void) {
  g_tick_ms = 0u;
  board_system_clock_init();
  SysTick_Config(SystemCoreClock / 1000u);
}

void SysTick_Handler(void) {
  ++g_tick_ms;
}

static void port_system_stm32_reset(void) {
  NVIC_SystemReset();
}

static uint32_t port_system_stm32_get_tick(void) {
  return g_tick_ms;
}

static void port_system_stm32_delay_ms(uint32_t delay_ms) {
  uint32_t start = g_tick_ms;

  while ((g_tick_ms - start) < delay_ms) {
  }
}

static int port_system_stm32_is_flash_addr(uint32_t addr) {
  return (addr >= FLASH_BASE_ADDR) && (addr < FLASH_END_ADDR);
}

static int port_system_stm32_is_ram_addr(uint32_t addr) {
  return (addr & 0x2FFE0000u) == 0x20000000u;
}

static void port_system_stm32_prepare_jump(uint32_t vector_addr,
                                           uint32_t stack_ptr) {
  __disable_irq();

  for (uint32_t i = 0u; i < 8u; ++i) {
    NVIC->ICER[i] = 0xFFFFFFFFu;
    NVIC->ICPR[i] = 0xFFFFFFFFu;
  }

  SysTick->CTRL = 0u;
  SysTick->LOAD = 0u;
  SysTick->VAL = 0u;
  g_tick_ms = 0u;

  __set_MSP(stack_ptr);
  SCB->VTOR = vector_addr;

  __enable_irq();
}

const port_system_ops_t *port_system_stm32_get_ops(void) {
  static const port_system_ops_t ops = {
      .init = port_system_stm32_init,
      .reset = port_system_stm32_reset,
      .get_tick = port_system_stm32_get_tick,
      .delay_ms = port_system_stm32_delay_ms,
      .is_flash_addr = port_system_stm32_is_flash_addr,
      .is_ram_addr = port_system_stm32_is_ram_addr,
      .prepare_jump = port_system_stm32_prepare_jump,
  };

  return &ops;
}
