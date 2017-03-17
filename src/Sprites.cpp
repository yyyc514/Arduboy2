/**
 * @file Sprites.cpp
 * \brief
 * A class for drawing animated sprites from image and mask bitmaps.
 */

#include "Sprites.h"

RotationVector::RotationVector(int d) :
degreez(d) {

  this->degreez = normalize(d);
  this->cosFractional = this->cos(d);
  this->sinFractional = this->sin(d);
}

// (0..89).to_a.map { |x| (Math.sin(x.degrees)*128).round }
int8_t const PROGMEM sinTable[] = {0, 2, 4, 7, 9, 11, 13, 15, 18, 20, 22,
24, 26, 29, 31, 33, 35, 37, 39, 41, 43, 46, 48, 50, 52, 54, 56, 58, 60, 62,
63, 65, 67, 69, 71, 73, 75, 76, 78, 80, 82, 83, 85, 87, 88, 90, 91, 93, 94,
96, 97, 99, 100, 101, 103, 104, 105, 107, 108, 109, 110, 111, 112, 113,
114, 115, 116, 117, 118, 119, 119, 120, 121, 121, 122, 123, 123, 124, 124,
125, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 127 };

int16_t RotationVector::normalize(int16_t degrees) {
  if (degrees < 0) {
    return (degrees%360)+360;
  } else {
    return degrees;
  }
}


int8_t RotationVector::cos(int16_t degrees) {
  // rotate 90 degrees and then we can just ask sin
  return sin((degrees+90)%360);
}

int8_t RotationVector::sin(int16_t degrees) {
  int8_t modifier = 1;

  if (degrees >= 180) {
    modifier =- 1;
    degrees %= 180;
  }
  if (degrees >= 90) {
    // rotate 90-179 backwards onto 89-0, ie 100 becomes 80, tec.
    degrees = 90 - (degrees - 90);
  // } else if (degrees >= 85) {
    // return 0;
  }
  return pgm_read_byte(sinTable + degrees) * modifier;
}

Coord RotationVector::transform(int x, int y)
{
  Coord result;

  result.x = (x * this->cosFractional + y * this->sinFractional) >> 7;
  result.y = (y * this->cosFractional + x * -this->sinFractional) >> 7;

  return result;
}

void Sprites::drawExternalMask(int16_t x, int16_t y, const uint8_t *bitmap,
                               const uint8_t *mask, uint8_t frame, uint8_t mask_frame)
{
  draw(x, y, bitmap, frame, mask, mask_frame, SPRITE_MASKED);
}

void Sprites::drawOverwrite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  draw(x, y, bitmap, frame, NULL, 0, SPRITE_OVERWRITE);
}

void Sprites::drawErase(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  draw(x, y, bitmap, frame, NULL, 0, SPRITE_IS_MASK_ERASE);
}

void Sprites::drawSelfMasked(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  draw(x, y, bitmap, frame, NULL, 0, SPRITE_IS_MASK);
}

void Sprites::drawPlusMask(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  draw(x, y, bitmap, frame, NULL, 0, SPRITE_PLUS_MASK);
}

