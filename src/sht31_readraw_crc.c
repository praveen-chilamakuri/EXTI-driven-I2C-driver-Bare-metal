#include "sht31_crc.h"
#include "sht31_hw.h"

uint8_t SHT31_ReadRaw_CRC(uint8_t *raw)
{
    SHT31_StartMeasurement();
    delay_ms(15);
    SHT31_ReadRaw(raw);

    if ((SHT31_CRC8(raw) != raw[2]) || (SHT31_CRC8(&raw[3]) != raw[5]))
        return 1;

    return 0;
}
