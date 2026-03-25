#include <Arduino.h>
#include <freertos.h>
#include "traffic-controller.h"

// Mode: 0 = German (R-RY-G-Y-R), 1 = Dutch (R-G-Y-R)
#define TRAFFIC_MODE 0
// Control mode: 0 = Automatic, 1 = Manual (button on pin 41), 2 = circuit test while button is pressed
#define CONTROL_MODE 2

void setup() {
    startTrafficSystem(TRAFFIC_MODE, CONTROL_MODE);
}

void loop() {
    // Traffic system is handled by FreeRTOS tasks
}