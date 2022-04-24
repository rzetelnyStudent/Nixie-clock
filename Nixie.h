#ifndef NIXIE_H
#define NIXIE_H

#include <stdint.h>

struct Nixie {
  uint8_t HR_tens;
  uint8_t HR_ones;
  uint8_t MIN_tens;
  uint8_t MIN_ones;
};

#endif
