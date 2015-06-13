// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#include "TrackPlan.h"

const unsigned int ADR_ESIG = 16;
const unsigned int ADR_ASIG1 = 17;
const unsigned int ADR_ASIG2 = 18;
const unsigned int TURNOUT_1 = 0;
const unsigned int TURNOUT_2 = 1;
const unsigned int TURNOUT_3 = 2;
const unsigned int TURNOUT_4 = 3;
const unsigned int TURNOUT_5 = 4;
const unsigned int TURNOUT_6 = 5;
const unsigned int TURNOUT_7 = 6;
const unsigned int TURNOUT_9 = 8;
const unsigned int TURNOUT_10 = 9;



TrackPlan::TrackPlan(CharLCD* lcd, RotaryEncoder* encoder) {
  this->encoder = encoder;
  this->frame = new CharFrame(lcd, 0, 0, 20, 8);

  items.add(new AccessoryItem(1, 7, 0x80, ADR_ESIG));

  currentItem = new AccessoryItem(0, 6, 0x12, TURNOUT_10);
  items.add(currentItem);
  items.add(new AccessoryItem(0, 2, 0x82, ADR_ASIG1));

  items.add(new AccessoryItem(1, 6, 0x1c, TURNOUT_9));
  items.add(new AccessoryItem(1, 4, 0x81, ADR_ASIG2));

  items.add(new AccessoryItem(11, 2, 0x18, TURNOUT_7));
  items.add(new AccessoryItem(11, 1, 0x1a, TURNOUT_6));
  items.add(new AccessoryItem(12, 1, 0x19, TURNOUT_5));
  items.add(new AccessoryItem(14, 4, 0x1e, TURNOUT_2, 1));
  items.add(new AccessoryItem(15, 2, 0x19, TURNOUT_3));
  items.add(new AccessoryItem(15, 1, 0x1f, TURNOUT_4, 1));
  items.add(new AccessoryItem(16, 2, 0x1e, TURNOUT_1, 1));
}

void TrackPlan::show() {
  frame->write("          \xb\x3\x7");
  frame->print(0x0a);
  frame->write("         \xb\x3..\x3\x3.\x3\xa");
  frame->print(0x0a);
  frame->write("#\x6\x3\x3\x3\x3\x3\x3\x3\x3\x3.\x3\x3\x3..\x3\x3\xa");
  frame->print(0x0a);
  frame->write("\x6\x4             \x6\x4");
  frame->print(0x0a);
  frame->write("\x2#\x6\x3\x3\x3\x3\x3\x3\x3\x3\x3\x3\x3.\x4");
  frame->print(0x0a);
  frame->write("\x2\x6\x4          \x6\x4");
  frame->print(0x0a);
  frame->write("..\x3\x3\x3\x3\x3\x3\x3\x3\x3\x3\x3\x4");
  frame->print(0x0a);
  frame->write("\x2#");
  
  for (AccessoryItem* item = items.getFirst(); item != NULL; item = item->next) {
    item->print(false, frame);
  }
}

void TrackPlan::sendAllPackets() {
  for (AccessoryItem* item = items.getFirst(); item != NULL; item = item->next) {
    item->sendPacket();
  }
}

void TrackPlan::loop() {
  currentItem->print(true, frame);

  AccessoryItem* prevItem = currentItem;
  
  switch (encoder->getKey()) {
  case -2:
  case -1:
    for (unsigned char count = items.getCount(); count-- > 0; ) {
      currentItem = items.getPrev(currentItem);
      if (canToggle()) {
        break;
      }
    }
    prevItem->print(false, frame);
    return;
    
  case 2:
  case 1:
    for (unsigned char count = items.getCount(); count-- > 0; ) {
      currentItem = items.getNext(currentItem);
      if (canToggle()) {
        break;
      }
    }
    prevItem->print(false, frame);
    return;

  case 4:
    itemClicked(true);
    return;

  case 0:
    itemClicked(false);
    return;
  }
}

bool TrackPlan::canToggle() {
  switch (currentItem->getAddress()) {
  case ADR_ESIG:
    return items.getState(ADR_ASIG1) == 0
        && items.getState(ADR_ASIG2) == 0;
  case ADR_ASIG1:
    return items.getState(ADR_ESIG) == 0
        && items.getState(ADR_ASIG2) == 0
        && items.getState(TURNOUT_10) == 0;
  case ADR_ASIG2:
    return items.getState(ADR_ESIG) == 0
        && items.getState(ADR_ASIG1) == 0
        && items.getState(TURNOUT_10) == 1
        && items.getState(TURNOUT_9) == 0;
  case TURNOUT_4: // will be toggled using TURNOUT_3
  case TURNOUT_6: // will be toggled using TURNOUT_7
    return false;
  default:
    return true;
  }
}

void TrackPlan::itemClicked(bool hold) {
  switch (currentItem->getAddress()) {
  case ADR_ESIG:
    if (currentItem->getState() > 0) {
      currentItem->setState(0, frame);
    }
    else if (hold) {
      currentItem->setState(3, frame);
    }
    else if (items.getState(TURNOUT_10) == 0) {
      currentItem->setState(1, frame);
    }
    else {
      currentItem->setState(2, frame);
    }
    return;

  case ADR_ASIG1:
    if (currentItem->getState() > 0) {
      currentItem->setState(0, frame);
    }
    else if (hold) {
      currentItem->setState(3, frame);
    }
    else {
      currentItem->setState(1, frame);
    }
    return;

  case ADR_ASIG2:
    if (currentItem->getState() > 0) {
      currentItem->setState(0, frame);
    }
    else if (hold) {
      currentItem->setState(3, frame);
    }
    else {
      currentItem->setState(2, frame);
    }
    return;

  case TURNOUT_2:
    items.getItem(TURNOUT_1)->setState(1, frame);
    break;
  case TURNOUT_3:
    items.getItem(TURNOUT_4)->toggle(frame);
    break;
  case TURNOUT_7:
    items.getItem(TURNOUT_6)->toggle(frame);
    break;
  case TURNOUT_9:
  case TURNOUT_10:
    items.getItem(ADR_ESIG)->setState(0, frame);
    items.getItem(ADR_ASIG1)->setState(0, frame);
    items.getItem(ADR_ASIG2)->setState(0, frame);
    break;
  }
  currentItem->toggle(frame);
}

