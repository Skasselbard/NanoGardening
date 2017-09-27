#include "manager.h"
#include "plantSensor.h"
#include "protocoll.h"
#include "spii.h"

Manager *Manager::managerInstance = nullptr;

Manager::Manager() {
  Manager::managerInstance = this;
  Spii::initAsSlave();
  for (int i = 0; i < 22; i++) {
    components[i] = nullptr;
  }
  initializeComponents();
  while (true) {
    eventLoop();
  }
}

void Manager::initializeComponents() {
  for (u8 i = A0; i <= A7; i++) {
    PlantSensor *sensor = new PlantSensor(i);
    components[i] = sensor;
  }
}
bool Manager::readPin(byte pin, byte *valueOut) {
  Pin *component = components[pin];
  if (component->isAnalog()) {
    *valueOut = ((AnalogPin *)component)->readValue();
  }
  if (!component->isAnalog()) {
    *valueOut = ((DigitalPin *)component)->readValue();
  }
  return false;
}
bool Manager::writePin(byte pin, byte writeValue) {
  Pin *component = components[pin];
  if (component->isAnalog()) {
    ((AnalogPin *)component)->writeValue((u8)writeValue);
  }
  if (!component->isAnalog()) {
    ((DigitalPin *)component)->writeValue((bool)writeValue);
  }
  return false;
}

void Manager::eventLoop() {
  String rx = Spii::read();
  if (rx != "") {
    Serial.print("Manager got Message: ");
    Serial.println(rx);
    const char *message = rx.c_str();
    processControl((Control *)message);
  }
  Spii::printBuffer();
  Spii::printReadData();
  Serial.flush();
  delay(2000);
}