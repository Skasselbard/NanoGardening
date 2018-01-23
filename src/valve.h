#ifndef VALVE
#define VALVE

#include <Arduino.h>

class Valve {
private:
  static const uint8_t valve = 2;
  static const uint8_t valveRead = 3;
  bool valveOpen;

public:
  void initValve();
  void openValve();
  void closeValve();
  bool isValveOpen();
  void printPin();
};

#endif
