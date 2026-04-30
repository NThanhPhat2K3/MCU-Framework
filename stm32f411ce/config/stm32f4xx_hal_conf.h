/*
 * File: stm32f4xx_hal_conf.h
 * Author: Phat Nguyen
 * Date: 2026-04-29
 * Description: Provides the minimal HAL feature configuration required by the STM32F411CE example.
 */

#ifndef STM32F4XX_HAL_CONF_H
#define STM32F4XX_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ########################## Module Selection ############################## */
/*
 * Keep this file close to ST's template structure, but only enable the
 * modules this framework really uses. That keeps compile time and vendor
 * surface area small while staying familiar to anyone who has seen the
 * official HAL config template before.
 */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_uart.h"

/* ########################## Oscillator Values ############################# */
/*
 * These constants are read by the RCC HAL when it computes clock values.
 * The example currently boots from HSI, but we keep the standard set of
 * values here so the file still looks like the ST template and is easier
 * to extend later if the board moves to HSE/PLL.
 */
#define HSE_VALUE ((uint32_t)8000000u)
#define HSE_STARTUP_TIMEOUT ((uint32_t)100u)
#define HSI_VALUE ((uint32_t)16000000u)
#define EXTERNAL_CLOCK_VALUE ((uint32_t)12288000u)
#define LSI_VALUE ((uint32_t)32000u)
#define LSE_VALUE ((uint32_t)32768u)
#define LSE_STARTUP_TIMEOUT ((uint32_t)5000u)

/* ########################### System Configuration ######################### */
/*
 * VDD_VALUE is used by some HAL helpers for timing/flash calculations.
 * TICK_INT_PRIORITY controls the HAL tick interrupt priority when SysTick
 * is used as the time base.
 */
#define VDD_VALUE ((uint32_t)3300u)
#define TICK_INT_PRIORITY ((uint32_t)0u)
#define USE_RTOS 0u
#define PREFETCH_ENABLE 1u
#define INSTRUCTION_CACHE_ENABLE 1u
#define DATA_CACHE_ENABLE 1u

/* ########################## Assert Selection ############################## */
/*
 * This educational framework keeps HAL asserts disabled by default to stay
 * lightweight. For bring-up/debug work, enabling full assert can be useful.
 */
#define assert_param(expr) ((void)0u)

#ifdef __cplusplus
}
#endif

#endif
