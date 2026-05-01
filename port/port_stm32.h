/*
 * File: port_stm32.h
 * Author: Phat Nguyen
 * Date: 2026-05-01
 * Description: Selects the correct STM32 device and LL headers for the active
 * target.
 */

#ifndef PORTABLE_BOOT_EXAMPLE_PORT_STM32_H
#define PORTABLE_BOOT_EXAMPLE_PORT_STM32_H

#if defined(STM32F103xB)
#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_usart.h"
#elif defined(STM32F411xE)
#include "stm32f4xx.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_usart.h"
#else
#error Unsupported STM32 family for port_stm32.h
#endif

#if defined(USART_CR1_OVER8)
#define PORT_USART_SET_BAUD(USARTX, PERIPH_CLK, BAUDRATE) \
  LL_USART_SetBaudRate((USARTX), (PERIPH_CLK), LL_USART_OVERSAMPLING_16, \
                       (BAUDRATE))
#else
#define PORT_USART_SET_BAUD(USARTX, PERIPH_CLK, BAUDRATE) \
  LL_USART_SetBaudRate((USARTX), (PERIPH_CLK), (BAUDRATE))
#endif

#endif /* PORTABLE_BOOT_EXAMPLE_PORT_STM32_H */
