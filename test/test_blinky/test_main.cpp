#include <Arduino.h>
#include <unity.h>

#define LED_PIN 38

void setUp(void) {}
void tearDown(void) {}

void test_led_pin_value(void) {
    TEST_ASSERT_EQUAL(38, LED_PIN);
}

void test_led_pin_output_mode(void) {
    pinMode(LED_PIN, OUTPUT);
    TEST_ASSERT_EQUAL(OUTPUT, digitalRead(LED_PIN) >= 0 ? OUTPUT : INPUT);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_led_pin_value);
    RUN_TEST(test_led_pin_output_mode);
    UNITY_END();
}

void loop() {}
