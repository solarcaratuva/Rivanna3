#ifndef PINDEF_H
#define PINDEF_H

// #ifdef TARGET_POWER_BOARD // if compiling for PowerBoard

// #define DEBUG_BTN       PC_13
// #define DEBUG_LED_1     PC_1
// #define DEBUG_LED_2     PC_2
// #define STROBE_EN       PC_2
// #define BRAKE_LIGHT_EN  PC_3
// #define RIGHT_TURN_EN   PA_0
// #define LEFT_TURN_EN    PA_1
// #define DRL_EN          PA_2

// #define THROTTLE_WIPER  PA_3
// #define BRAKE_WIPER     PA_4

// #define AUX             PB_11
// #define CONT_12         PB_12
// #define MPPT_HAL_SENSE  PB_13
// #define MTR_HAL_SENSE   PB_14

// #define DIS_CHARGE_EN   PB_15
// #define MTR_PRE_EN      PC_6
// #define CHARGE_EN       PC_7
// #define MPPT_PRE_EN     PC_8

// #define MTR_SDA         PA_8
// #define MTR_SCL         PA_9

// #define MTR_CAN_RX      PA_11
// #define MTR_CAN_TX      PA_12

// #define MAIN_CAN_STBY   PB_4
// #define MAIN_CAN_RX     PB_5
// #define MAIN_CAN_TX     PB_6

// #define DEBUG_SWITCH    PB_9


// #else // if not compiling for PowerBoard

// #define DEBUG_BTN       NC
// #define DEBUG_LED_1     NC
// #define DEBUG_LED_2     NC
// #define STROBE_EN       NC
// #define BRAKE_LIGHT_EN  NC
// #define RIGHT_TURN_EN   NC
// #define LEFT_TURN_EN    NC
// #define DRL_EN          NC

// #define THROTTLE_WIPER  NC
// #define BRAKE_WIPER     NC

// #define AUX             NC
// #define CONT_12         NC
// #define MPPT_HAL_SENSE  NC
// #define MTR_HAL_SENSE   NC

// #define DIS_CHARGE_EN   NC
// #define MTR_PRE_EN      NC
// #define CHARGE_EN       NC
// #define MPPT_PRE_EN     NC

// #define MTR_SDA         NC
// #define MTR_SCL         NC

// #define MTR_CAN_RX      NC
// #define MTR_CAN_TX      NC

// #define MAIN_CAN_STBY   NC
// #define MAIN_CAN_RX     NC
// #define MAIN_CAN_TX     NC

// #define PACK_VOLTAGE    NC

// #define DEBUG_SWITCH    NC

// #endif
// #endif



// #define DEBUG_BTN       
#define DEBUG_LED_1         PE_11
// #define DEBUG_LED_2         PE_15
#define STROBE_EN           PF_13
#define BRAKE_LIGHT_EN      PF_12
#define RIGHT_TURN_EN       PD_15
#define LEFT_TURN_EN        PD_14
#define DRL_EN              PA_7

#define THROTTLE_WIPER      PA_6
#define BRAKE_WIPER         PA_5

#define AUX                 PC_3
#define CONT_12             PC_1
#define MPPT_HAL_SENSE      PC_4
#define MTR_HAL_SENSE       PC_5

#define DIS_CHARGE_EN       PF_2
#define MTR_PRE_EN          PF_1
#define CHARGE_EN           PF_0
#define MPPT_PRE_EN         PD_0

#define MTR_SDA             PB_11
#define MTR_SCL             PB_10

#define MTR_CAN_RX          PG_0
#define MTR_CAN_TX          PD_1

#define MAIN_CAN_STBY       NC
#define MAIN_CAN_RX         PB_12
#define MAIN_CAN_TX         PB_13

// #define DEBUG_SWITCH   

#endif