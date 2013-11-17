// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#ifndef _DCC_SIGNALER_H
#define _DCC_SIGNALER_H

class DccSignaler {
  volatile unsigned char _state;
  unsigned char currentByte;
  unsigned char bitMask;
  unsigned char byteIndex;
  unsigned char byteCount;
  unsigned char packet[3];
public:
  DccSignaler();
  void isr();
  void setup();
  void setNextPacket(unsigned char a, unsigned char b);
  void sendAccessoryPacket(unsigned int address, unsigned char value, unsigned char count = 3);
};

extern DccSignaler DCC;

#endif
