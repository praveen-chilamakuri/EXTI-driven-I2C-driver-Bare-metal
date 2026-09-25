#ifndef SHT31_HW_H
#define SHT31_HW_H

#include <stdint.h>

void SHT31_StartMeasurement(void);
void SHT31_ReadRaw(uint8_t *buf);
void delay_ms(uint32_t ms);

#endif
