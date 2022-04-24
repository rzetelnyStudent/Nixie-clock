#ifndef CONFIG_H
#define CONFIG_H

namespace cfg {
enum class Mode {
  OLED,
  NIXIE
};
//=================================================================================================================================
//#define USE_OLED_LIBRARIES   // SWITCH HERE BETWEEN OLED AND NIXIE  ===========================================================
//=================================================================================================================================
//=================================================================================================================================
#ifdef USE_OLED_LIBRARIES
const Mode mode = Mode::OLED;
#else
const Mode mode = Mode::NIXIE;
#endif
/* ^ It looks like good old macros are unbeatable :-). I tried my best with templates. Even if I use Displaying<mode::NIXIE>, I need to
   include adafruit libraries and program memory goes +1000 bytes. */
const uint16_t FLASHING_INTERVAL = 300;      // period of blinking (300ms), after a time change is applied
const uint8_t ANIMATION_INTERVAL = 10;   //  time in mins between animations
const uint16_t LONG_PRESS_TIME = 600;       // time in ms needed go into fast increment when the button is held
const uint16_t LONG_PRESS_SW_MODES = 3500;       // time in ms that button CHG has to be held to change animation mode
const uint16_t TIMER_INTERVAL = 16;
};

#endif
