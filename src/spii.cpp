#include <SPI.h>

#include "manager.h"
#include "spii.h"

#define BUFFER_SIZE 100

char inputBuffer[BUFFER_SIZE];
char outputBuffer[BUFFER_SIZE];
byte bufferPosition;
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
  // Serial.println();
  // Serial.println("BufferPosition: " + String(bufferPosition, DEC));
  byte received = SPDR;
  // if (bufferPosition == BUFFER_SIZE - 1) {
  //   bufferPosition = 0;                     // reset buffer
  //   readData += inputBuffer;                // save read data
  //   for (int i = 0; i < BUFFER_SIZE; i++) { // delete input buffer
  //     inputBuffer[i] = 0;
  //   }
  //   addWriteData(0);
  //   Serial.print("in: ");
  //   Serial.println(inputBuffer);
  //   Serial.print("out: ");
  //   Serial.println(outputBuffer);
  //   Serial.println("read: " + readData);
  //   Serial.println("write: " + writeData);
  // }
  // Serial.println("tx: " + String(received, HEX));
  // Serial.print("buffer: ");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(String(inputBuffer[i], HEX) + ".");
  }
  Serial.println();
  inputBuffer[bufferPosition] = received; // SPDR;
  // SPDR = outputBuffer[bufferPosition];
  // outputBuffer[bufferPosition] = 0;
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

String Spii::read() {
  int messageEnd = readData.indexOf(0x04); // End Of Transmission
  if (messageEnd == -1) {
    return "";
  } else {
    String message = readData.substring(0, messageEnd + 1);
    readData = readData.substring(messageEnd + 1);
    return message;
  }
}
