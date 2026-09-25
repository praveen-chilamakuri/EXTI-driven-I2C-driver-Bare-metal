#include "unity.h"
#include "system_logic.h"
#include "mock_sht31_readraw_crc.h"

void setUp(void) {}
void tearDown(void) {}

void test_system_process_no_exti_should_do_nothing(void)
{
    uint8_t raw[6] = {0xBE, 0xEF, 0x92, 0x12, 0x34, 0x37};

    uint8_t result = System_Process(0, raw);
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

void test_system_process_exti_crc_ok_should_return_0(void)
{
    uint8_t raw[6] = {0xBE, 0xEF, 0x92, 0x12, 0x34, 0x37};

    SHT31_ReadRaw_CRC_ExpectAndReturn(raw, 0);

    uint8_t result = System_Process(1, raw);
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

void test_system_process_exti_crc_fail_should_return_1(void)
{
    uint8_t raw[6] = {0xBE, 0xEF, 0x00, 0x12, 0x34, 0x37};

    SHT31_ReadRaw_CRC_ExpectAndReturn(raw, 1);

    uint8_t result = System_Process(1, raw);
    TEST_ASSERT_EQUAL_UINT8(1, result);
}
