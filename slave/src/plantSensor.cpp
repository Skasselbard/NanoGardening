#include "plantSensor.h"
#include "Arduino.h"

PlantSensor::PlantSensor(byte pin) : AnalogPin(pin, IOType::Read) {
  pinMode(pin, INPUT);
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
