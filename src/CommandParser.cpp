// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#include <Arduino.h>
#include "DccSignaler.h"
#include "CommandParser.h"

unsigned char toUpper(unsigned char chr) {
  return chr >= 'a' && chr <= 'z' 
    ? chr - 0x20
    : chr;
}

void CommandParser::setup() {
  Serial.begin(9600);
}

void CommandParser::readSerialCommand() {
  length = 0;
  for (;;) {
    int readValue = Serial.read();
    if (readValue < 0) {
      continue;
    }
    
    unsigned char chr = readValue & 0xFF;
    if (chr == 0x7f && length > 0) {
      length--;
      continue;
    }
    
    if (chr == 0x0a || chr == 0x0d) {
      if (length > 0) {
        return;
      }
      
      continue;
    }
    
    if (length >= BUFFER_SIZE) {
      continue;
    }

    buffer[length] = chr;
    length++;
  }
}

void CommandParser::printBuffer() {
  for (unsigned char i = 0; i < length; i++) {
    Serial.write(buffer[i]);
  }
  Serial.write('\n');
}

void CommandParser::processCommand() {
  readSerialCommand();
  Serial.write('>');
  printBuffer();
  
  parsePosition = 0;
  if (parse()) {
    return;
  }

  printBuffer();
  for (unsigned char i = 0; i < parsePosition; i++) {
    Serial.print(' ');
  }
  Serial.print("^\n");
}

bool CommandParser::parse() {
  unsigned char address = 0;
  if (!parseHex2(address)) {
    return false;
  }

  unsigned char chr = 0;
  if (!getNextUpperChar(chr)) {
    return false;
  }

  if (chr != 'R' && chr != 'G') {
    parsePosition--;
    return false;
  }
  
  unsigned char redGreen = chr == 'R' ? 0 : 1;
  if (getNextChar(chr)) {
    parsePosition--;
    return false;
  }
  
  setAccessoryPacket(address, redGreen);
  return true;
}

bool CommandParser::parseHex2(unsigned char &value) {
  unsigned char value2 = 0;
  if (!parseHex(value2)) {
    return false;
  }
  
  value2 <<= 4;
  if (!parseHex(value2)) {
    return false;
  }
  
  value = value2;
  return true;
}

bool CommandParser::getNextChar(unsigned char &chr) {
  if (length <= parsePosition) {
    return false;
  }
  
  chr = buffer[parsePosition];
  parsePosition++;
  return true;
}

bool CommandParser::getNextUpperChar(unsigned char &chr) {
  if (!getNextChar(chr)) {
    return false;
  }
  
  chr = toUpper(chr);
  return true;
}

bool CommandParser::parseHex(unsigned char &value) {
  unsigned char chr = 0;
  if (!getNextUpperChar(chr)) {
    parsePosition--;
    return false;
  }

  if (chr >= '0' && chr <= '9') {
    chr -= '0';
  }
  else if (chr >= 'A' && chr <= 'F') {
    chr -= 'A';
    chr += 10;
  }
  else {
    parsePosition--;
    return false;
  }

  value &= 0xF0;
  value += chr;
  return true;
}

void CommandParser::setAccessoryPacket(unsigned int address, unsigned char value) {
  Serial.print("address=");
  Serial.println(address, DEC);
  value &= 0x01;
  Serial.print("redGreen=");
  Serial.println(value, DEC);
  value |= (address & 0x03) << 1;
  value |= 0x08;
  address >>= 2;
  unsigned char byte0 = 0x80 | (address & 0x3F);
  address = (~address >> 2);
  unsigned char byte1 = 0x80 | (address & 0x70) | (value & 0x0F);
  Serial.print(byte0, HEX);
  Serial.write('(');
  Serial.print(byte0, BIN);
  Serial.write(") ");
  Serial.print(byte1, HEX);
  Serial.write('(');
  Serial.print(byte1, BIN);
  Serial.write(")\n");
  DCC.setNextPacket(byte0, byte1);
  DCC.setNextPacket(byte0, byte1);
  DCC.setNextPacket(byte0, byte1);
}

