#include "sht31_crc.h"

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
