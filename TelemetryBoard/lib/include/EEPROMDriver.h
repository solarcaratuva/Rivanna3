#ifndef EEPROMDRIVER_H
#define EEPROMDRIVER_H

#include "mbed.h"

class EEPROMDriver {
public:
    EEPROMDriver(PinName mosi, PinName miso, PinName sclk, PinName cs, int frequency = 1000000);
    void write_enable();
    void write_byte(uint32_t address, uint8_t data);
    uint8_t read_byte(uint32_t address);
    void wait_for_write_complete();

private:
    SPI _spi;
    DigitalOut _cs;
};

#endif
