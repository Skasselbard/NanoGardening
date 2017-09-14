#ifndef PIN
#define PIN

#include <Arduino.h>

enum IOType { Read = 0, Write = 1 };

class Pin {
protected:
  uint8_t pinNumber;
  IOType ioType;

public:
  virtual bool isAnalog() = 0;
  uint8_t getPinNumber() { return pinNumber; }
};

class DigitalPin : public Pin {
public:
  bool isAnalog() { return false; }
  virtual bool readValue() = 0;
  virtual void writeValue(bool) = 0;
};

class AnalogPin : public Pin {
public:
  bool isAnalog() { return true; }
  virtual int readValue() = 0;
  virtual void writeValue(int) = 0;
};

#endif