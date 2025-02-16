#ifndef PINDEF_H
#define PINDEF_H

#ifdef TARGET_UVA_SOLAR_CAR

// PINS BEING CORRECTED. SHOULD BE GOOD, BUT SOME MAY POSSIBLY BE INCORRECT OR HAVE PLACEHOLDERS

#define SD_SPI_CS   PA_4

#define SPI_MOSI    PB_15 // Switched From PA_7
#define SPI_MISO    PB_14 // Switched From PA_6
#define SPI_SCK     PB_13 // Switched From PA_5
//#define SPI_CLK     PA_5      (Unsure if this part is correct)
#define SD_CS       PA_4

/*
#define EEPROM_SPI_CS PA_3
#define EEPROM_CS   PA_3
*/

// CS means Chip Select, which is seemingly the same as CE or Chip Enable

#endif

#endif