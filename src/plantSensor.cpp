#include "Arduino.h"
#include "plantSensor.h"

void PlantSensor::initPlantSensor(){
  pinMode(humidity, INPUT);
}

int PlantSensor::readHumidity(){
  int currentHumidity = analogRead(humidity);
  Serial.print("humidity: ");
  Serial.println(currentHumidity);
  return currentHumidity;
}

int PlantSensor::readPlantSensor(){
  return readHumidity();
}
