#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"
#include "pheripherals.h"

class Timer {
    uint16_t timer_ticks;
    bool enabled;
  public:

    Timer() {
      timer_ticks = 0;
      enabled = false;
    }
    bool isEnabled() {     // returns true if the timer is already running
      return enabled;
    }
    uint16_t getTicks() {     // returns timer ticks (equals to real time / 16, in mS)
      return timer_ticks;
    }

    void start() {     // starts timer, can also be used as restart or reset
      setupTimer2_16mS();
      timer_ticks = 0;
      enabled = true;
    }

    void stop() {     // stops timer (in order to cut down power consumption, to avoid to wake up MCU too often)
      power_timer2_disable();
      enabled = false;
    }

    void operator++(int) {     // call every time ISR triggers, but not inside ISR (use flag)
      timer_ticks++;
    }
};

#endif
