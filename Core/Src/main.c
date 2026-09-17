#include "stm32f411xe.h"
#include "clock.h"
#include "peripherals_init.h"

extern volatile uint32_t msTicks;
extern volatile uint8_t exti_flag;

void delay_ms(uint32_t ms)
{
    uint32_t start = msTicks;
    while ((msTicks - start) < ms);
}

// =====================CRC function==================

uint8_t SHT31_CRC8(uint8_t *data)
{
    uint8_t crc = 0xFF;

    for (int i = 0; i < 2; i++)
    {
        crc ^= data[i];
        for (int b = 0; b < 8; b++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// =========================check crc=================

uint8_t SHT31_ReadRaw_CRC(uint8_t *raw)
{
    // Trigger measurement
    SHT31_StartMeasurement();
    delay_ms(15);

    // Read 6 bytes
    SHT31_ReadRaw(raw);

    // CRC check: temperature and humidity
    if ((SHT31_CRC8(raw) != raw[2]) || (SHT31_CRC8(&raw[3]) != raw[5]))
    {
        return 1;   // CRC fail
    }
    else
    {
    return 0;  // CRC OK
    }
}

int main(void)
{
    Clock_Init();
    SysTick_Init();
    GPIO_Init();
    I2C1_Init();

    uint8_t raw[6];

   while (1)
    {

	   GPIOA->BSRR = (1 << 6);          // HIGH
	   delay_ms(1);
	   GPIOA->BSRR = (1 << (6 + 16));   // LOW
	   delay_ms(1);

	   Enter_STOP_Mode();
	   if (exti_flag == 1)
		   {
		     SystemClock_Restore();
		     SysTick_Init();
		     I2C1_Init();

		   uint8_t status = SHT31_ReadRaw_CRC(raw);
		    exti_flag = 0;

		    delay_ms(1);

	      if (status == 0)
	   	           {
	   	        	   GPIOA->BSRR = (1 << 5);   // Set PA5
	   	        	   delay_ms(1);
	   	        	   GPIOA->BSRR = (1 << (5 + 16));   // Reset PA5
	   	        	   delay_ms(1);
	   	           }
		   }
     }
}
