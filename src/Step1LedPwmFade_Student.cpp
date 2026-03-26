/*
 * disclaimer:
 * The implementation choices in this file are made to improve the focus of the given lecture session.
 * They are not universally best design or implementation choices for all projects or production code.
 * Please consider this as a pedagogical scaffold, not a reference for production grade implementation
 */

#include <Arduino.h>

// TODO 1: Assign the correct pin for your LED
const int LED_GPIO_PIN = 38;
// TODO 2: Choose a PWM channel (0-15)
const int LED_PWM_CHANNEL = 0;
// TODO 3: Choose a suitable PWM frequency for LED (e.g., 1000 Hz)
const int LED_PWM_FREQ_HZ = 500;
// TODO 4: Choose a suitable PWM resolution (e.g., 8-bit)
const int LED_PWM_RES = 8;
TaskHandle_t ledTaskHandle = NULL;

// TODO 5: Implement LedPwmInit to configure PWM channel and attach pin
static void LedPwmInit()
{
    ledcSetup(LED_PWM_CHANNEL, LED_PWM_FREQ_HZ, LED_PWM_RES);
    ledcAttachPin(LED_GPIO_PIN, LED_PWM_CHANNEL);
}

// TODO 6: Implement TaskLoop to fade the LED in and out using PWM
static void TaskLoop(void* args)
{
    // TODO 6a: Initialize PWM and resources
    LedPwmInit();
    
    const int maxDutyCycle = (1 << LED_PWM_RES) - 1;
    const int fadeStep = 5;
    const int delayMs = 20;
    
    for(;;)
    {
        // TODO 7: Fade in (increase duty cycle from 0 to max)
        for(int dutyCycle = 0; dutyCycle <= maxDutyCycle; dutyCycle += fadeStep)
        {
            ledcWrite(LED_PWM_CHANNEL, dutyCycle);
            vTaskDelay(pdMS_TO_TICKS(delayMs));
        }
        
        // TODO 8: Fade out (decrease duty cycle from max to 0)
        for(int dutyCycle = maxDutyCycle; dutyCycle >= 0; dutyCycle -= fadeStep)
        {
            ledcWrite(LED_PWM_CHANNEL, dutyCycle);
            vTaskDelay(pdMS_TO_TICKS(delayMs));
        }
    }
}

// TODO 0: Implement Step1LedPwm_CreateTask to create the FreeRTOS task for TaskLoop
bool Step1LedPwm_CreateTask(void)
{
    bool result = false;
    BaseType_t taskCreateResult = xTaskCreate(
        TaskLoop,
        "LedFadeTask",
        2048,
        NULL,
        1,
        &ledTaskHandle
    );
    result = (taskCreateResult == pdPASS);
    return result;
}