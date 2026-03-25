#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "blinky.h"

#define LED_PIN 38

void blinkTask(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);

    for (;;) {
        digitalWrite(LED_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(1000));  // ON for 1 second
        digitalWrite(LED_PIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(1000));  // OFF for 1 second
    }
}

void startBlinkTask() {
    xTaskCreate(
        blinkTask,   // task function
        "BlinkTask", // task name
        2048,        // stack size in bytes
        NULL,        // parameters
        1,           // priority
        NULL         // task handle
    );
}
