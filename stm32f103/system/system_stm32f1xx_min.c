/*
 * File: system_stm32f1xx_min.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Provides a minimal SystemInit implementation for the STM32F103 example target.
 */

#include "stm32f1xx.h"

uint32_t SystemCoreClock = 8000000u;

void SystemInit(void)
{
    SystemCoreClock = 8000000u;
}
