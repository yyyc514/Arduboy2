#ifndef ARDUBOY2GFX_H
#define ARDUBOY2GFX_H

#include <Arduino.h>
#include "Arduboy2Base.h"
#include "Sprites.h"
#include <Print.h>
#include <limits.h>

// If defined, it is safe to draw outside of the screen boundaries.
// Pixels that would exceed the display limits will be ignored.
#define PIXEL_SAFE_MODE


class Arduboy2Gfx : public Arduboy2Base
{
  friend class Arduboy2Ex;
  friend class Sprites;

  public:
  Arduboy2Gfx();

  /** \brief
   * Clear the display buffer.
   *
   * \details
   * The entire contents of the screen buffer are cleared to BLACK.
   *
   * \see display(bool)
   */
  void clear();

  /** \brief
   * Copy the contents of the display buffer to the display.
   *
   * \details
   * The contents of the display buffer in RAM are copied to the display and
   * will appear on the screen.
   *
   * \see display(bool)
   */
  void display();

  /** \brief
   * Copy the contents of the display buffer to the display. The display buffer
   * can optionally be cleared.
   *
   * \param clear If `true` the display buffer will be cleared to zero.
   * The defined value `CLEAR_BUFFER` should be used instead of `true` to make
   * it more meaningful.
   *
   * \details
   * Operation is the same as calling `display()` without parameters except
   * additionally the display buffer will be cleared if the parameter evaluates
   * to `true`. (The defined value `CLEAR_BUFFER` can be used for this)
   *
   * Using `display(CLEAR_BUFFER)` is faster and produces less code than
   * calling `display()` followed by `clear()`.
   *
   * \see display() clear()
   */
  void display(bool clear);

  /** \brief
   * Set a single pixel in the display buffer to the specified color.
   *
   * \param x The X coordinate of the pixel.
   * \param y The Y coordinate of the pixel.
   * \param color The color of the pixel (optional; defaults to WHITE).
   *
   * \details
   * The single pixel specified location in the display buffer is set to the
   * specified color. The values WHITE or BLACK can be used for the color.
   * If the `color` parameter isn't included, the pixel will be set to WHITE.
   */
  void drawPixel(int16_t x, int16_t y, uint8_t color = WHITE);

  /** \brief
   * Returns the state of the given pixel in the screen buffer.
   *
   * \param x The X coordinate of the pixel.
   * \param y The Y coordinate of the pixel.
   *
   * \return WHITE if the pixel is on or BLACK if the pixel is off.
   */
  uint8_t getPixel(uint8_t x, uint8_t y);

