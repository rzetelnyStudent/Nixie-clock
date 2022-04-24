#ifndef MY_PCF8563_H
#define MY_PCF8563_H

#include "Arduino.h"
#include "time.h"
#include "myTWI.h"

enum class Read_status {
  SUCCESS,
  ERROR
};

class My_Pcf8563 {

    const bool ACK = true;
    const bool NACK = false;
    const uint8_t RTC_READ = 0xA3;
    const uint8_t RTC_WRITE = 0xA2;
    const uint8_t SEC_ADDR = 0x02;
    const uint8_t MIN_ADDR = 0x03;
    const uint8_t HR_ADDR = 0x04;
    const uint8_t MINUTES_MSK = 0b10000000;
    const uint8_t HOURS_MSK = 0b11000000;
    bool getTimeRoutine(Time &time);
  public:
    Read_status getTime(Time &time);
    void setTime(uint8_t hours, uint8_t minutes) const;
};

#endif
