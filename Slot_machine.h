#ifndef SLOT_MACHINE_H
#define SLOT_MACHINE_H

#include "Arduino.h"
#include "displaying.h"
#include "Nixie.h"
#include "Slot_machine.h"


/* Slot machine animation sequence. First HR_tens slows down and stops, then HR_ones, MIN_tens, MIN_ones.
   HR_tens will ++ 6 times, HR_ones - 15, MIN_tens - 24, MIN_ones - 33. */
enum Slot_machine_state {
  all_roll_1 = 1,
  all_roll_2 = 2,
  all_roll_3 = 3,
  all_roll_4 = 4,
  all_roll_5 = 5,
  HR_tens_pauses_rest_rolls_1 = 6,        //  HR_tens slows down
  HR_tens_pauses_rest_rolls_2 = 7,
  HR_tens_pauses_rest_rolls_3 = 8,
  all_roll_6 = 9,
  roll_except_HR_tens_1 = 10,             // HR_tens stops
  roll_except_HR_tens_2 = 11,
  roll_except_HR_tens_3 = 12,
  roll_except_HR_tens_4 = 13,
  roll_except_HR_tens_5 = 14,
  HR_tens_ones_pauses_rest_rolls_1 = 15,      //  HR_ones slows down
  HR_tens_ones_pauses_rest_rolls_2 = 16,
  HR_tens_ones_pauses_rest_rolls_3 = 17,
  roll_except_HR_tens_6 = 18,
  roll_except_HR_tens_ones_1 = 19,             // HR_ones stops
  roll_except_HR_tens_ones_2 = 20,
  roll_except_HR_tens_ones_3 = 21,
  roll_except_HR_tens_ones_4 = 22,
  roll_except_HR_tens_ones_5 = 23,
  only_MIN_ones_rolls_1 = 24,      //  MIN_tens slows down
  only_MIN_ones_rolls_2 = 25,
  only_MIN_ones_rolls_3 = 26,
  roll_except_HR_tens_ones_6 = 27,
  roll_only_MIN_ones_1 = 28,             // MIN_tens stops
  roll_only_MIN_ones_2 = 29,
  roll_only_MIN_ones_3 = 30,
  roll_only_MIN_ones_4 = 31,
  roll_only_MIN_ones_5 = 32,
  nothing_1 = 33,                     // MIN_ones slows down
  nothing_2 = 34,
  nothing_3 = 35,
  roll_only_MIN_ones_6 = 36          // last tick
};

enum class Anim_state {
  END, 
  NOT_FIN 
};

inline Slot_machine_state operator++(Slot_machine_state &state, int)  {
  return state = Slot_machine_state(state + 1);
}

class Slot_machine {
    Slot_machine_state state;
    Nixie digits;

    /* takes digit from 0 - 9. Subtracts subtrahend (5 or 3 in my case). Outputs digit from 0 - 9. Subtrahend should be 0 - 9 */
    template <int subtrahend>
    uint8_t subtractbcd(uint8_t value) {
      if (value >= subtrahend) {
        return (value - subtrahend);
      } else {    // < subtrahend
        return (value + (10 - subtrahend));
      }
    }

    // used to increment single digit, and rollover, when > 9, for slot machine effect
    template <int max>
    uint8_t incFrom0to(uint8_t value) {     
      if (value >= max)
        return 0;
      else
        return (++value);
    }

  public:
    Anim_state update(Displaying<cfg::mode> &display);
    void init(Time time, Displaying<cfg::mode> &display);
};

#endif
