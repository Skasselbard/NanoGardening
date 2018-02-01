#include "pin.h"

void DigitalPin::writeValue(bool value) {
  if (ioType == IOType::Write) {
    digitalWrite(pinNumber, value);
  } else {
    Serial.println("Cant write digital pin configured as read pin.");
  }
}