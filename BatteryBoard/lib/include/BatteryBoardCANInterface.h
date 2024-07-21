#ifndef BATTERY_BOARD_CAN_INTERFACE_H
#define BATTERY_BOARD_CAN_INTERFACE_H

#include "MainCANInterface.h"

class BatteryBoardCANInterface : public MainCANInterface {
  public:
    BatteryBoardCANInterface(PinName rd, PinName td, PinName standby_pin)
        : MainCANInterface(rd, td, standby_pin, NC, NC) {}

};

#endif
