import unittest
import sys
import os
import time

from gpioPins import DigitalInput, DigitalOutput
from CANMessage import CanMessage
from CANPi import writeOut

class DriverBoardTests(unittest.TestCase):

    def test_turn_signals(self):
        left_turn_flash = DigitalInput("LEFT_TURN_OUT")
        left_turn_signal = DigitalOutput("LEFT_TURN_IN")

        left_turn_signal.write(False)
        time.sleep(1)
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

        #print(f'Flash Count: {flash_count} | Current State: {current_state} | Previous State: {previous_state}')
        self.assertGreaterEqual(flash_count, 2, "LEFT_TURN_OUT did not flash as expected.")
        #self.assertEquals(True,True)
        left_turn_signal.write(False)

    def test_bms_strobe(self):
        name = "BPSError"
        id = 0x106
        signals = {"internal_communications_fault" : 1}
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

    def test_failing_test_right_turn_signal(self):
        #right_turn_flash = DigitalInput("RIGHT_TURN_OUT")
        #right_turn_signal = DigitalOutput("RIGHT_TURN_IN")

        #right_turn_signal.write(False)
        right_turn_signal = False
        
        self.assertTrue(right_turn_signal)
