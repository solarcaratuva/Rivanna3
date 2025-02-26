#include "EEPROMDriver.h"

EEPROMDriver::EEPROMDriver(PinName mosi, PinName miso, PinName sclk, PinName cs, int frequency)
    : _spi(mosi, miso, sclk), _cs(cs, 1)
{
    _spi.frequency(frequency);
    _spi.format(8, 0); // 8 bits per transfer, SPI mode 0 (CPOL = 0, CPHA = 0)
}

void EEPROMDriver::write_enable() {
    _cs = 0;
    _spi.write(0x06);
    _cs = 1;
}

void EEPROMDriver::write_byte(uint32_t address, uint8_t data) {
    write_enable();
    _cs = 0;
    _spi.write(0x02); // WRITE command (0x02)
    _spi.write((address >> 8) & 0xFF);
    _spi.write(address & 0xFF);
    _spi.write(data);
    _cs = 1; // Deselect the device, triggering the write cycle
    wait_for_write_complete();
}

uint8_t EEPROMDriver::read_byte(uint32_t address) {
    _cs = 0;
    _spi.write(0x03);
    _spi.write((address >> 8) & 0xFF);
    _spi.write(address & 0xFF);
    // Perform a dummy write to receive the data byte
    uint8_t received = _spi.write(0xFF);
    _cs = 1;
    return received;
}

void EEPROMDriver::wait_for_write_complete() {
    while (true) {
        _cs = 0;
        _spi.write(0x05); // RDSR (Read Status Register) command
        uint8_t status = _spi.write(0xFF); // Dummy write to read status
        _cs = 1; // Deselect the device

        // Write in progress bit is the LSB wait until it's cleared
        if ((status & 0x01) == 0) {
            break;
        }
        ThisThread::sleep_for(1ms);
    }
}
