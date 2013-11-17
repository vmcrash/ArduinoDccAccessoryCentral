// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#ifndef _CHARLCD_H
#define _CHARLCD_H

#include <inttypes.h>
#include "GLCD.h"

class CharLCD {
  
  GLCD lcd;
  unsigned char buffer[24 * 8];
  unsigned char charX;
  unsigned char charY;

  void _print(unsigned char chr, unsigned char x, unsigned char y);

public:
  CharLCD();
  unsigned char get(unsigned char x, unsigned char y);
  void set(unsigned char chr, unsigned char x, unsigned char y);
  void flushBuffer();
};


class CharFrame {
  CharLCD* lcd;
  unsigned char x;
  unsigned char y;
  unsigned char w;
  unsigned char h;

  bool scrollMode;

  unsigned char charX;
  unsigned char charY;

  void scroll();

public:
  CharFrame(CharLCD* lcd, unsigned char x, unsigned char y, unsigned char w, unsigned char h);
  
  void setScrollMode(bool scrollMode);
  void clear();
  void setCursor(unsigned char x, unsigned char y);
  
  unsigned char get(unsigned char x, unsigned char y);
  void set(unsigned char chr, unsigned char x, unsigned char y);

  // write the chr as symbol, even control characters
  void write(unsigned char chr, bool inverse = false);
  void write(const char* string, bool inverse = false);

  // will interpret control characters like tab, \r or \n
  void print(unsigned char chr);
  void print(const char* string);

  void setInverse(bool inverse, unsigned char x, unsigned char y);
};

#endif
