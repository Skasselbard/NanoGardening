#include "protocoll.h"
#include "spii.h"

#include "manager.h"

Headerr *lastHeader = (Headerr *)calloc(1, sizeof(Headerr));
Headerr *receivedHeader = nullptr;
byte sendData = 0;

void processControl(Control *control) {
  switch ((byte)*control) {
  case Null: { // do nothing
    break;
  }
  case StartOfHeading: {
    byte *firstByte = (byte *)control;
    processHeader(firstByte);
    break;
  }
  case Enquiry: {
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
      }
    }
    if (receivedHeader->packetNumber == lastHeader->packetNumber) {
      sendResponse();
    }
    if (receivedHeader->packetNumber < lastHeader->packetNumber) {
      Serial.println("dropping message with too little packetNumber");
      Serial.print(receivedHeader->packetNumber);
      Serial.print(" < ");
      Serial.println(lastHeader->packetNumber);
      sendError(UnexpectedPacket);
    }
    if (receivedHeader->packetNumber == 0) {
      Serial.println("resetting packetNumber");
      copyHeader(receivedHeader, lastHeader);
      sendData = 0;
      sendResponse();
    }
  } else {
    Serial.println("dropping message with header of unknown type");
  }
}

void copyHeader(Headerr *from, Headerr *to) {
  memcpy(to, from, sizeof(Headerr));
}

void sendResponse() {
  String message;
  message += (byte)Acknowledge;
  message += (byte)sendData;
  message += (byte)EndOfTransmission;
  Spii::write(message);
}
void sendHeartbeat() {
  String message;
  message += (byte)Acknowledge;
  message += (byte)EndOfTransmission;
  Spii::write(message);
}
void sendError(Error error) {
  String message;
  message += (byte)NegativeAcknowledge;
  message += (byte)error;
  message += (byte)EndOfTransmission;
  Spii::write(message);
}