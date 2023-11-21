# Example

The following example is written for testing the SerialMuxProt with two microcontroller boards, in this case, the ESP32 was used. Each board shall have an LED on the Pin 13.

## Installation

- Install the library, depending on your IDE.
- Compile `main.cpp` from SideA and flash it onto the first board.
- Compile `main.cpp` from SideB and flash it onto the second board.
- Connect the UART peripherals of both boards with each other:
    - Rx from Board 1 with Tx from Board 2
    - Tx from Board 1 with Rx from Board 2
    - GND from Board 1 with GND from Board 2
- The LEDs should blink only when the boards are connected
