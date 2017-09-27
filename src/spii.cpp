#include <SPI.h>

#include "manager.h"
#include "spii.h"

#define BUFFER_SIZE 20

char inputBuffer[BUFFER_SIZE];
char outputBuffer[BUFFER_SIZE];
byte inputBufferPosition;
byte outputBufferPosition;
String readData = "";
String writeData = "";
bool messageComplete = false;

void Spii::printBuffer() {
  Serial.print("inputbuffer: ");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(String(inputBuffer[i], HEX) + ".");
  }
  Serial.println();
  Serial.print("outputbuffer: ");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(String(outputBuffer[i], HEX) + ".");
  }
  Serial.println();
}

void Spii::printReadData() {
  Serial.println("ReadData: ");
  // Serial.print("length: ");
  // Serial.println(String(readData.length(), DEC));
  for (int i = 0; i < readData.length(); i++) {
    Serial.print(String(readData[i], HEX) + ".");
  }
  Serial.println();
}

void addWriteData(int bufferStart) {
  for (int i = 0; i < BUFFER_SIZE;
       i++) { // copy write data to buffer and remove the part from the write
              // data
    if (outputBuffer[(i + bufferStart) % bufferStart] != 0 ||
        writeData.length() == i) {
      if (writeData.length() == i) { // nothing left to send
        writeData = "";
      } else {
        writeData = writeData.substring(i);
      }
      break;
    }
    outputBuffer[(i + bufferStart) % bufferStart] = writeData[i];
  }
}

void processMessage() {
  for (int i = 0; i < inputBufferPosition; i++) {
    readData += inputBuffer[i];
    inputBuffer[i] = 0;
  }
  inputBufferPosition = 0;
}

// SPI interrupt routine
ISR(SPI_STC_vect) {
  byte received = SPDR;
  inputBuffer[inputBufferPosition] = received; // SPDR;
  SPDR = outputBuffer[outputBufferPosition];
  outputBuffer[outputBufferPosition] = 0;
  inputBufferPosition++;
  outputBufferPosition++;
  if (received == 0x04) {
    processMessage();
  }
  if (outputBufferPosition == BUFFER_SIZE - 1) {
    outputBufferPosition = 0; // reset buffer
    addWriteData(0);
  }
} // end of interrupt routine SPI_STC_vect

void Spii::initAsSlave() {
  Serial.begin(9600);
  // turn on SPI in slave mode
  pinMode(MISO, OUTPUT);
  SPCR |= bit(SPE); // set slave mode
  // SPCR &= ~bit(CPOL); // clock polarity
  // SPCR &= ~bit(CPHA); // clock phase
  // SPCR &= ~bit(MSTR);
  // SPCR |= bit(DORD); // most significant byte first
  inputBufferPosition = 0;
  outputBufferPosition = 0;
  // now turn on interrupts
  SPI.attachInterrupt();
}

void Spii::write(String message) {
  writeData += message;
  addWriteData(outputBufferPosition);
}

String Spii::read() {
  int messageEnd = readData.indexOf((byte)0x04); // End Of Transmission
  Serial.println(messageEnd, DEC);
  if (messageEnd == -1) {
    return "";
  } else {
    String message = readData.substring(0, messageEnd + 1);
    readData = readData.substring(messageEnd + 1);
    return message;
  }
}
