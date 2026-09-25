#include "unity.h"
#include "sht31_crc.h"
#include "sht31_readraw_crc.h"
#include "mock_sht31_hw.h"

void setUp(void) {}
void tearDown(void) {}

void test_readraw_crc_ok_should_return_0(void)
{
    uint8_t raw[6] = {0xBE, 0xEF, 0x92, 0x12, 0x34, 0x37};

    SHT31_StartMeasurement_Expect();
    delay_ms_Expect(15);
    SHT31_ReadRaw_Expect(raw);

    uint8_t status = SHT31_ReadRaw_CRC(raw);

    TEST_ASSERT_EQUAL_UINT8(0, status);
}

void test_readraw_crc_fail_temp_should_return_1(void)
{
    uint8_t raw[6] = {0xBE, 0xEF, 0x00, 0x12, 0x34, 0x37};

    SHT31_StartMeasurement_Expect();
    delay_ms_Expect(15);
    SHT31_ReadRaw_Expect(raw);

    uint8_t status = SHT31_ReadRaw_CRC(raw);

    TEST_ASSERT_EQUAL_UINT8(1, status);
}

void test_readraw_crc_fail_humidity_should_return_1(void)
{
    uint8_t raw[6] = {0xBE, 0xEF, 0x92, 0x12, 0x34, 0x00};

    SHT31_StartMeasurement_Expect();
    delay_ms_Expect(15);
    SHT31_ReadRaw_Expect(raw);

    uint8_t status = SHT31_ReadRaw_CRC(raw);

    TEST_ASSERT_EQUAL_UINT8(1, status);
}

void test_readraw_crc_both_fail_temp_humidity_should_return_1(void)
{
    uint8_t raw[6] = {0xBE, 0xEF, 0x11, 0x12, 0x34, 0x00};

    SHT31_StartMeasurement_Expect();
    delay_ms_Expect(15);
    SHT31_ReadRaw_Expect(raw);

    uint8_t status = SHT31_ReadRaw_CRC(raw);

    TEST_ASSERT_EQUAL_UINT8(1, status);
}