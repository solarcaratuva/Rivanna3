#include "mbed.h"
#include "unity.h"
#include "utest.h"

using namespace utest::v1;

void test_led_toggle()
{
    DigitalOut led(LED1);
    led = 1;
    TEST_ASSERT_EQUAL(1, led.read());
    led = 0;
    TEST_ASSERT_EQUAL(0, led.read());
}

utest::v1::status_t test_setup(const size_t number_of_cases)
{
    return greentea_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("Test LED toggle", test_led_toggle)
};

Specification specification(test_setup, cases);

int main()
{
    // Initialize the Greentea test framework
    GREENTEA_SETUP(10, "default_auto");
    
    // Run the test specification
    return !Harness::run(specification);
}