#ifndef PLANTSCRIPT
#define PLANTSCRIPT

#include "manager.h"

int main() {
  init();
  Serial.begin(115200);
  Manager *manager = new Manager();
}

#endif
