#include "mbed.h"
#include "TouchGFXGeneratedHAL.hpp"  // Provided by TouchGFX generation (or the appropriate HAL header)
#include "touchgfx/hal/HAL.hpp"       // Optional: if you need access to HAL functions

extern "C" {
    #include "app_touchgfx.h"
}    

// Create a thread for running the TouchGFX main loop.
Thread touchgfx_thread;

int main() {
    // (Optional) Call any hardware or TouchGFX initialization here.
    // For example, if your generated code has an init function, you might call it here.
    // MX_TouchGFX_Init();

    // Start the TouchGFX main loop on a separate thread.
    // MX_TouchGFX_Process() typically processes one frame when called.
    // Here we start a thread that repeatedly calls it.
    touchgfx_thread.start([]() {
        while (true) {
            MX_TouchGFX_Process();
            // Optionally yield to allow other threads to run.
            ThisThread::yield();
        }
    });

    // The main thread can perform other tasks, or simply sleep indefinitely.
    while (true) {
        ThisThread::sleep_for(1000ms);
    }
}
