#ifndef MANAGER
#define MANAGER

#include <Arduino.h>

#include "pin.h"

class Manager {
private:
  Pin *components[22];
  DigitalPin vcc = DigitalPin(2, IOType::Write);
  static Manager *managerInstance;
  void initializeComponents();
  void eventLoop();
  void setVCC();
  void unsetVCC();

public:
  Manager();
  static Manager *getInstance() { return managerInstance; }
  bool readPin(byte pin, byte *valueOut);
  bool writePin(byte pin, byte writeValue);
};

#endif