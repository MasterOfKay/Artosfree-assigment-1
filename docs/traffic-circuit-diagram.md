# Traffic Circuit Diagram (Editable)

## 1) Wiring Overview

- Board: LilyGO ESP32-S3 (T3-S3)
- Logic style: GPIO pin drives lamp plus side directly
- Return path: lamp minus side -> resistor -> GND rail -> ESP32 GND pin

## 2) Pin Mapping From Current Code

- Traffic Light 1
- GPIO 40 -> Red lamp (+)
- GPIO 39 -> Yellow lamp (+)
- GPIO 38 -> Green lamp (+)

- Traffic Light 2
- GPIO 16 -> Red lamp (+)
- GPIO 15 -> Yellow lamp (+)
- GPIO 12 -> Green lamp (+)

- Button input
- GPIO 0 -> Button (INPUT_PULLUP in software)

## 3) Circuit Diagram (Mermaid)

```mermaid
flowchart LR
    subgraph MCU[LilyGO ESP32-S3]
      P40[GPIO 40]
      P39[GPIO 39]
      P38[GPIO 38]
      P16[GPIO 16]
      P15[GPIO 15]
      P12[GPIO 12]
      P0[GPIO 0 Button In]
      GNDM[ESP32 GND Pin]
    end

    subgraph TL1[Traffic Light 1]
      L1R[Red Lamp 1]
      L1Y[Yellow Lamp 1]
      L1G[Green Lamp 1]
    end

    subgraph TL2[Traffic Light 2]
      L2R[Red Lamp 2]
      L2Y[Yellow Lamp 2]
      L2G[Green Lamp 2]
    end

    subgraph GNDRAIL[Ground Rail]
      GND[(Common GND Line)]
    end

    R1[[Resistor]]
    R2[[Resistor]]
    R3[[Resistor]]
    R4[[Resistor]]
    R5[[Resistor]]
    R6[[Resistor]]

    BTN[Push Button]

    P40 --> L1R --> R1 --> GND
    P39 --> L1Y --> R2 --> GND
    P38 --> L1G --> R3 --> GND

    P16 --> L2R --> R4 --> GND
    P15 --> L2Y --> R5 --> GND
    P12 --> L2G --> R6 --> GND

    GNDM --- GND

    P0 --- BTN --- GND
```

## External Button

The External Button was connected on GPIO 42. Sadly it brock. But for this we used the 3.3V rail from the esp for power a resistor to ground and conected the button as a Push-down. I didnt include it in the diagram as i didnt used it in the, but instead i used the internal button on the esp witch is in the diagram included.
