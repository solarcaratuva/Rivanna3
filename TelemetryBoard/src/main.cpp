#include "TelemetryCANInterface.h"
#include "ThisThread.h"
#include "log.h"
#include <mbed.h>
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

#define LOG_LEVEL LOG_ERROR

int main() {
    while (true) {
        ThisThread::sleep_for(1s);
    }
}