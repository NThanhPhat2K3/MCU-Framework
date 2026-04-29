/*
 * File: port_hal.h
 * Author: Phat Nguyen
 * Date: 2026-04-29
 * Description: Selects the correct STM32 HAL header for the active target.
 */

#ifndef PORTABLE_BOOT_EXAMPLE_PORT_HAL_H
#define PORTABLE_BOOT_EXAMPLE_PORT_HAL_H

#if defined(STM32F103xB)
#include "stm32f1xx_hal.h"
#elif defined(STM32F411xE)
#include "stm32f4xx_hal.h"
#else
#error Unsupported STM32 family for port_hal.h
#endif

#endif
