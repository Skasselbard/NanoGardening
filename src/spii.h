#ifndef SPII
#define SPII

#include "Arduino.h"

class Spii {
private:
public:
  static void write(String message);
  static String read();
  static void initAsSlave();
  // void initAsMaster();
};

#endif