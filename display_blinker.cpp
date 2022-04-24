#include "display_blinker.h"

// API:
/* call start() first, then call update() every 16ms, it will auto - stop, when blinking sequence ends. If want to stop prematurely,
 * call stop(). 
 */
 
void Display_blinker::stop() {
  flashing = false;
}

void Display_blinker::start() {
  flashing = true;        // setup timer?
  flashing_state = false;
  flash_counter = 0;
  timer_ticks_flashing = 0;
}

Display_blinker::Display_blinker() {
  flashing = false;
  flashing_state = true;
  flash_counter = 0;
  timer_ticks_flashing = 0;
}

void Display_blinker::update(Displaying<cfg::mode> &display, Blank_what blanking, Time time) {
  if (flashing) {
    timer_ticks_flashing++;
    if (timer_ticks_flashing >= cfg::FLASHING_INTERVAL / cfg::TIMER_INTERVAL) {
      timer_ticks_flashing = 0;
      flashing_state = !flashing_state;
      display.display(time, flashing_state, blanking);
      flash_counter++;
      if (flash_counter >= 5) {
        stop();
      }
    }
  }
}

// use to check if the blinking has finished or is it still going on
bool Display_blinker::isBlinking() {
  return flashing;
}
