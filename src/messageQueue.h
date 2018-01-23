#ifndef MESSAGEQUEUE
#define MESSAGEQUEUE

#include "message.h"

class Element {
public:
  Message *value;
  Element *next;
};

class MessageQueue {
private:
  Element *_head;
  Element *_tail;

public:
  MessageQueue();
  void push(Message *message);
  Message *pop();
  Message *head() {
    if (_head == nullptr) {
      return nullptr;
    } else {
      return _head->value;
    }
  }
  unsigned int getSize();
};

#endif