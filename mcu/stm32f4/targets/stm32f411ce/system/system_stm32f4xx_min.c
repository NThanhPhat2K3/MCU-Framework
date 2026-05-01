/*
 * File: system_stm32f4xx_min.c
 * Author: Phat Nguyen
 * Date: 2026-04-29
 * Description: Provides a minimal SystemInit implementation for the STM32F411CE example target.
 */

#include "stm32f4xx.h"

uint32_t SystemCoreClock = 16000000u;

void SystemInit(void)
{
    /*
     * Minimal bring-up:
     * - keep the device on reset-default HSI
     * - let higher-level code perform any explicit clock reconfiguration
     */
    SystemCoreClock = 16000000u;
}

void SystemCoreClockUpdate(void)
{
    /*
     * A full ST system file would re-read RCC registers and recompute the
     * live core clock here. This minimal example stays on HSI, so the value
     * is constant unless the project later adds more advanced clock setup.
     */
    SystemCoreClock = 16000000u;
}
