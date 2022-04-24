#ifndef INTERNAL_STATE_H
#define INTERNAL_STATE_H

enum class State {
  INC_FAST,
  INC_ONCE,
  CHANGE_EDITABLE,       // change what should be edited: minutes or hours
  CHANGE_ANIM_MODE,
  ANIM_GOING_ON,
  WAIT_FOR_INC_FAST,
  WAIT_FOR_CHANGE_ANIM_MODE,
  WAIT_FOR_HIT,
  BLINK_THE_DISPLAY  
};

#endif
