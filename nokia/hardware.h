/**
* Hardware definitions
*
* May-4-2015 valfrom, based on https://github.com/thegaragelab/tinytemplate
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

// Use digitalWrite(), pinMode()
//#define LCD_USE_ARDUINO_STYLE_PINS

// Use separate pin to reset LCD
#define LCD_USE_PROGRAM_RESET

#define LCD_CONTRAST_VALUE 55

// Nokia LCD pin numbers
#if defined(USE_ARDUINO_STYLE_PINS) && defined(ARDUINO) && ARDUINO >= 100
	#define LCD_MOSI  8
	#define LCD_SCK   9
	#define LCD_CD    10
	#define LCD_RESET 11
#else
	#define LCD_MOSI  PINB0
	#define LCD_SCK   PINB1
	#define LCD_CD    PINB2
	#define LCD_RESET PINB3
#endif


#if defined(LCD_USE_ARDUINO_STYLE_PINS) && defined(ARDUINO) && ARDUINO >= 100

  #define LCD_MOSI_SET_OUTPUT() pinMode(LCD_MOSI, OUTPUT)
  #define LCD_SCK_SET_OUTPUT() pinMode(LCD_SCK, OUTPUT)
  #define LCD_CD_SET_OUTPUT() pinMode(LCD_CD, OUTPUT)
  #define LCD_RESET_SET_OUTPUT() pinMode(LCD_RESET, OUTPUT)

  #define LCD_MOSI_LOW() digitalWrite(LCD_MOSI, LOW)
  #define LCD_SCK_LOW() digitalWrite(LCD_SCK, LOW)
  #define LCD_CD_LOW() digitalWrite(LCD_CD, LOW)
  #define LCD_RESET_LOW() digitalWrite(LCD_RESET, LOW)

  #define LCD_MOSI_HIGH() digitalWrite(LCD_MOSI, HIGH)
  #define LCD_SCK_HIGH() digitalWrite(LCD_SCK, HIGH)
  #define LCD_CD_HIGH() digitalWrite(LCD_CD, HIGH)
  #define LCD_RESET_HIGH() digitalWrite(LCD_RESET, HIGH)
#else
  #define LCD_MOSI_SET_OUTPUT() DDRB |= (1 << LCD_MOSI)
  #define LCD_SCK_SET_OUTPUT() DDRB |= (1 << LCD_SCK)
  #define LCD_CD_SET_OUTPUT() DDRB |= (1 << LCD_CD)
  #define LCD_RESET_SET_OUTPUT() DDRB |= (1 << LCD_RESET)

  #define LCD_MOSI_LOW() PORTB &= ~(1 << LCD_MOSI)
  #define LCD_SCK_LOW() PORTB &= ~(1 << LCD_SCK)
  #define LCD_CD_LOW() PORTB &= ~(1 << LCD_CD)
  #define LCD_RESET_LOW() PORTB &= ~(1 << LCD_RESET)

  #define LCD_MOSI_HIGH() PORTB |= (1 << LCD_MOSI)
  #define LCD_SCK_HIGH() PORTB |= (1 << LCD_SCK)
  #define LCD_CD_HIGH() PORTB |= (1 << LCD_CD)
  #define LCD_RESET_HIGH() PORTB |= (1 << LCD_RESET)

#endif

#endif /* __HARDWARE_H */
