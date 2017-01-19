/**
 * @file Arduboy2Base.h
 * \brief
 * The Arduboy2Base class and support objects and definitions.
 */

#ifndef ARDUBOY2BASE_H
#define ARDUBOY2BASE_H

#include <Arduino.h>
#include "Arduboy2Core.h"
#include "Arduboy2Audio.h"
// #include "Sprites.h"
#include <Print.h>
#include <limits.h>


#define CLEAR_BUFFER true /**< Value to be passed to `display()` to clear the screen buffer. */


/** \brief
 * A rectangle object for collision functions.
 *
 * \details
 * The X and Y coordinates specify the top left corner of a rectangle with the
 * given width and height.
 *
 * \see Arduboy2Base::collide(Point, Rect) Arduboy2Base::collide(Rect, Rect)
 */
struct Rect
{
  int16_t x;      /**< The X coordinate of the top left corner */
  int16_t y;      /**< The Y coordinate of the top left corner */
  uint8_t width;  /**< The width of the rectangle */
  uint8_t height; /**< The height of the rectangle */
};

/** \brief
 * An object to define a single point for collision functions.
 *
 * \details
 * The location of the point is given by X and Y coordinates.
 *
 * \see Arduboy2Base::collide(Point, Rect)
 */
struct Point
{
  int16_t x; /**< The X coordinate of the point */
  int16_t y; /**< The Y coordinate of the point */
};



//==================================
//========== Arduboy2Base ==========
//==================================

/** \brief
 * The main functions provided for writing sketches for the Arduboy,
 * _minus_ text output.
 *
 * This class in inherited by Arduboy2, so if text output functions are
 * required Arduboy2 should be used instead.
 *
 * \note
 * \parblock
 * An Arduboy2Audio class object named `audio` will be created by the
 * Arduboy2Base class, so there is no need for a sketch itself to create an
 * Arduboy2Audio object. Arduboy2Audio functions can be called using the
 * Arduboy2 or Arduboy2Base `audio` object.
 *
 * Example:
 *
 * \code
 * #include <Arduboy2.h>
 *
 * Arduboy2 arduboy;
 *
 * // Arduboy2Audio functions can be called as follows:
 *   arduboy.audio.on();
 *   arduboy.audio.off();
 * \endcode
 * \endparblock
 *
 * \note
 * \parblock
 * A friend class named _Arduboy2Ex_ is declared by this class. The intention
 * is to allow a sketch to create an _Arduboy2Ex_ class which would have access
 * to the private and protected members of the Arduboy2Base class. It is hoped
 * that this may eliminate the need to create an entire local copy of the
 * library, in order to extend the functionality, in most circumstances.
 * \endparblock
 *
 * \see Arduboy2
 */
class Arduboy2Base : public Arduboy2Core
{
 friend class Arduboy2Ex;
 friend class Sprites;

 public:
  Arduboy2Base();

  /** \brief
   * An object created to provide audio control functions within this class.
   *
   * \details
   * This object is created to eliminate the need for a sketch to create an
   * Arduboy2Audio class object itself.
   *
   * \see Arduboy2Audio
   */
  Arduboy2Audio audio;

  /** \brief
   * Initialize the hardware, display the boot logo, provide boot utilities, etc.
   *
   * \details
   * This function should be called once near the start of the sketch,
   * usually in `setup()`, before using any other functions in this class.
   * It initializes the display, displays the boot logo, provides "flashlight"
   * and system control features and initializes audio control.
   *
   * \note
   * To free up some code space for use by the sketch, `boot()` can be used
   * instead of `begin()` allow the elimination of some of the things that
   * aren't really required, such as displaying the boot logo.
   *
   * \see boot()
   */
  void begin();

  /** \brief
   * Flashlight mode turns the RGB LED and display fully on.
   *
   * \details
   * Checks if the UP button is pressed and if so turns the RGB LED and all
   * display pixels fully on. Pressing the DOWN button will exit flashlight mode.
   *
   * This function is called by `begin()` and can be called by a sketch
   * after `boot()`.
   *
   * \see begin() boot()
   */
  void flashlight();

  /** \brief
   * Handle buttons held on startup for system control.
   *
   * \details
   * This function is called by `begin()` and can be called by a sketch
   * after `boot()`.
   *
   * Hold the B button when booting to enter system control mode.
   * The B button must be held continuously to remain in this mode.
   * Then, pressing other buttons will perform system control functions:
   *
   * - UP: Set "sound enabled" in EEPROM
   * - DOWN: Set "sound disabled" (mute) in EEPROM
   *
   * \see begin() boot()
   */
  void systemButtons();

