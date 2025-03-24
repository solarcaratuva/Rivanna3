#ifndef HEARTBEAT_SYSTEM_H
#define HEARTBEAT_SYSTEM_H

#include <mbed.h>
#include <events/EventQueue.h>
#include "HeartBeatCANStruct.h"

// Board identifiers
#define HB_TELEMETRY_BOARD 1
#define HB_WHEEL_BOARD 2
#define HB_POWER_BOARD 3

// Timing constants
#define TIME_BEFORE_TIMEOUT_MS 100ms
#define HEARTBEAT_SEND_PERIOD_MS 50ms

class HeartBeatSystem {
private:
    Timeout WheelBoard_timeout;
    Timeout TelemetryBoard_timeout;
    Timeout PowerBoard_timeout;
    EventQueue* queue;

    bool listenWheel = false;
    bool listenTelemetry = false;
    bool listenPower = false;

    uint8_t which_board;
    mbed::Callback<void()> timeout_function;

public:
    // Constructor
    HeartBeatSystem(mbed::Callback<void()> timeout_function, events::EventQueue* queue, uint8_t which_board);

    // Initialize timeouts
    void initializeTimeouts(bool start_telemetry_timeout, bool start_wheel_timeout, bool start_power_timeout);

    // Refresh timeout for received heartbeat message
    void refreshTimer(HeartBeat* can_struct);

    // Return a heartbeat message to specified board
    HeartBeat send_heartbeat(uint8_t to_which_board);
};

#endif // HEARTBEAT_SYSTEM_H
