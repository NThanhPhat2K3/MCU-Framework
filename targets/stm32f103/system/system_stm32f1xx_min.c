/*
 * File: system_stm32f1xx_min.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Provides a minimal SystemInit implementation for the STM32F103 example target.
 */

#include "stm32f1xx.h"

uint32_t SystemCoreClock = 8000000u;

/*
 * Same idea as the F4 file:
 * ST's RCC HAL reads encoded prescaler fields from RCC registers and uses
 * these tables to map them into shift counts before computing HCLK/PCLK.
 *
 * They are required support symbols for the HAL, even in a minimal project.
 */
const uint8_t AHBPrescTable[16] = {0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 6u, 7u, 8u, 9u};
const uint8_t APBPrescTable[8] = {0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u};

void SystemInit(void)
{
    /* Reset-default clock source for this minimal target is HSI = 8 MHz. */
    SystemCoreClock = 8000000u;
}

void SystemCoreClockUpdate(void)
{
    /* Kept minimal for now; extend this if the project later reprograms RCC. */
    SystemCoreClock = 8000000u;
}
