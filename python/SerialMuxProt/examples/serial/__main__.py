""" Main programm entry point"""

# MIT License
#
# Copyright (c) 2023 - 2024 Gabryel Reyes <gabryelrdiaz@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#


################################################################################
# Imports
################################################################################

import sys
import time
from serial_client import SerialClient
from SerialMuxProt import SerialMuxProt

################################################################################
# Variables
################################################################################

g_socket = SerialClient("COM3", 115200)
smp_server = SerialMuxProt(10, g_socket)
START_TIME = round(time.time()*1000)

################################################################################
# Classes
################################################################################

################################################################################
# Functions
################################################################################


def get_milliseconds() -> int:
    """ Get current system milliseconds """
    return round(time.time()*1000)


def millis() -> int:
    """ Get current program milliseconds """
    current_time = get_milliseconds()
    return current_time - START_TIME


def callback_timestamp(payload: bytearray) -> None:
    """ Callback of TIMESTAMP Channel """

    print(payload.hex())


def main():
    """The program entry point function.adadadadada
    Returns:
        int: System exit status
    """
    print("Starting System.")
    last_time = 0

    try:
        g_socket.connect_to_server()
    except Exception as err:  # pylint: disable=broad-exception-caught
        print(err)
        return

    smp_server.subscribe_to_channel("LED", callback_timestamp)

    while True:
        if (millis() - last_time) >= 5:
            last_time = millis()
            smp_server.process(millis())

################################################################################
# Main
################################################################################


if __name__ == "__main__":
    sys.exit(main())
