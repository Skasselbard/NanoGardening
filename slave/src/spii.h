#ifndef SPII
#define SPII

#include "message.h"
#include <Arduino.h>

enum State { WaitForStart, ProcessHeader, ProcessMessage, WaitForEnd };

class Spii {
private:
public:
  static void write(Message *message);
  static Message *read();
  static void initAsSlave();
  // void initAsMaster();

  static void printBuffer();
  static void printReadData();
};

#endif