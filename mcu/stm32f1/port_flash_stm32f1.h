/*
 * File: port_flash_stm32f1.h
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Declares the STM32F1 flash backend used by the generic port layer.
 */

#ifndef MCU_STM32F1_PORT_FLASH_STM32F1_H
#define MCU_STM32F1_PORT_FLASH_STM32F1_H

#include "port_flash.h"

const port_flash_ops_t *port_flash_stm32f1_get_ops(void);

#endif /* MCU_STM32F1_PORT_FLASH_STM32F1_H */
