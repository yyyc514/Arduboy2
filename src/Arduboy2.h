/**
 * @file Arduboy2.h
 * \brief
 * The Arduboy2Base and Arduboy2 classes and support objects and definitions.
 */

#ifndef ARDUBOY2_H
#define ARDUBOY2_H

#include <Arduino.h>
#include "Arduboy2Core.h"
// #include "Arduboy2Base.h"
// #include "Arduboy2Gfx.h"
// #include "Sprites.h"
#include <Print.h>
#include <limits.h>

/** \brief
 * Library version
 *
 * \details
 * For a version number in the form of x.y.z the value of the define will be
 * ((x * 10000) + (y * 100) + (z)) as a decimal number.
 * So, it will read as xxxyyzz, with no leading zeros on x.
 *
 * A user program can test this value to conditionally compile based on the
 * library version. For example:
 *
 * \code
 * // If the library is version 2.1.0 or higher
 * #if ARDUBOY_LIB_VER >= 20100
 *   // ... code that make use of a new feature added to V2.1.0
 * #endif
 * \endcode
 */
#define ARDUBOY_LIB_VER 30000

// eeprom settings above are neded for audio
#include "Arduboy2Audio.h"




//==============================
//========== Arduboy2 ==========
//==============================

/** \brief
 * The main functions provided for writing sketches for the Arduboy,
 * _including_ text output.
 *
 * \details
 * This class is derived from Arduboy2Base. It provides text output functions
 * in addition to all the functions inherited from Arduboy2Base.
 *
 * \note
 * A friend class named _Arduboy2Ex_ is declared by this class. The intention
 * is to allow a sketch to create an _Arduboy2Ex_ class which would have access
 * to the private and protected members of the Arduboy2 class. It is hoped
 * that this may eliminate the need to create an entire local copy of the
 * library, in order to extend the functionality, in most circumstances.
 *
 * \see Arduboy2Base
 */

class Arduboy2Gfx {
  public:
    void clear();
    void drawPixel(int16_t x, int16_t y, uint8_t color = WHITE);
};

class Arduboy2 : public Print, public Arduboy2Gfx
{
 friend class Arduboy2Ex;

 public:
  Arduboy2();

  /** \class Print
   * \brief
   * The Arduino `Print` class is available for writing text to the screen
   * buffer.
   *
   * \details
   * For an `Arduboy2` class object, functions provided by the Arduino `Print`
   * class can be used to write text to the screen buffer, in the same manner
   * as the Arduino `Serial.print()`, etc., functions.
   *
   * Print will use the `write()` function to actually draw each character
   * in the screen buffer.
   *
   * See:
   * https://www.arduino.cc/en/Serial/Print
   *
   * Example:
   * \code
   * int value = 42;
   *
   * arduboy.println("Hello World"); // Prints "Hello World" and then moves the
   *                                 // text cursor to the start of the next line
   * arduboy.print(value);  // Prints "42"
   * arduboy.print('\n');   // Moves the text cursor to the start of the next line
   * arduboy.print(78, HEX) // Prints "4E" (78 in hexadecimal)
   * \endcode
   *
   * \see Arduboy2::write()
   */

  /** \brief
   * Write a single ASCII character at the current text cursor location.
   *
   * \param c The ASCII value of the character to be written.
   *
   * \return The number of characters written (will always be 1).
   *
   * \details
   * This is the Arduboy implemetation of the Arduino virtual `write()`
   * function. The single ASCII character specified is written to the
   * the screen buffer at the current text cursor. The text cursor is then
   * moved to the next character position in the screen buffer. This new cursor
   * position will depend on the current text size and possibly the current
   * wrap mode.
   *
   * Two special characters are handled:
   *
   * - The newline character `\n`. This will move the text cursor to the start
   *   of the next line based on the current text size.
   * - The carriage return character `\r`. This character will be ignored.
   *
   * \note
   * This function is rather low level and, although it's available as a public
   * function, it wouldn't normally be used. In most cases the Arduino Print
   * class should be used for writing text.
   *
   * \see Print setTextSize() setTextWrap()
   */
  virtual size_t write(uint8_t);

