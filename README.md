# CRJ‑200 Autopilot Panel – STM32 Interface

This project implements the **hardware interface layer** for a custom CRJ‑200 autopilot panel, using an **STM32F4 microcontroller** to send and receive data with the **X‑Plane 11** flight simulator.  

It handles **buttons, encoders, LEDs, data packing/unpacking, debouncing, and UART communication** to create a realistic autopilot hardware module.

The main firmware logic is located in [`main.c`](./Core/Src/main.c).

---

## Overview

The STM32 performs three primary functions:

1. **Send hardware input events** (buttons and rotary encoders) to X‑Plane via UART.  
2. **Receive LED state data** from X‑Plane to update panel indicators.  
3. **Handle all timing, debouncing, event queuing, and communication** in real time.

The communication protocol uses a **custom 8‑bit packed data format** to ensure fast, low‑bandwidth operation.

> This firmware is designed for efficiency, reliability, and stable performance in an avionics‑style embedded system.

---

## Architecture Summary

### **Input Handling**
- **17 physical buttons** (pull‑up inputs)
- **6 rotary encoders** using:
  - EXTI interrupts on rising edges
  - Debouncing via `debounceStates[]`
  - Direction detection (A/B channel sampling)
- Inputs are transformed into small event packets and pushed into a **linked‑list queue**.

### **Event Queue**
A custom **dynamic linked list queue** stores outgoing actions:

```c
typedef struct LinkedListNode {
    struct LinkedListNode* ptrNextNode;
    uint8_t index;
    uint8_t sendData;
    uint8_t element;
} LinkedListNodeDef;
```

### UART Communication

#### **Transmit Path**
1. A button press or encoder step is detected  
2. Event is encoded using `transformData()`  
3. Data is transmitted via `HAL_UART_Transmit_IT()` every `SEND_DELAY` ms  
4. The processed queue node is freed after transmission

#### **Receive Path**
- X‑Plane sends a 16‑bit variable containing **12 LED states**  
- `updateLEDOutput()` updates only the LEDs whose state actually changed  
- UART reception is fully **non‑blocking** to avoid delays in the control loop

---

## LED Management

- Controls **12 autopilot LEDs** across multiple GPIO ports  
- Uses bitwise XOR masking to detect changed LED bits  
- Updates only LEDs that differ from the previous state  
- Initializes the panel with **all LEDs OFF** using `turnOffLED()`

---

## Features

- **Rotary Encoder Support**: CRS1, CRS2, SPD, HDG, ALT, VS — all with direction‑detection logic  
- **17 Button Inputs**: Debounced, state‑change detection, clean event generation  
- **12 LED Outputs**: Efficient delta‑update logic avoids redundant writes  
- **Bidirectional UART Communication** with the X‑Plane plugin  
- **8‑bit Data Packing** using bitwise operations:  
  - **5 bits** → element index  
  - **1 bit** → action/value  
  - **2 bits** → element type (button/encoder)  

- **Reliable Event Queue** implemented using a dynamic linked list  
- **Non‑Blocking I/O** using EXTI + interrupt‑driven UART  
- **Clean, Modular Codebase** designed for avionics‑style reliability

## 📸 Media


## 📬 Contact
If you have any questions:
📧 badillouribeguillermoca@gmail.com

## 📄 License
This repository is licensed under the MIT License.

