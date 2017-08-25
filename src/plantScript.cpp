#ifndef PLANTSCRIPT
#define PLANTSCRIPT

#include "plantSensor.h"
#include "spii.h"
#include "valve.h"
#include <Arduino.h>

PlantSensor plantSensor = PlantSensor();
Valve valve = Valve();

void eventLoop() {
  plantSensor.readPlantSensor();
  valve.openValve();
  delay(500);
  valve.closeValve();
  delay(500);
  Serial.flush();
}

int main() {
  init();
  Serial.begin(9600);
  valve.initValve();
  plantSensor.initPlantSensor();
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  while (true) {
    eventLoop();
  }
}

#endif
