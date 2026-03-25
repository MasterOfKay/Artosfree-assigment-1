#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "traffic-controller.h"

// Traffic Light 1
#define TL1_RED 40
#define TL1_YELLOW 39
#define TL1_GREEN 38

// Traffic Light 2
#define TL2_RED 16
#define TL2_YELLOW 15
#define TL2_GREEN 12

// #define MANUAL_BUTTON_PIN 41
#define MANUAL_BUTTON_PIN 0

#define CIRCUIT_TEST_STEP_MS 300

// Global controller data
static TrafficControllerData controllerData = {TL_RED, TL_RED, 0, CONTROL_AUTOMATIC, 0, NULL};
static volatile bool circuitTestEnabled = false;

static uint8_t getPhaseCount(uint8_t mode) {
    // German: R -> RY -> G -> Y -> R for each direction (5 phases)
    // Dutch:  R -> G -> Y -> R for each direction (4 phases)
    return (mode == 0) ? 5 : 4;
}

static uint32_t getPhaseDurationMs(uint8_t mode, uint8_t phaseIndex) {
    if (mode == 0) {
        // German mode
        static const uint32_t germanDurationsMs[5] = {4000, 2000, 4000, 2000, 2000};
        return germanDurationsMs[phaseIndex % 5];
    }

    // Dutch mode
    static const uint32_t dutchDurationsMs[4] = {3000, 2000, 3000, 2000};
    return dutchDurationsMs[phaseIndex % 4];
}

static void setStatesForPhase(uint8_t mode, uint8_t phaseIndex) {
    if (mode == 0) {
        switch (phaseIndex % 5) {
            case 0:
                controllerData.state1 = TL_RED;
                controllerData.state2 = TL_GREEN;
                break;
            case 1:
                controllerData.state1 = TL_RED_YELLOW;
                controllerData.state2 = TL_GREEN;
                break;
            case 2:
                controllerData.state1 = TL_GREEN;
                controllerData.state2 = TL_RED;
                break;
            case 3:
                controllerData.state1 = TL_YELLOW;
                controllerData.state2 = TL_RED;
                break;
            default:
                controllerData.state1 = TL_RED;
                controllerData.state2 = TL_RED_YELLOW;
                break;
        }
        return;
    }

    switch (phaseIndex % 4) {
        case 0:
            controllerData.state1 = TL_RED;
            controllerData.state2 = TL_GREEN;
            break;
        case 1:
            controllerData.state1 = TL_RED;
            controllerData.state2 = TL_YELLOW;
            break;
        case 2:
            controllerData.state1 = TL_GREEN;
            controllerData.state2 = TL_RED;
            break;
        default:
            controllerData.state1 = TL_YELLOW;
            controllerData.state2 = TL_RED;
            break;
    }
}

void initTrafficController(uint8_t mode, uint8_t controlMode) {
    controllerData.state1 = TL_RED;
    controllerData.state2 = TL_RED;
    controllerData.mode = mode;
    controllerData.controlMode = controlMode;
    controllerData.phaseIndex = 0;
    controllerData.mutex = xSemaphoreCreateMutex();
}

TrafficControllerData* getControllerData() {
    return &controllerData;
}

void MyCircuitTest_Enable(void) {
    if (xSemaphoreTake(controllerData.mutex, portMAX_DELAY)) {
        circuitTestEnabled = true;
        xSemaphoreGive(controllerData.mutex);
    }
}

void MyCircuitTest_Disable(void) {
    if (xSemaphoreTake(controllerData.mutex, portMAX_DELAY)) {
        circuitTestEnabled = false;
        setStatesForPhase(controllerData.mode, controllerData.phaseIndex);
        xSemaphoreGive(controllerData.mutex);
    }
}

// Set LED state
void setTrafficLightLED(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin, TrafficLightState state) {
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);

    switch (state) {
        case TL_RED:
            digitalWrite(redPin, HIGH);
            break;
        case TL_RED_YELLOW:
            digitalWrite(redPin, HIGH);
            digitalWrite(yellowPin, HIGH);
            break;
        case TL_GREEN:
            digitalWrite(greenPin, HIGH);
            break;
        case TL_YELLOW:
            digitalWrite(yellowPin, HIGH);
            break;
    }
}

