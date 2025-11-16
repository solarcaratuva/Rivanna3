import unittest
import time
import os
import sys

# Absolute path to the correct Testing_Library directory
testing_lib_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "HiL_Testing", "Testing_Library"))
sys.path.insert(0, testing_lib_path)

from gpioPins import reset_nucleo


class TestPowerBoardBasic(unittest.TestCase):

    def setUp(self):
        # reset Nucleo before each test
        reset_nucleo()

        # clear CAN messages
        self.bus = CANBus()
        for msg in self.bus.CANBusDict.keys():
            # reset can queue
            self.bus.clearReceivedMessages(msg)

        # Give firmware time to reboot
        time.sleep(0.2)

    def system_check(self):
        self.assertEqual(2 + 2, 4)

