""" Stream interface for SerialMuxProt. """

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

from abc import ABC, abstractmethod

################################################################################
# Variables
################################################################################

################################################################################
# Classes
################################################################################


class Stream(ABC):
    """
    Stream Interface for SerialMuxProt.
    """

    @abstractmethod
    def available(self) -> int:
        """ Get the number of bytes available in the stream.

        Returns:
        --------
        Number of bytes available in the stream.
        """

    @abstractmethod
    def read_bytes(self, length: int) -> tuple[int, bytearray]:
        """ Read a number of bytes from the stream.

        Parameters:
        -----------
        length : int
            Number of bytes to read.

        Returns
        ----------
        Tuple:
        - int: Number of bytes received.
        - bytearray: Received data.
        """

    @abstractmethod
    def write(self, payload: bytearray) -> int:
        """
        Write a bytearray to the stream.

        Parameters:
        -----------
        payload: bytearray
            Data to write to the stream.

        Returns:
        --------
        Number of bytes written.
        """

################################################################################
# Functions
################################################################################

################################################################################
# Main
################################################################################
