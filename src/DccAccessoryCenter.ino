// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "DccSignaler.h"
#include "CharLCD.h"
#include "RotaryEncoder.h"
#include "TrackPlan.h"


void sendResetPackets() {
  for (unsigned char i = 0; i < 20; i++) {
    DCC.setNextPacket(0x00, 0x00);
  }
}


void sendIdlePackets() {
  for (unsigned char i = 0; i < 20; i++) {
    DCC.setNextPacket(0xFF, 0x00);
  }
}


CharLCD charLcd;
RotaryEncoder encoder(A5);

void setup() {
  DCC.setup();
  sei();

  sendResetPackets();
  sendIdlePackets();
}

void loop() {
  switch (mainMenu()) {
  case 0:
    trackPlan();
    break;

  case 1:
    accessory();
    break;
  }
}

unsigned char mainMenu() {
  CharFrame frame(&charLcd, 0, 0, 20, 2);
  unsigned char selection = 0;
  for (;;) {
    frame.setCursor(0, 0);
    frame.write("Track plan", selection == 0);
    frame.setCursor(0, 1);
    frame.write("Accessory ", selection == 1);
    
    switch (encoder.getKey()) {
    case -1:
      if (selection > 0) {
        selection--;
      }
      break;
    
    case 1:
      if (selection < 1) {
        selection++;
      }
      break;

    case 0:
      return selection;
    }
  }
}

void trackPlan() {
  TrackPlan plan(&charLcd, &encoder);
  plan.show();
  plan.sendAllPackets();
  for (;;) {
    plan.loop();
  }
}

void accessory() {
  CharFrame frame(&charLcd, 0, 0, 20, 2);
  unsigned char selection = 0;
  for (;;) {
    frame.setCursor(0, 0);
    printHex(&frame, selection >> 1);
    frame.write(' ');
    frame.write((selection & 1) > 0 ? 'G' : 'R');
    
    switch (encoder.getKey()) {
    case -1:
      if (selection > 0) {
        selection--;
      }
      break;
    
    case 1:
      if (selection < 255) {
        selection++;
      }
      break;

    case 0:
      DCC.sendAccessoryPacket(selection >> 1, selection & 1, 7);
      break;

    case 4:
      return;
    }
  }
}

void printHex(CharFrame* frame, unsigned value) {
  printHexNibble(frame, value >> 4);
  printHexNibble(frame, value);
}

void printHexNibble(CharFrame* frame, unsigned value) {
  value &= 0x0F;
  if (value > 9) {
    value += 'A' - '9' - 1;
  }
  value += '0';
  frame->write(value);
}

