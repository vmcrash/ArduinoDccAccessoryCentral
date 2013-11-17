// Copyright 2012 by Thomas Singer
// Licensed under GPL v3, http://opensource.org/licenses/GPL-3.0

#ifndef _COMMAND_PARSER_H
#define _COMMAND_PARSER_H

const unsigned char BUFFER_SIZE = 10;

class CommandParser {
  unsigned char buffer[BUFFER_SIZE];
  unsigned char length;
  unsigned char parsePosition;
  
  void readSerialCommand();
  void printBuffer();
  bool parse();
  bool parseHex2(unsigned char &value);
  bool parseHex(unsigned char &value);
  bool getNextChar(unsigned char &chr);
  bool getNextUpperChar(unsigned char &chr);
  void setAccessoryPacket(unsigned int address, unsigned char value);
  
public:
  void setup();
  void processCommand();
};

#endif _COMMAND_PARSER_H
