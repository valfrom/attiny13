/**
* Nokia 5110 LCD interface
*
* May-4-2015 valfrom, based on https://github.com/thegaragelab/tinytemplate
**/

#ifndef __NOKIA5110_H
#define __NOKIA5110_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#endif

/** Number of columns */
#define LCD_COL 84

/** Number of text rows */
#define LCD_ROW 6

#ifdef __cplusplus
extern "C" {
#endif

/** Initialise the LCD
 *
 * Sets up the pins required to communicate with the LCD screen and then does
 * the actual chipset initialisation. The pin numbers to use are defined in
 * @ref hardware.h.
 */
void lcdInit();

/** Send a data byte to the LCD
 *
 * @param data the data byte to send.
 */
void lcdData(uint8_t data);

/** Send a command byte to the LCD
 *
 * @param cmd the command byte to send.
 */
void lcdCommand(uint8_t cmd);

/** Clear the screen
 *
 * Clear the entire display.
 *
 */
void lcdClear();

/** Write a single character
 *
 * Display a single ASCII character at the position described by the row and
 * column parameters. Note that the row indicates an 8 pixel high character
 * row while the column represents individual pixels. This code uses a built
 * in character set where each character is 5 pixels wide and adds a single
 * column of pixels as spacing giving a total width of 6 pixels.
 *
 * @param row the row number (0 to 5) to display the character.
 * @param col the column position (0 to 83) for the start of the left side of
 *            the character.
 * @param ch  the character to display. If the character is out of range it
 *            will be replaced with the '?' character.
 */
void lcdPrintChar(uint8_t row, uint8_t col, char ch, uint8_t size);

/** Write a nul terminated string
 *
 * Display a string of ASCII characters at the position described by the row
 * and column parameters. Note that the row indicates an 8 pixel high character
 * row while the column represents individual pixels. This code uses a built
 * in character set where each character is 6 pixels wide.
 *
 * This function does not perform any word wrapping or other text layout
 * functions. If the string exceeds the size of the display it will simply
 * be clipped.
 *
 * @param row the row number (0 to 5) to display the character.
 * @param col the column position (0 to 83) for the start of the left side of
 *            the character.
 * @param str the string to display. If a character in the string is out of
 *            range it will be replaced with the '?' character.
 */
void lcdPrint(uint8_t row, uint8_t col, const char *str, uint8_t size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* __NOKIA5110_H */
