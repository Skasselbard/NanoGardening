#include <SPI.h>

#include "manager.h"
#include "spii.h"
#include <QueueList.h>

#define BUFFER_SIZE 20

char inputBuffer[BUFFER_SIZE];
byte inputBufferPosition;
String currentWriteMessage = "";
byte writeMessagePosition = 0;
QueueList<byte *> readData = QueueList<byte *>();
QueueList<String> writeData = QueueList<String>();
bool messageComplete = false;

void Spii::printBuffer() {
  Serial.print("inputbuffer: ");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(String(inputBuffer[i], DEC) + ".");
  }
  Serial.println();
}

// prints only the next element
void Spii::printReadData() {
  Serial.println("ReadData: ");
  if (!readData.isEmpty()) {
    for (int i = 0; readData.peek()[i] != 0x04; i++) {
      Serial.print(String(readData.peek()[i], DEC) + ".");
    }
  }
  Serial.println();
}

void processMessage() {
  byte *message = (byte *)malloc(sizeof(byte) * inputBufferPosition);
  for (int i = 0; i < inputBufferPosition; i++) {
    message[i] = inputBuffer[i];
    inputBuffer[i] = 0;
  }
  readData.push(message);
  inputBufferPosition = 0;
}

byte getNextSendCharacter() {
  if (currentWriteMessage == "") {
    if (writeData.isEmpty()) { // nothing to write
      return 0x00;
    }
    currentWriteMessage = writeData.pop();
    writeMessagePosition = 0;
  }
  if (writeMessagePosition + 1 ==
      currentWriteMessage.length()) { // end of message
    currentWriteMessage = "";
    return 0x00;
  }
  writeMessagePosition++;
  return (currentWriteMessage)[writeMessagePosition - 1];
}

// SPI interrupt routine
ISR(SPI_STC_vect) {
  byte received = SPDR;
  inputBuffer[inputBufferPosition] = received; // SPDR;
  SPDR = getNextSendCharacter();
  if (inputBufferPosition < BUFFER_SIZE - 1) {
    inputBufferPosition++;
  }
  if (received == 0x04) {
    processMessage();
  }
} // end of interrupt routine SPI_STC_vect

void Spii::initAsSlave() {
  // turn on SPI in slave mode
  pinMode(MISO, OUTPUT);
  SPCR |= bit(SPE); // set slave mode
  // SPCR &= ~bit(CPOL); // clock polarity
  // SPCR &= ~bit(CPHA); // clock phase
  // SPCR &= ~bit(MSTR);
  // SPCR |= bit(DORD); // most significant byte first
  inputBufferPosition = 0;
  // now turn on interrupts
  SPI.attachInterrupt();
}

void Spii::write(String message) { writeData.push(String(message)); }

byte *Spii::read() {
  if (readData.isEmpty()) {
    return nullptr;
  } else {
    return readData.pop();
  }
}
