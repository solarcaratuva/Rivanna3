#include "HeartBeatSystem.h"

HeartBeatSystem::HeartBeatSystem(Callback<void()> timeout_function, EventQueue* queue, uint8_t which_board) :
queue(queue), which_board(which_board), timeout_function(timeout_function) {}

void HeartBeatSystem::initializeTimeouts(bool start_telemetry_timeout, bool start_wheel_timeout, bool start_power_timeout) {
    if (start_telemetry_timeout) {
        TelemetryBoard_timeout.attach(queue->event(timeout_function), TIME_BEFORE_TIMEOUT_MS);
        listenTelemetry = true;
    }

    if (start_wheel_timeout) {
        WheelBoard_timeout.attach(queue->event(timeout_function), TIME_BEFORE_TIMEOUT_MS);
        listenWheel = true;
    }

    if (start_power_timeout) {
        PowerBoard_timeout.attach(queue->event(timeout_function), TIME_BEFORE_TIMEOUT_MS);
        listenPower = true;
    }
}

void HeartBeatSystem::refreshTimer(HeartBeat *can_struct) {
    // if it received a heartbeat message for itself (should not happen), it will have a null pointer exception

    if (can_struct->from_wheel_board == 1 && listenWheel) {
        // Reset current timeout
        WheelBoard_timeout.detach();
        // Set new timeout for TIME_BEFORE_TIMEOUT_MS from now
        WheelBoard_timeout.attach(queue->event(timeout_function), TIME_BEFORE_TIMEOUT_MS);
    }
    else if (can_struct->from_telemetry_board == 1 && listenTelemetry){
        TelemetryBoard_timeout.detach();

        TelemetryBoard_timeout.attach(queue->event(timeout_function), TIME_BEFORE_TIMEOUT_MS);
    }
    else if (can_struct->from_power_board == 1 && listenPower){
        PowerBoard_timeout.detach();

        PowerBoard_timeout.attach(queue->event(timeout_function), TIME_BEFORE_TIMEOUT_MS);
    }
}

HeartBeat HeartBeatSystem::send_heartbeat() {
    HeartBeat heartbeat_struct;

    heartbeat_struct.from_telemetry_board = 0;
    heartbeat_struct.from_wheel_board = 0;
    heartbeat_struct.from_power_board = 0;

    switch (which_board)
    {
    case HB_WHEEL_BOARD:
        heartbeat_struct.from_wheel_board = 1;
        break;
    
    case HB_TELEMETRY_BOARD:
        heartbeat_struct.from_telemetry_board = 1;
        break;

    case HB_POWER_BOARD:
        heartbeat_struct.from_power_board = 1;
        break;
    default:
        log_error("Invalid Board in Heartbeat Library send_heartbeat");
        break;
    }
    
    return heartbeat_struct;
}
    