#ifndef GPIO_H
#define GPIO_H

void GPIO_Init(void);
void I2C1_Init(void);
void SHT31_ReadRaw(uint8_t *buf);
void SHT31_StartMeasurement(void);
void I2C1_Stop(void);
uint8_t I2C1_ReadByte_NACK(void);
uint8_t I2C1_ReadByte_ACK(void);
void I2C1_WriteByte(uint8_t data);
void I2C1_SendAddress(uint8_t addr);
void I2C1_Start(void);
void Enter_STOP_Mode(void);
void SystemClock_Restore(void);

#endif
