import unittest
import sys
import os
import time

# module_path = os.path.abspath("solarcar/HiL_Testing/Testing_Library")

# sys.path.append(module_path)
print(sys.path)

from gpioPins import DigitalInput, DigitalOutput

class TestGPIO(unittest.TestCase):

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

        print(f'Flash Count: {flash_count} | Current State: {current_state} | Previous State: {previous_state}')
        self.assertGreaterEqual(flash_count, 2, "LEFT_TURN_OUT did not flash as expected.")
        #self.assertEquals(True,True)
        
test1 = TestGPIO()
test1.test_turn_signals()

        
