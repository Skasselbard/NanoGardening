#include "spii.h"
#include <SPI.h>

#define BUFFER_SIZE 100

char inputBuffer[BUFFER_SIZE];
char outputBuffer[BUFFER_SIZE];
volatile byte bufferPosition;
String readData = "";
String writeData = "";

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

// SPI interrupt routine
ISR(SPI_STC_vect) {
  if (bufferPosition == BUFFER_SIZE) {
    bufferPosition = 0;      // reset buffer
    readData += inputBuffer; // save read data
    addWriteData(0);
  }
  inputBuffer[bufferPosition] = SPDR;
  SPDR = outputBuffer[bufferPosition];
  outputBuffer[bufferPosition] = 0;
  bufferPosition++;
} // end of interrupt routine SPI_STC_vect

void Spii::initAsSlave() {
  Serial.begin(9600);
  // turn on SPI in slave mode
  SPCR |= bit(SPE); // set slave mode
  // SPCR &= ~bit(CPOL); //clock polarity
  // SPCR &= ~bit(CPHA); //clock phase
  // SPCR &= ~bit(MSTR); //most significant byte first
  // SPCR &= ~bit(DORD);
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  // get ready for an interrupt
  bufferPosition = 0;
  // now turn on interrupts
  SPI.attachInterrupt();
}

void Spii::write(String message) {
  writeData += message;
  addWriteData(bufferPosition);
}

String Spii::read() { return ""; }
