""" Main programm entry point"""

# MIT License
#
# Copyright (c) 2023 Gabryel Reyes <gabryelrdiaz@gmail.com>
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
from struct import Struct
import keyboard  # pylint: disable=import-error
import numpy as np
from SerialMuxProt import SerialMuxProt
# from socket_client import SocketClient
from serial_client import SerialClient

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


def callback_line_sensors(payload: bytearray) -> None:
    """ Callback of LINE_SENS Channel """
    unpacker = Struct(">HHHHH")
    data = unpacker.unpack_from(payload)
    print(np.array(data, dtype=np.int16))


def callback_timestamp(payload: bytearray) -> None:
    """ Callback of TIMESTAMP Channel """

    print(payload.hex())

    # unpacker = Struct(">HH")
    # data = unpacker.unpack_from(payload)
    # print(np.array(data, dtype=np.int16))


def callback_remote_response(payload: bytearray) -> None:
    """ Callback of REMOTE_CMD Channel """
    if payload == b'\x00':
        print("Command OK")
    elif payload == b'\x01':
        print("Command Pending")
    elif payload == b'\x02':
        print("Command Error")


def send_motor_setpoints(set_point_left: int, set_point_right: int):
    """
    Send Motor Setpoints
    """
    payload = bytearray()
    payload.append(0x01)
    payload.append(0x00)
    payload.append(0x00)
    payload.append(0x00)

    if len(payload) == 4:
        smp_server.send_data("COUNTER", payload)


def send_command(command: str) -> None:
    """Send command to RadonUlzer"""

    payload = bytearray()

    if command == "line_calib":
        payload.append(0x01)
    elif command == "motor_calib":
        payload.append(0x02)
    elif command == "enable_drive":
        payload.append(0x03)

    smp_server.send_data("REMOTE_CMD", payload)


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

    # smp_server.create_channel("MOT_SPEEDS", 4)
    # smp_server.create_channel("REMOTE_CMD", 1)
    # smp_server.subscribe_to_channel("REMOTE_RSP", callback_remote_response)
    # smp_server.subscribe_to_channel("LINE_SENS", callback_line_sensors)
    smp_server.subscribe_to_channel("TIMESTAMP", callback_timestamp)
    smp_server.create_channel("COUNTER", 4)

    keyboard.on_press_key("w", lambda e: send_motor_setpoints(0x8000, 0x8000))
    # keyboard.on_press_key("s", lambda e: send_motor_setpoints(0x7FFF, 0x7FFF))
    # keyboard.on_press_key("a", lambda e: send_motor_setpoints(0x7FFF, 0x8000))
    # keyboard.on_press_key("d", lambda e: send_motor_setpoints(0x8000, 0x7FFF))
    # keyboard.on_release_key(
    #     "w", lambda e: send_motor_setpoints(0x0000, 0x0000))
    # keyboard.on_release_key(
    #     "a", lambda e: send_motor_setpoints(0x0000, 0x0000))
    # keyboard.on_release_key(
    #     "s", lambda e: send_motor_setpoints(0x0000, 0x0000))
    # keyboard.on_release_key(
    #     "d", lambda e: send_motor_setpoints(0x0000, 0x0000))
    # keyboard.on_press_key("l", lambda e: send_command("line_calib"))
    # keyboard.on_press_key("m", lambda e: send_command("motor_calib"))
    # keyboard.on_press_key("e", lambda e: send_command("enable_drive"))

    while True:
        if (millis() - last_time) >= 5:
            last_time = millis()
            smp_server.process(millis())

################################################################################
# Main
################################################################################


if __name__ == "__main__":
    sys.exit(main())
