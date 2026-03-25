#ifndef TRAFFIC_CONTROLLER_H
#define TRAFFIC_CONTROLLER_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Traffic Light States
enum TrafficLightState : uint8_t {
    TL_RED = 0,
    TL_RED_YELLOW = 1,
    TL_GREEN = 2,
    TL_YELLOW = 3
};

enum TrafficControlMode : uint8_t {
    CONTROL_AUTOMATIC = 0,
    CONTROL_MANUAL = 1,
    CONTROL_TEST_ON_BUTTON = 2
};

// Mode: 0 = German (R-RY-G-Y-R), 1 = Dutch (R-G-Y-R)
typedef struct {
    TrafficLightState state1;
    TrafficLightState state2;
    uint8_t mode;          // 0 = German, 1 = Dutch
    uint8_t controlMode;   // 0 = Automatic, 1 = Manual, 2 = Automatic with test while button is pressed
    uint8_t phaseIndex;    // current state step in the active sequence
    SemaphoreHandle_t mutex;
} TrafficControllerData;

// Initialize the traffic controller
void initTrafficController(uint8_t mode, uint8_t controlMode);

// Get controller data
TrafficControllerData* getControllerData();

// Controller task
void controllerTask(void *pvParameters);

// Circuit test task and software interface
void circuitTestTask(void *pvParameters);
void MyCircuitTest_Enable(void);
void MyCircuitTest_Disable(void);

// Traffic light tasks
void trafficLight1Task(void *pvParameters);
void trafficLight2Task(void *pvParameters);

// Start all tasks
void startTrafficSystem(uint8_t mode, uint8_t controlMode);

#endif
