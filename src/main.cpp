#include <Arduino.h>
#include <freertos.h>
#include "traffic-controller.h"
#include "Step1LedPwmFade.h"
#include "Step2PololuMotorControl.h"

// Mode: 0 = German (R-RY-G-Y-R), 1 = Dutch (R-G-Y-R)
#define TRAFFIC_MODE 0
// Control mode: 0 = Automatic, 1 = Manual (button on pin 41), 2 = circuit test while button is pressed
#define CONTROL_MODE 2

void setup() {
    Step2PololuMotor_CreateTask();
    // Step1LedPwm_CreateTask();
    // startTrafficSystem(TRAFFIC_MODE, CONTROL_MODE);
}

void loop() {
    // Traffic system is handled by FreeRTOS tasks
}