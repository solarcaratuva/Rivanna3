#include "mbed.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "EEPROMDriver.h"
#include "pindef.h"
#include "log.h"

#define LOG_LEVEL          LOG_DEBUG

int main() {
    log_set_level(LOG_LEVEL);
    log_debug("Start main()");
    SDBlockDevice sd(SPI2_MOSI, SPI2_MISO, SPI2_SCK, SD_SELECT);
    FATFileSystem fs("sd");
    int err = sd.init();
    if (err == 0) {
        err = fs.mount(&sd);
        if (err == 0) {
            FILE *fp = fopen("/sd/test.txt", "w");
            if (fp) {
                // fprintf(fp, "SD Card test successful\n");
                EEPROMDriver eeprom(SPI2_MOSI, SPI2_MISO, SPI2_SCK, EEPROM_SELECT, 1000000); // clock speed here is 1Mhz
                uint32_t addr = 0x0100;
                uint8_t dataToWrite = 0xA5;
                eeprom.write_byte(addr, dataToWrite);
                uint8_t dataRead = eeprom.read_byte(addr);
                log_debug("EEPROM wrote: 0x%02X, read: 0x%02X\n", dataToWrite, dataRead);
                fprintf(fp, "EEPROM wrote: 0x%02X, read: 0x%02X\n", dataToWrite, dataRead);
                fclose(fp);
                fp = fopen("/sd/test.txt", "r");
                if (fp) {
                    char buf[64];
                    fgets(buf, sizeof(buf), fp);
                    log_debug("SD Card read: %s\n", buf);
                    fclose(fp);
                }
            } else {
                log_debug("Error opening file on SD\n");
            }
            fs.unmount();
        } else {
            log_debug("Filesystem mount error: %d\n", err);
        }
        sd.deinit();
    } else {
        log_debug("SD init error: %d\n", err);
    }
    
    // SDBlockDevice sd2(SPI2_MOSI, SPI2_MISO, SPI2_SCK, SD_SELECT);
    // FATFileSystem fs2("sd");
    // int err2 = sd2.init();
    // if (err2 == 0) {
    //     err2 = fs2.mount(&sd2);
    //     if (err2 == 0) {
    //         FILE *fp = fopen("/sd/test2.txt", "w");
    //         if (fp) {
    //             EEPROMDriver eeprom(SPI2_MOSI, SPI2_MISO, SPI2_SCK, EEPROM_SELECT, 1000000);
    //             uint32_t addr = 0x0100;
    //             uint8_t dataToWrite = 0xA5;
    //             eeprom.write_byte(addr, dataToWrite);
    //             uint8_t dataRead = eeprom.read_byte(addr);
    //             fprintf(fp, "EEPROM wrote: 0x%02X, read: 0x%02X\n", dataToWrite, dataRead);
    //         } else {
    //             log_debug("Error opening file on SD\n");
    //         }
    //         fs2.unmount();
    //     } else {
    //         log_debug("Filesystem mount error: %d\n", err2);
    //     }
    //     sd2.deinit();
    // } else {
    //     log_debug("SD init error: %d\n", err2);
    // }
    
    while (true) {
        ThisThread::sleep_for(1s);
    }
}
