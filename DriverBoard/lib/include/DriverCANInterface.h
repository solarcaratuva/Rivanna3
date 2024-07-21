#ifndef DRIVER_CAN_INTERFACE_H
#define DRIVER_CAN_INTERFACE_H

#include "BPSCANStructs.h"
#include "MotorControllerCANStructs.h"
#include "CANInterface.h"
#include "ECUCANStructs.h"

class DriverCANInterface : public CANInterface {
  public:
    DriverCANInterface(PinName rd, PinName td, PinName standby_pin);
    void handle(BPSError *can_struct);
    void handle(MotorControllerPowerStatus *can_struct);
    void handle(ECUPowerAuxCommands *can_struct);
    int send(CANStruct *can_struct);

  private:
    void message_handler() override;
};

#endif
