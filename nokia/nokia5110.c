/**
* Nokia 5110 LCD interface
*
* May-4-2015 valfrom
**/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "../hardware.h"
#include "smallfont.h"
#include "nokia5110.h"

// Only provide the functions if the driver is enabled
#ifdef LCD_ENABLED

#define CLEAR_BYTE 0x00

// Determine the inner loop for the 'wait()' function. This is based on 6
// instruction cycles per loop.
#define WAIT_INNER (F_CPU / 6000)

/** An inaccurate delay function
 *
 * This function will delay for the given number of milliseconds. This is not
 * an accurate delay (interrupt activity will interfere with the timing and it
 * uses instruction timing approximations to consume the time) but should be
 * close enough for many purposes without having to revert to a timer interrupt.
 */
void wait(uint16_t millis) {
  for(uint16_t outer=0; outer<millis; outer++) {
    for(uint16_t inner=0; inner<WAIT_INNER; inner++) {
      asm volatile(
        "  nop    \n\t"
        );
    }
  }
}

/** Transfer data to a slave (MSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param data the data to transfer
 * @param bits the number of bits to transfer
 */
void sspiOutMSB(uint16_t data) {
  uint16_t mask = (1 << (8 - 1));
  uint8_t output = (1 << LCD_MOSI);
  uint8_t clock = (1 << LCD_SCK);
  for(uint8_t bits=0;bits<8;bits++) {
    // Set data
    if(data & mask) {
      PORTB |= output;
    } else {
      PORTB &= ~output;
    }
    // Bring the clock high
    PORTB |= clock;
    // Move to the next bit
    mask = mask >> 1;
    // Bring the clock low again
    PORTB &= ~clock;
  }
}

/** Send a data byte to the LCD
 *
 * @param data the data byte to send.
 */
void lcdData(uint8_t data) {
  // Bring CD high
  PORTB |= (1 << LCD_CD);
  // Send the data
  sspiOutMSB(data);
}

/** Send a command byte to the LCD
 *
 * @param cmd the command byte to send.
 */
void lcdCommand(uint8_t cmd) {
  // Bring CD low
  PORTB &= ~(1 << LCD_CD);
  // Send the data
  sspiOutMSB(cmd);
}

/** Initialise the LCD
 *
 * Sets up the pins required to communicate with the LCD screen and then does
 * the actual chipset initialisation. The pin numbers to use are defined in
 * @ref hardware.h.
 */
void lcdInit() {
  // // Set up the output pins, ensuring they are all 'low' to start
  uint8_t val = (1 << LCD_SCK) | (1 << LCD_MOSI) | (1 << LCD_RESET) | (1 << LCD_CD);
  PORTB &= ~val;
  DDRB |= val;
  // Do a hard reset on the LCD
  wait(10);
  PORTB |= (1 << LCD_RESET);
  // Initialise the LCD
  lcdCommand(0x21);  // LCD Extended Commands.
  lcdCommand(0xA1);  // Set LCD Vop (Contrast).
  lcdCommand(0x04);  // Set Temp coefficent.
  lcdCommand(0x14);  // LCD bias mode 1:48.
  lcdCommand(0x20);  // LCD Normal commands
  lcdCommand(0x0C);  // Normal display, horizontal addressing
}

/** Clear the screen
 *
 * Clear the entire display.
 *
 */
void lcdClear() {
  // Set the position
  lcdCommand(0x80);
  lcdCommand(0x40);
  // Fill in the whole display
  for(uint16_t index = 0; index < (LCD_COL * LCD_ROW); index++) {
    lcdData(CLEAR_BYTE);
  }
}

/** Clear a single row
 *
 * Clears a single character row from the left edge of the screen to the right.
 *
 * @param row the row number (0 to 5) to clear.
 */
void lcdClearRow(uint8_t row) {
  // Set the position
  lcdCommand(0x80);
  lcdCommand(0x40 | (row % LCD_ROW));
  // Fill in the row
  for(uint8_t index = 0; index < LCD_COL; index++) {
    lcdData(CLEAR_BYTE);
  }
}

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
void lcdPrintChar(uint8_t row, uint8_t col, char ch) {
  // Make sure it is on the screen
  if((row>=LCD_ROW)||(col>=LCD_COL)) {
    return;
  }
  // If the character is invalid replace it with the '?'
  if((ch<0x20)||(ch>0x7f)) {
    ch = '?';
  }
  // Set the starting address
  lcdCommand(0x80 | col);
  lcdCommand(0x40 | (row % LCD_ROW));
  // And send the column data
  const uint8_t *chdata = SMALL_FONT + ((ch - 0x20) * 5);
  for(uint8_t pixels = 0; (pixels < DATA_WIDTH) && (col < LCD_COL); pixels++, col++, chdata++) {
    uint8_t data = pgm_read_byte_near(chdata);
    lcdData(data);
  }
  // Add the padding byte
  if(col < LCD_COL) {
    lcdData(CLEAR_BYTE);
  }
}

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
void lcdPrint(uint8_t row, uint8_t col, const char *str) {
  for(;(*str!='\0')&&(col<LCD_COL);col+=CHAR_WIDTH,str++) {
    lcdPrintChar(row, col, *str);
  }
}

// Only provide the functions if the driver is enabled
#endif /* LCD_ENABLED */