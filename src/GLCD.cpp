// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#include <avr/io.h>
#include "GLCD.h"

#define DI_PORT PORTC
#define DI_DDR  DDRC
#define DI_MASK 0x01

#define EN_PORT PORTC
#define EN_DDR  DDRC
#define EN_MASK 0x04

#define RW_PORT PORTC
#define RW_DDR  DDRC
#define RW_MASK 0x02

#define CS1_PORT PORTB
#define CS1_DDR  DDRB
#define CS1_MASK 0x04

#define CS2_PORT PORTC
#define CS2_DDR  DDRC
#define CS2_MASK 0x08

#define CS3_PORT PORTC
#define CS3_DDR  DDRC
#define CS3_MASK 0x10


#define _setHigh(port,mask) (port |= mask)
#define _setLow(port,mask)  (port &= ~mask)
#define LOW 0
#define HIGH 1
void setDI (bool bit) { if (bit) { _setHigh(DI_PORT , DI_MASK ); } else { _setLow(DI_PORT , DI_MASK ); } }
void setEN (bool bit) { if (bit) { _setHigh(EN_PORT , EN_MASK ); } else { _setLow(EN_PORT , EN_MASK ); } }
void setRW (bool bit) { if (bit) { _setHigh(RW_PORT , RW_MASK ); } else { _setLow(RW_PORT , RW_MASK ); } }
void setCS1(bool bit) { if (bit) { _setHigh(CS1_PORT, CS1_MASK); } else { _setLow(CS1_PORT, CS1_MASK); } }
void setCS2(bool bit) { if (bit) { _setHigh(CS2_PORT, CS2_MASK); } else { _setLow(CS2_PORT, CS2_MASK); } }
void setCS3(bool bit) { if (bit) { _setHigh(CS3_PORT, CS3_MASK); } else { _setLow(CS3_PORT, CS3_MASK); } }


void setBits(unsigned char mask) {
    PORTC |= mask;
}

void clearBits(unsigned char mask) {
    PORTC &= ~mask;
}

void selectChip(unsigned char chip) {
    if (chip == 0) {
      setCS2(HIGH);
      setCS3(HIGH);
      setCS1(LOW);
    }
    else if (chip == 1) {
      setCS1(HIGH);
      setCS3(HIGH);
      setCS2(LOW);
    }
    else if (chip == 2) {
      setCS1(HIGH);
      setCS2(HIGH);
      setCS3(LOW);
    }
    else {
      setCS1(HIGH);
      setCS2(HIGH);
      setCS3(HIGH);
    }
}

void enDelay() {
    asm volatile("nop \n\t");
    asm volatile("nop \n\t");
    asm volatile("nop \n\t");
    asm volatile("nop \n\t");
}

void ledOn() {
    PORTB |= 0x20; // LED
}

void ledOff() {
    PORTB &= ~0x20; // LED
}

unsigned char GLCD::readLcd() {
    unsigned char value = 0;
    if (PIND & 0x04) value |= 0x01;
    if (PIND & 0x08) value |= 0x02;
    if (PIND & 0x10) value |= 0x04;
    if (PIND & 0x20) value |= 0x08;
    if (PIND & 0x40) value |= 0x10;
    if (PIND & 0x80) value |= 0x20;
    if (PINB & 0x01) value |= 0x40;
    if (PINB & 0x02) value |= 0x80;
    return value;
}

void GLCD::writeLcd(unsigned char value) {
    if (value & 0x01) PORTD |= 0x04; else PORTD &= ~0x04;
    if (value & 0x02) PORTD |= 0x08; else PORTD &= ~0x08;
    if (value & 0x04) PORTD |= 0x10; else PORTD &= ~0x10;
    if (value & 0x08) PORTD |= 0x20; else PORTD &= ~0x20;
    if (value & 0x10) PORTD |= 0x40; else PORTD &= ~0x40;
    if (value & 0x20) PORTD |= 0x80; else PORTD &= ~0x80;
    if (value & 0x40) PORTB |= 0x01; else PORTB &= ~0x01;
    if (value & 0x80) PORTB |= 0x02; else PORTB &= ~0x02;
}

void GLCD::setLcdOutput() {
    DDRB |= 0x03;
    DDRD |= 0xFC;
}

void GLCD::setLcdInput() {
    PORTB &= 0xFC;
    DDRB &= 0xFC;

    PORTD &= 0x03;
    DDRD &= 0x03;
}

void GLCD::waitReady() {
    setLcdInput();
    setDI(LOW);
    setRW(HIGH);
    ledOn();
    do {
        setEN(LOW);
        enDelay();
        setEN(HIGH);
        enDelay();

        setEN(LOW);
        enDelay();
        setEN(HIGH);
        enDelay();
    }
    while ((readLcd() & 0x80) != 0);
    setEN(LOW);
    enDelay();
    setRW(LOW);
    ledOff();
}

unsigned char GLCD::readData(unsigned char chip) {
    selectChip(chip);
    waitReady();

    setLcdInput();
    setDI(HIGH);
    setRW(HIGH);
    ledOn();

    setEN(LOW);
    enDelay();
    setEN(HIGH);
    enDelay();

    setEN(LOW);
    enDelay();
    setEN(HIGH);
    enDelay();

    unsigned char value = readLcd();

    setEN(LOW);
    enDelay();
    setRW(LOW);
    ledOff();
    return value;
}

void GLCD::writeChip(unsigned char value, unsigned char chip, unsigned char di) {
    enDelay();
    selectChip(chip);
    waitReady();
    if (di) {
      setDI(HIGH);
    }
    else {
      setDI(LOW);
    }
    setRW(LOW);
    setLcdOutput();
    enDelay();
    writeLcd(value);
    enDelay();
    setEN(HIGH);
    enDelay();
    setEN(LOW);
    enDelay();
    setLcdInput();
}

void GLCD::writeCommand(unsigned char cmd, unsigned char chip) {
    writeChip(cmd, chip, false);
}

void GLCD::writeData(unsigned char data, unsigned char chip) {
    writeChip(data, chip, true);
}

void GLCD::init(unsigned char chip) {
    writeCommand(0x3F, chip);
    writeCommand(0x40, chip);
    writeCommand(0xB8, chip);
    writeCommand(0xC0, chip);
}

void GLCD::init() {
    selectChip(0);
    DI_DDR |= DI_MASK;
    EN_DDR |= EN_MASK;
    RW_DDR |= RW_MASK;
    CS1_DDR |= CS1_MASK;
    CS2_DDR |= CS2_MASK;
    CS3_DDR |= CS3_MASK;
    enDelay();
    setDI(LOW);
    setRW(LOW);
    setEN(LOW);
    enDelay();
    init(0);
    init(1);
    init(2);
    enDelay();
}

