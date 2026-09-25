#include "system_logic.h"
#include "sht31_readraw_crc.h"

uint8_t System_Process(uint8_t exti_flag, uint8_t *raw)
{
    if (exti_flag == 0)
        return 0;

    uint8_t status = SHT31_ReadRaw_CRC(raw);

    return status;
}
