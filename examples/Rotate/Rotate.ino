/*
Buttons example
June 11, 2015
Copyright (C) 2015 David Martinez
All rights reserved.
This code is the most basic barebones code for showing how to use buttons in
Arduboy.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
*/

#include <Arduboy2.h>
#include "../assets.h"

// Make an instance of arduboy used for many functions
Arduboy2 arduboy;
Sprites sprites;

// This function runs once in your game.
// use it for anything that needs to be set only once in your game.
void setup() {
  //initiate arduboy instance
  arduboy.begin();

  // here we set the framerate to 30, we do not need to run at default 60 and
  // it saves us battery life.
  // arduboy.setFrameRate(30);

  // set x and y to the middle of the screen
  // x = (WIDTH / 2) - (NUM_CHARS * CHAR_WIDTH / 2);
  // y = (HEIGHT / 2) - (CHAR_HEIGHT / 2);

}

long seconds = 0;
long iterations = 0;
long fps;
int degrees = 0;

// our main game loop, this runs once every cycle/frame.
// this is where our game logic goes.
void loop() {
  arduboy.clear();

  degrees++;
  if (degrees>=360) {
    degrees = 0;
  }
  RotationVector v(degrees);

  sprites.drawRotated(64,32,slack,0,degrees);
  // delay(10);

  // pause render until it's time for the next frame
  // if (!(arduboy.nextFrame()))
    // return;

  iterations++;
  seconds = millis()/1000;
  fps = iterations / seconds;

  arduboy.setCursor(0,0);
  arduboy.print("FPS: ");
  arduboy.println(fps);
  // arduboy.println(degrees);

  // arduboy.print("cos");
  // arduboy.println(v.cos(degrees));

  // arduboy.print("sin");
  // arduboy.println(v.sin(degrees));

  // then we finaly we tell the arduboy to display what we just wrote to the display.
  arduboy.display();
}
