#include "Change_time.h"

// method for changing whether the incrementation of time (by pressing UP button) is aplied to time.hours or time.minutes
void Change_time::changeWhatIsEdited() {
  if (hrs_or_mins == Hrs_or_mins::HOURS) {   // just like negation of the enum
    hrs_or_mins = Hrs_or_mins::MINUTES;
  } else {
    hrs_or_mins = Hrs_or_mins::HOURS;
  }
}

// method for incrementing time (by button input)
void Change_time::incrementTime(Time &time, const My_Pcf8563 &RTC) {
  if (hrs_or_mins == Hrs_or_mins::HOURS) {
    time.hours = bcdToDec(time.hours) + 1;  // convert to dec for the sake of adding change, set by user
    if (time.hours > 23) {        //rollover hours
      time.hours = 0;
    }
    time.hours = decToBcd(time.hours);
  } else {
    time.minutes = bcdToDec(time.minutes) + 1;
    if (time.minutes > 59) {     //rollover minutess
      time.minutes = 0;
    }
    time.minutes = decToBcd(time.minutes);
  }
  RTC.setTime(time.hours, time.minutes);
}
