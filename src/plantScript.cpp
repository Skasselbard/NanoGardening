#ifndef PLANTSCRIPT
#define PLANTSCRIPT

#include <Arduino.h>
#include "valve.h"
#include "plantSensor.h"

#include <SPI.h>

char buf[100];
volatile byte pos;
volatile boolean process_it;
volatile byte sendPosition;
char sendbuffer[] = "bapedibupi\n";

// PlantSensor plantSensor = PlantSensor();
// Valve valve = Valve();

// SPI interrupt routine
ISR(SPI_STC_vect)
{
  byte c = SPDR; // grab byte from SPI Data Register
  // add to buffer if room
  if (pos < (sizeof(buf) - 1))
    buf[pos++] = c;

  // example: newline means time to process buffer
  if (pos == 11)
    process_it = true;

  if (sendPosition < (sizeof(sendbuffer) - 1))
    SPDR = sendbuffer[sendPosition++];
  else
    SPDR = 0;
} // end of interrupt routine SPI_STC_vect

void eventLoop()
{
  if (process_it)
  {
    buf[pos] = 0;
    pos = 0;
    sendPosition = 0;
    process_it = false;
    Serial.print(buf);
  } // end of flag set
}

void setup()
{
  Serial.begin(9600);
  // turn on SPI in slave mode
  SPCR |= bit(SPE);
  // SPCR &= ~bit(CPOL);
  // SPCR &= ~bit(CPHA);
  // SPCR &= ~bit(MSTR);
  //SPCR &= ~bit(DORD);
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  // get ready for an interrupt
  pos = 0; // buffer empty
  sendPosition = 0;
  process_it = false;
  // now turn on interrupts
  SPI.attachInterrupt();
}
int main()
{
  init();
  setup();
  while (true)
  {
    eventLoop();
  }
}

// void eventLoop()
// {
//   plantSensor.readPlantSensor();
//   valve.openValve();
//   delay(500);
//   valve.closeValve();
//   delay(500);
//   Serial.flush();
// }

// int main()
// {
//   init();
//   Serial.begin(9600);
//   valve.initValve();
//   plantSensor.initPlantSensor();
//   pinMode(2, OUTPUT);
//   digitalWrite(2, HIGH);
//   while (true)
//   {
//     eventLoop();
//   }
// }

#endif
