# Traffic System UML

This diagram is generated from the current codebase implementation.

## 1) Class/Responsibility View

```mermaid
classDiagram
    class Main {
      +setup()
      +loop()
    }

    class TrafficControllerData {
      +TrafficLightState state1
      +TrafficLightState state2
      +uint8_t mode
      +uint8_t controlMode
      +uint8_t phaseIndex
      +SemaphoreHandle_t mutex
    }

    class TrafficController {
      +initTrafficController(mode, controlMode)
      +startTrafficSystem(mode, controlMode)
      +getControllerData()
      +MyCircuitTest_Enable()
      +MyCircuitTest_Disable()
      +setTrafficLightLED(redPin, yellowPin, greenPin, state)
      +controllerTask(void*)
      +trafficLight1Task(void*)
      +trafficLight2Task(void*)
      +circuitTestTask(void*)
    }

    class GPIO {
      +digitalWrite(pin, value)
      +digitalRead(pin)
      +pinMode(pin, mode)
    }

    class FreeRTOS {
      +xTaskCreate(...)
      +vTaskDelay(...)
      +xSemaphoreTake(...)
      +xSemaphoreGive(...)
    }

    Main --> TrafficController : calls startTrafficSystem()
    TrafficController --> TrafficControllerData : reads/writes shared state
    TrafficController --> GPIO : controls and reads pins
    TrafficController --> FreeRTOS : tasks + synchronization
```

## 2) Port-to-Light Interaction View

```mermaid
flowchart LR
    subgraph MCU[ESP32-S3 Controller]
      CT[controllerTask]
      T1[trafficLight1Task]
      T2[trafficLight2Task]
      TT[circuitTestTask]
      BTN[digitalRead BUTTON]
    end

    subgraph Inputs
      B0[GPIO 0/ GPIO 42\nManual/Test Button\nINPUT_PULLUP]
    end

    subgraph TrafficLight1[Traffic Light 1]
      L1R[GPIO 40 -> TL1 RED]
      L1Y[GPIO 39 -> TL1 YELLOW]
      L1G[GPIO 38 -> TL1 GREEN]
    end

    subgraph TrafficLight2[Traffic Light 2]
      L2R[GPIO 16 -> TL2 RED]
      L2Y[GPIO 15 -> TL2 YELLOW]
      L2G[GPIO 12 -> TL2 GREEN]
    end

    B0 --> BTN
    BTN --> CT

    CT -->|updates state1/state2| T1
    CT -->|updates state1/state2| T2
    TT -->|overrides state1/state2 when enabled| T1
    TT -->|overrides state1/state2 when enabled| T2

    T1 --> L1R
    T1 --> L1Y
    T1 --> L1G

    T2 --> L2R
    T2 --> L2Y
    T2 --> L2G
```

## 3) Mode Behavior Summary

- Mode 0: Automatic sequence only.
- Mode 1: Manual stepping on button press.
- Mode 2: Automatic sequence, and while button is held the circuit test task pattern takes control.

## 4) Notes

- Button pin in current code is GPIO 0.
- The previously used GPIO 41 line is currently commented out in source.
