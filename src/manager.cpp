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
  for (uint8_t i = A0; i <= A7; i++) {
    PlantSensor *sensor = new PlantSensor(i);
    components[i] = sensor;
  }
}
bool Manager::readPin(byte pin, unsigned int *valueOut) {
  setVCC();
  Pin *component = components[pin];
  if (component->isAnalog()) {
    *valueOut =
        static_cast<unsigned int>(((AnalogPin *)component)->readValue());
  }
  if (!component->isAnalog()) {
    *valueOut =
        static_cast<unsigned int>(((DigitalPin *)component)->readValue());
  }
  unsetVCC();
  return true;
}
bool Manager::writePin(byte pin, byte writeValue) {
  Pin *component = components[pin];
  if (component->isAnalog()) {
    ((AnalogPin *)component)->writeValue((uint8_t)writeValue);
  }
  if (!component->isAnalog()) {
    ((DigitalPin *)component)->writeValue((bool)writeValue);
  }
  return false;
}

void Manager::eventLoop() {
  Message *rx = Spii::read();
  if (rx) {
    Serial.print("Manager got Message: ");
    for (int i = 0; rx->data()[i] != 0x04; i++) {
      Serial.print(String(rx->data()[i], DEC) + ".");
    }
    Serial.println();
    processControl((Control *)rx->data());
    delete rx;
  }
  // Spii::printReadData();
  Serial.flush();
  delay(1000);
}

void Manager::setVCC() {
  vcc.writeValue(HIGH);
  delay(200);
}
void Manager::unsetVCC() { vcc.writeValue(LOW); }