void Sprites::drawRotated(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame,
  uint16_t degrees)
{
  int16_t xOffset;
  int8_t yOffset;
  RotationVector v(degrees);
  Coord xy;
  int8_t xCenter, yCenter;
  int16_t plotX, plotY;
  int16_t cursorX, cursorY;

  if (bitmap == NULL)
    return;

  uint8_t width = pgm_read_byte(bitmap++);
  uint8_t height = pgm_read_byte(bitmap++);

  // setup rotational transforms
  boolean negativeCos, negativeSin;
  negativeCos = v.cosFractional < 0;
  negativeSin = v.sinFractional < 0;
  uint8_t ucf, usf;
  ucf = abs(v.cosFractional)*2;
  usf = abs(v.sinFractional)*2;

  // setup offsets
  xCenter = width / 2;
  yCenter = height / 2;

  xy = v.transform(xCenter, yCenter);
  xOffset = xCenter - xy.x;
  yOffset = yCenter - xy.y;

  // bump the number for our fixed 7-bit floating point math
  cursorX = xOffset * 256;
  cursorY = yOffset * 256;

  uint8_t pixels, color;
  uint8_t ix, iy;
  uint16_t xofs;
  for (uint8_t x=0; x< width; x++) {
    xofs = bitmap + x;
    plotX = cursorX;
    plotY = cursorY;
    // every 8 pixels we need to load more pixel data from the bitmap
    for (uint8_t y=0; y < height; y++ ) {
      if (y%8==0) {
        pixels = pgm_read_byte(xofs);
        xofs += width;
      }
      color = pixels & 0x01;
      pixels >>= 1;

      ix = *((unsigned char *) (&plotX) + 1);
      iy = *((unsigned char *) (&plotY) + 1);
      // Arduboy2Base::drawPixel(ix, iy, color);
      // BEGIN inline drawpixel
      uint16_t pofs;
      // if (!(ix < 0 || ix > (WIDTH-1) || iy < 0 || iy > (HEIGHT-1))) {
      if (!(ix > (WIDTH-1) || iy > (HEIGHT-1))) {
        uint8_t row = iy / 8;
        pofs = ((uint16_t)row*WIDTH) + ix;
        if (color) {
          Arduboy2Base::sBuffer[pofs] |=   _BV(iy % 8);
        } else {
          Arduboy2Base::sBuffer[pofs] &= ~ _BV(iy % 8);
        }
      }
      // END inline drawpixel
      plotX += (negativeSin ? -usf : +usf);
      plotY += (negativeCos ? -ucf : +ucf);
    }
    // update cursor X and Y
    cursorX += (negativeCos ? -ucf : +ucf);
    cursorY += (negativeSin ? +usf : -usf); // sign purposely reversed
  }
}

//common functions
void Sprites::draw(int16_t x, int16_t y,
                   const uint8_t *bitmap, uint8_t frame,
                   const uint8_t *mask, uint8_t sprite_frame,
                   uint8_t drawMode)
{
  unsigned int frame_offset;

  if (bitmap == NULL)
    return;

  uint8_t width = pgm_read_byte(bitmap);
  uint8_t height = pgm_read_byte(++bitmap);
  bitmap++;
  if (frame > 0 || sprite_frame > 0) {
    frame_offset = (width * ( height / 8 + ( height % 8 == 0 ? 0 : 1)));
    // sprite plus mask uses twice as much space for each frame
    if (drawMode == SPRITE_PLUS_MASK) {
      frame_offset *= 2;
    } else if (mask != NULL) {
      mask += sprite_frame * frame_offset;
    }
    bitmap += frame * frame_offset;
  }

  // if we're detecting the draw mode then base it on whether a mask
  // was passed as a separate object
  if (drawMode == SPRITE_AUTO_MODE) {
    drawMode = mask == NULL ? SPRITE_UNMASKED : SPRITE_MASKED;
  }

  drawBitmap(x, y, bitmap, mask, width, height, drawMode);
}

