#include "unity.h"
#include "sht31_crc.h"

void setUp(void) {}
void tearDown(void) {}

void test_crc_should_match_known_value_BE_EF(void)
{
    uint8_t data[2] = {0xBE, 0xEF};
    uint8_t crc = SHT31_CRC8(data);
    TEST_ASSERT_EQUAL_HEX8(0x92, crc); 
}

void test_crc_should_match_known_value_12_34(void)
{
    uint8_t data[2] = {0x12, 0x34};
    uint8_t crc = SHT31_CRC8(data);
    TEST_ASSERT_EQUAL_HEX8(0x37, crc); 
}
