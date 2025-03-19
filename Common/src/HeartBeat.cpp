#include <mbed.h>
#include <events/EventQueue.h>
#include "HeartBeatCANStruct.h"

#define t_time 100ms // timeout time
#define s_time 50ms // send frequency time

// send xyz_heartbeat
void send_heartbeat(std::string which_board, CANInterface* vehicle_can_interface) {
    HeartBeat heartbeat_struct;

    heartbeat_struct.from_telemetry_board = 0;
    heartbeat_struct.from_wheel_board = 0;
    heartbeat_struct.from_power_board = 0;

    if (which_board == "wheel") {heartbeat_struct.from_wheel_board = 1;}
    if (which_board == "tele") {heartbeat_struct.from_telemetry_board = 1;}
    if (which_board == "power") {heartbeat_struct.from_power_board = 1;}
    
    vehicle_can_interface->send(&powerboard_heartbeat_struct);
}

// handle board_timeout
void handle_timeout(std::string which_board) {
    if (which_board == "wheel") {printf("Wheel Board Timeout");}
    else if (which_board == "tele") {printf("Telemetry Board Timeout");}
    else if (which_board == "power") {printf("Power Board Timeout");}
}

// refresh_timer
void refreshTimer(Timeout* WheelBoard_timeout, Timeout* TelemetryBoard_timeout, Timeout* PowerBoard_timeout, HeartBeat *can_struct, EventQueue* queue) {
    // if it received a heartbeat message for itself (should not happen), it will have a null pointer exception

    if (can_struct->from_power_board == 0 && can_struct->from_wheel_board == 1 && can_struct->from_telemetry_board == 0) {
        // Reset current timeout
        WheelBoard_timeout->detach();
        // Set new timeout for 100ms from now
        WheelBoard_timeout->attach(
        queue.event(handle_timeout, "wheel"), t_time); // was event_queue in original motor main.cpp
    }
    else if (can_struct->from_power_board == 0 && can_struct->from_wheel_board == 0 && can_struct->from_telemetry_board == 1){
        // Reset current timeout
        TelemetryBoard_timeout->detach();
        // Set new timeout for 100ms from now
        TelemetryBoard_timeout->attach(
        queue.event(handle_timeout, "tele"), t_time); // was event_queue in original motor main.cpp
    }
    else if (can_struct->from_power_board == 1 && can_struct->from_wheel_board == 0 && can_struct->from_telemetry_board == 0){
        // Reset current timeout
        PowerBoard_timeout->detach();
        // Set new timeout for 100ms from now
        PowerBoard_timeout->attach(
        queue.event(handle_timeout, "power"), t_time); // was event_queue in original motor main.cpp
    }
}

// initializing
void initialize(Timeout* WheelBoard_timeout, Timeout* TelemetryBoard_timeout, Timeout* PowerBoard_timeout, EventQueue* queue, CANInterface* vehicle_can_interface) {
    // There needs to be n - 1 (where n is number of boards) Timeouts called, with the timeout for itself being null pointer

    // set initial heartbeat timer
    // WheelBoard
    if (WheelBoard_timeout != nullptr) {
        WheelBoard_timeout->attach(
            queue->event(handle_timeout, "wheel"), t_time);
    }
    else {
        // Send wheelboard heartbeat out every 50 ms
        queue->call_every(s_time, send_heartbeat, "wheel", vehicle_can_interface);
    }

    // TelemetryBoard
    if (TelemetryBoard_timeout != nullptr) {
        TelemetryBoard_timeout->attach(
            queue->event(handle_timeout, "tele"), t_time);
    }
    else {
        // Send telemetry heartbeat out every 50 ms
        queue->call_every(s_time, send_heartbeat, "tele", vehicle_can_interface);
    }

    // PowerBoard
    if (PowerBoard_timeout != nullptr) {
        PowerBoard_timeout->attach(
            queue->event(handle_timeout, "power"), t_time);
    }
    else {
        // Send powerboard heartbeat out every 50 ms
        queue->call_every(s_time, send_heartbeat, "power", vehicle_can_interface);
    }
}