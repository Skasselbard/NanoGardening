#ifndef VALVE
#define VALVE

#include <Arduino.h>

class Valve {
private:
  static const byte valve = 2;
  static const byte valveRead = 3;
  bool valveOpen;

public:
  void initValve();
  void openValve();
  void closeValve();
  bool isValveOpen();
  void printPin();
};

#endif
