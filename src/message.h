#ifndef MESSAGE
#define MESSAGE

#include "protocoll.h"

class Message {
private:
  unsigned int _length;
  byte *_data;

public:
  Message() : _length(0), _data(nullptr){};
  Message(byte *raw);
  Message(byte *message, unsigned int length);
  ~Message() { free(_data); }

  unsigned int length() { return _length; }
  byte *data() { return _data; }

  /// Returns sum of all bytes befor StartOfText excluding StartOfText
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