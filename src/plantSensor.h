#ifndef PLANTSENSOR
#define PLANTSENSOR

#include "Arduino.h"

class PlantSensor{
private:
  static const uint8_t humidity = A0;
public:
  void initPlantSensor();
  int readHumidity();
  int readPlantSensor();
};

#endif
