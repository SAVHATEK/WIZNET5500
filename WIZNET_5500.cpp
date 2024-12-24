#include "WIZNET_5500.h"

WIZNET_5500::WIZNET_5500(IPAddress ip, int localPort, int csPin) {
  IP = ip;
  PORT = localPort;
  CS_PIN = csPin;
  PACKET_BUFFER = new char[packetBufferSize];
}
void WIZNET_5500::setSubnet(IPAddress subnetMask) {
  SUBNET = subnetMask;
  if (DEBUG == true) {
    Serial.println("DEBUG: PORT address: " + String(PORT));
  }
}
void WIZNET_5500::setPort(int localPort) {
  PORT = localPort;
  if (DEBUG == true) {
    //Serial.println("DEBUG: PORT adress: "+ PORT);
  }
}
void WIZNET_5500::setGateway(IPAddress gatewayAddress) {
  GATEWAY = gatewayAddress;
  if (DEBUG == true) {
    Serial.println("DEBUG: GATEWAY address: " + GATEWAY.toString());
  }
}
void WIZNET_5500::setPacketBufferSize(size_t bufferSize) {
  if (PACKET_BUFFER != nullptr) {
    delete[] PACKET_BUFFER;
  }
  packetBufferSize = bufferSize;
  PACKET_BUFFER = new char[packetBufferSize];
  if (DEBUG == true) {
    Serial.println("DEBUG: Buffer size: " + String(packetBufferSize));
  }
}
void WIZNET_5500::setPacketBufferSizeHigh() {
  if (PACKET_BUFFER != nullptr) {
    delete[] PACKET_BUFFER;
  }
  packetBufferSize = 512;
  PACKET_BUFFER = new char[packetBufferSize];
  if (DEBUG == true) {
    Serial.println("DEBUG: Buffer size: 512");
  }
}
void WIZNET_5500::setPacketBufferSizeMedium() {
  if (PACKET_BUFFER != nullptr) {
    delete[] PACKET_BUFFER;
  }
  packetBufferSize = 256;
  PACKET_BUFFER = new char[packetBufferSize];
  if (DEBUG == true) {
    Serial.println("DEBUG: Buffer size: 256");
  }
}
void WIZNET_5500::setPacketBufferSizeLow() {
  if (PACKET_BUFFER != nullptr) {
    delete[] PACKET_BUFFER;
  }
  packetBufferSize = 128;
  PACKET_BUFFER = new char[packetBufferSize];
  if (DEBUG == true) {
    Serial.println("DEBUG: Buffer size: 128");
  }
}
void WIZNET_5500::setPacketBufferSizeVeryLow() {
  if (PACKET_BUFFER != nullptr) {
    delete[] PACKET_BUFFER;
  }
  packetBufferSize = 64;
  PACKET_BUFFER = new char[packetBufferSize];
  if (DEBUG == true) {
    Serial.println("DEBUG: Buffer size: 64");
  }
}
void WIZNET_5500::setChecksumCharacter(char checksumCharacter) {
  checksumChar = checksumCharacter;
  if (DEBUG == true) {
    Serial.println("DEBUG: Checksum Char: " + String(checksumChar));
  }
}
void WIZNET_5500::setChecksumOn() {
  checksumControl = true;
  if (DEBUG == true) {
    Serial.println("DEBUG: Checksum Control: TRUE");
  }
}
void WIZNET_5500::setChecksumOff() {
  checksumControl = false;
  if (DEBUG == true) {
    Serial.println("DEBUG: Checksum Control: FALSE");
  }
}
void WIZNET_5500::setMessageAndChecksumOn() {
  messageAndchecksum = true;
  if (DEBUG == true) {
    Serial.println("DEBUG: Checksum and Message: TRUE");
  }
}
void WIZNET_5500::setMessageAndChecksumOff() {
  messageAndchecksum = false;
  if (DEBUG == true) {
    Serial.println("DEBUG: Checksum and Message: FALSE");
  }
}
void WIZNET_5500::setTimeout(int milis) {
  MILIS_TIMEOUT = milis;
  if (DEBUG == true) {
    Serial.println("DEBUG: Time out time: " + String(MILIS_TIMEOUT));
  }
}
void WIZNET_5500::begin() {
  Ethernet.init(CS_PIN);
  if (Ethernet.begin(MAC) == 0) {
    if (DEBUG == true) {
      Serial.println("DEBUG: Ethernet failed to initialize.");
    }
    while (true)
      ;
  }
  Ethernet.begin(MAC, IP);
  if (DEBUG == true) {
    Serial.println("DEBUG: IP address: " + Ethernet.localIP().toString());
  }
  UDP.begin(PORT);
  if (DEBUG == true) {
    Serial.println("DEBUG: Listen Port: " + String(PORT));
  }
}
String WIZNET_5500::listen() {
  packetSize = 0;
  packetSize = UDP.parsePacket();
  currentTime = millis();
  if (packetSize) {
    lastPacketTime = currentTime;
    if (packetSize > packetBufferSize) {
      if (DEBUG == true) {
        Serial.println("DEBUG: ERROR: PACKET_SIZE_EXCEEDED");
      }
      return "ERROR: PACKET_SIZE_EXCEEDED";
    }
    UDP.read(PACKET_BUFFER, packetSize);
    udpMessage = "";
    receivedChecksum = "";
    isDelimiterFound = false;
    for (int i = 0; i < packetSize; i++) {
      char currentChar = (char)PACKET_BUFFER[i];
      if (currentChar == '|') {
        isDelimiterFound = true;
      } else if (!isDelimiterFound) {
        udpMessage += currentChar;
      } else {
        receivedChecksum += currentChar;
      }
    }

    if (checksumControl == true) {
      if (!isDelimiterFound) {
        if (DEBUG == true) {
          Serial.println("DEBUG: ERROR: INVALID_FORMAT");
        }
        return "ERROR: INVALID_FORMAT";
      }
      calculatedChecksum = calculateChecksum(udpMessage);
      if (calculatedChecksum == (unsigned int)strtol(receivedChecksum.c_str(), NULL, 16)) {
        if (messageAndchecksum == true) {
          return udpMessage + checksumChar + receivedChecksum;
        } else {
          return udpMessage;
        }

      } else {
        if (DEBUG == true) {
          Serial.println("DEBUG: ERROR: CHECKSUM_INVALID");
        }
        return "ERROR: CHECKSUM_INVALID";
      }

    } else {
      if (messageAndchecksum == true) {
        return udpMessage + checksumChar + receivedChecksum;
      } else {
        return udpMessage;
      }
    }
  } else {
    if (Ethernet.linkStatus() == EthernetLinkStatus::LinkOFF) {
      if (DEBUG == true) {
        Serial.println("DEBUG: ERROR: CABLE_DISCONNECTED");
      }
      return "ERROR: CABLE_DISCONNECTED";
    } else if (currentTime - lastPacketTime > MILIS_TIMEOUT) {
      if (DEBUG == true) {
        Serial.println("DEBUG: ERROR: TIMEOUT");
      }
      return "ERROR: TIMEOUT";
    } else {
      if (DEBUG == true) {
        Serial.println("DEBUG: ERROR: NO_DATA");
      }
      return "ERROR: NO_DATA";
    }
  }
}
String WIZNET_5500::listenOnlyData() {
  onlyData = listen();
  if (!onlyData.startsWith("ERROR")) {
    lastOnlyData = onlyData;
    return onlyData;
  } else {
    if (DEBUG == true) {
      Serial.println("DEBUG: ERROR: NO_NEW DATA_USING_PREVIOUS_DATA");
    }
    return lastOnlyData;
  }
}
void WIZNET_5500::enableDebug() {
  DEBUG = true;
  if (DEBUG == true) {
    Serial.println("DEBUG: DEBUG MODE : TRUE");
  }
}
void WIZNET_5500::disableDebug() {
  DEBUG = false;
  if (DEBUG == true) {
    Serial.println("DEBUG: DEBUG MODE : TRUE");
  }
}
void WIZNET_5500::showInfo() {
  Serial.println("DEBUG: IP Address: " + IP.toString());
  Serial.println("DEBUG: PORT Address: " + String(PORT));
  Serial.println("DEBUG: SUBNET Address: " + SUBNET.toString());
  Serial.println("DEBUG: GATEWAY Address: " + GATEWAY.toString());
  Serial.println("DEBUG: Checksum Character: " + String(checksumChar));
  Serial.println("DEBUG: Packet Buffer Size: " + String(packetBufferSize));
  Serial.println("DEBUG: Checksum Control: " + String(checksumControl ? "ENABLED" : "DISABLED"));
  Serial.println("DEBUG: Checksum and Message Combined: " + String(messageAndchecksum ? "ENABLED" : "DISABLED"));
  Serial.println("DEBUG: Timeout Duration: " + String(MILIS_TIMEOUT) + " ms");
}
unsigned int WIZNET_5500::calculateChecksum(const String& data) {
  unsigned int checksum = 0;
  for (size_t i = 0; i < data.length(); i++) {
    checksum += data[i];
  }
  if (DEBUG == true) {
    //Serial.println("DEBUG: Checksum :"+checksum);
  }
  return checksum;
}