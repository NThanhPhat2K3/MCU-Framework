/*
 * File: system_stm32f1xx_min.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Provides a minimal SystemInit implementation for the STM32F103 example target.
 */

#include "stm32f1xx.h"

uint32_t SystemCoreClock = 8000000u;
const uint8_t AHBPrescTable[16] = {0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 6u, 7u, 8u, 9u};
const uint8_t APBPrescTable[8] = {0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u};

void SystemInit(void)
{
    SystemCoreClock = 8000000u;
}

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = 8000000u;
}
