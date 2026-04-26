/*
 * File: app_main.c
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Provides a simple STM32F103 application used to validate the boot flow.
 */

#include "app_update.h"

#include "boot_config.h"

#include "stm32f1xx_hal.h"

static UART_HandleTypeDef huart1;

static void app_clock_init(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.PLL.PLLState = RCC_PLL_NONE;
    (void)HAL_RCC_OscConfig(&osc);

    clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                    RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    (void)HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_0);
}

static void app_gpio_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_13;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);
}

static void app_uart_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_9;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = GPIO_PIN_10;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    (void)HAL_UART_Init(&huart1);
}

static void app_send_text(const char *text)
{
    const char *ptr = text;
    uint16_t len = 0;

    while (*ptr != '\0')
    {
        ++ptr;
        ++len;
    }

    (void)HAL_UART_Transmit(&huart1, (uint8_t *)text, len, HAL_MAX_DELAY);
}

int main(void)
{
    uint8_t rx_byte;
    uint32_t last_tick = 0u;

    SCB->VTOR = APP_ADDR;

    HAL_Init();
    app_clock_init();
    app_gpio_init();
    app_uart_init();

    app_send_text("APP RUNNING\r\n");
    app_send_text("Send 'u' to enter programmer\r\n");

    for (;;)
    {
        if ((HAL_GetTick() - last_tick) >= 500u)
        {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            last_tick = HAL_GetTick();
        }

        if (HAL_UART_Receive(&huart1, &rx_byte, 1u, 10u) == HAL_OK)
        {
            if ((rx_byte == 'u') || (rx_byte == 'U'))
            {
                app_send_text("GO PROGRAMMER\r\n");
                HAL_Delay(50u);
                app_request_programmer_and_reset();
            }
        }
    }
}