  /** \brief
   * Draw a single ASCII character at the specified location in the screen
   * buffer.
   *
   * \param x The X coordinate, in pixels, for where to draw the character.
   * \param y The Y coordinate, in pixels, for where to draw the character.
   * \param c The ASCII value of the character to be drawn.
   * \param color the forground color of the character.
   * \param bg the background color of the character.
   * \param size The size of the character to draw.
   *
   * \details
   * The specified ASCII character is drawn starting at the provided
   * coordinate. The point specified by the X and Y coordinates will be the
   * top left corner of the character.
   *
   * \note
   * This is a low level function used by the `write()` function to draw a
   * character. Although it's available as a public function, it wouldn't
   * normally be used. In most cases the Arduino Print class should be used for
   * writing text.
   *
   * \see Print write() setTextColor() setTextBackground() setTextSize()
   */
  void drawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);

  /** \brief
   * Set the location of the text cursor.
   *
   * \param x The X coordinate, in pixels, for the new location of the text cursor.
   * \param y The Y coordinate, in pixels, for the new location of the text cursor.
   *
   * \details
   * The location of the text cursor is set the the specified coordinates.
   * The coordinates are in pixels. Since the coordinates can specify any pixel
   * location, the text does not have to be placed on specific rows.
   * As with all drawing functions, location 0, 0 is the top left corner of
   * the display. The cursor location will be the top left corner of the next
   * character written.
   */
  void setCursor(int16_t x, int16_t y);

  /** \brief
   * Get the X coordinate of the current text cursor position.
   *
   * \return The X coordinate of the current text cursor position.
   *
   * \details
   * The X coordinate returned is a pixel location with 0 indicating the
   * leftmost column.
   */
  int16_t getCursorX();

  /** \brief
   * Get the Y coordinate of the current text cursor position.
   *
   * \return The Y coordinate of the current text cursor position.
   *
   * \details
   * The Y coordinate returned is a pixel location with 0 indicating the
   * topmost row.
   */
  int16_t getCursorY();

  /** \brief
   * Set the text foreground color.
   *
   * \param color The color to be used for following text.
   */
  void setTextColor(uint8_t color);

  /** \brief
   * Set the text background color.
   *
   * \param bg The background color to be used for following text.
   */
  void setTextBackground(uint8_t bg);

  /** \brief
   * Set the text character size.
   *
   * \param s The text size multiplier. Must be 1 or higher.
   *
   * \details
   * Setting a text size of 1 will result in standard size characters which
   * occupy 6x8 pixels (the result of 5x7 characters with spacing on the
   * right and bottom edges).
   *
   * The value specified is a multiplier. A value of 2 will double the
   * size so they will occupy 12x16 pixels. A value of 3 will result in
   * 18x24, etc.
   */
  void setTextSize(uint8_t s);

  /** \brief
   * Set or disable text wrap mode.
   *
   * \param w `true` enables text wrap mode. `false` disables it.
   *
   * \details
   * Text wrap mode is enabled by specifying `true`. In wrap mode, the text
   * cursor will be moved to the start of the next line (based on the current
   * text size) if the following character wouldn't fit entirely at the end of
   * the current line.

   * If wrap mode is disabled, characters will continue to be written to the
   * same line. A character at the right edge of the screen may only be
   * partially displayed and additional characters will be off screen.
   */
  void setTextWrap(bool w);

  /** \brief
   * Clear the display buffer and set the text cursor to location 0, 0
   */
  void clear();

 protected:
  int16_t cursor_x;
  int16_t cursor_y;
  uint8_t textColor;
  uint8_t textBackground;
  uint8_t textSize;
  bool textWrap;

};

#endif