void Sprites::drawBitmap(int16_t x, int16_t y,
                         const uint8_t *bitmap, const uint8_t *mask,
                         int8_t w, int8_t h, uint8_t draw_mode)
{
  // no need to draw at all of we're offscreen
  if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
    return;

  if (bitmap == NULL)
    return;

  // xOffset technically doesn't need to be 16 bit but the math operations
  // are measurably faster if it is
  uint16_t xOffset, ofs;
  int8_t yOffset = abs(y) % 8;
  int8_t sRow = y / 8;
  uint8_t loop_h, start_h, rendered_width;

  if (y < 0 && yOffset > 0) {
    sRow--;
    yOffset = 8 - yOffset;
  }

  // if the left side of the render is offscreen skip those loops
  if (x < 0) {
    xOffset = abs(x);
  } else {
    xOffset = 0;
  }

  // if the right side of the render is offscreen skip those loops
  if (x + w > WIDTH - 1) {
    rendered_width = ((WIDTH - x) - xOffset);
  } else {
    rendered_width = (w - xOffset);
  }

  // if the top side of the render is offscreen skip those loops
  if (sRow < -1) {
    start_h = abs(sRow) - 1;
  } else {
    start_h = 0;
  }

  loop_h = h / 8 + (h % 8 > 0 ? 1 : 0); // divide, then round up

  // if (sRow + loop_h - 1 > (HEIGHT/8)-1)
  if (sRow + loop_h > (HEIGHT / 8)) {
    loop_h = (HEIGHT / 8) - sRow;
  }

  // prepare variables for loops later so we can compare with 0
  // instead of comparing two variables
  loop_h -= start_h;

  sRow += start_h;
  ofs = (sRow * WIDTH) + x + xOffset;
  uint8_t *bofs = (uint8_t *)bitmap + (start_h * w) + xOffset;
  uint8_t *mask_ofs;
  if (mask != 0)
    mask_ofs = (uint8_t *)mask + (start_h * w) + xOffset;
  uint8_t data;

  uint8_t mul_amt = 1 << yOffset;
  uint16_t mask_data;
  uint16_t bitmap_data;

  switch (draw_mode) {
    case SPRITE_UNMASKED:
      // we only want to mask the 8 bits of our own sprite, so we can
      // calculate the mask before the start of the loop
      mask_data = ~(0xFF * mul_amt);
      // really if yOffset = 0 you have a faster case here that could be
      // optimized
      for (uint8_t a = 0; a < loop_h; a++) {
        for (uint8_t iCol = 0; iCol < rendered_width; iCol++) {
          bitmap_data = pgm_read_byte(bofs) * mul_amt;

          if (sRow >= 0) {
            data = Arduboy2Base::sBuffer[ofs];
            data &= (uint8_t)(mask_data);
            data |= (uint8_t)(bitmap_data);
            Arduboy2Base::sBuffer[ofs] = data;
          }
          if (yOffset != 0 && sRow < 7) {
            data = Arduboy2Base::sBuffer[ofs + WIDTH];
            data &= (*((unsigned char *) (&mask_data) + 1));
            data |= (*((unsigned char *) (&bitmap_data) + 1));
            Arduboy2Base::sBuffer[ofs + WIDTH] = data;
          }
          ofs++;
          bofs++;
        }
        sRow++;
        bofs += w - rendered_width;
        ofs += WIDTH - rendered_width;
      }
      break;

    case SPRITE_IS_MASK:
      for (uint8_t a = 0; a < loop_h; a++) {
        for (uint8_t iCol = 0; iCol < rendered_width; iCol++) {
          bitmap_data = pgm_read_byte(bofs) * mul_amt;
          if (sRow >= 0) {
            Arduboy2Base::sBuffer[ofs] |= (uint8_t)(bitmap_data);
          }
          if (yOffset != 0 && sRow < 7) {
            Arduboy2Base::sBuffer[ofs + WIDTH] |= (*((unsigned char *) (&bitmap_data) + 1));
          }
          ofs++;
          bofs++;
        }
        sRow++;
        bofs += w - rendered_width;
        ofs += WIDTH - rendered_width;
      }
      break;

    case SPRITE_IS_MASK_ERASE:
      for (uint8_t a = 0; a < loop_h; a++) {
        for (uint8_t iCol = 0; iCol < rendered_width; iCol++) {
          bitmap_data = pgm_read_byte(bofs) * mul_amt;
          if (sRow >= 0) {
            Arduboy2Base::sBuffer[ofs]  &= ~(uint8_t)(bitmap_data);
          }
          if (yOffset != 0 && sRow < 7) {
            Arduboy2Base::sBuffer[ofs + WIDTH] &= ~(*((unsigned char *) (&bitmap_data) + 1));
          }
          ofs++;
          bofs++;
        }
        sRow++;
        bofs += w - rendered_width;
        ofs += WIDTH - rendered_width;
      }
      break;

    case SPRITE_MASKED:
      for (uint8_t a = 0; a < loop_h; a++) {
        for (uint8_t iCol = 0; iCol < rendered_width; iCol++) {
          // NOTE: you might think in the yOffset==0 case that this results
          // in more effort, but in all my testing the compiler was forcing
          // 16-bit math to happen here anyways, so this isn't actually
          // compiling to more code than it otherwise would. If the offset
          // is 0 the high part of the word will just never be used.

          // load data and bit shift
          // mask needs to be bit flipped
          mask_data = ~(pgm_read_byte(mask_ofs) * mul_amt);
          bitmap_data = pgm_read_byte(bofs) * mul_amt;

          if (sRow >= 0) {
            data = Arduboy2Base::sBuffer[ofs];
            data &= (uint8_t)(mask_data);
            data |= (uint8_t)(bitmap_data);
            Arduboy2Base::sBuffer[ofs] = data;
          }
          if (yOffset != 0 && sRow < 7) {
            data = Arduboy2Base::sBuffer[ofs + WIDTH];
            data &= (*((unsigned char *) (&mask_data) + 1));
            data |= (*((unsigned char *) (&bitmap_data) + 1));
            Arduboy2Base::sBuffer[ofs + WIDTH] = data;
          }
          ofs++;
          mask_ofs++;
          bofs++;
        }
        sRow++;
        bofs += w - rendered_width;
        mask_ofs += w - rendered_width;
        ofs += WIDTH - rendered_width;
      }
      break;


    case SPRITE_PLUS_MASK:
      // *2 because we use double the bits (mask + bitmap)
      bofs = (uint8_t *)(bitmap + ((start_h * w) + xOffset) * 2);

      uint8_t xi = rendered_width; // used for x loop below
      uint8_t yi = loop_h; // used for y loop below

      asm volatile(
        "push r28\n" // save Y
        "push r29\n"
        "mov r28, %A[buffer_page2_ofs]\n" // Y = buffer page 2 offset
        "mov r29, %B[buffer_page2_ofs]\n"
        "loop_y:\n"
        "loop_x:\n"
        // load bitmap and mask data
        "lpm %A[bitmap_data], Z+\n"
        "lpm %A[mask_data], Z+\n"

        // shift mask and buffer data
        "tst %[yOffset]\n"
        "breq skip_shifting\n"
        "mul %A[bitmap_data], %[mul_amt]\n"
        "mov %A[bitmap_data], r0\n"
        "mov %B[bitmap_data], r1\n"
        "mul %A[mask_data], %[mul_amt]\n"
        "mov %A[mask_data], r0\n"
        // "mov %B[mask_data], r1\n"


        // SECOND PAGE
        // if yOffset != 0 && sRow < 7
        "cpi %[sRow], 7\n"
        "brge end_second_page\n"
        // then
        "ld %[data], Y\n"
        // "com %B[mask_data]\n" // invert high byte of mask
        "com r1\n"
        "and %[data], r1\n" // %B[mask_data]
        "or %[data], %B[bitmap_data]\n"
        // update buffer, increment
        "st Y+, %[data]\n"

        "end_second_page:\n"
        "skip_shifting:\n"


        // FIRST PAGE
        "ld %[data], %a[buffer_ofs]\n"
        // if sRow >= 0
        "tst %[sRow]\n"
        "brmi end_first_page\n"
        // then
        "com %A[mask_data]\n"
        "and %[data], %A[mask_data]\n"
        "or %[data], %A[bitmap_data]\n"

        "end_first_page:\n"
        // update buffer, increment
        "st %a[buffer_ofs]+, %[data]\n"


        // "x_loop_next:\n"
        "dec %[xi]\n"
        "brne loop_x\n"

        // increment y
        "next_loop_y:\n"
        "dec %[yi]\n"
        "breq finished\n"
        "mov %[xi], %[x_count]\n" // reset x counter
        // sRow++;
        "inc %[sRow]\n"
        "clr __zero_reg__\n"
        // sprite_ofs += (w - rendered_width) * 2;
        "add %A[sprite_ofs], %A[sprite_ofs_jump]\n"
        "adc %B[sprite_ofs], __zero_reg__\n"
        // buffer_ofs += WIDTH - rendered_width;
        "add %A[buffer_ofs], %A[buffer_ofs_jump]\n"
        "adc %B[buffer_ofs], __zero_reg__\n"
        // buffer_ofs_page_2 += WIDTH - rendered_width;
        "add r28, %A[buffer_ofs_jump]\n"
        "adc r29, __zero_reg__\n"

        "rjmp loop_y\n"
        "finished:\n"
        // put the Y register back in place
        "pop r29\n"
        "pop r28\n"
        "clr __zero_reg__\n" // just in case
        : [xi] "+&r" (xi),
        [yi] "+&r" (yi),
        [sRow] "+&a" (sRow), // CPI requires an upper register
        [data] "+&r" (data),
        [mask_data] "+&r" (mask_data),
        [bitmap_data] "+&r" (bitmap_data)
        :
        [x_count] "r" (rendered_width),
        [y_count] "r" (loop_h),
        [sprite_ofs] "z" (bofs),
        [buffer_ofs] "x" (Arduboy2Base::sBuffer+ofs),
        [buffer_page2_ofs] "r" (Arduboy2Base::sBuffer+ofs+WIDTH), // Y pointer
        [buffer_ofs_jump] "r" (WIDTH-rendered_width),
        [sprite_ofs_jump] "r" ((w-rendered_width)*2),
        [yOffset] "r" (yOffset),
        [mul_amt] "r" (mul_amt)
        :
      );
      break;
  }
}
