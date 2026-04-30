/*
 * File: system_stm32f4xx_min.c
 * Author: Phat Nguyen
 * Date: 2026-04-29
 * Description: Provides a minimal SystemInit implementation for the STM32F411CE example target.
 */

#include "stm32f4xx.h"

uint32_t SystemCoreClock = 16000000u;

/*
 * These lookup tables are expected by the ST RCC HAL implementation.
 *
 * The RCC registers do not directly store the divisor number "1, 2, 4, 8".
 * Instead they store a small encoded bitfield. HAL code reads that bitfield
 * and uses these tables to convert:
 *
 *   encoded prescaler bits -> "shift count"
 *
 * Example:
 * - AHB divide-by-1  means shift 0  because HCLK = SYSCLK >> 0
 * - AHB divide-by-2  means shift 1  because HCLK = SYSCLK >> 1
 * - AHB divide-by-4  means shift 2  because HCLK = SYSCLK >> 2
 *
 * So the table values are not the final divisors themselves; they are the
 * exponents used by HAL when reconstructing clock frequencies.
 *
 * In this minimal target we still define them even though clock setup is
 * simple, because stm32f4xx_hal_rcc.c references these symbols internally.
 */
const uint8_t AHBPrescTable[16] = {0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 6u, 7u, 8u, 9u};
const uint8_t APBPrescTable[8] = {0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u};

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
