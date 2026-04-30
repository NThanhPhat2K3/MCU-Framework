/*
 * File: stm32f1xx_hal_conf.h
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Provides the minimal HAL feature configuration required by the STM32F103 example.
 */

#ifndef STM32F1XX_HAL_CONF_H
#define STM32F1XX_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_uart.h"

#define HSE_VALUE ((uint32_t)8000000u)
#define HSE_STARTUP_TIMEOUT ((uint32_t)100u)
#define HSI_VALUE ((uint32_t)8000000u)
#define LSI_VALUE ((uint32_t)40000u)
#define LSE_VALUE ((uint32_t)32768u)
#define LSE_STARTUP_TIMEOUT ((uint32_t)5000u)
#define VDD_VALUE ((uint32_t)3300u)
#define TICK_INT_PRIORITY ((uint32_t)0u)
#define USE_RTOS 0u
#define PREFETCH_ENABLE 1u

#define assert_param(expr) ((void)0u)

#ifdef __cplusplus
}
#endif

#endif
