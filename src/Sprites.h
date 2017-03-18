/**
 * @file Sprites.h
 * \brief
 * A class for drawing animated sprites from image and mask bitmaps.
 */

#ifndef Sprites_h
#define Sprites_h

#include "Arduboy2.h"

#define SPRITE_MASKED 1
#define SPRITE_UNMASKED 2
#define SPRITE_OVERWRITE 2
#define SPRITE_PLUS_MASK 3
#define SPRITE_IS_MASK 250
#define SPRITE_IS_MASK_ERASE 251
#define SPRITE_AUTO_MODE 255

struct Coord
{
    int16_t x;
    int8_t y;
};

/** \brief
 * A class to help support rotational transforms (by degrees).
*/
class RotationVector
{
public:
    RotationVector(int16_t d);

    /** \brief
     * Perform a raw rotational tranform of the given coordinates.
     *
     * \param x Origin X coordinate.
     * \param y Origin Y coordinate.
     * \return a Coord struct with the transformed X and Y coordinates.
     *
     * \details
     * This assumes 0,0 is the center of the rotation.  If your sprite
     * is 64x64 then when rotated 180 degrees the new "top left" visible
     * corner would be at -63, -63.  If you want to rotate around the
     * center of your image you'll need to calcuate the offsets yourself.
    */
    Coord transform(int16_t x, int16_t y);

    /** \brief
     * Calculates a 8-bit signed cosine fractional value.
     *
     * \param degrees The degrees you wish to calculate the cosine of.
     *
     * \details
     * The value returned is in the range of -127 to 127, corresponding
     * with actual floating point values of -1.0 to 1.0.
     *
     * This is intended to be used with 16 bit signed ints serving to
     * hold fixed floating point values - the high byte storing the integer
     * portion and the low byte storing the fractional. Or 9 high bits,
     * and 7 low bits (as in the example below).
     *
     * Example:
     * Assume our cosine is 0.5 (64).
     *
     * // shift left 7 bits to allow room for our fractional
     * x = x << 7;
     * // add the fractional
     * x += 64;
     * // shift the fractional bytes back off to get the integer portion
     * x = x >> 7;
     *
     * Note in this example the value of X wouldn't actually change. To
     * see a fractional difference you'd have to be iterating inside a loop
     * and then converting X back to an integer over time to observe the
     * change adding up.
    */
    static int8_t cos(int16_t degrees);

    /** \brief
     * Calculates a 8-bit signed sine fractional value.
     *
     * \param degrees The degrees you wish to calculate the sine of.
     *
     * \details
     * The value returned is in the range of -127 to 127, corresponding
     * with actual floating point values of -1.0 to 1.0.
     *
     * See cos docs for further details.
    */
    static int8_t sin(int16_t degrees);

    /** \brief
     * Converts negative degree amounts to the equivalent positive rotation.
     * ie, -30 becomes 330.
     *
     * \param degrees The amount you are wanting to normalize.
     *
    */
    static int16_t normalize(int16_t degrees);

    int8_t cosFractional;
    int8_t sinFractional;
    int16_t degrees;


private:

};

/** \brief
 * A class for drawing animated sprites from image and mask bitmaps.
 *
 * \details
 * The functions in this class will draw to the screen buffer an image
 * contained in an array located in program memory. A mask can also be
 * specified or implied, which dictates how existing pixels in the buffer,
 * within the image boundaries, will be affected.
 *
 * A sprite or mask array contains one or more "frames". Each frame is intended
 * to show whatever the sprite represents in a different position, such as the
 * various poses for a running or jumping character. By specifying a different
 * frame each time the sprite is drawn, it can be animated.
 *
 * Each array begins with values for the width and height of the sprite, in
 * pixels. The width can be any value. The height must be a multiple of
 * 8 pixels, but with proper masking, a sprite of any height can be created.
 *
 * After the width and height values, the remainder of the array contains the
 * image and/or mask data for each frame. Each byte represents a vertical
 * column of 8 pixels with the least significant bit (bit 0) at the top.
 * The bytes are drawn as 8 pixel high rows from left to right, top to bottom.
 * When the end of a row is reached, as specified by the width value, the next
 * byte in the array will be the start of the next row.
 *
 * Data for each frame after the first one immediately follows the previous
 * frame. Frame numbers start at 0.
 */
