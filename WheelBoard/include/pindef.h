#ifndef PINDEF_H
#define PINDEF_H

#ifdef TARGET_WHEEL_BOARD


#define LEFT_SIGNAL                     PA_0
#define RIGHT_SIGNAL                    PA_1
#define HAZARDS                         PA_2
#define CRUISE_CONTROL_ENABLE           PA_3
#define CRUISE_CONTROL_INC              PA_4
#define CRUISE_CONTROL_DEC              PA_5
#define REGEN_ENABLE                    PA_6
#define CAN_RX                          PA_7
#define CAN_TX                          PA_8
#define CAN_STBY                        PB_9 

#else

#define LEFT_SIGNAL                     NC
#define RIGHT_SIGNAL                    NC
#define HAZARDS                         NC
#define CRUISE_CONTROL_ENABLE           NC
#define CRUISE_CONTROL_INC              NC
#define CRUISE_CONTROL_DEC              NC
#define REGEN_ENABLE                    NC
#define CAN_RX                          NC
#define CAN_TX                          NC
#define CAN_STBY                        NC 

#endif
#endif
