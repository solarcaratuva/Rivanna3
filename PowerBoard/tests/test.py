import unittest
import sys
import os
import time
import math

from gpioPins import DigitalInput, DigitalOutput, AnalogOutput
from CANMessage import CanMessage
from CANPi import writeOut
from MotorInterfaceTest import MotorInterfaceTest

# I2C Test Mode: 0 = Throttle only, 1 = Regen only, 2 = Both (tests swap)
# Set this to match I2C_TEST_MODE in main.cpp
# Note: The PowerBoard (STM32 Nucleo) is the I2C master and automatically sends
I2C_TEST_MODE = 0


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
        """Test throttle reading. Only runs when I2C_TEST_MODE is 0 or 2."""
        PI_GPIO_VOLTAGE = 3.3
        def voltage_to_duty(voltage: float) -> float:
            return voltage / PI_GPIO_VOLTAGE

        def expected_throttle_value(voltage: float):
            # Derived from PowerBoard/lib/src/ReadPedals.cpp
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

        print(f"\n[I2C_TEST_MODE={I2C_TEST_MODE}] Testing throttle...")
        
        motor_interface =  MotorInterfaceTest()
        # GPIO pin 6 of the Raspberry Pi is mapped to the Throttle Wiper (PA_6)
        # server_config.json --> {nucleo_pin_name_to_number_mapping} --> {PA_6} --> 6
        throttle_pin = AnalogOutput("6") 
        
        testing_voltages = [0.3, 1.2, 2.7]
        i = 1
        while(True):
            if i%2 == 0:
                throttle_pin.write_voltage(0.1)
                time.sleep(0.4)
                throttle_pin.read()
                throttle_pin.off()
            else: 
                throttle_pin.write_voltage(0.9)
                throttle_pin.sleep(0.4)
                throttle_pin.read()
                throttle_pin.off()
            time.sleep(0.4)
            i+=1  
        for i,tv in enumerate(testing_voltages):
            throttle_pin.write_voltage(tv)  
            time.sleep(.4)
            print(f"Voltage for test {i+1} = {throttle_pin.read()}")
            time.sleep(0.5)  # Allow time for PowerBoard to read voltage, send I2C, Arduino to process and send Serial
            exp_norm, exp_raw = expected_throttle_value(tv)
            norm, raw = motor_interface.get_throttle(), motor_interface.get_throttle_raw()
            
            print(f"  Voltage: {tv}V -> Expected: {exp_raw}, Got: {raw if raw is not None else 'None'}")
            self.assertIsNotNone(raw, f"Throttle value is None at {tv}V ")
            self.assertAlmostEqual(exp_norm,norm, delta=0.05, msg=f"Throttle Norm failed at {tv}V. Exp: {exp_norm}, Got: {norm}")
            self.assertAlmostEqual(exp_raw, raw, delta=1.0, msg=f"Throttle Raw failed at {tv}V. Exp: {exp_raw}, Got: {raw}")

    def test_regen(self):
        # Regen logic from main.cpp
        PI_GPIO_VOLTAGE = 3.3
        def voltage_to_duty(voltage: float) -> float:
            return voltage / PI_GPIO_VOLTAGE

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
                # derived from PowerBoard/src/main.cpp regen_drive() function
                val = 79.159 * math.pow(50 - raw_value, 0.3)
                norm = val / 256.0
                return norm, val
            return 0.0, 0.0

        print(f"\n[I2C_TEST_MODE={I2C_TEST_MODE}] Testing regen...")
       
        motor_interface = MotorInterfaceTest()
        throttle_pin = AnalogOutput("6")

        # Rivanna3.dbc has ID 768 or hex 0x300 for DashboardCommands 
        # send regen_en signal to enable regen
        # WriteOut sends via UART to Nucleo
        # look at first 'mbed_serial' in CANPi.py
        cmd_msg = CanMessage(name="DashboardCommands", id=0x300, signals={"regen_en": 1}, timestamp=time.time())
        writeOut(cmd_msg)
        time.sleep(0.1) 

        testing_voltages = [1.75] # 0.85V => ~3 raw => High Regen
        for tv in testing_voltages:
            throttle_pin.write_voltage(tv)
            throttle_pin.on()
            time.sleep(0.5)

            exp_norm, exp_raw = expected_regen_from_throttle(tv)
            norm, raw = motor_interface.get_regen(), motor_interface.get_regen_raw()
            
            print(f"Voltage:{tv}V -> Expected: {exp_raw}, Got: {raw if raw is not None else 'None'}")
            self.assertIsNotNone(raw, f"Regen value is None at {tv}V - check Serial connection, Arduino, and regen_en CAN message")
            self.assertAlmostEqual(exp_norm, norm, delta=0.05, msg=f"Regen Norm failed at {tv}V. Exp: {exp_norm}, Got: {norm}")
            self.assertAlmostEqual(exp_raw, raw, delta=1.0, msg=f"Regen Raw failed at {tv}V. Exp: {exp_raw}, Got: {raw}")
