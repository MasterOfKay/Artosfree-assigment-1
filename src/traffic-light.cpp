#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define GREEN_PIN 38
#define ORANGE_PIN 39
#define RED_PIN 40

void trafficLightTask(void *pvParameters) {
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(ORANGE_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    
    // Initialize all LEDs to OFF
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(ORANGE_PIN, LOW);
    digitalWrite(RED_PIN, LOW);

    for (;;) {
        // GREEN phase - 1 second
        digitalWrite(GREEN_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(1000));
        digitalWrite(GREEN_PIN, LOW);
        
        // ORANGE phase - 1 second
        digitalWrite(ORANGE_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(1000));
        digitalWrite(ORANGE_PIN, LOW);
        
        // RED phase - 3 seconds
        digitalWrite(RED_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(3000));
        digitalWrite(RED_PIN, LOW);
    }
}

void startTrafficLightTask() {
    xTaskCreate(
        trafficLightTask, 
        "TrafficLightTask",  
        2048,                 
        NULL,                 
        1,                 
        NULL                   
    );
}
