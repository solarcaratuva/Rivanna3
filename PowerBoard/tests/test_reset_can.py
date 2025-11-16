import unittest
import time
import os
import sys

# Absolute path to the correct Testing_Library directory
testing_lib_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "HiL_Testing", "Testing_Library"))
sys.path.insert(0, testing_lib_path)

from gpioPins import reset_nucleo


class TestPowerBoardBasic(unittest.TestCase):

    def test_setUp(self):
        # reset Nucleo before each test
        reset_nucleo()
        printf("Reset nucleo")

        # clear CAN messages
        self.bus = CANBus()
        for msg in self.bus.CANBusDict.keys():
            # reset can queue
            self.bus.clearReceivedMessages(msg)
        printf("Cleared CAN messages")

        for msg in self.bus.CANBusDict:
            self.assertEqual(len(bus.getReceivedMessages(msg)), 0, f"CAN queue '{msg}' not empty after clearing")


        # Give firmware time to reboot
        time.sleep(0.2)

