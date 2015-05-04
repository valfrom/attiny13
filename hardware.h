/**
* Hardware definitions
*
* May-4-2015 valfrom
*
* Defines the hardware pins to use for parts of the shared library.
*--------------------------------------------------------------------------*/

#ifndef __HARDWARE_H
#define __HARDWARE_H

// Bring in what we need for pin definitions
#include <avr/io.h>

//---------------------------------------------------------------------------
// Nokia LCD device support
//
// To use this device uncomment the LCD_ENABLED line below and define your
// pins with the LCD_xxx defines
//---------------------------------------------------------------------------

// Enable Nokia LCD support
#define LCD_ENABLED

// Nokia LCD pin numbers
#define LCD_MOSI  PINB2
#define LCD_SCK   PINB4
#define LCD_CD    PINB1
#define LCD_RESET PINB0

#endif /* __HARDWARE_H */
