#include <SPI.h>

#include "manager.h"
#include "messageQueue.h"
#include "spii.h"

#define BUFFER_SIZE 20

byte inputBuffer[BUFFER_SIZE];
byte inputBufferPosition;
Message *currentWriteMessage = nullptr;
byte writeMessagePosition = 0;
MessageQueue readData = MessageQueue();
MessageQueue writeData = MessageQueue();
bool messageComplete = false;
State state = State::WaitForStart;

void Spii::printBuffer() {
  Serial.print("inputbuffer: ");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(String(inputBuffer[i], DEC) + ".");
  }
  Serial.println();
}

void processMessage() {
  Spii::printBuffer();
  readData.push(
      new Message((byte *)&inputBuffer, (unsigned int)inputBufferPosition));
  inputBufferPosition = 0;
}

byte getNextSendCharacter() {
  if (currentWriteMessage) {
    if (writeData.head() == nullptr) { // nothing to write
      return 0x00;
    }
    currentWriteMessage = writeData.pop();
    writeMessagePosition = 0;
    if (writeMessagePosition + 1 ==
        currentWriteMessage->length()) { // end of message
      delete currentWriteMessage;
      currentWriteMessage = nullptr;
      return 0x00;
    }
    writeMessagePosition++;
    return (currentWriteMessage->data())[writeMessagePosition - 1];
  }
  return 0x00;
}

// SPI interrupt routine
ISR(SPI_STC_vect) {
  byte received = SPDR;
  Serial.println(String(SPDR, DEC));
  inputBuffer[inputBufferPosition] = received; // SPDR;
  SPDR = getNextSendCharacter();
  if (inputBufferPosition < BUFFER_SIZE - 1) {
    inputBufferPosition++;
  }
  switch (state) {
  case State::WaitForStart: {
    if (received == Control::StartOfHeading) {
      state = State::ProcessHeader;
    }
    break;
  }
  case State::ProcessHeader: {
    if (received == Control::StartOfText) {
      state = State::ProcessMessage;
    }
    break;
  }
  case State::ProcessMessage: {
    if (received == Control::EndOfText) {
      state = State::WaitForEnd;
    }
  case State::WaitForEnd: {
    if (received == Control::EndOfTransmission) {
      state = State::WaitForStart;
      processMessage();
    }
  }
  }
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

void Spii::write(Message *message) {
  Serial.print("pushed message to spii: ");
  message->print();
  writeData.push(message);
}

Message *Spii::read() {
  if (readData.head() == nullptr) {
    return nullptr;
  } else {
    return readData.pop();
  }
}
