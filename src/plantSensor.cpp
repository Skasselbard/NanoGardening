#include "plantSensor.h"
#include "Arduino.h"

PlantSensor::PlantSensor(uint8_t pin) {
  pinMode(pin, INPUT);
  pinNumber = pin;
  ioType = Read;
}

int PlantSensor::readValue() {
  int currentHumidity = analogRead(pinNumber);
  Serial.print("humidity: ");
  Serial.println(currentHumidity);
  return currentHumidity;
}

void PlantSensor::writeValue(int) {
  Serial.print("Error: trying to write humidity sensor");
}
