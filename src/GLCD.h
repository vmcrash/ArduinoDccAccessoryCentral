// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#ifndef _GLCD_H
#define _GLCD_H

#include <inttypes.h>

class GLCD {
    void init(unsigned char chip);
    void waitReady();
    void writeChip(unsigned char value, unsigned char chip, unsigned char di);
public:
    unsigned char readLcd();
    void writeLcd(unsigned char value);
    void setLcdOutput();
    void setLcdInput();
    void writeCommand(unsigned char cmd, unsigned char chip);
    unsigned char readData(unsigned char chip);
    void writeData(unsigned char data, unsigned char chip);
    void init();
};

#endif
