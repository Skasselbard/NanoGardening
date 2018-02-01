#include "valve.h"


void Valve::initValve(){
  pinMode(valve, OUTPUT);
  pinMode(valveRead, INPUT);
  closeValve();
}

void Valve::openValve(){
  digitalWrite(valve, HIGH);
  valveOpen = true;
  Serial.println("valve opened");
}

void Valve::closeValve(){
  digitalWrite(valve, LOW);
  valveOpen = false;
  Serial.println("valve closed");
}

bool Valve::isValveOpen(){
  return valveOpen;
}

void Valve::printPin(){
  Serial.println("Valve listens on pin: " + valve);
}
