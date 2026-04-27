/*
 * File: startup_stm32f103xb.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Defines the STM32F103 vector table and default interrupt
 * handlers.
 */

#include "startup_portable_cortexm.h"

#define STM32F103_WEAK_HANDLER(name)                                           \
  /* define prototype and weak alias for the handler */                        \
  void name(void) __attribute__((weak));                                       \
  /* define the handler function */                                            \
  void name(void) { Default_Handler(); }

typedef void (*stm32f103_isr_handler_t)(void);

STM32F103_WEAK_HANDLER(NMI_Handler)
STM32F103_WEAK_HANDLER(HardFault_Handler)
STM32F103_WEAK_HANDLER(MemManage_Handler)
STM32F103_WEAK_HANDLER(BusFault_Handler)
STM32F103_WEAK_HANDLER(UsageFault_Handler)
STM32F103_WEAK_HANDLER(SVC_Handler)
STM32F103_WEAK_HANDLER(DebugMon_Handler)
STM32F103_WEAK_HANDLER(PendSV_Handler)
STM32F103_WEAK_HANDLER(SysTick_Handler)

STM32F103_WEAK_HANDLER(WWDG_IRQHandler)
STM32F103_WEAK_HANDLER(PVD_IRQHandler)
STM32F103_WEAK_HANDLER(TAMPER_IRQHandler)
STM32F103_WEAK_HANDLER(RTC_IRQHandler)
STM32F103_WEAK_HANDLER(FLASH_IRQHandler)
STM32F103_WEAK_HANDLER(RCC_IRQHandler)
STM32F103_WEAK_HANDLER(EXTI0_IRQHandler)
STM32F103_WEAK_HANDLER(EXTI1_IRQHandler)
STM32F103_WEAK_HANDLER(EXTI2_IRQHandler)
STM32F103_WEAK_HANDLER(EXTI3_IRQHandler)
STM32F103_WEAK_HANDLER(EXTI4_IRQHandler)
STM32F103_WEAK_HANDLER(DMA1_Channel1_IRQHandler)
STM32F103_WEAK_HANDLER(DMA1_Channel2_IRQHandler)
STM32F103_WEAK_HANDLER(DMA1_Channel3_IRQHandler)
STM32F103_WEAK_HANDLER(DMA1_Channel4_IRQHandler)
STM32F103_WEAK_HANDLER(DMA1_Channel5_IRQHandler)
STM32F103_WEAK_HANDLER(DMA1_Channel6_IRQHandler)
STM32F103_WEAK_HANDLER(DMA1_Channel7_IRQHandler)
STM32F103_WEAK_HANDLER(ADC1_2_IRQHandler)
STM32F103_WEAK_HANDLER(USB_HP_CAN1_TX_IRQHandler)
STM32F103_WEAK_HANDLER(USB_LP_CAN1_RX0_IRQHandler)
STM32F103_WEAK_HANDLER(CAN1_RX1_IRQHandler)
STM32F103_WEAK_HANDLER(CAN1_SCE_IRQHandler)
STM32F103_WEAK_HANDLER(EXTI9_5_IRQHandler)
STM32F103_WEAK_HANDLER(TIM1_BRK_IRQHandler)
STM32F103_WEAK_HANDLER(TIM1_UP_IRQHandler)
STM32F103_WEAK_HANDLER(TIM1_TRG_COM_IRQHandler)
STM32F103_WEAK_HANDLER(TIM1_CC_IRQHandler)
STM32F103_WEAK_HANDLER(TIM2_IRQHandler)
STM32F103_WEAK_HANDLER(TIM3_IRQHandler)
STM32F103_WEAK_HANDLER(TIM4_IRQHandler)
STM32F103_WEAK_HANDLER(I2C1_EV_IRQHandler)
STM32F103_WEAK_HANDLER(I2C1_ER_IRQHandler)
STM32F103_WEAK_HANDLER(I2C2_EV_IRQHandler)
STM32F103_WEAK_HANDLER(I2C2_ER_IRQHandler)
STM32F103_WEAK_HANDLER(SPI1_IRQHandler)
STM32F103_WEAK_HANDLER(SPI2_IRQHandler)
STM32F103_WEAK_HANDLER(USART1_IRQHandler)
STM32F103_WEAK_HANDLER(USART2_IRQHandler)
STM32F103_WEAK_HANDLER(USART3_IRQHandler)
STM32F103_WEAK_HANDLER(EXTI15_10_IRQHandler)
STM32F103_WEAK_HANDLER(RTC_Alarm_IRQHandler)
STM32F103_WEAK_HANDLER(USBWakeUp_IRQHandler)

__attribute__((section(".isr_vector")))
const stm32f103_isr_handler_t g_pfnVectors[] = {
    (stm32f103_isr_handler_t)&_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,
    PVD_IRQHandler,
    TAMPER_IRQHandler,
    RTC_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA1_Channel1_IRQHandler,
    DMA1_Channel2_IRQHandler,
    DMA1_Channel3_IRQHandler,
    DMA1_Channel4_IRQHandler,
    DMA1_Channel5_IRQHandler,
    DMA1_Channel6_IRQHandler,
    DMA1_Channel7_IRQHandler,
    ADC1_2_IRQHandler,
    USB_HP_CAN1_TX_IRQHandler,
    USB_LP_CAN1_RX0_IRQHandler,
    CAN1_RX1_IRQHandler,
    CAN1_SCE_IRQHandler,
    EXTI9_5_IRQHandler,
    TIM1_BRK_IRQHandler,
    TIM1_UP_IRQHandler,
    TIM1_TRG_COM_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    USART3_IRQHandler,
    EXTI15_10_IRQHandler,
    RTC_Alarm_IRQHandler,
    USBWakeUp_IRQHandler,
};
