#include "message.h"

Message::Message(uint8_t *raw) {
  unsigned int i = 0;
  while (*raw != (uint8_t)StartOfHeading && *raw != (uint8_t)Enquiry &&
         *raw != (uint8_t)Acknowledge && *raw != (uint8_t)NegativeAcknowledge) {
    raw++;
  }
  i = headerLength();
  while (i != (uint8_t)EndOfText) {
    i++;
  }
  while (i != (uint8_t)EndOfTransmission) {
    i++;
  }
  _length = i + 1;
  _data = (uint8_t *)malloc(_length * sizeof(uint8_t));
  memcpy(_data, raw, _length);
}
Message::Message(uint8_t *message, unsigned int length) {
  _length = length;
  _data = (uint8_t *)malloc(_length * sizeof(uint8_t));
  memcpy(_data, message, _length);
}