// if USE_OLED_LIBRARIES is defined in config.h, code compiles for OLED (debug - like mode), else for nixie (release)

#include "config.h"
#include "displaying.h"
#include "Button.h"
#include "Change_time.h"
#include "my_Pcf8563.h"
#include "pheripherals.h"
#include "time.h"
#include "Blank_what.h"
#include "timer.h"
#include "animation.h"
#include "display_blinker.h"
#include "my_utility.h"
#include "Internal_state.h"

volatile bool button_flag = false;
volatile bool timer_flag = false;
volatile uint8_t WDT_counter = 0;


int main() {

  //====================SETUP========================

  cli();
  handleWDTreset();
  setupWDT8s();
  sei();
  disableSomePher();
  attachButtonsInts();
  Displaying<cfg::mode> Display;
  Display_blinker disp_blinker;
  Display.init();
  Animation animation;
  animation.readModeEeprom();     // Read mode setting stored in EEPROM
  Blank_what blanking;
  blanking = Blank_what::ALL;
  State internal_state;
  internal_state = State::WAIT_FOR_HIT;     // default
  Timer timer;
  Button btnUP;
  Button btnCHG;
  Change_time change_time;
  Time time;
  My_Pcf8563 RTC;
  RTC.getTime(time);     // take initial reading. If getTime accounts an error, time will be 00:00
  Display.display(time);
#ifndef USE_OLED_LIBRARIES
  RelayON();  
#endif
  cli();
  button_flag = false;     // Clear the interrupt buffer, attachInterrupt() likely throws some fake interrupts
  sei();
  
  //====================LOOP========================

  while (1) {
    WDTCSR |= (1 << WDIE);      // enable WDT interrupt (equivalent to wdt_reset() in non-interrupt mode)
    if (Read_status::SUCCESS == RTC.getTime(time)) {      // if the device is working properly (RTC is responding)
      cli();
      bool button_flag_cpy = button_flag;
      bool timer_flag_cpy = timer_flag;
      sei();

      // check, what caused device wakeup:
      if (button_flag_cpy == true) {     // if it was button interrupt:
        cli();
        button_flag = false;
        sei();
        if (!timer.isEnabled()) {
          timer.start();
        }
      }

      else if (timer_flag_cpy == true && timer.isEnabled()) {    // if it was timer interrupt:
        cli();
        timer_flag = false;
        sei();
        timer++;     // increment timer counter
        btnUP.update(!digitalReadBtnUP());     // read button state every (16ms + wakeup) (debounce them)
        btnCHG.update(!digitalReadBtnCHG());
        switch (internal_state) {

          case State::INC_ONCE:
            change_time.incrementTime(time, RTC);
            timer.start();
            internal_state = State::WAIT_FOR_INC_FAST;    // go to the next state
            break;

          case State::WAIT_FOR_INC_FAST:
            Display.display(time);
            if (btnUP == Sequence::S00) {
              internal_state = State::BLINK_THE_DISPLAY;    // go to the next state
              (Hrs_or_mins::HOURS == change_time.getWhatWasEdited()) ? blanking = Blank_what::HOURS : blanking = Blank_what::MINS;
              disp_blinker.start();
            } else if (timer.getTicks() > cfg::LONG_PRESS_TIME / cfg::TIMER_INTERVAL) {
              internal_state = State::INC_FAST;
            }
            break;

          case State::INC_FAST:
            if ((timer.getTicks() % 3) == 0) {     // every 3 * 16ms increments fast, when the button is held down
              change_time.incrementTime(time, RTC);
            }
            Display.display(time);
            if (btnUP == Sequence::S00) {
              internal_state = State::BLINK_THE_DISPLAY;
              (Hrs_or_mins::HOURS == change_time.getWhatWasEdited()) ? blanking = Blank_what::HOURS : blanking = Blank_what::MINS;
              disp_blinker.start();
            }
            break;

          case State::CHANGE_EDITABLE:
            change_time.changeWhatIsEdited();
            (Hrs_or_mins::HOURS == change_time.getWhatWasEdited()) ? blanking = Blank_what::HOURS : blanking = Blank_what::MINS;
            timer.start();
            disp_blinker.start();
            internal_state = State::WAIT_FOR_CHANGE_ANIM_MODE;    // go to the next state
            break;

          case State::WAIT_FOR_CHANGE_ANIM_MODE:
            disp_blinker.update(Display, blanking, time);
            if (timer.getTicks() > cfg::LONG_PRESS_SW_MODES / cfg::TIMER_INTERVAL) {
              internal_state = State::CHANGE_ANIM_MODE;
              disp_blinker.stop();     // safety, but it should rather finish first on its own (if you leave config times as they are)
            } else if (btnCHG == Sequence::S00 && disp_blinker.isBlinking()) {    // it hasn't finished blinking
              internal_state = State::BLINK_THE_DISPLAY;
              (Hrs_or_mins::HOURS == change_time.getWhatWasEdited()) ? blanking = Blank_what::HOURS : blanking = Blank_what::MINS;
              disp_blinker.start();
            } else if (btnCHG == Sequence::S00 && btnUP == Sequence::S00 && !disp_blinker.isBlinking()) {
              internal_state = State::WAIT_FOR_HIT;
              timer.stop();
              cli();
              timer_flag = false;
              sei();
            }
            break;

          case State::CHANGE_ANIM_MODE:
            animation.mode++;
            animation.saveModeEeprom();     // save new setting in EEPROM
            if (animation.mode == Animation_mode::OFF) {
              internal_state = State::BLINK_THE_DISPLAY;
              blanking = Blank_what::ALL;
              disp_blinker.start();
            }
            else if (animation.mode == Animation_mode::EVERY_10_MIN) {
              animation.start(time, Display);
              internal_state = State::ANIM_GOING_ON;
            } else {     // EV_10_MIN_23_7
              internal_state = State::BLINK_THE_DISPLAY;
              blanking = Blank_what::_23_7;
              disp_blinker.start();
            }
            cli();
            WDT_counter = 0;    // reset animation counter, so that the next animation will happen INTERVAL from now (of course if the mode != OFF)
            sei();
            break;

          case State::ANIM_GOING_ON:
            if (!animation.isGoingOn() && btnUP == Sequence::S00 && btnCHG == Sequence::S00) {     // if the animation has already finished
              internal_state = State::WAIT_FOR_HIT;
              timer.stop();
              cli();
              timer_flag = false;
              sei();
            }
            else if (animation.isGoingOn()) {
              animation.update(Display);
            }
            break;

          case State::BLINK_THE_DISPLAY:
            if (btnUP == Sequence::S01) {
              disp_blinker.stop();
              internal_state = State::INC_ONCE;
            }
            else if (btnCHG == Sequence::S01) {
              disp_blinker.stop();
              internal_state = State::CHANGE_EDITABLE;
            } 
            else if (!disp_blinker.isBlinking() && btnUP == Sequence::S00 && btnCHG == Sequence::S00) {      // if the blinking has already finished
              internal_state = State::WAIT_FOR_HIT;
              timer.stop();
              cli();
              timer_flag = false;
              sei();
            }
            else if (disp_blinker.isBlinking()){
              disp_blinker.update(Display, blanking, time);
            }
            break;

          default:    
            if (btnUP == Sequence::S01) {
              internal_state = State::INC_ONCE;
            }
            else if (btnCHG == Sequence::S01) {
              internal_state = State::CHANGE_EDITABLE;
            } else {
              /* this potentially executes after reset (pin interrupt buffer has something inside and triggers "button interrupt". But buttons are 00, so we need to tell MCU,
                 this is a fake trigger: Don't start debouncing timer */
              timer.stop();
              cli();
              timer_flag = false;
              sei();
            }
            break;
        }
      }

      else if (!timer.isEnabled()) {     
        // wakeup is caused by WDT interrput or reset and the timer isn't running. (This check is needed, so that WDT trigger wouldn't interrupt handling buttons.
        cli();
        uint8_t WDT_counter_cpy = WDT_counter;
        sei();               // v ANIMATION_INTERVAL[min] * 60[s] / WDT_interval
        if (WDT_counter_cpy >= cfg::ANIMATION_INTERVAL * 15 / 2 && animation.mode != Animation_mode::OFF) {
          cli();
          WDT_counter = 0;
          sei();
          if (animation.mode == Animation_mode::EVERY_10_MIN || (animation.mode == Animation_mode::EV_10_MIN_23_7 && (time.hours < decToBcd(7) || time.hours == decToBcd(23)))) {
            animation.start(time, Display);
            timer.start();
            internal_state = State::ANIM_GOING_ON;
          }
        }
        else {     // do your job - just update the display (no button handling or animation occur)
          Display.display(time);
        }
      }
    }
    else {    // the device isn't working properly (RTC is dead)
      Display.displayError();
    }
    goToSleep();
  }
}


ISR(WDT_vect) {
  WDT_counter++;
}

ISR(INT0_vect) {
  button_flag = true;
}

ISR(INT1_vect) {
  button_flag = true;
}

ISR(TIMER2_COMPA_vect) {
  timer_flag = true;
}
