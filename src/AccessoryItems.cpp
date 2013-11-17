// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#include <stddef.h>
#include "AccessoryItems.h"
#include "DccSignaler.h"

AccessoryItem::AccessoryItem(unsigned char x, unsigned char y, unsigned char type, unsigned int address, unsigned char state)
  : x(x), y(y), type(type), address(address) {
  this->next = NULL;
  this->state = state;
}

unsigned char AccessoryItem::getChar() {
  unsigned char type = this->type;
  if (type < 0x20) {
    unsigned char state = this->state;
    state &= 0x01;
    state <<= 2;
    type ^= state;
    return type;
  }
  else {
    unsigned char state = this->state;
    state &= 0x03;
    return 0x0C + state;
  }
}

unsigned int AccessoryItem::getAddress() {
  return address;
}

unsigned char AccessoryItem::getState() {
  return state;
}

void AccessoryItem::setState(unsigned char state, CharFrame* frame) {
  if (type < 0x20) {
    if (state > 1) {
      return;
    }
  }
  else if (state > 3) {
    return;
  }
  
  if (this->state == state) {
    return;
  }
  
  this->state = state;
  sendPacket();
  print(false, frame);
}

void AccessoryItem::toggle(CharFrame* frame) {
  if (type < 0x20) {
    setState(state ^ 0x01, frame);
  }
  else {
    setState((state + 1) & 0x03, frame);
  }
}

void AccessoryItem::print(bool inverse, CharFrame* frame) {
  unsigned char chr = getChar();
  if (inverse) {
    chr |= 0x80;
  }
  frame->set(chr, x, y);
}

void AccessoryItem::sendPacket() {
  DCC.sendAccessoryPacket(address + (state >> 1), state & 0x01);
}



AccessoryItemList::AccessoryItemList() {
  count = 0;
  first = NULL;
  last = NULL;
}

unsigned char AccessoryItemList::getCount() {
  return count;
}

void AccessoryItemList::add(AccessoryItem* item) {
  if (first == NULL) {
    first = item;
  }
  if (last != NULL) {
    last->next = item;
  }
  last = item;
  count++;
}

unsigned char AccessoryItemList::indexOf(AccessoryItem* itemToSearch) {
  unsigned char index = 0;
  for (AccessoryItem* item = first; item != NULL; item = item->next) {
    if (item == itemToSearch) {
      return index;
    }
    
    index++;
  }
  
  // to be safe
  return 0;
}

AccessoryItem* AccessoryItemList::get(unsigned char index) {
  AccessoryItem* item = first;
  for (; index > 0; index--) {
    // to be safe
    if (item == NULL) {
      return first;
    }

    item = item->next;
  }
  return item;
}

AccessoryItem* AccessoryItemList::getFirst() {
  return first;
}

AccessoryItem* AccessoryItemList::getPrev(AccessoryItem* item) {
  unsigned char index = indexOf(item);
  if (index == 0) {
    return last;
  }
  
  return get(index - 1);
}

AccessoryItem* AccessoryItemList::getNext(AccessoryItem* item) {
  AccessoryItem* next = item->next;
  if (next == NULL) {
    return first;
  }
  
  return next;
}

AccessoryItem* AccessoryItemList::getItem(unsigned int address) {
  for (AccessoryItem* item = first; item != NULL; item = item->next) {
    if (item->getAddress() == address) {
      return item;
    }
  }
  // nothing found, return first
  return first;
}

unsigned char AccessoryItemList::getState(unsigned int address) {
  AccessoryItem* item = getItem(address);
  return item->getState();
}