  /** \brief
   * Display the boot logo sequence.
   *
   * \details
   * This function is called by `begin()` and can be called by a sketch
   * after `boot()`.
   *
   * The Arduboy logo scrolls down from the top of the screen to the center
   * while the RGB LEDs light in sequence.
   *
   * \see begin() boot()
   */
  void bootLogo();


  void clear();
  void display();


  /** \brief
   * Seed the random number generator with a random value.
   *
   * \details
   * The Arduino random number generator is seeded with a random value
   * derrived from entropy from the temperature, voltage reading, and
   * microseconds since boot.
   *
   * This method is still most effective when called after a semi-random time,
   * such as after a user hits a button to start a game or other semi-random
   * event.
   */
  void initRandomSeed();

  // Swap the values of two int16_t variables passed by reference.
  void swap(int16_t& a, int16_t& b);

  /** \brief
   * Set the frame rate used by the frame control functions.
   *
   * \param rate The desired frame rate in frames per second.
   *
   * Set the frame rate, in frames per second, used by `nextFrame()` to update
   * frames at a given rate. If this function isn't used, the default rate will
   * be 60.
   *
   * Normally, the frame rate would be set to the desired value once, at the
   * start of the game, but it can be changed at any time to alter the frame
   * update rate.
   *
   * \see nextFrame()
   */
  void setFrameRate(uint8_t rate);

  /** \brief
   * Indicate that it's time to render the next frame.
   *
   * \return `true` if it's time for the next frame.
   *
   * \details
   * When this function returns `true`, the amount of time has elapsed to
   * display the next frame, as specified by `setFrameRate()`.
   *
   * This function will normally be called at the start of the rendering loop
   * which would wait for `true` to be returned before rendering and
   * displaying the next frame.
   *
   * example:
   * \code
   * void loop() {
   *   if (!arduboy.nextFrame()) {
   *     return; // go back to the start of the loop
   *   }
   *   // render and display the next frame
   * }
   * \endcode
   *
   * \see setFrameRate()
   */
  bool nextFrame();

  /** \brief
   * Indicate if the specified number of frames has elapsed.
   *
   * \param frames The desired number of elapsed frames.
   *
   * \return `true` if the specified number of frames has elapsed.
   *
   * This function should be called with the same value each time for a given
   * event. It will return `true` if the given number of frames has elapsed
   * since the previous frame in which it returned `true`.
   *
   * For example, if you wanted to fire a shot every 5 frames while the A button
   * is being held down:
   *
   * \code
   * if (arduboy.everyXframes(5)) {
   *   if arduboy.pressed(A_BUTTON) {
   *     fireShot();
   *   }
   * }
   * \endcode
   *
   * \see setFrameRate() nextFrame()
   */
  bool everyXFrames(uint8_t frames);

  /** \brief
   * Return the load on the CPU as a percentage.
   *
   * \return The load on the CPU as a percentage of the total frame time.
   *
   * \details
   * The returned value gives the time spent processing a frame as a percentage
   * the total time allotted for a frame, as determined by the frame rate.
   *
   * This function normally wouldn't be used in the final program. It is
   * intended for use during program development as an aid in helping with
   * frame timing.
   *
   * \note
   * The percentage returned can be higher than 100 if more time is spent
   * processing a frame than the time allotted per frame. This would indicate
   * that the frame rate should be made slower or the frame processing code
   * should be optimized to run faster.
   *
   * \see setFrameRate() nextFrame()
   */
  int cpuLoad();

  // Useful for getting raw approximate voltage values.
  uint16_t rawADC(uint8_t adc_bits);

  /** \brief
   * Test if the specified buttons are pressed.
   *
   * \param buttons A bit mask indicating which buttons to test.
   * (Can be a single button)
   *
   * \return `true` if *all* buttons in the provided mask are currently pressed.
   *
   * \details
   * Read the state of the buttons and return `true` if all the buttons in the
   * specified mask are being pressed.
   *
   * Example: `if (pressed(LEFT_BUTTON + A_BUTTON))`
   *
   * \note
   * This function does not perform any button debouncing.
   */
  bool pressed(uint8_t buttons);

