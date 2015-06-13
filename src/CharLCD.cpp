// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#include <avr/io.h>
#include "font8x8.h"
#include "CharLCD.h"

CharLCD::CharLCD() {
  lcd.init();

  for (unsigned char i = 0; i < 24 * 8; i++) {
    buffer[i] = ' ';
  }

  flushBuffer();  
}

void CharLCD::flushBuffer() {
  for (unsigned char y = 8; y-- > 0; ) {
    for (unsigned char x = 24; x-- > 0; ) {
      _print(get(x, y), x, y);
    }
  }
}

void CharLCD::_print(unsigned char chr, unsigned char x, unsigned char y) {
  if (x >= 24 || y >= 8) {
    return;
  }

  unsigned char xorMask = (chr & 0x80) != 0 ? 0xFF : 0;
  chr &= 0x7F;

  unsigned char chip = x / 8;

  lcd.writeCommand(0xb8 | y, chip);
  lcd.writeCommand(0x40 | ((x & 7) * 8), chip);

  const uint8_t* ptr = FONT8x8 + chr * 8;
  for (unsigned i = 8; i-- > 0; ptr++) {
    unsigned char data = pgm_read_byte(ptr);
    data ^= xorMask;
    lcd.writeData(data, chip);
  }
}

unsigned char CharLCD::get(unsigned char x, unsigned char y) {
  if (x >= 24 || y >= 8) {
    return 0;
  }
  
  return buffer[y * 24 + x];
}

void CharLCD::set(unsigned char chr, unsigned char x, unsigned char y) {
  if (x >= 24 || y >= 8) {
    return;
  }

  unsigned char i = y * 24 + x;
  if (buffer[i] == chr) {
    return;
  }

  buffer[i] = chr;
  _print(chr, x, y);  
}


CharFrame::CharFrame(CharLCD* lcd, unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  this->lcd = lcd;
  this->x = x;
  this->y = y;
  // ensure at least a size of 1x1
  this->w = w > 0 ? w : 1;
  this->h = h > 0 ? h : 1;
  
  this->scrollMode = false;

  clear();
}

void CharFrame::clear() {
  for (unsigned char y = 0; y < h; y++) {
    for (unsigned char x = 0; x < w; x++) {
      set(' ', x, y);
    }
  }

  setCursor(0, 0);
}

void CharFrame::setScrollMode(bool scrollMode) {
  this->scrollMode = scrollMode;
}

void CharFrame::write(unsigned char chr, bool inverse) {
  if (charX >= w) {
    charX = 0;
    charY++;
  }

  if (charY >= h) {
    if (scrollMode) {
      scroll();
      charY = h - 1;
    }
    else {
      charY = 0;
    }
  }

  if (inverse) {
    chr ^= 0x80;
  }
  set(chr, charX, charY);
  
  charX++;
}

void CharFrame::write(const char* string, bool inverse) {
  for (unsigned char character = *string; character != 0; string++, character = *string) {
    write(character, inverse);
  }
}

void CharFrame::print(unsigned char chr) {
  if (chr == 0x0a || chr == 0x0d) {
    charX = 0;
    charY++;
    if (charY >= 8) {
      scroll();
      charY = 7;
    }
    return;
  }

  if (chr == 0x0c) {
    clear();
    return;
  }

  write(chr);
}

void CharFrame::print(const char* string) {
  for (unsigned char character = *string; character != 0; string++, character = *string) {
    print(character);
  }
}

void CharFrame::setCursor(unsigned char x, unsigned char y) {
  charX = x;
  charY = y;
}

unsigned char CharFrame::get(unsigned char x, unsigned char y) {
  if (x >= w || y >= h) {
    return 0;
  }

  return lcd->get(x + this->x, y + this->y);
}

void CharFrame::set(unsigned char chr, unsigned char x, unsigned char y) {
  if (x >= w || y >= h) {
    return;
  }
  
  lcd->set(chr, x + this->x, y + this->y);
}

void CharFrame::setInverse(bool inverse, unsigned char x, unsigned char y) {
  unsigned char chr = get(x, y);
  if (inverse) {
    chr |= 0x80;
  }
  else {
    chr &= 0x7F;
  }
  set(chr, x, y);
}

void CharFrame::scroll() {
  for (unsigned char y = 0; y < h; y++) {
    for (unsigned char x = 0; x < w; x++) {
      unsigned char nextY = y + 1;
      unsigned char chr = nextY < h ? get(x, nextY) : ' ';
      set(chr, x, y);
    }
  }
}

