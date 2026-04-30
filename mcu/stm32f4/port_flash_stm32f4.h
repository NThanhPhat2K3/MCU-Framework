/*
 * File: port_flash_stm32f4.h
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Declares the STM32F4 flash backend used by the generic port layer.
 */

#ifndef MCU_STM32F4_PORT_FLASH_STM32F4_H
#define MCU_STM32F4_PORT_FLASH_STM32F4_H

#include "port_flash.h"

const port_flash_ops_t *port_flash_stm32f4_get_ops(void);

#endif /* MCU_STM32F4_PORT_FLASH_STM32F4_H */
