/**
 * @file ReadPedals.cpp
 * @brief Implements functions to read and scale throttle and brake pedal voltages.
 *
 * This file provides functions to read analog voltages from throttle and brake pedals,
 * normalize them based on predefined minimum and maximum voltage thresholds, and scale
 * the normalized values to a range of [0, 256] as 16-bit unsigned integers.
 *
 * Macros:
 * - THROTTLE_VOLTAGE_LOW: Minimum voltage corresponding to 0% throttle.
 * - THROTTLE_VOLTAGE_HIGH: Maximum voltage corresponding to 100% throttle.
 * - THROTTLR_VOLTAGE_DIFFERENCE: Voltage range for throttle pedal.
 * - BRAKE_VOLTAGE_LOW: Minimum voltage corresponding to 0% brake.
 * - BRAKE_VOLTAGE_HIGH: Maximum voltage corresponding to 100% brake.
 * - BRAKE_VOLTAGE_DIFFERENCE: Voltage range for brake pedal.
 *
 * Functions:
 * - uint16_t read_throttle(): Reads and scales the throttle pedal voltage.
 * - uint16_t read_brake(): Reads and scales the brake pedal voltage.
 *
 * The scaling ensures that voltages below the minimum threshold return 0,
 * voltages above the maximum threshold return 256, and intermediate voltages
 * are linearly mapped to the range [0, 256].
 *
 * @note 5V analog inputs are scaled down to 3.3V using a voltage divider.
 * @note The inputs are read using the `throttle_pedal` and `brake_pedal` objects,
 *       which are assumed to be instances of a class that provides a `read_voltage()` method.
 * @note The 'read_voltage()' method is expected to return the voltage from 0V to 5V using the reference of 5V defined in main.cpp.
 */
#include <mbed.h>
#include <rtos.h>
#include "main.h"
#include "ReadPedals.h"
#include "log.h"

/**
* Input: Throttle
* - Source Voltage Reference: 5V
* - Voltage Divider: 3.3V
* - Measurement date: 6/21/2025
    * - Source Max Voltage: __V
    * - Source Min Voltage: __V
    * - STM Pin Max Voltage: 3.31V
    * - STM Pin Min Voltage: 0.83V
* - Analog Object Reference Voltage: 5V
*/
#define THROTTLE_VOLTAGE_LOW                 1.24 // voltage at 0% throttle
#define THROTTLE_VOLTAGE_HIGH                5 // voltage at 100% throttle
#define THROTTLR_VOLTAGE_DIFFERENCE          (THROTTLE_VOLTAGE_HIGH - THROTTLE_VOLTAGE_LOW) // voltage range


/**
* Input: Brake
* - Source Voltage Reference: 5V
* - Voltage Divider: 3.3V
* - Measurement date: 6/21/2025
    * - Source Max Voltage: __V
    * - Source Min Voltage: __V
    * - STM Pin Max Voltage: 3.31V
    * - STM Pin Min Voltage: 0V
* - Analog Object Reference Voltage: 5V
*/
#define BRAKE_VOLTAGE_LOW                    1.56 // voltage at 0% brake
#define BRAKE_VOLTAGE_HIGH                   2.66 // voltage at 100% brake
#define BRAKE_VOLTAGE_DIFFERENCE             (BRAKE_VOLTAGE_HIGH - BRAKE_VOLTAGE_LOW) // voltage range




/**
 * @brief Reads and processes the throttle pedal input voltage, returning a scaled value.
 *
 * This function reads the voltage from the throttle pedal, adjusts it based on predefined
 * minimum and maximum voltage values, and scales the result to a 16-bit unsigned integer range [0, 256].
 * - If the adjusted input is less than or equal to 0, returns 0.
 * - If the adjusted input is greater than or equal to 1, returns 256.
 * - Otherwise, returns the scaled value.
 *
 * @return uint16_t Scaled throttle value in the range [0, 256].
 */
uint16_t read_throttle() {
    float adjusted_throttle_input = (throttle_pedal.read_voltage() - THROTTLE_VOLTAGE_LOW) / THROTTLR_VOLTAGE_DIFFERENCE;
    if (adjusted_throttle_input <= 0.0f) {
        return 0;
    } else if (adjusted_throttle_input >= 1.0f) {
        return 256;
    } else {
        return (uint16_t)(adjusted_throttle_input * 256.0);
    }
}

/**
 * @brief Reads and processes the brake pedal input voltage, returning a scaled value.
 *
 * This function reads the voltage from the brake pedal, adjusts it based on predefined
 * minimum and maximum voltage values, and scales the result to a 16-bit unsigned integer range [0, 256].
 * - If the adjusted input is less than or equal to 0, returns 0.
 * - If the adjusted input is greater than or equal to 1, returns 256.
 * - Otherwise, returns the scaled value.
 *
 * @return uint16_t Scaled brake value in the range [0, 256].
 */
uint16_t read_brake() {
    float adjusted_brake_input = (brake_pedal.read_voltage() - BRAKE_VOLTAGE_LOW) / BRAKE_VOLTAGE_DIFFERENCE;
    if (adjusted_brake_input <= 0.0f) { 
        return 0;
    } else if (adjusted_brake_input >= 1.0f) { 
        return 256;
    } else {
        return (uint16_t)(adjusted_brake_input * 256.0);
    }
}