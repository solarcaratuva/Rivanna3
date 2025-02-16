#include "mbed.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "EEPROMDriver.h"
#include "pindef.h"

int main() {
    SDBlockDevice sd(SPI_MOSI, SPI_MISO, SPI_SCLK, SD_CS);
    FATFileSystem fs("sd");
    int err = sd.init();
    if (err == 0) {
        err = fs.mount(&sd);
        if (err == 0) {
            FILE *fp = fopen("/sd/test.txt", "w");
            if (fp) {
                fprintf(fp, "SD Card test successful\n");
                fclose(fp);
                fp = fopen("/sd/test.txt", "r");
                if (fp) {
                    char buf[64];
                    fgets(buf, sizeof(buf), fp);
                    printf("SD Card read: %s\n", buf);
                    fclose(fp);
                }
            } else {
                printf("Error opening file on SD\n");
            }
            fs.unmount();
        } else {
            printf("Filesystem mount error: %d\n", err);
        }
        sd.deinit();
    } else {
        printf("SD init error: %d\n", err);
    }
    
    EEPROMDriver eeprom(SPI_MOSI, SPI_MISO, SPI_SCLK, EEPROM_CS, 1000000);
    uint32_t addr = 0x0100;
    uint8_t dataToWrite = 0xA5;
    eeprom.write_byte(addr, dataToWrite);
    uint8_t dataRead = eeprom.read_byte(addr);
    printf("EEPROM wrote: 0x%02X, read: 0x%02X\n", dataToWrite, dataRead);
    
    while (true) {
        ThisThread::sleep_for(1s);
    }
}
