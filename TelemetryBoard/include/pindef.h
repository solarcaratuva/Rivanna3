#ifndef PINDEF_H
#define PINDEF_H

#ifdef TARGET_UVA_SOLAR_CAR

// PINS BEING CORRECTED. SHOULD BE GOOD, BUT SOME MAY POSSIBLY BE INCORRECT OR HAVE PLACEHOLDERS
// Also, for some reason the PC_# pins don't like compiling

#define SPI_MOSI    PB_15 // Switched From PA_7
#define SPI_MISO    PB_14 // Switched From PA_6
#define SPI_SCLK    PB_13 // (Technically this is the SPI_SCK pin, but both SCK and SCLK are clock pins that should do the same function??)
//#define SPI_CLK     PA_5      (Replaced with SCLK)
#define EEPROM_CS   PC_6 // Switched From PA_3, for EEPROM (Chip) Select Bit
#define SD_CS       PB_12 // Swtiched From PA_4, for SD Card (Chip) Select Bit

#define EEPROM_WRITE    PC_7
#define EEPROM_HOLD     PC_8

/*
Pins that I'm not sure we will use and only have placeholder values

#define SD_SPI_CS   PA_4
#define EEPROM_SPI_CS PA_3
*/

// CS means Chip Select, which is seemingly the same as CE or Chip Enable

#endif

#endif