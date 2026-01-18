import unittest
import sys
import os
import time
import math

from gpioPins import DigitalInput, DigitalOutput, AnalogOutput
from CANMessage import CanMessage
from CANPi import writeOut
from MotorInterfaceTest import MotorInterfaceTest


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

    def test_throttle(self):
        def expected_throttle_value(voltage):
            THROTTLE_LOW = 0.82
            THROTTLE_HIGH = 3.3
            THROTTLE_DIFF = THROTTLE_HIGH - THROTTLE_LOW
            
            if voltage <= THROTTLE_LOW:
                raw_value = 0
            elif voltage >= THROTTLE_HIGH:
                raw_value = 256
            else:
                adjusted = (voltage - THROTTLE_LOW) / THROTTLE_DIFF
                raw_value = math.floor(adjusted * 256.0)
            
            norm_value = raw_value / 256.0
            return norm_value, raw_value

        motor_interface =  MotorInterfaceTest()
        # Use THROTTLE_WIPER (PA_6) mapped in server_config.json
        throttle_pin = AnalogOutput("THROTTLE_WIPER") 
        
        testing_voltages = [0.5, 1.5, 3.0]
        
        for tv in testing_voltages:
            throttle_pin.write_voltage(tv)
            time.sleep(0.5)
            exp_norm, exp_raw = expected_throttle_value(tv)
            norm, raw = motor_interface.get_throttle(), motor_interface.get_throttle_raw()
            
            self.assertAlmostEqual(exp_norm, norm, delta=0.05, 
                msg=f"Throttle Norm failed at {tv}V. Exp: {exp_norm}, Got: {norm}")
            self.assertAlmostEqual(exp_raw, raw, delta=1.0, 
                msg=f"Throttle Raw failed at {tv}V. Exp: {exp_raw}, Got: {raw}")

    def test_regen(self):
        # Regen logic from main.cpp
        def expected_regen_from_throttle(voltage):
            THROTTLE_LOW = 0.82
            THROTTLE_HIGH = 3.3
            THROTTLE_DIFF = THROTTLE_HIGH - THROTTLE_LOW
            
            if voltage <= THROTTLE_LOW:
                raw_value = 0
            elif voltage >= THROTTLE_HIGH:
                raw_value = 256
            else:
                adjusted = (voltage - THROTTLE_LOW) / THROTTLE_DIFF
                raw_value = math.floor(adjusted * 256.0)

            if raw_value <= 50:
                val = 79.159 * math.pow(50 - raw_value, 0.3)
                norm = val / 256.0
                return norm, val
            return 0.0, 0.0

        motor_interface = MotorInterfaceTest()
        throttle_pin = AnalogOutput("THROTTLE_WIPER")

        # Rivanna3.dbc has ID 768 or hex 0x300 for DashboardCommands 
        cmd_msg = CanMessage(name="DashboardCommands", id=0x300, signals={"regen_en": 1}, timestamp=time.time())
        writeOut(cmd_msg)
        time.sleep(0.1) 

        testing_voltages = [0.85] # 0.85V => ~3 raw => High Regen
        for tv in testing_voltages:
            throttle_pin.write_voltage(tv)
            time.sleep(0.5)

            exp_norm, exp_raw = expected_regen_from_throttle(tv)
            norm, raw = motor_interface.get_regen(), motor_interface.get_regen_raw()
            
            self.assertAlmostEqual(exp_norm, norm, delta=0.05, 
                msg=f"Regen Norm failed at {tv}V. Exp: {exp_norm}, Got: {norm}")
            self.assertAlmostEqual(exp_raw, raw, delta=1.0, 
                msg=f"Regen Raw failed at {tv}V. Exp: {exp_raw}, Got: {raw}")
