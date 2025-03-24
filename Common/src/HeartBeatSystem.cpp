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

HeartBeat* HeartBeatSystem::send_heartbeat(uint8_t to_which_board) {
    HeartBeat heartbeat_struct;

    heartbeat_struct.from_telemetry_board = 0;
    heartbeat_struct.from_wheel_board = 0;
    heartbeat_struct.from_power_board = 0;

    switch (to_which_board)
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
    
    return &heartbeat_struct;
}    
    // // send xyz_heartbeat
    // void send_heartbeat(std::string to_which_board) {
    //     HeartBeat heartbeat_struct;

    //     heartbeat_struct.from_telemetry_board = 0;
    //     heartbeat_struct.from_wheel_board = 0;
    //     heartbeat_struct.from_power_board = 0;

    //     if (which_board == WHEEL_BOARD) {heartbeat_struct.from_wheel_board = 1;}
    //     if (which_board == TELEMETRY_BOARD) {heartbeat_struct.from_telemetry_board = 1;}
    //     if (which_board == POWER_BOARD) {heartbeat_struct.from_power_board = 1;}
        
    //     vehicle_can_interface->send(&powerboard_heartbeat_struct);
    // }

    // // // handle board_timeout
    // // void handle_timeout(std::string which_board) {
    // //     if (which_board == "wheel") {printf("Wheel Board Timeout");}
    // //     else if (which_board == "tele") {printf("Telemetry Board Timeout");}
    // //     else if (which_board == "power") {printf("Power Board Timeout");}
    // // }

    // // refresh_timer
    // void refreshTimer(Timeout* WheelBoard_timeout, Timeout* TelemetryBoard_timeout, Timeout* PowerBoard_timeout, HeartBeat *can_struct, EventQueue* queue) {
    //     // if it received a heartbeat message for itself (should not happen), it will have a null pointer exception

    //     if (can_struct->from_power_board == 0 && can_struct->from_wheel_board == 1 && can_struct->from_telemetry_board == 0) {
    //         // Reset current timeout
    //         WheelBoard_timeout->detach();
    //         // Set new timeout for 100ms from now
    //         WheelBoard_timeout->attach(
    //         queue.event(handle_timeout, "wheel"), TIME_BEFORE_TIMEOUT_MS); // was event_queue in original motor main.cpp
    //     }
    //     else if (can_struct->from_power_board == 0 && can_struct->from_wheel_board == 0 && can_struct->from_telemetry_board == 1){
    //         // Reset current timeout
    //         TelemetryBoard_timeout->detach();
    //         // Set new timeout for 100ms from now
    //         TelemetryBoard_timeout->attach(
    //         queue.event(handle_timeout, "tele"), TIME_BEFORE_TIMEOUT_MS); // was event_queue in original motor main.cpp
    //     }
    //     else if (can_struct->from_power_board == 1 && can_struct->from_wheel_board == 0 && can_struct->from_telemetry_board == 0){
    //         // Reset current timeout
    //         PowerBoard_timeout->detach();
    //         // Set new timeout for 100ms from now
    //         PowerBoard_timeout->attach(
    //         queue.event(handle_timeout, "power"), TIME_BEFORE_TIMEOUT_MS); // was event_queue in original motor main.cpp
    //     }
    // }

    // // initializing
    // void initialize(Timeout* WheelBoard_timeout, Timeout* TelemetryBoard_timeout, Timeout* PowerBoard_timeout, EventQueue* queue, CANInterface* vehicle_can_interface) {
    //     // There needs to be n - 1 (where n is number of boards) Timeouts called, with the timeout for itself being null pointer

    //     // set initial heartbeat timer
    //     // WheelBoard
    //     if (WheelBoard_timeout != nullptr) {
    //         WheelBoard_timeout->attach(
    //             queue->event(handle_timeout, "wheel"), TIME_BEFORE_TIMEOUT_MS);
    //     }
    //     else {
    //         // Send wheelboard heartbeat out every 50 ms
    //         queue->call_every(HEARTBEAT_SEND_PERIOD_MS, send_heartbeat, "wheel", vehicle_can_interface);
    //     }

    //     // TelemetryBoard
    //     if (TelemetryBoard_timeout != nullptr) {
    //         TelemetryBoard_timeout->attach(
    //             queue->event(handle_timeout, "tele"), TIME_BEFORE_TIMEOUT_MS);
    //     }
    //     else {
    //         // Send telemetry heartbeat out every 50 ms
    //         queue->call_every(HEARTBEAT_SEND_PERIOD_MS, send_heartbeat, "tele", vehicle_can_interface);
    //     }

    //     // PowerBoard
    //     if (PowerBoard_timeout != nullptr) {
    //         PowerBoard_timeout->attach(
    //             queue->event(handle_timeout, "power"), TIME_BEFORE_TIMEOUT_MS);
    //     }
    //     else {
    //         // Send powerboard heartbeat out every 50 ms
    //         queue->call_every(HEARTBEAT_SEND_PERIOD_MS, send_heartbeat, "power", vehicle_can_interface);
    //     }
    // }