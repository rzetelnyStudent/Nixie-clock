#ifndef DISPLAY_BLINKER_H
#define DISPLAY_BLINKER_H

#include "Displaying.h"
#include "config.h"

class Display_blinker {
  bool flashing;
  uint8_t flash_counter;
  bool flashing_state;
  public:
  bool isBlinking();
  uint16_t timer_ticks_flashing;
  Display_blinker();
  void start();
  void stop();
  void update(Displaying<cfg::mode> &display, Blank_what blanking, Time time);
};

#endif
