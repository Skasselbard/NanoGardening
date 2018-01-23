#include "messageQueue.h"

MessageQueue::MessageQueue() {
  this->_head = nullptr;
  this->_tail = nullptr;
}
void MessageQueue::push(Message *message) {
  if (message == nullptr) {
    return;
  }
  Element *element = new Element();
  element->value = message;
  element->next = nullptr;
  if (_tail == nullptr) {
    _head = element;
    _tail = element;
    return;
  }
  _tail->next = element;
  _tail = element;
}
Message *MessageQueue::pop() {
  if (_head == nullptr) {
    return nullptr;
  }
  Message *returnValue = _head->value;
  if (_head == _tail) { // last element
    _tail = nullptr;
    delete _head;
    _head = nullptr;
    return returnValue;
  }
  Element *newHead = _head->next;
  delete _head;
  _head = newHead;
  return returnValue;
}
unsigned int MessageQueue::getSize() {
  if (_head == nullptr) {
    return 0;
  }
  int size = 0;
  for (Element *current = _head; current != nullptr; current = current->next) {
    size++;
  }
  return size;
}
