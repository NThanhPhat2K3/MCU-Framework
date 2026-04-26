#include "stm32f1xx.h"

uint32_t SystemCoreClock = 8000000u;

void SystemInit(void)
{
    SystemCoreClock = 8000000u;
}