  /** \brief
   * Draw a circle of a given radius.
   *
   * \param x0 The X coordinate of the circle's center.
   * \param y0 The Y coordinate of the circle's center.
   * \param r The radius of the circle in pixels.
   * \param color The circle's color (optional; defaults to WHITE).
   */
  void drawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color = WHITE);

  // Draw one or more "corners" of a circle.
  // (Not officially part of the API)
  void drawCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t corners, uint8_t color = WHITE);

  /** \brief
   * Draw a filled-in circle of a given radius.
   *
   * \param x0 The X coordinate of the circle's center.
   * \param y0 The Y coordinate of the circle's center.
   * \param r The radius of the circle in pixels.
   * \param color The circle's color (optional; defaults to WHITE).
   */
  void fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color = WHITE);

  // Draw one or both vertical halves of a filled-in circle or
  // rounded rectangle edge.
  // (Not officially part of the API)
  void fillCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t sides, int16_t delta, uint8_t color = WHITE);

  /** \brief
   * Draw a line between two specified points.
   *
   * \param x0,x1 The X coordinates of the line ends.
   * \param y0,y1 The Y coordinates of the line ends.
   * \param color The line's color (optional; defaults to WHITE).
   *
   * \details
   * Draw a line from the start point to the end point using
   * Bresenham's algorithm.
   * The start and end points can be at any location with respect to the other.
   */
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color = WHITE);

  /** \brief
   * Draw a rectangle of a specified width and height.
   *
   * \param x The X coordinate of the upper left corner.
   * \param y The Y coordinate of the upper left corner.
   * \param w The width of the rectangle.
   * \param h The height of the rectangle.
   * \param color The color of the pixel (optional; defaults to WHITE).
   */
  void drawRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Draw a vertical line.
   *
   * \param x The X coordinate of the upper start point.
   * \param y The Y coordinate of the upper start point.
   * \param h The height of the line.
   * \param color The color of the line (optional; defaults to WHITE).
   */
  void drawFastVLine(int16_t x, int16_t y, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Draw a horizontal line.
   *
   * \param x The X coordinate of the left start point.
   * \param y The Y coordinate of the left start point.
   * \param w The width of the line.
   * \param color The color of the line (optional; defaults to WHITE).
   */
  void drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color = WHITE);

  /** \brief
   * Draw a filled-in rectangle of a specified width and height.
   *
   * \param x The X coordinate of the upper left corner.
   * \param y The Y coordinate of the upper left corner.
   * \param w The width of the rectangle.
   * \param h The height of the rectangle.
   * \param color The color of the pixel (optional; defaults to WHITE).
   */
  void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Fill the screen buffer with the specified color.
   *
   * \param color The fill color (optional; defaults to WHITE).
   */
  void fillScreen(uint8_t color = WHITE);

  /** \brief
   * Draw a rectangle with rounded corners.
   *
   * \param x The X coordinate of the left edge.
   * \param y The Y coordinate of the top edge.
   * \param w The width of the rectangle.
   * \param h The height of the rectangle.
   * \param r The radius of the semicircles forming the corners.
   * \param color The color of the rectangle (optional; defaults to WHITE).
   */
  void drawRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color = WHITE);

  /** \brief
   * Draw a filled-in rectangle with rounded corners.
   *
   * \param x The X coordinate of the left edge.
   * \param y The Y coordinate of the top edge.
   * \param w The width of the rectangle.
   * \param h The height of the rectangle.
   * \param r The radius of the semicircles forming the corners.
   * \param color The color of the rectangle (optional; defaults to WHITE).
   */
  void fillRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color = WHITE);

  /** \brief
   * Draw a triangle given the coordinates of each corner.
   *
   * \param x0,x1,x2 The X coordinates of the corners.
   * \param y0,y1,y2 The Y coordinates of the corners.
   * \param color The triangle's color (optional; defaults to WHITE).
   *
   * \details
   * A triangle is drawn by specifying each of the three corner locations.
   * The corners can be at any position with respect to the others.
   */
  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color = WHITE);

  /** \brief
   * Draw a filled-in triangle given the coordinates of each corner.
   *
   * \param x0,x1,x2 The X coordinates of the corners.
   * \param y0,y1,y2 The Y coordinates of the corners.
   * \param color The triangle's color (optional; defaults to WHITE).
   *
   * \details
   * A triangle is drawn by specifying each of the three corner locations.
   * The corners can be at any position with respect to the others.
   */
  void fillTriangle (int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color = WHITE);

  /** \brief
   * Draw a bitmap from an array in program memory.
   *
   * \param x The X coordinate of the top left pixel affected by the bitmap.
   * \param y The Y coordinate of the top left pixel affected by the bitmap.
   * \param bitmap A pointer to the bitmap array in program memory.
   * \param w The width of the bitmap in pixels.
   * \param h The height of the bitmap in pixels.
   * \param color The color of pixels for bits set to 1 in the bitmap.
   *              If the value is INVERT, bits set to 1 will invert the
   *              corresponding pixel. (optional; defaults to WHITE).
   *
   * \details
   * Bits set to 1 in the provided bitmap array will have their corresponding
   * pixel set to the specified color. For bits set to 0 in the array, the
   * corresponding pixel will be left unchanged.
   *
   * Each byte in the array specifies a vertical column of 8 pixels, with the
   * least significant bit at the top.
   *
   * The array must be located in program memory by using the PROGMEM modifier.
   */
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Draw a bitmap from a horizontally oriented array in program memory.
   *
   * \param x The X coordinate of the top left pixel affected by the bitmap.
   * \param y The Y coordinate of the top left pixel affected by the bitmap.
   * \param bitmap A pointer to the bitmap array in program memory.
   * \param w The width of the bitmap in pixels.
   * \param h The height of the bitmap in pixels.
   * \param color The color of pixels for bits set to 1 in the bitmap.
   *              (optional; defaults to WHITE).
   *
   * \details
   * Bits set to 1 in the provided bitmap array will have their corresponding
   * pixel set to the specified color. For bits set to 0 in the array, the
   * corresponding pixel will be left unchanged.
   *
   * Each byte in the array specifies a horizontal row of 8 pixels, with the
   * most significant bit at the left end of the row.
   *
   * The array must be located in program memory by using the PROGMEM modifier.
   *
   * \note
   * This function requires a lot of additional CPU power and will draw images
   * slower than `drawBitmap()`, which uses bitmaps that are stored in a format
   * that allows them to be directly written to the screen. It is recommended
   * you use `drawBitmap()` when possible.
   */
  void drawSlowXYBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Draw a bitmap from an array of compressed data.
   *
   * \param sx The X coordinate of the top left pixel affected by the bitmap.
   * \param sy The Y coordinate of the top left pixel affected by the bitmap.
   * \param bitmap A pointer to the compressed bitmap array in program memory.
   * \param color The color of pixels for bits set to 1 in the bitmap.
   *
   * \details
   * Draw a bitmap starting at the given coordinates from an array that has
   * been compressed using an algorthm implemented by Team A.R.G.
   * For more information see:
   * https://github.com/TEAMarg/drawCompressed
   * https://github.com/TEAMarg/Cabi
   *
   * Bits set to 1 in the provided bitmap array will have their corresponding
   * pixel set to the specified color. For bits set to 0 in the array, the
   * corresponding pixel will be left unchanged.
   *
   * The array must be located in program memory by using the PROGMEM modifier.
   */
  void drawCompressed(int16_t sx, int16_t sy, const uint8_t *bitmap, uint8_t color = WHITE);

  /** \brief
   * Get a pointer to the display buffer in RAM.
   *
   * \return A pointer to the display buffer array in RAM
   *
   * \details
   * The location of the display buffer in RAM, which is displayed using
   * `display()`, can be gotten using this function. The buffer can then be
   *  read and directly manipulated.
   */
  uint8_t* getBuffer();


  protected:
  // Screen buffer
  static uint8_t sBuffer[(HEIGHT*WIDTH)/8];

};

#endif
