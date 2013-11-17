// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#include "DccSignaler.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

// PB3/D11/MOSI is output

#define DCC_DDR      DDRB
#define DCC_PORT_OUT PORTB
#define DCC_PORT_IN  PINB
#define DCC_PIN      3
#define DCC_PIN_MASK (1 << DCC_PIN)

// PB4/D12 is enable pin for L298
#define ENABLE_PIN_MASK (1 << 4)


const unsigned char TIMER_VALUE_1 = 115; // 58us
const unsigned char TIMER_VALUE_0 = 199; // 100us


const unsigned char STATE_IDLE     = 0;
const unsigned char STATE_PREAMBLE = STATE_IDLE + 1;
const unsigned char STATE_STARTBIT = STATE_PREAMBLE + 14;
const unsigned char STATE_BYTE     = STATE_STARTBIT + 1;
const unsigned char STATE_ENDBIT   = STATE_BYTE + 1;


DccSignaler DCC;



ISR(TIMER2_COMPA_vect) {
  if ((DCC_PORT_IN & DCC_PIN_MASK) != 0) {
    return;
  }

  DCC.isr();
}


void DccSignaler::isr() {
  const unsigned char state = _state;
  if (state == STATE_BYTE) {
    const unsigned char bitToSend = currentByte & bitMask;
    OCR2A = bitToSend != 0
      ? TIMER_VALUE_1
      : TIMER_VALUE_0;

    bitMask >>= 1;
    if (bitMask > 0) {
      return;
    }

    byteIndex++;
    if (byteIndex < byteCount) {
      _state = STATE_STARTBIT;
    }
    else {
      _state = STATE_ENDBIT;
    }
    return;
  }

  if (state == STATE_STARTBIT) {
    OCR2A = TIMER_VALUE_0;
    bitMask = 0x80;
    currentByte = packet[byteIndex];
    _state = STATE_BYTE;
    return;
  }

  if (state == STATE_ENDBIT) {
    OCR2A = TIMER_VALUE_1;
    _state = STATE_IDLE;
    return;
  }

  if (state == STATE_IDLE) {
    OCR2A = TIMER_VALUE_1;

    packet[0] = 0xFF;
    packet[1] = 0x00;
    packet[2] = 0xFF;
    byteCount = 3;
    _state = STATE_PREAMBLE;
    return;
  }

  if (state == STATE_PREAMBLE) {
    OCR2A = TIMER_VALUE_1;
    byteIndex = 0;
  }

  _state = state + 1;
}


DccSignaler::DccSignaler() {
  _state = STATE_IDLE;
}


void DccSignaler::setup() {
  DDRB |= DCC_PIN_MASK;
  TCCR2A = (0x01 << COM2A0)
         | (0x02 << WGM20);
  TCCR2B = (0 << WGM22)
         | (0x02 << CS20);
  OCR2A = TIMER_VALUE_1;
  TCNT2 = 0;
  TIMSK2 = 1 << OCIE2A;

  // set enable pin for L298  
  DDRB |= ENABLE_PIN_MASK;
  PORTB |= ENABLE_PIN_MASK;
}


void DccSignaler::setNextPacket(unsigned char a, unsigned char b) {
  for (;;) {
    while (_state != STATE_IDLE) {
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      if (_state != STATE_IDLE) {
        continue;
      }

      packet[0] = a;
      packet[1] = b;
      packet[2] = a ^ b;
      byteCount = 3;
      _state = STATE_PREAMBLE;
      return;
    }
  }
}

void DccSignaler::sendAccessoryPacket(unsigned int address, unsigned char value, unsigned char count) {
  value &= 0x01;
  value |= (address & 0x03) << 1;
  value |= 0x08;
  address >>= 2;
  unsigned char byte0 = 0x80 | (address & 0x3F);
  address = (~address >> 2);
  unsigned char byte1 = 0x80 | (address & 0x70) | (value & 0x0F);
  // send it count-times
  for (; count > 0; count--) {
    setNextPacket(byte0, byte1);
  }
}

