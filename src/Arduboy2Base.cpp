#include "ab_logo.c"
#include "Arduboy2Base.h"

//========================================
//========== class Arduboy2Base ==========
//========================================

uint8_t Arduboy2Base::sBuffer[];

Arduboy2Base::Arduboy2Base()
{
  currentButtonState = 0;
  previousButtonState = 0;
  // frame management
  setFrameRate(60);
  frameCount = -1;
  nextFrameStart = 0;
  justRendered = false;
  // init not necessary, will be reset after first use
  // lastFrameStart
  // lastFrameDurationMs
}

// functions called here should be public so users can create their
// own init functions if they need different behavior than `begin`
// provides by default
void Arduboy2Base::begin()
{
  boot(); // raw hardware

  blank(); // blank the display

  flashlight(); // light the RGB LED and screen if UP button is being held.

  // check for and handle buttons held during start up for system control
  systemButtons();

  bootLogo();

  audio.begin();
}

void Arduboy2Base::flashlight()
{
  if(!pressed(UP_BUTTON)) {
    return;
  }

  sendLCDCommand(OLED_ALL_PIXELS_ON); // smaller than allPixelsOn()
  digitalWriteRGB(RGB_ON, RGB_ON, RGB_ON);

  while(!pressed(DOWN_BUTTON)) {
    idle();
  }

  digitalWriteRGB(RGB_OFF, RGB_OFF, RGB_OFF);
  sendLCDCommand(OLED_PIXELS_FROM_RAM);
}

void Arduboy2Base::systemButtons() {
  while (pressed(B_BUTTON)) {
    digitalWrite(BLUE_LED, RGB_ON); // turn on blue LED
    sysCtrlSound(UP_BUTTON + B_BUTTON, GREEN_LED, 0xff);
    sysCtrlSound(DOWN_BUTTON + B_BUTTON, RED_LED, 0);
    delay(200);
  }

  digitalWrite(BLUE_LED, RGB_OFF); // turn off blue LED
}

void Arduboy2Base::sysCtrlSound(uint8_t buttons, uint8_t led, uint8_t eeVal) {
  if (pressed(buttons)) {
    digitalWrite(BLUE_LED, RGB_OFF); // turn off blue LED
    delay(200);
    digitalWrite(led, RGB_ON); // turn on "acknowledge" LED
    EEPROM.update(EEPROM_AUDIO_ON_OFF, eeVal);
    delay(500);
    digitalWrite(led, RGB_OFF); // turn off "acknowledge" LED

    while (pressed(buttons)) {} // Wait for button release
  }
}

void Arduboy2Base::bootLogo()
{
  digitalWrite(RED_LED, RGB_ON);

  for(int8_t y = -18; y <= 24; y++) {
    if (y == -4) {
      digitalWriteRGB(RGB_OFF, RGB_ON, RGB_OFF); // green LED on
    }
    else if (y == 24) {
      digitalWriteRGB(RGB_OFF, RGB_OFF, RGB_ON); // blue LED on
    }

    clear();
    // drawBitmap(20, y, arduboy_logo, 88, 16, WHITE);
    display();
    delay(27);
    // longer delay post boot, we put it inside the loop to
    // save the flash calling clear/delay again outside the loop
    if (y==-16) {
      delay(250);
    }
  }

  delay(750);
  digitalWrite(BLUE_LED, RGB_OFF);
}

void Arduboy2Base::clear() {

}

void Arduboy2Base::display() {
}


/* Frame management */

void Arduboy2Base::setFrameRate(uint8_t rate)
{
  eachFrameMillis = 1000 / rate;
}

bool Arduboy2Base::everyXFrames(uint8_t frames)
{
  return frameCount % frames == 0;
}

bool Arduboy2Base::nextFrame()
{
  unsigned long now = millis();
  bool tooSoonForNextFrame = now < nextFrameStart;

  if (justRendered) {
    lastFrameDurationMs = now - lastFrameStart;
    justRendered = false;
    return false;
  }
  else if (tooSoonForNextFrame) {
    // if we have MORE than 1ms to spare (hence our comparison with 2),
    // lets sleep for power savings.  We don't compare against 1 to avoid
    // potential rounding errors - say we're actually 0.5 ms away, but a 1
    // is returned if we go to sleep we might sleep a full 1ms and then
    // we'd be running the frame slighly late.  So the last 1ms we stay
    // awake for perfect timing.

    // This is likely trading power savings for absolute timing precision
    // and the power savings might be the better goal. At 60 FPS trusting
    // chance here might actually achieve a "truer" 60 FPS than the 16ms
    // frame duration we get due to integer math.

    // We should be woken up by timer0 every 1ms, so it's ok to sleep.
    if ((uint8_t)(nextFrameStart - now) >= 2)
      idle();

    return false;
  }

  // pre-render
  justRendered = true;
  lastFrameStart = now;
  nextFrameStart = now + eachFrameMillis;
  frameCount++;

  return true;
}

int Arduboy2Base::cpuLoad()
{
  return lastFrameDurationMs*100 / eachFrameMillis;
}

void Arduboy2Base::initRandomSeed()
{
  power_adc_enable(); // ADC on
  randomSeed(~rawADC(ADC_TEMP) * ~rawADC(ADC_VOLTAGE) * ~micros() + micros());
  power_adc_disable(); // ADC off
}

uint16_t Arduboy2Base::rawADC(uint8_t adc_bits)
{
  ADMUX = adc_bits;
  // we also need MUX5 for temperature check
  if (adc_bits == ADC_TEMP) {
    ADCSRB = _BV(MUX5);
  }

  delay(2); // Wait for ADMUX setting to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  return ADC;
}


bool Arduboy2Base::pressed(uint8_t buttons)
{
  return (buttonsState() & buttons) == buttons;
}

bool Arduboy2Base::notPressed(uint8_t buttons)
{
  return (buttonsState() & buttons) == 0;
}

void Arduboy2Base::pollButtons()
{
  previousButtonState = currentButtonState;
  currentButtonState = buttonsState();
}

bool Arduboy2Base::justPressed(uint8_t button)
{
  return (!(previousButtonState & button) && (currentButtonState & button));
}

bool Arduboy2Base::justReleased(uint8_t button)
{
  return ((previousButtonState & button) && !(currentButtonState & button));
}

bool Arduboy2Base::collide(Point point, Rect rect)
{
  return ((point.x >= rect.x) && (point.x < rect.x + rect.width) &&
      (point.y >= rect.y) && (point.y < rect.y + rect.height));
}

bool Arduboy2Base::collide(Rect rect1, Rect rect2)
{
  return !(rect2.x                >= rect1.x + rect1.width  ||
           rect2.x + rect2.width  <= rect1.x                ||
           rect2.y                >= rect1.y + rect1.height ||
           rect2.y + rect2.height <= rect1.y);
}

void Arduboy2Base::swap(int16_t& a, int16_t& b)
{
  int16_t temp = a;
  a = b;
  b = temp;
}
