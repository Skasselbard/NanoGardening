#include "protocoll.h"
#include "manager.h"
#include "message.h"
#include "spii.h"

Headerr *lastHeader = (Headerr *)calloc(1, sizeof(Headerr));
Headerr *receivedHeader = nullptr;
unsigned int sendData = 0;

void processControl(Control *control) {
  // Serial.println("Processing");
  switch ((byte)*control) {
  case Null: { // do nothing
    Serial.println("got NULL");
    break;
  }
  case StartOfHeading: {
    // Serial.println("got StartOfHeading");
    byte *firstByte = (byte *)control;
    processHeader(firstByte);
    break;
  }
  case Enquiry: {
    // Serial.println("got Enquiry");
    sendHeartbeat();
    break;
  }
  default: {
    Serial.print("Error: Unexpected control character ->");
    Serial.println(String((byte)*control, DEC));
    sendError(UnexpectedControl);
    break;
  }
  }
}

void processHeader(byte *firstByte) {
  if (firstByte) {
    firstByte++;
    receivedHeader = (Headerr *)firstByte;
    if (!lastHeader ||
        receivedHeader->packetNumber > lastHeader->packetNumber) {
      copyHeader(receivedHeader, lastHeader);
      bool success = false;
      if (receivedHeader->ioType == 1) {
        success = Manager::getInstance()->writePin(receivedHeader->pin,
                                                   receivedHeader->writeValue);
      } else {
        success =
            Manager::getInstance()->readPin(receivedHeader->pin, &sendData);
      }
      if (!success) {
        sendError(CannotProcessCommand);
        return;
      } else {
        copyHeader(receivedHeader, lastHeader);
        sendResponse();
        return;
      }
    }
    if (receivedHeader->packetNumber == lastHeader->packetNumber) {
      sendResponse();
      return;
    }
    if (receivedHeader->packetNumber < lastHeader->packetNumber) {
      Serial.println("dropping message with too little packetNumber");
      Serial.print(receivedHeader->packetNumber);
      Serial.print(" < ");
      Serial.println(lastHeader->packetNumber);
      sendError(UnexpectedPacket);
      return;
    }
    if (receivedHeader->packetNumber == 0) {
      Serial.println("resetting packetNumber");
      copyHeader(receivedHeader, lastHeader);
      sendData = 0;
      sendResponse();
      return;
    }
  } else {
    Serial.println("dropping message with header of unknown type");
  }
}

void copyHeader(Headerr *from, Headerr *to) {
  memcpy(to, from, sizeof(Headerr));
}

void sendResponse() {
  unsigned int hsize = sizeof(Headerr);
  unsigned int arraySize = 5 + hsize;
  byte data[arraySize];
  data[0] = (byte)Acknowledge;
  data[1] = (byte)StartOfHeading;
  copyHeader(receivedHeader, (Headerr *)&data[2]);
  data[1 + hsize] = (byte)StartOfText;
  data[2 + hsize] = sendData;
  data[3 + hsize] = (byte)EndOfText;
  data[4 + hsize] = (byte)EndOfTransmission;
  Spii::write(new Message((byte *)&data, arraySize));
}
void sendHeartbeat() {
  byte data[2];
  data[0] = (byte)Acknowledge;
  data[1] = (byte)EndOfTransmission;
  Spii::write(new Message((byte *)&data, 2));
}
void sendError(Error error) {
  unsigned int hsize = sizeof(Headerr);
  unsigned int arraySize = 5 + hsize;
  byte data[arraySize];
  data[0] = (byte)NegativeAcknowledge;
  data[1] = (byte)StartOfHeading;
  copyHeader(receivedHeader, (Headerr *)&data[2]);
  data[1 + hsize] = (byte)StartOfText;
  data[2 + hsize] = (byte)error;
  data[3 + hsize] = (byte)EndOfText;
  data[4 + hsize] = (byte)EndOfTransmission;
  Spii::write(new Message((byte *)&data, arraySize));
}