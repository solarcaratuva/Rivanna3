#ifndef PINDEF_H
#define PINDEF_H

#ifdef TARGET_POWER_BOARD // if compiling for PowerBoard


#define DEBUG_BTN       PC_13
#define DEBUG_LED_1     PC_0
#define DEBUG_LED_2     PC_1
#define STROBE_EN       PC_2
#define BRAKE_LIGHT_EN  PC_3
#define RIGHT_TURN_EN   PA_0
#define LEFT_TURN_EN    PA_1
#define DRL_EN          PA_2

#define THROTTLE_WIPER  PA_3
#define BRAKE_WIPER     PA_4


#define MTR_CAN_TX      PA_12
#define MTR_CAN_RX      PA_11
#define ACCEL_SCL       PA_9
#define ACCEL_SDA       PA_8
#define REGEN_SDA       PC_9
#define REGEN_SCL       PC_8
#define MPPT_PRE_EN     PC_6
#define CHARGE_EN       PB_15
#define MTR_PRE_EN      PB_14
#define DIS_CHARGE_EN   PB_13
#define CONT_12         PB_12
#define AUX             PB_11

#define DEBUG_SWITCH    PB_9
#define MAIN_CAN_TX     PB_6
#define MAIN_CAN_RX     PB_5
#define MAIN_CAN_STBY   PB_4
#define UART5_RX        PD_2
#define UART5_TX        PC_12

#else // if not compiling for PowerBoard


#define DEBUG_BTN       NC
#define DEBUG_LED_1     NC
#define DEBUG_LED_2     NC
#define STROBE_EN       NC
#define BRAKE_LIGHT_EN  NC
#define RIGHT_TURN_EN   NC
#define LEFT_TURN_EN    NC
#define DRL_EN          NC

#define THROTTLE_WIPER  NC
#define BRAKE_WIPER     NC

#define MTR_CAN_TX      NC
#define MTR_CAN_RX      NC
#define ACCEL_SCL       NC
#define ACCEL_SDA       NC
#define REGEN_SDA       NC
#define REGEN_SCL       NC
#define MPPT_PRE_EN     NC
#define CHARGE_EN       NC
#define MTR_PRE_EN      NC
#define DIS_CHARGE_EN   NC
#define CONT_12         NC
#define AUX             NC

#define DEBUG_SWITCH    NC
#define MAIN_CAN_TX     NC
#define MAIN_CAN_RX     NC
#define MAIN_CAN_STBY   NC
#define UART5_RX        NC
#define UART5_TX        NC
#endif
#endif
