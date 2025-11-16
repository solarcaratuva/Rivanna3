import unittest
import time

from Testing_Library.gpiopins import reset_nucleo
from Testing_Library.canbus import CANBus


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

