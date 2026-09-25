#include "stm32f411xe.h"

#define SHT31_ADDR 0x45

void GPIO_Init(void)
{
    // I2C

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // PB6, PB7 → AF mode
    GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOB->MODER |=  (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);

    // Open-drain
    GPIOB->OTYPER |= (GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);

    // Medium speed
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED6_0 | GPIO_OSPEEDR_OSPEED7_0);

    // No internal pull-ups (external pull-ups required)
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);

    // AF4 for I2C1
    GPIOB->AFR[0] &= ~((0xF << (6*4)) | (0xF << (7*4)));
    GPIOB->AFR[0] |=  (0x4 << (6*4)) | (0x4 << (7*4));

// PA5 (led)

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

        GPIOA->MODER &= ~(3 << (5*2));
        GPIOA->MODER |=  (1 << (5*2));   // PA5 = output

           GPIOA->MODER &= ~(3 << (6 * 2));
                GPIOA->MODER |=  (1 << (6 * 2));   // PA6 output

            // EXTI

                // Enable GPIOC clock
                RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

                // PC13 as input (default)
                GPIOC->MODER &= ~(3 << (13 * 2));

                // Pull-up (PC13 is active-low button)
                GPIOC->PUPDR &= ~(3 << (13 * 2));
                GPIOC->PUPDR |=  (1 << (13 * 2));   // pull-up

                // Enable SYSCFG clock
                RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

                // Connect EXTI13 to PC13
                SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
                SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC;

                // Configure EXTI13
                EXTI->IMR  |= (1 << 13);   // unmask line 13
                EXTI->FTSR |= (1 << 13);   // falling edge trigger (button press)
                EXTI->RTSR &= ~(1 << 13);  // no rising edge

                NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void Enter_STOP_Mode(void)
{
    // Clear wakeup flag
    PWR->CR |= PWR_CR_CWUF;

    // STOP mode (not standby)
    PWR->CR &= ~PWR_CR_PDDS;
    PWR->CR &= ~PWR_CR_LPDS;

    // Deep sleep
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    __WFI();   // enter STOP

    // Clear deep sleep bit after wake
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
}

void SystemClock_Restore(void)
{
    // Enable HSI
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    // Switch SYSCLK to HSI
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |=  RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    // Re-enable GPIO and I2C clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
}



void I2C1_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    I2C1->CR1 = 0;
    I2C1->CR2 = 16;      // PCLK1 = 16 MHz
    I2C1->CCR = 80;      // 100 kHz standard mode
    I2C1->TRISE = 17;    // TRISE = Fpclk(MHz) + 1

    I2C1->CR1 |= I2C_CR1_PE;
}

void I2C1_Start(void)
{
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void I2C1_SendAddress(uint8_t addr)
{
    I2C1->DR = addr;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;
}

void I2C1_WriteByte(uint8_t data)
{
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF));   // byte fully transmitted
}

uint8_t I2C1_ReadByte_ACK(void)
{
    I2C1->CR1 |= I2C_CR1_ACK;
    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    return I2C1->DR;
}

uint8_t I2C1_ReadByte_NACK(void)
{
    I2C1->CR1 &= ~I2C_CR1_ACK;
    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    return I2C1->DR;
}

void I2C1_Stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
    while ((I2C1->SR1 & I2C_SR1_SB));
}


void SHT31_StartMeasurement(void)
{
    I2C1_Start();
    I2C1_SendAddress(SHT31_ADDR << 1 | 0);   // write
    I2C1_WriteByte(0x24);
    I2C1_WriteByte(0x0B);
    I2C1_Stop();
}

void SHT31_ReadRaw(uint8_t *buf)
{
	 I2C1->CR1 |= I2C_CR1_ACK;   // enable ACK before start
	 I2C1_Start();
    I2C1_SendAddress((SHT31_ADDR << 1) | 1);   // read

    buf[0] = I2C1_ReadByte_ACK();
    buf[1] = I2C1_ReadByte_ACK();
    buf[2] = I2C1_ReadByte_ACK();
    buf[3] = I2C1_ReadByte_ACK();
    buf[4] = I2C1_ReadByte_ACK();
    buf[5] = I2C1_ReadByte_NACK();

    I2C1_Stop();
}

