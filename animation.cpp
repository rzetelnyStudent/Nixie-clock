#include "animation.h"

// API:
/* call start() first, then call update() every 20ms, it will auto - stop, when animation sequence ends. If want to stop prematurely,
   call stop().
*/

/* With this function I can call mode++. It returns the next mode in the sequence or rollovers to the beginning */
Animation_mode operator++(Animation_mode &mode, int) {
  if (mode == Animation_mode::OFF) {
    return mode = Animation_mode::EVERY_10_MIN;
  } else if (mode == Animation_mode::EVERY_10_MIN) {
    return mode = Animation_mode::EV_10_MIN_23_7;
  } else {   // if mode == Animation_mode::EV_10_MIN_23_7
    return mode = Animation_mode::OFF;
  }
}

// returns whether the animation is running or has ended
bool Animation::isGoingOn() {
  return anim_going_on;
}

// force stop prematurely
void Animation::stop() {
  anim_going_on = false;
}

/* method inits an animation. */
void Animation::start(Time time, Displaying<cfg::mode> &display) {
  anim_going_on = true;
  slot_machine.init(time, display);
  anim_timer_ticks = 0;
}

// call every 16ms
void Animation::update(Displaying<cfg::mode> &display) {
  if (anim_going_on) {
    anim_timer_ticks++;
    if (anim_timer_ticks >= 6) {    // call slot_machine.update() every 6 * 16ms
      anim_timer_ticks = 0;
      if (Anim_state::END == slot_machine.update(display)) {
        stop();
      }
    }
  }
}

// Method reads set animation mode from EEPROM.
void Animation::readModeEeprom() {
  uint8_t value = eeprom_read_byte(EEPROM_LOCATION);
  switch (value) {     // it would be much more efficient with generic enum...
    case 2:
      mode = Animation_mode::EV_10_MIN_23_7;
      break;
    case 1:
      mode = Animation_mode::EVERY_10_MIN;
      break;
    default:    // case 0, or any other erratic eeprom read
      mode = Animation_mode::OFF;
      break;
  }
}

// Method saves animation mode in EEPROM.
void Animation::saveModeEeprom() {
  switch (mode) {
    case Animation_mode::OFF:
      eeprom_write_byte(EEPROM_LOCATION, 0);
      break;
    case Animation_mode::EVERY_10_MIN:
      eeprom_write_byte(EEPROM_LOCATION, 1);
      break;
    default:     // case Animation_mode::EV_10_MIN_23_7:
      eeprom_write_byte(EEPROM_LOCATION, 2);
      break;
  }
}
