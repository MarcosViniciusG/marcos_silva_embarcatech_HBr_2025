#include "unity.h"
#include "temperature.h"

void setUp(void) {
}

void tearDown(void) {
    
}

// 876 ~= 0,706V
void test_adc_to_celsius_should_return_correct_values(void)
{
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 27.0, (adc_to_celsius(876)));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_adc_to_celsius_should_return_correct_values);
    return UNITY_END();
}