#ifndef CHANGE_TIME_H
#define CHANGE_TIME_H

#include "Arduino.h"
#include "Blank_what.h"
#include "my_Pcf8563.h"
#include "my_utility.h"

enum class Hrs_or_mins {
  HOURS,
  MINUTES
};

class Change_time {
    Hrs_or_mins hrs_or_mins;

  public:
    Change_time() {
      hrs_or_mins = Hrs_or_mins::HOURS;
    }
    // returns what was edited last time: hours or minutes
    Hrs_or_mins getWhatWasEdited() {
      return hrs_or_mins;
    }
    void changeWhatIsEdited();
    void incrementTime(Time &time, const My_Pcf8563 &RTC);
};

#endif
