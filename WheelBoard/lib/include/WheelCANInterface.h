#ifndef WHEEL_CAN_INTERFACE_H
#define WHEEL_CAN_INTERFACE_H

#include "BPSCANStructs.h"
#include "CANInterface.h"
#include "ECUCANStructs.h"
#include "MotorControllerCANStructs.h"

class WheelCANInterface : public CANInterface {
  public:
    WheelCANInterface(PinName rd, PinName td, PinName standby_pin);
    int send(CANStruct *can_struct);

  private:
    void message_handler() override;
};

#endif