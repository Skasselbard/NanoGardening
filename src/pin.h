#ifndef PIN
#define PIN

#include <Arduino.h>

enum IOType { Read = 0, Write = 1 };

class Pin {
protected:
  uint8_t pinNumber;
  IOType ioType;

public:
  Pin(u8 pin, IOType io) {
    pinNumber = pin;
    ioType = io;
    if (ioType == IOType::Read) {
      pinMode(pinNumber, INPUT);
    } else {
      pinMode(pinNumber, OUTPUT);
    }
  }
  virtual bool isAnalog() = 0;
  uint8_t getPinNumber() { return pinNumber; }
};

class DigitalPin : public Pin {
public:
  DigitalPin(u8 pinNumber, IOType io) : Pin(pinNumber, io) {}
  bool isAnalog() { return false; }
  virtual bool readValue() { return digitalRead(pinNumber); }
  virtual void writeValue(bool value) {
    if (ioType == IOType::Write) {
      digitalWrite(pinNumber, value);
    } else {
      Serial.println("Cant write digital pin configured as read pin.");
    }
  }
};

class AnalogPin : public Pin {
public:
  AnalogPin(u8 pinNumber, IOType io) : Pin(pinNumber, io) {}
  bool isAnalog() { return true; }
  virtual int readValue() = 0;
  virtual void writeValue(int) = 0;
};

#endif