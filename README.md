# STM32 Project - CRJ-200 Autopilot Panel Control

## Description

This project uses an STM32 microcontroller to send the data to the X-Plane 11 simulator to control the CRJ-200 autopilot panel. The main code is located in [`main.c`](https://github.com/GuillermoBaUr/CRJ-200_Autopilot_Panel_STM32/blob/main/Core/Src/main.c), where peripherals are configured, interrupts are handled, and inputs and outputs are managed.


## Installation

To install and set up the project, follow these steps:

1. Clone the repository:
   
   ```
   bash git clone https://github.com/GuillermoBaUr/CRJ-200_Autopilot_Panel_STM32
   ```
   
3. Open the project in STM32CubeIDE.

4. Ensure you have STM32CubeMX installed to configure the peripherals.

5. Compile and upload the code to your STM32F4RE11 microcontroller.

## Main Features

- **LED Control**: The information sent via serial port from the simulator is handled to turn the corresponding LED on the panel.
- **Button Reading**: It reads the states of several buttons, taking care of debouncing. Even if the user maintains the button pressed, the information is only sent once.
- **Encoder Reading**: The encoders are read through interrupts, with debouncing handled, and can manage the encoder direction.
- **UART**: Serial communication to send and receive data.
- **Data Structure**: We use a queue to send actions at certain intervals. This queue is implemented with a dynamic linked list, ensuring every action is sent without error.
- **Data Handling**: The information is sent using a single unsigned 8-bit variable, utilizing bitwise operators to pack the data. The structure is: first 5 bits for the element index, next bit for the action, and the last two bits for the element (button or encoder). We also unpack the data sent from the simulator to turn on the LEDs.


## License
This project is licensed under the MIT License. See the LICENSE file for more details.
