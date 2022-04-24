#ifndef BUTTON_H
#define BUTTON_H

#include "Arduino.h"

enum class Sequence {
  S00,
  S01,
  S11,
  S10
};

class Button {
    bool BTN_STATE[2];
  public:
    Button() {
      reset();
    }

    void reset() {
      BTN_STATE[0] = false;     // set buttons to "released" or "LOW"
      BTN_STATE[1] = false;
    }

    // function makes old current state new previous, and parameter "current_state" new current, kind of stack data structure
    void update(bool current_state) {
      BTN_STATE[0] = BTN_STATE[1];
      BTN_STATE[1] = current_state;
    }

    /* method compares sequence of button states, to the actual sequence stored in a private member.
      For example if ButtonUP == Sequence::S01, this means the button rising transition has been detected.
      Caution, I store button values as if they were: pressed = 1, released = 0. It could be wired the opposite in the actual circuit (and it is in my case).
      Though I intuitively think as if they were as I said. */
    bool operator== (Sequence seq) {
      switch (seq) {
        case Sequence::S00:
          return (BTN_STATE[0] == false && BTN_STATE[1] == false);
          break;
        case Sequence::S01:
          return (BTN_STATE[0] == false && BTN_STATE[1] == true);
          break;
        case Sequence::S11:
          return (BTN_STATE[0] == true && BTN_STATE[1] == true);
          break;
        default:     // Sequence::S10:
          return (BTN_STATE[0] == true && BTN_STATE[1] == false);
          break;
      }
    }
};

#endif
