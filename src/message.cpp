#include "message.h"

Message::Message(byte *raw) {
  unsigned int i = 0;
  while (*raw != (byte)StartOfHeading && *raw != (byte)Enquiry &&
         *raw != (byte)Acknowledge && *raw != (byte)NegativeAcknowledge) {
    raw++;
  }
  i = headerLength();
  while (i != (byte)EndOfText) {
    i++;
  }
  while (i != (byte)EndOfTransmission) {
    i++;
  }
  _length = i + 1;
  _data = (byte *)malloc(_length * sizeof(byte));
  memcpy(_data, raw, _length);
}
Message::Message(byte *message, unsigned int length) {
  _length = length;
  _data = (byte *)malloc(_length * sizeof(byte));
  memcpy(_data, message, _length);
}

void Message::print() {
  for (int i = 0; i < _length; i++) {
    Serial.print(_data[i], DEC);
    Serial.print('.');
  }
  Serial.println();
}