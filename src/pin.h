#ifndef PIN
#define PIN

#include <Arduino.h>

enum IOType { Read = 0, Write = 1 };

class Pin {
protected:
  byte pinNumber;
  IOType ioType;

public:
  Pin(byte pin, IOType io) {
    pinNumber = pin;
    ioType = io;
    if (ioType == IOType::Read) {
      pinMode(pinNumber, INPUT);
    } else {
      pinMode(pinNumber, OUTPUT);
    }
  }
  virtual bool isAnalog() = 0;
  byte getPinNumber() { return pinNumber; }
};

class DigitalPin : public Pin {
public:
  DigitalPin(byte pinNumber, IOType io) : Pin(pinNumber, io) {}
  bool isAnalog() { return false; }
  virtual bool readValue() { return digitalRead(pinNumber); }
  virtual void writeValue(bool value);
};

class AnalogPin : public Pin {
public:
  AnalogPin(byte pinNumber, IOType io) : Pin(pinNumber, io) {}
  bool isAnalog() { return true; }
  virtual int readValue() = 0;
  virtual void writeValue(int) = 0;
};

#endif