class Sprites
{
  public:
    /** \brief
     * Draw a sprite using a separate image and mask array.
     *
     * \param x,y The coordinates of the top left pixel location.
     * \param bitmap A pointer to the array containing the image frames.
     * \param mask A pointer to the array containing the mask frames.
     * \param frame The frame number of the image to draw.
     * \param mask_frame The frame number for the mask to use (can be different
     * from the image frame number).
     *
     * \details
     * An array containing the image frames, and another array containing
     * corresponding mask frames, are used to draw a sprite.
     *
     * Bits set to 1 in the mask indicate that the pixel will be set to the
     * value of the corresponding image bit. Bits set to 0 in the mask will be
     * left unchanged.
     *
     *     image  mask   before  after
     *
     *     .....  .OOO.  .....   .....
     *     ..O..  OOOOO  .....   ..O..
     *     OO.OO  OO.OO  .....   OO.OO
     *     ..O..  OOOOO  .....   ..O..
     *     .....  .OOO.  .....   .....
     *
     *     image  mask   before  after
     *
     *     .....  .OOO.  OOOOO   O...O
     *     ..O..  OOOOO  OOOOO   ..O..
     *     OO.OO  OOOOO  OOOOO   OO.OO
     *     ..O..  OOOOO  OOOOO   ..O..
     *     .....  .OOO.  OOOOO   O...O
     */
    void drawExternalMask(int16_t x, int16_t y, const uint8_t *bitmap,
                          const uint8_t *mask, uint8_t frame, uint8_t mask_frame);

    /** \brief
     * Draw a sprite using an array containing both image and mask values.
     *
     * \param x,y The coordinates of the top left pixel location.
     * \param bitmap A pointer to the array containing the image/mask frames.
     * \param frame The frame number of the image to draw.
     *
     * \details
     * An array containing combined image and mask data is used to draw a
     * sprite. Bytes are given in pairs with the first byte representing the
     * image pixels and the second byte specifying the corresponding mask.
     * The width given in the array still specifies the image width, so each
     * row of image and mask bytes will be twice the width value.
     *
     * Bits set to 1 in the mask indicate that the pixel will be set to the
     * value of the corresponding image bit. Bits set to 0 in the mask will be
     * left unchanged.
     *
     *     image  mask   before  after
     *
     *     .....  .OOO.  .....   .....
     *     ..O..  OOOOO  .....   ..O..
     *     OO.OO  OO.OO  .....   OO.OO
     *     ..O..  OOOOO  .....   ..O..
     *     .....  .OOO.  .....   .....
     *
     *     image  mask   before  after
     *
     *     .....  .OOO.  OOOOO   O...O
     *     ..O..  OOOOO  OOOOO   ..O..
     *     OO.OO  OOOOO  OOOOO   OO.OO
     *     ..O..  OOOOO  OOOOO   ..O..
     *     .....  .OOO.  OOOOO   O...O
     */
    void drawPlusMask(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame);

