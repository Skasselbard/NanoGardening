#include "manager.h"
#include "plantSensor.h"

Manager *Manager::managerInstance = nullptr;

Manager::Manager() {
  Manager::managerInstance = this;
  for (int i = 0; i < 22; i++) {
    components[i] = nullptr;
  }
  initializeComponents();
  Serial.println("Initialized");
  while (true) {
    eventLoop();
  }
}

void Manager::initializeComponents() {
  for (byte i = A0; i <= A3; i++) {
    PlantSensor *sensor = new PlantSensor(i);
    components[i] = sensor;
  }
}
bool Manager::readPin(byte pin, unsigned int *valueOut) {
  if (!components[pin]) {
    return false;
  }
  setVCC();
  Pin *component = components[pin];
  if (component->isAnalog()) {
    int test = ((AnalogPin *)component)->readValue();
    *valueOut = static_cast<unsigned int>(test);
  }
  if (!component->isAnalog()) {
    *valueOut =
        static_cast<unsigned int>(((DigitalPin *)component)->readValue());
  }
  unsetVCC();
  return true;
}
bool Manager::writePin(byte pin, unsigned int writeValue) {
  if (!components[pin]) {
    return false;
  }
  Pin *component = components[pin];
  if (component->isAnalog()) {
    ((AnalogPin *)component)->writeValue(static_cast<int>(writeValue));
  }
  if (!component->isAnalog()) {
    ((DigitalPin *)component)->writeValue((bool)writeValue);
  }
  return false;
}

void Manager::eventLoop() { delay(1000); }

void Manager::setVCC() {
  vcc.writeValue(HIGH);
  delay(200);
}
void Manager::unsetVCC() { vcc.writeValue(LOW); }