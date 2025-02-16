import unittest
import sys
import os
import time

module_path = os.path.abspath("../HiL_Testing/Testing_Library")

sys.path.append(module_path)

from gpio import DigitalInput, DigitalOutput
from CANMessage import CanMessage
from CANPi import writeOut

class DriverBoardTests(unittest.TestCase):

    def test_turn_signals(self):
        left_turn_flash = DigitalInput("LEFT_TURN_OUT")
        left_turn_signal = DigitalOutput("LEFT_TURN_IN")

        left_turn_signal.write(True)

        previous_state = left_turn_flash.read()
        flash_count = 0

        start_time = time.time()
        timeout = 5  # Adjust this based on expected flash frequency

        while time.time() - start_time < timeout:
            current_state = left_turn_flash.read()
            if current_state != previous_state:
                flash_count += 1
                previous_state = current_state

            if flash_count >= 2:
                break

            time.sleep(0.5)  # Adjust sample rate based on expected blinking speed

        self.assertGreaterEqual(flash_count, 2, "LEFT_TURN_OUT did not flash as expected.")

    def test_bms_strobe(self):
        name = "BPSError"
        id = 262
        signals = {"internal_communications_fault": 1}
        timestamp = 1.0

        can_message = CanMessage(name, id, signals, timestamp)
        writeOut(can_message)

        bms_strobe = DigitalInput("BMS_STROBE_OUT")

        previous_state = bms_strobe.read()
        flash_count = 0

        start_time = time.time()
        timeout = 5  # Adjust this based on expected flash frequency

        while time.time() - start_time < timeout:
            current_state = bms_strobe.read()
            if current_state != previous_state:
                flash_count += 1
                previous_state = current_state

            if flash_count >= 2:
                break

            time.sleep(0.5)  # Adjust sample rate based on expected blinking speed

        self.assertGreaterEqual(flash_count, 2, "BMS_STROBE_OUT did not flash as expected.")

    def failing_test_right_turn_signal(self):
        right_turn_flash = DigitalInput("RIGHT_TURN_OUT")
        right_turn_signal = DigitalOutput("RIGHT_TURN_IN")

        right_turn_signal.write(False)
        
        self.assertTrue(right_turn_flash)
        