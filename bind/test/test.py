"""
Binding test
"""

import sys
import SerialMuxProt


class Printer(SerialMuxProt.Print):
    """A class that implements the Print interface for testing purposes."""

    def __init__(self):
        SerialMuxProt.Print.__init__(self)

    def print(self, value):
        """Print a value without a newline."""
        print(value, end="")

    def println(self, value):
        """Print a value with a newline."""
        print(value)

    def write(self, buffer, length) -> int:
        """Write a buffer of data."""
        print(buffer[:length])
        return length


def main() -> int:
    """The program entry point function.
    Returns:
        int: System exit status
    """

    printer = Printer()
    SerialMuxProt.test_printer(printer)

    # Expected Output:

    # Hello123456World
    # 7
    # 8
    # 9
    # 10
    # 11
    # 12
    #
    # b'ABCDE'
    # All tests passed!

    return 0


if __name__ == "__main__":
    sys.exit(main())
