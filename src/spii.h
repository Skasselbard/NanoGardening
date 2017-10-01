#ifndef SPII
#define SPII

#include "Arduino.h"

class Spii {
private:
public:
  static void write(String message);
  static byte *read();
  static void initAsSlave();
  // void initAsMaster();

  static void printBuffer();
  static void printReadData();
};

#endif