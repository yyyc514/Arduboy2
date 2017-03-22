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


void setTimers(uint16_t loops, bool pause)
{
  uint8_t high = loops >> 8;
  uint8_t low = loops & 0xFF;
  // pause interrupt
  if (pause)
    TIMSK4 &= ~(1 << TOIE4); // overflow

  TCCR4A = 0;
  TCCR4B = 0;

  // OCR4D = 1;
  TC4H = high;  // top two bits are going to be set
  OCR4C = low; // set top of count range
  TCCR4B |= 0x08 ; // CK/128 prescaler

  // TIMSK4 |= (1 << OCIE4D);
  TIMSK4 |= (1 << TOIE4); // overflow
}

uint16_t top = 894;

// This function runs once in your game.
// use it for anything that needs to be set only once in your game.
void setup() {
  // initiate arduboy instance
  arduboy.begin();

  // here we set the framerate to 15, we do not need to run at
  // default 60 and it saves us battery life
  arduboy.setFrameRate(60);

  arduboy.LCDCommandMode();

  // contrast
  SPI.transfer(0x81);
  SPI.transfer(0xFF);
  // precharge
  SPI.transfer(0xD9);
  SPI.transfer(0xF1);
  arduboy.LCDDataMode();

  // 0xD9, 0xF1,

  setTimers(top, true);
}

// 126 FPS
// 1000 / 126 = every 7.936 ms
// every 7,936us

// CK/16 counts to 1024us (every 1us)
// CK/32 counts to 2048us (every 2us)
// CK/64 counts to 4096us (every 4us)
// CK/128 counts to 8192us (every 8us)

// ISR(TIMER4_COMPD_vect) {

// }


bool greyscale_rendered=false;
bool white_rendered=false;
bool render_done = false;
bool render_mode = false;

ISR(TIMER4_OVF_vect) {
  if (!render_done) {
    return; }

  arduboy.display();
  render_mode=!render_mode;
  render_done = false;
}

void white() {
  // paint shole screen white
  arduboy.clear();
  for(uint16_t i=128; i<1024; i++) {
    arduboy.sBuffer[i]=0xFF;
  }

  arduboy.setTextColor(WHITE);
  arduboy.setTextBackground(BLACK);
  arduboy.setCursor(64, 0);
  arduboy.println(top);
  // arduboy.println(millis());
}

void greyscale() {
  for(uint8_t x=0; x<64; x++) {
    for(uint16_t y=0; y<1024; y+=128) {
      arduboy.sBuffer[y+x]=0x00;
    }
  }

  arduboy.setCursor(0, 0);
  arduboy.println("Some grey");

  arduboy.setCursor(70, 16);
  arduboy.setTextColor(BLACK);
  arduboy.setTextBackground(WHITE);
  arduboy.println("Hello Josh!");
}


bool top_changed = false;
uint8_t held = 0;

// our main game loop, this runs once every cycle/frame.
// this is where our game logic goes.
void loop() {
  if (render_done)
    return;

  arduboy.pollButtons();

  if (held > 10) {
    if (arduboy.pressed(UP_BUTTON) ) {
      top+=1;
      top_changed = true;
    } else if (arduboy.pressed(DOWN_BUTTON)) {
      top-=1;
      top_changed = true;
    }
  }
  if (arduboy.buttonsState()) {
    held++;
  } else {
    held=0;
  }

  // first we clear our screen to black
  if (arduboy.justReleased(UP_BUTTON)) {
    top+=1;
    top_changed = true;
  } else if (arduboy.justReleased(DOWN_BUTTON)) {
    top-=1;
    top_changed = true;
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

