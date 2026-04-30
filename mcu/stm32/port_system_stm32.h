/*
 * File: port_system_stm32.h
 * Author: Phat Nguyen
 * Date: 2026-04-30
 * Description: Declares the STM32 system backend used by the generic port layer.
 */

#ifndef MCU_STM32_PORT_SYSTEM_STM32_H
#define MCU_STM32_PORT_SYSTEM_STM32_H

#include "port_system.h"

const port_system_ops_t *port_system_stm32_get_ops(void);

#endif /* MCU_STM32_PORT_SYSTEM_STM32_H */
