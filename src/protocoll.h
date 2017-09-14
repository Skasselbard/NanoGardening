#ifndef PROTOCOLL
#define PROTOCOLL

#include <Arduino.h>

enum Control {
  Null = 0,                // empty Data
  StartOfHeading = 1,      // first header character
  StartOfText = 2,         // first message character and last header character
  EndOfText = 3,           // interrupt message
  EndOfTransmission = 4,   // end message
  Enquiry = 5,             // ask for next message
  Acknowledge = 6,         // no problem occured
  NegativeAcknowledge = 21 // problem detected
};

enum Error {
  CannotReadHeader = 0,
  CannotProcessCommand = 1,
  UnexpectedPacket = 2,
  UnexpectedControl = 3
};

struct Header {
  u32 packetNumber;
  byte pin;
  byte ioType; // 1 = In = write; 0 = Out = read;
  byte writeValue;
};

void processControl(Control *control);
void processHeader(byte *firstByte);
void copyHeader(Header *from, Header *to);
void sendResponse();
void sendHeartbeat();
void sendError(Error error);

#endif