  /** \brief
   * Test if the specified buttons are not pressed.
   *
   * \param buttons A bit mask indicating which buttons to test.
   * (Can be a single button)
   *
   * \return `true` if *all* buttons in the provided mask are currently
   * released.
   *
   * \details
   * Read the state of the buttons and return `true` if all the buttons in the
   * specified mask are currently released.
   *
   * Example: `if (notPressed(UP_BUTTON))`
   *
   * \note
   * This function does not perform any button debouncing.
   */
  bool notPressed(uint8_t buttons);

  /** \brief
   * Poll the buttons and track their state over time.
   *
   * \details
   * Read and save the current state of the buttons and also keep track of the
   * button state when this function was previouly called. These states are
   * used by the `justPressed()` and `justReleased()` functions to determine
   * if a button has changed state between now and the previous call to
   * `pollButtons()`.
   *
   * This function should be called once at the start of each new frame.
   *
   * The `justPressed()` and `justReleased()` functions rely on this function.
   *
   * example:
   * \code
   * void loop() {
   *   if (!arduboy.nextFrame()) {
   *     return;
   *   }
   *   arduboy.pollButtons();
   *
   *   // use justPressed() as necessary to determine if a button was just pressed
   * \endcode
   *
   * \note
   * As long as the elapsed time between calls to this function is long
   * enough, buttons will be naturally debounced. Calling it once per frame at
   * a frame rate of 60 or lower (or possibly somewhat higher), should be
   * sufficient.
   *
   * \see justPressed() justReleased()
   */
  void pollButtons();

  /** \brief
   * Check if a button has just been pressed.
   *
   * \param button The button to test for. Only one button should be specified.
   *
   * \return `true` if the specified button has just been pressed.
   *
   * \details
   * Return `true` if the given button was pressed between the latest
   * call to `pollButtons()` and previous call to `pollButtons()`.
   * If the button has been held down over multiple polls, this function will
   * return `false`.
   *
   * There is no need to check for the release of the button since it must have
   * been released for this function to return `true` when pressed again.
   *
   * This function should only be used to test a single button.
   *
   * \see pollButtons() justReleased()
   */
  bool justPressed(uint8_t button);

  /** \brief
   * Check if a button has just been released.
   *
   * \param button The button to test for. Only one button should be specified.
   *
   * \return `true` if the specified button has just been released.
   *
   * \details
   * Return `true` if the given button, having previously been pressed,
   * was released between the latest call to `pollButtons()` and previous call
   * to `pollButtons()`. If the button has remained released over multiple
   * polls, this function will return `false`.
   *
   * There is no need to check for the button having been pressed since it must
   * have been previously pressed for this function to return `true` upon
   * release.
   *
   * This function should only be used to test a single button.
   *
   * \note
   * There aren't many cases where this function would be needed. Wanting to
   * know if a button has been released, without knowing when it was pressed,
   * is uncommon.
   *
   * \see pollButtons() justPressed()
   */
  bool justReleased(uint8_t button);

  /** \brief
   * Test if a point falls within a rectangle
   *
   * \param point A structure describing the location of the point.
   * \param rect A structure describing the location and size of the rectangle.
   *
   * \return `true` if the specified point is within the specified rectangle.
   *
   * This function is intended to detemine if an object, whose boundaries are
   * are defined by the given rectangle, is in contact with the given point.
   *
   * \see Point Rect
   */
  bool collide(Point point, Rect rect);

  /** \brief
   * Test if a rectangle is intersecting with another rectangle.
   *
   * \param rect1,rect2 Structures describing the size and locations of the
   * rectangles.
   *
   * \return `true1 if the first rectangle is intersecting the second.
   *
   * This function is intended to detemine if an object, whose boundaries are
   * are defined by the given rectangle, is in contact with another rectangular
   * object.
   *
   * \see Rect
   */
  bool collide(Rect rect1, Rect rect2);

 protected:
  // helper function for sound enable/disable system control
  void sysCtrlSound(uint8_t buttons, uint8_t led, uint8_t eeVal);

  // Screen buffer
  static uint8_t sBuffer[(HEIGHT*WIDTH)/8];

  // For button handling
  uint8_t currentButtonState;
  uint8_t previousButtonState;

  // For frame funcions
  uint16_t frameCount;
  uint8_t eachFrameMillis;
  unsigned long lastFrameStart;
  unsigned long nextFrameStart;
  bool justRendered;
  uint8_t lastFrameDurationMs;
};

#endif
