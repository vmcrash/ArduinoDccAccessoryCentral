// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#ifndef _ROTARY_ENCODER_H
#define _ROTARY_ENCODER_H

#include <Arduino.h>

class RotaryEncoder {
  uint8_t pin;
  int readAnalog();
  unsigned char getState();
public:
  RotaryEncoder(uint8_t pin);
  char getKey();
};

#endif
