#ifndef PLANTSCRIPT
#define PLANTSCRIPT

#include "manager.h"
#include <Arduino.h>

int main() {
  init();
  Serial.begin(9600);
  Manager *manager = new Manager();
}

#endif
