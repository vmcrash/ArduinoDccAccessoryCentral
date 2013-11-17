// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#ifndef __ACCESSORY_ITEMS_H
#define __ACCESSORY_ITEMS_H

#include "CharLCD.h"

class AccessoryItem {
  const unsigned char x;
  const unsigned char y;
  const unsigned char type;
  const unsigned int address;
  unsigned char state;

  unsigned char getChar();

public:
  AccessoryItem* next;

  AccessoryItem(unsigned char x, unsigned char y, unsigned char type, unsigned int address, unsigned char state = 0);
  unsigned int getAddress();
  unsigned char getState();
  void setState(unsigned char state, CharFrame* frame);
  void toggle(CharFrame* frame);
  void print(bool inverse, CharFrame* frame);
  void sendPacket();
};


class AccessoryItemList {
  unsigned char count;
  AccessoryItem* first;
  AccessoryItem* last;
  AccessoryItem* get(unsigned char index);
  unsigned char indexOf(AccessoryItem* item);

public:
  AccessoryItemList();
  unsigned char getCount();
  void add(AccessoryItem* item);
  AccessoryItem* getFirst();
  AccessoryItem* getPrev(AccessoryItem* item);
  AccessoryItem* getNext(AccessoryItem* item);
  AccessoryItem* getItem(unsigned int address);
  unsigned char getState(unsigned int address);
};

#endif
