#ifndef PLANTSENSOR
#define PLANTSENSOR

#include "pin.h"

class PlantSensor : public AnalogPin {
public:
  PlantSensor(uint8_t);
  int readHumidity() { return readValue(); }
  int readPlantSensor() { return readValue(); }
  int readValue();
  void writeValue(int);
};

#endif
