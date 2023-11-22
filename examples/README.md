# Example

The following example is written for testing the SerialMuxProt with two microcontroller boards, in this case, the ESP32 was used. Each board shall have an LED, either in the LED_BUILTIN Pin, or if its none on-board, on the Pin 13. This can be changed in the compiler switches in each of the `main.cpp` files.

## Installation

- Install the library, depending on your IDE.
- Compile `main.cpp` from ServerA and flash it onto the first board.
- Compile `main.cpp` from ServerB and flash it onto the second board.
- Connect the UART peripherals of both boards with each other:
    - Rx from Board 1 with Tx from Board 2
    - Tx from Board 1 with Rx from Board 2
    - GND from Board 1 with GND from Board 2
- The LEDs should be synchronized.

## ServerA

- ServerA acts as a host, which toggles its own LED and send a command to ServerB to toggle their LED.
- ServerA creates the channel "LED" defined in SerialMuxChannels.h

## ServerB

- ServerB acts as a client, where its LED mirrors the LED from ServerA
- ServerB subscribes to the channel "LED" defined in SerialMuxChannels.h
