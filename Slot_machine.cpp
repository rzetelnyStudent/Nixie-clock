#include "Slot_machine.h"

/* Takes slot machine animation to the next step, i. e. updates it. Should be called, every 100ms or so, for the best
    visual effects. Param &display is a reference to Displaying class object created in main().
    Returns true if if end of an animation is reached, else - false */
Anim_state Slot_machine::update(Displaying<cfg::mode> &display) {
  if (state <= Slot_machine_state::all_roll_5 || state == Slot_machine_state::all_roll_6) {
    digits.HR_tens = incFrom0to<2>(digits.HR_tens);
    digits.HR_ones = incFrom0to<9>(digits.HR_ones);
    digits.MIN_tens = incFrom0to<5>(digits.MIN_tens);
    digits.MIN_ones = incFrom0to<9>(digits.MIN_ones);
  } else if ((state >= Slot_machine_state::HR_tens_pauses_rest_rolls_1 && state <= Slot_machine_state::HR_tens_pauses_rest_rolls_3) ||
             (state >= Slot_machine_state::roll_except_HR_tens_1 && state <= Slot_machine_state::roll_except_HR_tens_5)
             || state == Slot_machine_state::roll_except_HR_tens_6) {
    digits.HR_ones = incFrom0to<9>(digits.HR_ones);
    digits.MIN_tens = incFrom0to<5>(digits.MIN_tens);
    digits.MIN_ones = incFrom0to<9>(digits.MIN_ones);
  } else if ((state >= Slot_machine_state::HR_tens_ones_pauses_rest_rolls_1 && state <= Slot_machine_state::HR_tens_ones_pauses_rest_rolls_3)
             || (state >= Slot_machine_state::roll_except_HR_tens_ones_1  && state <= Slot_machine_state::roll_except_HR_tens_ones_5)
             || state == Slot_machine_state::roll_except_HR_tens_ones_6) {
    digits.MIN_tens = incFrom0to<5>(digits.MIN_tens);
    digits.MIN_ones = incFrom0to<9>(digits.MIN_ones);
  } else if ((state >= Slot_machine_state::only_MIN_ones_rolls_1 && state <= Slot_machine_state::only_MIN_ones_rolls_3)
             || (state >= Slot_machine_state::roll_only_MIN_ones_1 && state <= Slot_machine_state::roll_only_MIN_ones_5)
             || state == Slot_machine_state::roll_only_MIN_ones_6) {
    digits.MIN_ones = incFrom0to<9>(digits.MIN_ones);
  } // else - do nothing last digit (MIN_ones is slowing down)
  Time time;
  time.hours = (digits.HR_tens << 4) | digits.HR_ones;       // make up time out of single digits
  time.minutes = (digits.MIN_tens << 4) | digits.MIN_ones;
  display.display(time);
  if (state == Slot_machine_state::roll_only_MIN_ones_6) {   // if we are in the last state
    return Anim_state::END;
  } else {                // prepare for the next update() call
    state++;
    return Anim_state::NOT_FIN;
  }
}

/* Resets animation state. (Previous animation could be "half-way terminated" by button press).
    param "time" is a current time. Animation should end with this time value on the display. It needs to calculate from which time value should it start. */
void Slot_machine::init(Time time, Displaying<cfg::mode> &display) {
  digits = display.splitTimeTo4(time);
  digits.HR_ones = subtractbcd<5>(digits.HR_ones);      // It turns out that tens don't require additional setup (coincidence of course)
  digits.MIN_ones = subtractbcd<3>(digits.MIN_ones);    // set starting time value. 5 or 3 is just dependent on how many steps animation has
  state = Slot_machine_state::all_roll_1;             // set state to starting point
}
