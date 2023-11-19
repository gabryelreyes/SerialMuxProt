# Example

The following example is written for testing the SerialMuxProt both with Python and CPP on a microcontroller.

## Installation

### CPP

- Using PlatformIO and the Arduino Framework, compile the files in the `examples\cpp` folder.
- Flash them onto an ESP32 board of your choice and leave it connected to your computer.
- Note the COM port the board is connected to, as you will need this in the next step.

### Python

- Install the requirements of the Python Script using the `pip install -r requirements.txt` command while inside the `examples\python` folder.
- Write the correct COM port number on the constructor of the SerialClient in `__main__.py`.
- Run `__main__.py` and follow the instructions on the terminal.
