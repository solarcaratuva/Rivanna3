# Rivanna3 Development Guide

*This page is the main source for explanations about development decisions, and how implemented logic is meant to work at a high-level, for the Rivanna3 Embedded Codebase*

[Rivanna3 GitHub Link](https://github.com/solarcaratuva/Rivanna3)

## Project Architecture

At a high level, the project is structured into folders as follows:
- Boards: `TelemetryBoard`, and `PowerBoard` hold code for those respective boards
- `Common` contains code that is used across the entire project; a significant portion of it is auto-generated
- MbedOS: `mbed-os` and `TARGET_...` folders contain MbedOS libraries and configuration
- `cmake_build` contains the compiled code from across the project that is actually uploaded to the car

## CAN

[Controller Area Network](https://en.wikipedia.org/wiki/CAN_bus) is a commonly used serial protocol in the automotive industry. We use this as our main mode of data transmission between components of the car for 2 main reasons:

1. Multiple devices can be connected at once. Many serial protocols only support having 2 connected devices at once, like [UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter).
2. All nodes can send and receive messages without central coordination. This removes the need for a 'central' hub node, unlike protocols using a [Master-Slave Topology](https://en.wikipedia.org/wiki/Master%E2%80%93slave_(technology)) like [I2C](https://en.wikipedia.org/wiki/I%C2%B2C).

There are 2 other benefits as well that come with CAN by default, but could be easily replicated and are common:

3. A robust error handling system for when messages fail to send.
4. The hardware specification is built on [differential signaling](https://en.wikipedia.org/wiki/Differential_signalling), which greatly reduces the impact of electrical noise and voltage supply imperfections.

**Terminology**

- Node: any device on the CAN network
- CAN bus: the physical communication network
- CAN network: the high-level concept of the CAN bus with all its nodes attached
- CAN frame: a serialized CAN message
- CAN message: data and identifier, in a readable format
- Signal: a specific piece of data in a CAN message, like a throttle value
- CAN Transceiver: special hardware that converts logic signals to and from the CAN bus's physical specification

**Defining CAN messages**

Each of our CAN messages is *defined* in a DBC file. This file specifies what data a given CAN message will hold and its identifier; CAN messages cannot be defined ad-hoc, they must meet an already defined specification. 

Our Messages are defined [here](https://github.com/solarcaratuva/CAN-messages). We use the Python package [cantools](https://cantools.readthedocs.io/en/latest/) to read and write CAN messages in Python, and to generate the C structs for each message used in the Embedded Codebase. 

**Sending and receiving CAN messages in the Embedded Codebase**

Under `<board name>/lib/src` for each board there is a file `<board name>CANInterface.cpp` that represents the actual CAN connection for each board.
- To send messages: code in `main.cpp` calls the `send()` function, and the message is sent and logged.
- To receive messages: the `message_handler()` function waits for CAN messages. When one is received, the function logs the message, and checks if the message is one of the types of CAN messages it is waiting for. If so, the function calls the appropriate `handle()` function in `main.cpp`; if not, the message is disregarded.

This is further discussed in an [onboarding presentation](https://drive.google.com/file/d/1rphQ6yFSpe3nt5k05yxor46LPvud4lQt/view?usp=sharing). 

**Rivanna3 CAN Networks**

The current CAN networks are as follows:

![CAN Diagram](/assets/images/Embedded/CAN_diagram.png)

- Main CAN - almost everything is attached to this network, when people say 'the CAN network' they mean main CAN
- Motor CAN - connects the motor controller and the PowerBoard
- BMS Config - used solely by the *Power subteam* to configure the battery management system

## Precharge

Precharge is a system used on the high-voltage part of the car's electrical system to prevent [inrush current](https://en.wikipedia.org/wiki/Inrush_current). This section refers to the motor, but also applies to the MPPTs as well. Precharge logic and hardware is managed by the *Power Subteam*; *Embedded* only implements the logic in software.

**Theory**

When the motor is connected to the HV system, the circuit can be modeled as a simple [RC circuit](https://en.wikipedia.org/wiki/RC_circuit), because of the inherent capacitance and resistance of the motor. The time taken to charge the capacitor is proportional to `resistance * capacitance`. By the inherent motor properties, the time constant is small, and the inherent resistance is very small. This results in very high current when the motor is first energized, as the capacitor is charging fast with little resistance; this is known as [inrush current](https://en.wikipedia.org/wiki/Inrush_current), which is bad because it causes extra wear and tear on the motor. 

![Precharge Schematic](/assets/images/Embedded/precharge.png)

The solution: Add another resistor in series to increase the time constant, and therefore reduce the magnitude of current. This resistor is in parallel with a relay (switch) to enable and disable (short it) it, because:
* The resistor should be enabled when first energizing the motor, to reduce inrush current.
* The resistor should be disabled when the motor is in operation, so the motor can quickly draw the current it needs; the time constant must be minimized for the motor to be responsive. 

*Embedded* implements the logic that controls this relay. 

**Normal Logic**

1. Wait until *contactor 12* rising edge. 
2. Wait until *discharge_relay_status* is high (this signal is sent by the BMS over CAN in the *BPSPackInformation* message).
3. Close (enable) *motor precharge enable*.
4. Wait until *discharge RC voltage* is greater than 95% of *pack_voltage* (this signal is sent by the BMS over CAN in the *BPSPackInformation* message).
5. Close (enable) *discharge contactor*, wait a small delay, open (disable) *motor precharge enable*.

Note that the process as described is for motor precharge, but MPPT precharge is the same process, just with *discharge* and *motor* variables replaced with *charge* and *MPPT* variables. Both motor and MPPT precharge processes happen concurrently and independently. 

**Fault Logic**

If at any time (before, during, or after precharge) *contactor 12* has a falling edge or a fault occurs elsewhere, then open (disable) the *charge* and *discharge contactor* and the *motor* and *MPPT precharge enable* relays. 


## Cruise Control

Cruise control is a common car feature that makes the car drive at a constant speed. For Rivanna3, cruise control is implemented using the [PID Algorithm](https://en.wikipedia.org/wiki/Proportional%E2%80%93integral%E2%80%93derivative_controller). PID works by setting the output of the motor based on 3 constants. The PID constants can be found in `PowerBoard/lib/include/CruiseControl.h`. These should be tuned if cruise control is not effectively reaching the target speed. See [PID without a PHD](https://web2.qatar.cmu.edu/~gdicaro/16311-Fall17/slides/PID-without-PhD.pdf) for more information on tuning and PID theory.

**Driver Usage**

The cruise control target speed can be changed by the driver. There are buttons that increase, decrease, and toggle cruise control. Cruise up / down change the target speed by a constant set in `PowerBoard/lib/include/CruiseControl.h`, which is currently set to 5mph.