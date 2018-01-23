#ifndef MESSAGE
#define MESSAGE

#include "protocoll.h"

class Message {
private:
  unsigned int _length;
  uint8_t *_data;

public:
  Message() : _length(0), _data(nullptr){};
  Message(uint8_t *raw);
  Message(uint8_t *message, unsigned int length);
  ~Message() { free(_data); }

  unsigned int length() { return _length; }
  uint8_t *data() { return _data; }

  /// Returns sum of all chars befor StartOfText excluding StartOfText
  /// Undefined for messages without StartOfText
  unsigned int headerLength() {
    unsigned int i = 0;
    while (_data[i] != StartOfText) {
      i++;
    }
    return i;
  }
};

#endif