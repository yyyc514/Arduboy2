/*
Hello, World! example
June 11, 2015
Copyright (C) 2015 David Martinez
All rights reserved.
This code is the most basic barebones code for writing a program for Arduboy.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
*/

#include <Arduboy2.h>

// make an instance of arduboy used for many functions
Arduboy2 arduboy;

// 140 FPS (actual, I think)
// XXXX----XXXX----XXXX----
//  |   |   |   |   |   |
// 280 FPS
//   |   |   |   |   |   |
// XX--XX--XX--XX--XX--XX--
//  |   |   |   |   |   |

uint16_t voltage;

void setTimers(uint16_t loops, bool pause)
{
  uint8_t high = loops >> 8;
  uint8_t low = loops & 0xFF;
  // pause interrupt
  if (pause)
    TIMSK3 &= ~(1 << TOIE3); // overflow

  TCCR3A = 3;
  TCCR3B = 3 << 3;

  // OCR4D = 1;
  // TC4H = high;  // top two bits are going to be set
  // OCR4C = low; // set top of count range
  OCR3AH = high;
  OCR3AL = low;
  TCCR3B |= 2; // CLK / 8

  // TCCR4B |= 0x08 ; // CK/128 prescaler
  // TCCR4B |= 0x09 ; // CK/256 prescaler

  // TIMSK4 |= (1 << OCIE4D);
  TIMSK3 |= (1 << TOIE3); // overflow
}

// 138 FPS
// 1000 / 138 = every 7.246 ms
// every 7,936us

// CK count to 64us (every 1/16 of a us)
// CK/16 counts to 1024us (every 1us)
// CK/32 counts to 2048us (every 2us)
// CK/64 counts to 4096us (every 4us)
// CK/128 counts to 8192us (every 8us)
// CK/256 counts to 16192us (every 16us)

// ISR(TIMER4_COMPD_vect) {

// }

// uint16_t top = 905;
// uint16_t top = 766;
uint16_t top = 12256;
uint16_t top2 = 67;
boolean granular = true;


// This function runs once in your game.
// use it for anything that needs to be set only once in your game.
void setup() {
  // initiate arduboy instance
  arduboy.begin();

  // here we set the framerate to 15, we do not need to run at
  // default 60 and it saves us battery life
  arduboy.setFrameRate(60);

  arduboy.LCDCommandMode();

  // clock
  // 0xD5, 0xF0,
  SPI.transfer(0xD5);
  SPI.transfer(0xF0);

  // contrast
  SPI.transfer(0x81);
  SPI.transfer(0xFF);
  // precharge
  SPI.transfer(0xD9);
  // SPI.transfer(0xF1);
  SPI.transfer(0x16);
  arduboy.LCDDataMode();

  // 0xD9, 0xF1,

  setTimers(top, true);
  power_adc_enable();
}



bool greyscale_rendered=false;
bool white_rendered=false;
bool render_done = false;
uint8_t render_mode = 0;


// 138 FPS
// 1000 / 138 = every 7.246 ms
// every 7,936us

// CK count to 64us (every 1/16 of a us)
// CK/2 counts to 128us (every 1/8 a us)
// CK/4 counts to 256us (every 0.25us)
// CK/8 counts to 512us (every 0.5us)
// CK/16 counts to 1024us (every 1us)
// CK/32 counts to 2048us (every 2us)
// CK/64 counts to 4096us (every 4us)
// CK/128 counts to 8192us (every 8us)
// CK/256 counts to 16192us (every 16us)


// ISR(TIMER4_OVF_vect) {
ISR(TIMER3_OVF_vect) {
  uint8_t high = top >> 8;
  uint8_t low = top & 0xFF;

  // uint8_t high2 = top2 >> 8;
  // uint8_t low2 = top2 & 0xFF;

  // TIMSK4 &= ~(1 << TOIE4); // overflow
  // TCCR4B = 0;
  // if (granular) {
  //   TC4H = high2;  // top two bits are going to be set
  //   OCR4C = low2; // set top of count range
  //   TCCR4B = 0x02 + 64 ; // CK prescaler
  //   granular = !granular;
  //   TCNT4 = 0;
  //   TIMSK4 |= (1 << TOIE4); // overflow
  //   return;
  // } else {
  //   TC4H = high;  // top two bits are going to be set
  //   OCR4C = low; // set top of count range
  //   TCCR4B = 0x08 + 64; // CK/128 prescaler
  //   granular = !granular;
  //   TCNT4 = 0;
  // }
  // TIMSK4 |= (1 << TOIE4); // overflow

  if (!render_done) {
    return; }

  arduboy.display();
  render_mode++;
  render_mode %= 2;
  render_done = false;
}

