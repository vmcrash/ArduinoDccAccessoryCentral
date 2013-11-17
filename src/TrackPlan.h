// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#ifndef __TRACK_PLAN_H
#define __TRACK_PLAN_H

#include "CharLCD.h"
#include "RotaryEncoder.h"
#include "AccessoryItems.h"


class TrackPlan {
  CharFrame* frame;
  RotaryEncoder* encoder;
  AccessoryItemList items;
  AccessoryItem* currentItem;
  void itemClicked(bool hold);
  bool canToggle();
  
public:
  TrackPlan(CharLCD* lcd, RotaryEncoder* encoder);
  void show();
  void sendAllPackets();
  void loop();
};

#endif
