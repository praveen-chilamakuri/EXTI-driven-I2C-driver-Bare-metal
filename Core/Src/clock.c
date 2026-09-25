#include "stm32f411xe.h"
#include "stm32f4xx_it.h"

extern uint32_t SystemCoreClock;

void Clock_Init(void)
{
    // 1. Enable HSI
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    // 2. Flash latency for 16 MHz
    FLASH->ACR = FLASH_ACR_LATENCY_0WS;

}

void SysTick_Init(void)
{
    // 1 ms tick
    SysTick_Config(SystemCoreClock / 1000);
}