void white() {
  // paint shole screen white
  arduboy.clear();
  for(uint16_t i=256; i<1024; i++) {
    arduboy.sBuffer[i]=0xFF;
  }

  arduboy.setTextColor(WHITE);
  arduboy.setTextBackground(BLACK);
  arduboy.setCursor(64, 0);
  arduboy.print(top);
  arduboy.setCursor(64, 8);
  arduboy.print(voltage);
  // arduboy.print(", ");
  // arduboy.println(top2);
  // arduboy.println(millis());
}

bool toggle;

void greyscale() {
  for(uint16_t y=0; y<1024; y+=128) {
    for(uint8_t x=0; x<64; x++) {
      // if (toggle) {
        arduboy.sBuffer[y+x]=00;
      //   x++;
      //   arduboy.sBuffer[y+x]=0b01010101;
      // } else {
      //   arduboy.sBuffer[y+x]=0b01010101;
      //   x++;
      //   arduboy.sBuffer[y+x]=0b10101010;
      // }
    }
  }
  toggle = !toggle;
  arduboy.setCursor(0, 0);
  arduboy.println("GRAY");
  // arduboy.println("more greys");
  // arduboy.println("Some greys");
  // arduboy.println("Some greys");
  // arduboy.println("Some greys");

  // arduboy.setCursor(70, 17);
  // arduboy.setTextColor(BLACK);
  // arduboy.setTextBackground(WHITE);
  // arduboy.println("Hello Josh!");
}


bool top_changed = false;
uint8_t held = 0;

uint8_t l = 30;
int8_t i = +1;

int8_t refresh=0x0F;

void simple() {
  arduboy.setFrameRate(120);
  if(!arduboy.nextFrame()) {
    return;
  }
  arduboy.pollButtons();
  arduboy.clear();

  for(uint8_t u=0; u<8; u++) {
    arduboy.sBuffer[u*128+l] = 0xFF;
  }

  // first we clear our screen to black
  if (arduboy.justReleased(UP_BUTTON)) {
    refresh+=1;
    top_changed = true;
  } else if (arduboy.justReleased(DOWN_BUTTON)) {
    refresh-=1;
    top_changed = true;
  }

  if (refresh>15)
    refresh = 15;

  if (refresh<0)
    refresh = 0;

  if (top_changed) {
    arduboy.LCDCommandMode();
    SPI.transfer(0xD5);
    SPI.transfer(refresh << 4);
    arduboy.LCDDataMode();
    top_changed=false;
  }
  arduboy.print(refresh);


  l+=i;
  if (l>=70) {
    i=-1;
  } else if (l<30) {
    i=+1;
  }

  arduboy.display();
}



// our main game loop, this runs once every cycle/frame.
// this is where our game logic goes.
void loop() {
  // simple();
  // return;



  if (render_done)
    return;

  if (arduboy.frameCount % 256 == 0) {
    voltage = arduboy.rawADC(ADC_VOLTAGE);
  }
  arduboy.frameCount++;

  arduboy.pollButtons();

  if (held > 20) {
    if (arduboy.pressed(UP_BUTTON) ) {
      top+=1;
      top_changed = true;
    } else if (arduboy.pressed(DOWN_BUTTON)) {
      top-=1;
      top_changed = true;
    }

    if (arduboy.pressed(LEFT_BUTTON)) {
      top2 -= 1;
    } else if (arduboy.pressed(RIGHT_BUTTON)) {
      top2 += 1;
    }

  }

  if (arduboy.buttonsState()) {
    held++;
  } else {
    held=0;
  }

  if (arduboy.justReleased(A_BUTTON)) {
    arduboy.LCDCommandMode();
    SPI.transfer(0xAE);
    SPI.transfer(0xAF);
    arduboy.LCDDataMode();
  }

  // first we clear our screen to black
  if (arduboy.justReleased(UP_BUTTON)) {
    top+=1;
    top_changed = true;
  } else if (arduboy.justReleased(DOWN_BUTTON)) {
    top-=1;
    top_changed = true;
  }

  if (arduboy.justReleased(LEFT_BUTTON)) {
    top2 -= 1;
  } else if (arduboy.justReleased(RIGHT_BUTTON)) {
    top2 += 1;
  }

  if (top_changed) {
   setTimers(top, false);
   top_changed = false;
  }

  if(render_mode) {
    // arduboy.clear();
    white();
  } else {
    greyscale();
  }
  render_done = true;

  // pause render until it's time for the next frame
  // if (!(arduboy.nextFrame())) {
  //   if (greyscale_rendered)
  //     return;

  //   // decide whether it's time to render the greyscle layer
  //   long now = millis();
  //   long diff = arduboy.nextFrameStart - now;
  //   if (diff < 7) {
  //     greyscale();
  //     greyscale_rendered=true;
  //     display_ready = true;
  //     // arduboy.display();
  //     return;
  //   }
  // }


  // then we print to screen what is in the Quotation marks ""
  // arduboy.print(F("Hello, world!"));

  // white();

  // we set our cursor 5 pixels to the right and 10 down from the top
  // (positions start at 0, 0)


  // then we finaly we tell the arduboy to display what we just wrote to the display
  // arduboy.display();
}

