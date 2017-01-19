/**
 * @file Arduboy2.cpp
 * \brief
 * The Arduboy2Base and Arduboy2 classes and support objects and definitions.
 */

#include "Arduboy2.h"
#include "Arduboy2Core.h"
#include "glcdfont.c"


//====================================
//========== class Arduboy2 ==========
//====================================

Arduboy2::Arduboy2()
{
  cursor_x = 0;
  cursor_y = 0;
  textColor = 1;
  textBackground = 0;
  textSize = 1;
  textWrap = 0;
}

size_t Arduboy2::write(uint8_t c)
{
  if (c == '\n')
  {
    cursor_y += textSize * 8;
    cursor_x = 0;
  }
  else if (c == '\r')
  {
    // skip em
  }
  else
  {
    drawChar(cursor_x, cursor_y, c, textColor, textBackground, textSize);
    cursor_x += textSize * 6;
    if (textWrap && (cursor_x > (WIDTH - textSize * 6)))
    {
      // calling ourselves recursively for 'newline' is
      // 12 bytes smaller than doing the same math here
      write('\n');
    }
  }
  return 1;
}

void Arduboy2::drawChar
  (int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size)
{
  bool draw_background = bg != color;

  if ((x >= WIDTH) ||              // Clip right
      (y >= HEIGHT) ||             // Clip bottom
      ((x + 5 * size - 1) < 0) ||  // Clip left
      ((y + 8 * size - 1) < 0)     // Clip top
     )
  {
    return;
  }

  for (int8_t i=0; i<6; i++ )
  {
    uint8_t line;
    if (i == 5)
    {
      line = 0x0;
    }
    else
    {
      line = pgm_read_byte(font+(c*5)+i);
    }

    for (int8_t j = 0; j<8; j++)
    {
      uint8_t draw_color = (line & 0x1) ? color : bg;

      if (draw_color || draw_background) {
        for (uint8_t a = 0; a < size; a++ ) {
          for (uint8_t b = 0; b < size; b++ ) {
            drawPixel(x + (i * size) + a, y + (j * size) + b, draw_color);
          }
        }
      }
      line >>= 1;
    }
  }
}

void Arduboy2::setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

int16_t Arduboy2::getCursorX() {
  return cursor_x;
}

int16_t Arduboy2::getCursorY() {
  return cursor_y;
}

void Arduboy2::setTextColor(uint8_t color)
{
  textColor = color;
}

void Arduboy2::setTextBackground(uint8_t bg)
{
  textBackground = bg;
}

void Arduboy2::setTextSize(uint8_t s)
{
  // size must always be 1 or higher
  textSize = max(1, s);
}

void Arduboy2::setTextWrap(bool w)
{
  textWrap = w;
}

void Arduboy2::clear() {
    Arduboy2Gfx::clear();
    cursor_x = cursor_y = 0;
}

