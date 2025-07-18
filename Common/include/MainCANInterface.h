#ifndef MAIN_CAN_INTERFACE_H
#define MAIN_CAN_INTERFACE_H

#include "CANInterface.h"
#include "CANStructs.h"

/**
 * CANInterface for use with the main CAN bus.
 *
 * The interface defines virtual methods for handling receipt of each type of
 * CAN struct. For example, the handle(ECUMotorCommands *can_struct) method
 * will be called whenever an ECUMotorCommands struct is received. By default,
 * all of these methods do nothing. As such, this interface is not intended to
 * be used directly; it should be extended by another class.
 *
 * Classes that extend MainCANInterface, such as SolarCANInterface, should
 * override the handle method for each struct they need to receive.
 *
 * The send method will send a CANStruct on the bus.
 */
class MainCANInterface : public CANInterface {
  public:
    MainCANInterface(PinName rd, PinName td, PinName standby_pin, PinName uart_rx, PinName uart_tx);

    int send(CANStruct *can_struct);
    int send_message(CANMessage *message);
    void send_to_pi(CANMessage *message, uint16_t message_id);

    virtual void handle(ECUMotorCommands *can_struct) {}
    virtual void handle(ECUPowerAuxCommands *can_struct) {}
    virtual void handle(PowerAuxError *can_struct) {}
    virtual void handle(SolarCurrent *can_struct) {}
    virtual void handle(SolarVoltage *can_struct) {}
    virtual void handle(SolarTemp *can_struct) {}
    virtual void handle(SolarPhoto *can_struct) {}
    virtual void handle(MotorControllerPowerStatus *can_struct) {}
    virtual void handle(MotorControllerDriveStatus *can_struct) {}
    virtual void handle(MotorControllerError *can_struct) {}
    virtual void handle(BPSPackInformation *can_struct) {}
    virtual void handle(BPSError *can_struct) {}

  private:
    void message_handler() override;
    PinName uartTX;
    PinName uartRX;
    //UnbufferedSerial raspberry_pi;
};

#endif
