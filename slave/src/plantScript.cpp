#ifndef PLANTSCRIPT
#define PLANTSCRIPT

#include "manager.h"
#include <Wire.h>

enum command { skip = 0, read = 1, write = 2 };
byte com = 0;
byte pin = 0;
uint16_t data = 0;
byte newPin = 0;
unsigned int newData = 0;
////////////////////////////////////////////////////////
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  uint8_t message[] = {((uint8_t *)&data)[0], ((uint8_t *)&data)[1]};
  Wire.write(message, 2); // block
  Serial.print("Send: ");
  Serial.print(message[1], HEX);
  Serial.println(message[0], HEX);
}

void receiveEvent(int byteCount) {
  if (Wire.available()) {
    com = (command)Wire.read();
  }
  if (com != command::skip) {
    if (Wire.available()) {
      newPin = (byte)Wire.read();
    }
    while (Wire.available()) {
      Wire.read();
    }
    Serial.print("Got Message: ");
    Serial.print(read);
    Serial.print(" ");
    Serial.println(newPin, HEX);
    Serial.flush();
    if (com == command::read) {
      if (Manager::getInstance()->readPin(newPin, &newData)) {
        Serial.flush();
        pin = newPin;
        data = static_cast<uint16_t>(newData);
        Serial.print("Read pin: ");
        Serial.print(pin, HEX);
        Serial.print(" = ");
        Serial.println(data, HEX);
      } else {
        Serial.println("Unable to read pin");
      }
    }
  }
}

int main() {
  init();
  Wire.begin(0x08);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);
  Manager *manager = new Manager();
}

#endif