    /** \brief
     * Draw a sprite by replacing the existing content completely.
     *
     * \param x,y The coordinates of the top left pixel location.
     * \param bitmap A pointer to the array containing the image frames.
     * \param frame The frame number of the image to draw.
     *
     * \details
     * A sprite is drawn by overwriting the pixels in the buffer with the data
     * from the specified frame in the array. No masking is done. A bit set
     * to 1 in the frame will set the pixel to 1 in the buffer, and a 0 in the
     * array will set a 0 in the buffer.
     *
     *     image  before  after
     *
     *     .....  .....   .....
     *     ..O..  .....   ..O..
     *     OO.OO  .....   OO.OO
     *     ..O..  .....   ..O..
     *     .....  .....   .....
     *
     *     image  before  after
     *
     *     .....  OOOOO   .....
     *     ..O..  OOOOO   ..O..
     *     OO.OO  OOOOO   OO.OO
     *     ..O..  OOOOO   ..O..
     *     .....  OOOOO   .....
     */
    void drawOverwrite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame);

    /** \brief
     * "Erase" a sprite.
     *
     * \param x,y The coordinates of the top left pixel location.
     * \param bitmap A pointer to the array containing the image frames.
     * \param frame The frame number of the image to erase.
     *
     * \details
     * The data from the specified frame in the array is used to erase a
     * sprite. To "erase" a sprite, bits set to 1 in the frame will set the
     * corresponding pixel in the buffer to 0. Frame bits set to 0 will remain
     * unchanged in the buffer.
     *
     *     image  before  after
     *
     *     .....  .....   .....
     *     ..O..  .....   .....
     *     OO.OO  .....   .....
     *     ..O..  .....   .....
     *     .....  .....   .....
     *
     *     image  before  after
     *
     *     .....  OOOOO   OOOOO
     *     ..O..  OOOOO   OO.OO
     *     OO.OO  OOOOO   ..O..
     *     ..O..  OOOOO   OO.OO
     *     .....  OOOOO   OOOOO
     */
    void drawErase(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame);

    /** \brief
     * Draw a sprite using only the bits set to 1.
     *
     * \param x,y The coordinates of the top left pixel location.
     * \param bitmap A pointer to the array containing the image frames.
     * \param frame The frame number of the image to draw.
     *
     * \details
     * Bits set to 1 in the frame will be used to draw the sprite by setting
     * the corresponding pixel in the buffer to 1. Bits set to 0 in the frame
     * will remain unchanged in the buffer.
     *
     *     image  before  after
     *
     *     .....  .....   .....
     *     ..O..  .....   ..O..
     *     OO.OO  .....   OO.OO
     *     ..O..  .....   ..O..
     *     .....  .....   .....
     *
     *     image  before  after
     *
     *     .....  OOOOO   OOOOO  (no change because all pixels were
     *     ..O..  OOOOO   OOOOO  already white)
     *     OO.OO  OOOOO   OOOOO
     *     ..O..  OOOOO   OOOOO
     *     .....  OOOOO   OOOOO
     */
    void drawSelfMasked(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame);


    /** \brief
     * Draw the given sprite rotated by a given number of degrees and
     * scaled as desired.
     *
     * \param x,y The coordinates of the top left pixel location.
     * \param bitmap A pointer to the array containing the image frames.
     * \param frame The frame number of the image to draw.
     * \param degrees The number of degrees to rotate the image.
     * \param scale The scale at which to draw the image (100 is 100%).
     *
     * \details
     * Auto-centering is performed so that x, y is always [roughly] the top
     * left of the rendering so that to rotate an image in place you call
     * this function with the same (x, y) and vary only the degrees.
     *
     * Currently supported are SPRITE_OVERWRITE and SPRITE_IS_MASK.
     *
     * Notes:
     * 1. For smaller sprites the difference between individual degrees may
     *    not be visibly noticeable.
     * 2. Scaling is intended for reducing not enlarging. If you provide a
     *    scale > 100 then your image will start to have a lot of dead
     *    space - ie, pixels are only transformed, not enlarged.
     *
     * Performance: This can rotate a full-screen image at around 20fps and
     * can rotate a 32x32 image at 90-100fps.
    */
    void drawRotatedOverwrite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame,
      uint16_t degrees, uint8_t scale = 100, uint8_t drawMode = SPRITE_OVERWRITE);


    // Master function. Needs to be abstracted into separate function for
    // every render type.
    // (Not officially part of the API)
    void draw(int16_t x, int16_t y,
              const uint8_t *bitmap, uint8_t frame,
              const uint8_t *mask, uint8_t sprite_frame,
              uint8_t drawMode);

    // (Not officially part of the API)
    void drawBitmap(int16_t x, int16_t y,
                    const uint8_t *bitmap, const uint8_t *mask,
                    int8_t w, int8_t h, uint8_t draw_mode);
};

#endif