// Controller task - manages state transitions
void controllerTask(void *pvParameters) {
    bool stepHandledForCurrentPress = false;
    bool testHandledForCurrentPress = false;

    // Idle state is HIGH, button press pulls pin to LOW.
    pinMode(MANUAL_BUTTON_PIN, INPUT_PULLUP);

    if (xSemaphoreTake(controllerData.mutex, portMAX_DELAY)) {
        setStatesForPhase(controllerData.mode, controllerData.phaseIndex);
        xSemaphoreGive(controllerData.mutex);
    }

    while (true) {
        if (controllerData.controlMode == CONTROL_TEST_ON_BUTTON) {
            bool isPressed = (digitalRead(MANUAL_BUTTON_PIN) == LOW);

            if (isPressed && !testHandledForCurrentPress) {
                vTaskDelay(pdMS_TO_TICKS(30));
                if (digitalRead(MANUAL_BUTTON_PIN) == LOW) {
                    MyCircuitTest_Enable();
                    testHandledForCurrentPress = true;
                }
            }

            if (!isPressed && testHandledForCurrentPress) {
                MyCircuitTest_Disable();
                testHandledForCurrentPress = false;
            }

            if (circuitTestEnabled) {
                vTaskDelay(pdMS_TO_TICKS(20));
                continue;
            }
        }

        // In automatic/manual modes, keep circuit test disabled so normal button logic is never blocked.
        if (controllerData.controlMode != CONTROL_TEST_ON_BUTTON && circuitTestEnabled) {
            MyCircuitTest_Disable();
        }

        if (controllerData.controlMode == CONTROL_AUTOMATIC || controllerData.controlMode == CONTROL_TEST_ON_BUTTON) {
            uint8_t localMode;
            uint8_t localPhase;

            if (xSemaphoreTake(controllerData.mutex, portMAX_DELAY)) {
                localMode = controllerData.mode;
                localPhase = controllerData.phaseIndex;
                setStatesForPhase(localMode, localPhase);
                xSemaphoreGive(controllerData.mutex);
            }

            vTaskDelay(pdMS_TO_TICKS(getPhaseDurationMs(localMode, localPhase)));

            if (xSemaphoreTake(controllerData.mutex, portMAX_DELAY)) {
                controllerData.phaseIndex = (controllerData.phaseIndex + 1) % getPhaseCount(controllerData.mode);
                xSemaphoreGive(controllerData.mutex);
            }
        } else {
            bool isPressed = (digitalRead(MANUAL_BUTTON_PIN) == LOW);

            // Advance exactly one phase on press, even if press is longer held.
            if (isPressed && !stepHandledForCurrentPress) {
                vTaskDelay(pdMS_TO_TICKS(30));
                if (digitalRead(MANUAL_BUTTON_PIN) == LOW) {
                    if (xSemaphoreTake(controllerData.mutex, portMAX_DELAY)) {
                        controllerData.phaseIndex = (controllerData.phaseIndex + 1) % getPhaseCount(controllerData.mode);
                        setStatesForPhase(controllerData.mode, controllerData.phaseIndex);
                        xSemaphoreGive(controllerData.mutex);
                    }
                    stepHandledForCurrentPress = true;
                }
            }

            // Must fully release before another step can happen.
            if (!isPressed && stepHandledForCurrentPress) {
                stepHandledForCurrentPress = false;
            }

            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}

// Circuit test task - when enabled, toggles a visible LED pattern.
void circuitTestTask(void *pvParameters) {
    static const TrafficLightState pattern1[] = {TL_RED, TL_YELLOW, TL_GREEN, TL_RED_YELLOW};
    static const TrafficLightState pattern2[] = {TL_GREEN, TL_RED_YELLOW, TL_RED, TL_YELLOW};
    uint8_t step = 0;

    while (true) {
        if (circuitTestEnabled) {
            if (xSemaphoreTake(controllerData.mutex, portMAX_DELAY)) {
                controllerData.state1 = pattern1[step % 4];
                controllerData.state2 = pattern2[step % 4];
                xSemaphoreGive(controllerData.mutex);
            }
            step++;
            vTaskDelay(pdMS_TO_TICKS(CIRCUIT_TEST_STEP_MS));
        } else {
            step = 0;
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}

// Traffic Light 1 Task
void trafficLight1Task(void *pvParameters) {
    pinMode(TL1_RED, OUTPUT);
    pinMode(TL1_YELLOW, OUTPUT);
    pinMode(TL1_GREEN, OUTPUT);

    while (true) {
        if (xSemaphoreTake(controllerData.mutex, portMAX_DELAY)) {
            setTrafficLightLED(TL1_RED, TL1_YELLOW, TL1_GREEN, controllerData.state1);
            xSemaphoreGive(controllerData.mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Traffic Light 2 Task
void trafficLight2Task(void *pvParameters) {
    pinMode(TL2_RED, OUTPUT);
    pinMode(TL2_YELLOW, OUTPUT);
    pinMode(TL2_GREEN, OUTPUT);

    while (true) {
        if (xSemaphoreTake(controllerData.mutex, portMAX_DELAY)) {
            setTrafficLightLED(TL2_RED, TL2_YELLOW, TL2_GREEN, controllerData.state2);
            xSemaphoreGive(controllerData.mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void startTrafficSystem(uint8_t mode, uint8_t controlMode) {
    initTrafficController(mode, controlMode);
    MyCircuitTest_Disable();
    
    xTaskCreate(
        controllerTask,
        "ControllerTask",
        2048,
        NULL,
        2,
        NULL
    );

    xTaskCreate(
        trafficLight1Task,
        "TrafficLight1Task",
        2048,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        trafficLight2Task,
        "TrafficLight2Task",
        2048,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        circuitTestTask,
        "CircuitTestTask",
        2048,
        NULL,
        1,
        NULL
    );
}
