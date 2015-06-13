// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

/*  5V
    |
   +-+
   | | R
   +-+
    |
    *------------*--------*---------> analog pin
    |            |        |
     / Select     / A      / B
    |            |        |
   +-+          +-+      +-+
   | | RS       | | RA   | | RB
   +-+          +-+      +-+
    |            |        |
   ---          ---      ---
    -            -        -
*/

#include "RotaryEncoder.h"

const unsigned int R = 103;
const unsigned int RS = 266;
const unsigned int RA = 99;
const unsigned int RB = 200;
const unsigned int _FACTOR = 30000;
const unsigned int RAB = _FACTOR / (_FACTOR / RA + _FACTOR / RB);
const unsigned int RAS = _FACTOR / (_FACTOR / RA + _FACTOR / RS);
const unsigned int RBS = _FACTOR / (_FACTOR / RB + _FACTOR / RS);
const unsigned int RABS = _FACTOR / (_FACTOR / RA + _FACTOR / RB + _FACTOR / RS);

const unsigned long MAX = 1023;
const unsigned int THRESHOLD = 10;
const unsigned int ANALOG_S = MAX * RS / (R + RS);
const unsigned int ANALOG_A = MAX * RA / (R + RA);
const unsigned int ANALOG_B = MAX * RB / (R + RB);
const unsigned int ANALOG_AB = MAX * RAB / (R + RAB);
const unsigned int ANALOG_AS = MAX * RAS / (R + RAS);
const unsigned int ANALOG_BS = MAX * RBS / (R + RBS);
const unsigned int ANALOG_ABS = MAX * RABS / (R + RABS);

const unsigned char STATE_NONE = 0;
const unsigned char STATE_A = 1;
const unsigned char STATE_AB = 2;
const unsigned char STATE_B = 3;
const unsigned char STATE_S = 0x80;


RotaryEncoder::RotaryEncoder(uint8_t pin) {
  this->pin = pin;
}

bool isMatching(unsigned int value, unsigned int reference) {
  return reference - THRESHOLD < value && value < reference + THRESHOLD;
}

int RotaryEncoder::readAnalog() {
  return analogRead(pin);
}

unsigned char RotaryEncoder::getState() {
  int analogValue = readAnalog();
  // discard noise
  for (unsigned char i = 0; i < 3; i++) {
    int secondAnalogValue = readAnalog();
    if (abs(analogValue - secondAnalogValue) < 3) {
      continue;
    }

    analogValue = secondAnalogValue;
    i = 0;
  }

  if (isMatching(analogValue, ANALOG_A)) return STATE_A;
  if (isMatching(analogValue, ANALOG_B)) return STATE_B;
  if (isMatching(analogValue, ANALOG_AB)) return STATE_AB;
  if (isMatching(analogValue, ANALOG_S)) return STATE_S;
  if (isMatching(analogValue, ANALOG_AS)) return STATE_A | STATE_S;
  if (isMatching(analogValue, ANALOG_BS)) return STATE_B | STATE_S;
  if (isMatching(analogValue, ANALOG_ABS)) return STATE_AB | STATE_S;
  if (analogValue < 1000) {
    Serial.print("### invalid=");
    Serial.println(analogValue);
  }
  return STATE_NONE;
}

char RotaryEncoder::getKey() {
  // wait for 0 in AB
  unsigned char prevState = getState();
  for (; (prevState & 3) != 0; prevState = getState());
  
  bool pressed = false;
  unsigned int repeatCounter = 0;
  
  char direction = 0;
  for(unsigned char state = getState(); ; prevState = state, state = getState()) {
    if (state == prevState) {
      // check for held select button
      if (state == STATE_S && pressed) {
        if (repeatCounter == 2000) {
          return 4;
        }
        
        repeatCounter++;
      }

      continue;
    }
    
    repeatCounter = 0;
    
    // Select pressed or released?
    if ((state & STATE_S) != (prevState & STATE_S)) {
      // pressed?
      if ((state & STATE_S) != 0) {
        pressed = true;
        continue;
      }
      
      // released after pressing?
      if (pressed) {
        return 0;
      }
      
      continue;
    }

    if (direction == 0) {
      // first rotation    
      if ((prevState & 3) == 0) {
        unsigned char rotaryState = state & 3;
        if (rotaryState == STATE_A) {
          direction = 1;
        }
        else if (rotaryState == STATE_B) {
          direction = -1;
        }
        else {
          // error
          direction = 0;
        }
      }
      
      // error, wait for 0
      continue;
    }
    
    if ((state & 3) == STATE_AB) {
      if ((state & STATE_S) != 0) {
        direction *= 2;
      }
      return direction;
    }
    
    direction = 0;
  }
}

