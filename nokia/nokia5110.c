/**
* Nokia 5110 LCD interface
*
* May-4-2015 valfrom, based on https://github.com/thegaragelab/tinytemplate
**/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "hardware.h"
#include "smallfont.h"
#include "nokia5110.h"

// Only provide the functions if the driver is enabled
#ifdef LCD_ENABLED

#define CLEAR_BYTE 0x00

// Determine the inner loop for the 'wait()' function. This is based on 6
// instruction cycles per loop.
#define WAIT_INNER (F_CPU / 6000)


#if defined(LCD_USE_PROGRAM_RESET) 

void waitReset() {
  uint16_t inner;
  for(inner=0; inner<WAIT_INNER*10; inner++) {
    asm volatile(
      "  nop    \n\t"
      );
  }
}

#endif

/** Transfer data to a slave (MSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param data the data to transfer
 * @param bits the number of bits to transfer
 */
void sspiOutMSB(uint8_t data) {
  uint8_t bit;
  for(bit = 0x80; bit; bit >>= 1) {
    // Bring the clock low
    LCD_SCK_LOW();
    // Set data
    if(data & bit) {
      LCD_MOSI_HIGH();
    } else {
      LCD_MOSI_LOW();
    }
    // Bring the clock high
    LCD_SCK_HIGH();
  }
}

/** Send a data byte to the LCD
 *
 * @param data the data byte to send.
 */
void lcdData(uint8_t data) {
  // // Bring CD high
  LCD_CD_HIGH();
  // Send the data
  sspiOutMSB(data);
}

/** Send a command byte to the LCD
 *
 * @param cmd the command byte to send.
 */
void lcdCommand(uint8_t cmd) {
  // Bring CD low
  LCD_CD_LOW();
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
  LCD_SCK_SET_OUTPUT();
  LCD_MOSI_SET_OUTPUT();
  LCD_CD_SET_OUTPUT();

  LCD_SCK_LOW();
  LCD_MOSI_LOW();
  LCD_CD_LOW();

#if defined(LCD_USE_PROGRAM_RESET) 
  LCD_RESET_SET_OUTPUT();
  LCD_RESET_LOW();
  // Do a hard reset on the LCD
  waitReset();
  //PORTB |= (1 << LCD_RESET);
  LCD_RESET_HIGH();
#endif

  // Initialise the LCD
  lcdCommand(0x21);  // LCD Extended Commands.
  lcdCommand(0x80|LCD_CONTRAST_VALUE);  // Set LCD Vop (Contrast).
  lcdCommand(0x04);  // Set Temp coefficent.
  lcdCommand(0x14);  // LCD bias mode 1:48.
  lcdCommand(0x20);  // LCD Normal commands
  lcdCommand(0x08 | 0x04);  // Normal display, horizontal addressing
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
  uint16_t index;
  for(index = 0; index < (LCD_COL * LCD_ROW); index++) {
    lcdData(CLEAR_BYTE);
  }
}

uint8_t bitScale(uint8_t b, uint8_t shift) {
  uint8_t i;
  uint8_t r = 0;
  for(i = 0; i < 8; i++) {
    uint8_t bit = i / 2;
    if(b & (1 << (bit + shift))) {
      bit = 1;
    } else {
      bit = 0;
    }
    r |= (bit << i);
  }
  return r;
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
void lcdPrintChar(uint8_t row, uint8_t acol, char ch, uint8_t size) {
  // Make sure it is on the screen
  if((row>=LCD_ROW)||(acol>=LCD_COL)) {
    return;
  }
  // If the character is invalid replace it with the '?'
  if((ch<0x20)||(ch>0x7f)) {
    ch = '?';
  }
  
  uint8_t i;
  for(i = 0; i < size; i++) {
    uint8_t col = acol;
    // Set the starting address
    lcdCommand(0x80 | col);
    lcdCommand(0x40 | ((row + i) % LCD_ROW));
    // And send the column data
    const uint8_t *chdata = SMALL_FONT + ((ch - 0x20) * 5);
    uint8_t pixels;

    for(pixels = 0; (pixels < DATA_WIDTH) && (col < LCD_COL); pixels++, col++, chdata++) {
      uint8_t data = pgm_read_byte_near(chdata);

      if(size != 2) {
        lcdData(data);
        continue;
      }
      data = bitScale(data, i * 4);
      lcdData(data);
      lcdData(data);
    }

    // Add the padding byte
    if(col < LCD_COL) {
      lcdData(CLEAR_BYTE);
    }
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
void lcdPrint(uint8_t row, uint8_t col, const char *str, uint8_t size) {
  for(;(*str!='\0')&&(col<LCD_COL);col+=CHAR_WIDTH*size,str++) {
    lcdPrintChar(row, col, *str, size);
  }
}

// Only provide the functions if the driver is enabled
#endif /* LCD_ENABLED */