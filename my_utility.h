#ifndef MY_UTILITY_H
#define MY_UTILITY_H

#include <stdint.h>

inline uint8_t bcdToDec(uint8_t val)        // https://github.com/elpaso/Rtc_Pcf8563
{
  return ((val / 16 * 10) + (val % 16));
}

inline uint8_t decToBcd(uint8_t val)
{
  return ((val / 10 * 16) + (val % 10));
}

#endif
