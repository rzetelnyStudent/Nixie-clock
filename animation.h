#ifndef ANIMATION_H
#define ANIMATION_H

#include "Arduino.h"
#include <avr/eeprom.h>
#include "displaying.h"
#include "Slot_machine.h"

enum class Animation_mode {
  OFF,
  EVERY_10_MIN,
  EV_10_MIN_23_7
};

class Animation {
    Slot_machine slot_machine;
    const uint8_t* EEPROM_LOCATION = 0xFF;    // a cell in EEPROM, where the mode is stored
    bool anim_going_on;
    uint8_t anim_timer_ticks;    // a frequency divider. While animation.update() is called every 16ms, slot_machine.update() needs to be called every 100ms or so
 
  public:  
    Animation_mode mode;
    Animation() {
      anim_going_on = false;
      mode = Animation_mode::OFF;
      anim_timer_ticks = 0;
    }
    bool isGoingOn();
    void start(Time time, Displaying<cfg::mode> &display);
    void stop();
    void update(Displaying<cfg::mode> &display);
    void readModeEeprom();
    void saveModeEeprom();
};

Animation_mode operator++(Animation_mode &mode, int);

#endif
