/*
 * File: port_system.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Implements STM32 system startup, reset, address validation, and image handover support.
 */

#include "port_system.h"

#include "boot_config.h"

#include "stm32f1xx_hal.h"

static void port_system_clock_init(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.PLL.PLLState = RCC_PLL_NONE;
    (void)HAL_RCC_OscConfig(&osc);

    clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                    RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    (void)HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_0);
}

void port_system_init(void)
{
    HAL_Init();
    port_system_clock_init();
}

void port_system_reset(void)
{
    NVIC_SystemReset();
}

int port_system_is_flash_addr(uint32_t addr)
{
    return (addr >= FLASH_BASE_ADDR) && (addr < FLASH_END_ADDR);
}

int port_system_is_ram_addr(uint32_t addr)
{
    return (addr & 0x2FFE0000u) == 0x20000000u;
}

void port_system_prepare_jump(uint32_t vector_addr, uint32_t stack_ptr)
{
    HAL_DeInit();

    __disable_irq();

    SysTick->CTRL = 0u;
    SysTick->LOAD = 0u;
    SysTick->VAL = 0u;

    __set_MSP(stack_ptr);
    SCB->VTOR = vector_addr;

    __enable_irq();
}
