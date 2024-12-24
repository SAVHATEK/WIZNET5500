#ifndef WIZNET_5500_H
#define WIZNET_5500_H

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

class WIZNET_5500 {
public:
    WIZNET_5500(IPAddress ip, int localPort, int cs);
    void begin();
    void setSubnet(IPAddress subnetMask);
    void setPort(int localPort);
    void setGateway(IPAddress gatewayAddress);
    void setPacketBufferSize(size_t bufferSize);
    void setPacketBufferSizeHigh();
    void setPacketBufferSizeMedium();
    void setPacketBufferSizeLow();
    void setPacketBufferSizeVeryLow();
    void setChecksumCharacter(char checksumCharacter);
    void setChecksumOn();
    void setChecksumOff();
    void setMessageAndChecksumOn();
    void setMessageAndChecksumOff();
    void setTimeout(int milis);
    unsigned int calculateChecksum(const String& data);
    void enableDebug();
    void disableDebug();
    void showInfo();
    String listen();
    String listenOnlyData();
    
    
    
private:
    IPAddress IP = IPAddress(192, 168, 1, 1);
    IPAddress GATEWAY = IPAddress(192, 168, 1, 1);
    IPAddress SUBNET = IPAddress(255, 255, 255, 0);
    byte MAC[6] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    unsigned int calculatedChecksum = 0;
    int PORT = 8888;
    int CS_PIN =5;
    int packetSize = 0;
    int MILIS_TIMEOUT = 2000;
    char* PACKET_BUFFER = nullptr;
    char checksumChar = '-';
    bool checksumControl = false;
    bool messageAndchecksum = false;
    bool DEBUG = false;
    bool isDelimiterFound = false;
    size_t packetBufferSize = 64;
    String udpMessage;
    String receivedChecksum;
    String onlyData;
    String lastOnlyData;
    unsigned long lastPacketTime = 0;
    unsigned long currentTime = 0;
    
    EthernetUDP UDP;



};

#endif 