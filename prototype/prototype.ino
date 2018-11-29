#include "cubesatConstants.h"
#include "dataHandler.h"
#include <SoftwareSerial.h>
#include <stdint.h>

SoftwareSerial mySerial(10, 11); // RX, TX

//----------------------------

uint32_t energyTime;

/**
   @brief This method takes in the raw Sensor Data and packs it into
         a format that can be used for internal communication or
        transmission to a ground station.
   @param senderID subsystemID of the sender
   @param receiverID subsystemID of the receiver
   @param type_sID defines if command or data and contains the globalSensorID
   @param data data to be send
   @return returns a dataPacket pointer to the created dataPacket
*/
void dataHandlerPack(dataPacket* packet,
                     uint8_t senderID, uint8_t  receiverID, uint8_t type_sID, uint32_t data) {

  //as this is a microcontroller, it may be bedder to have it global
  packet->senderID = senderID;
  packet->receiverID = receiverID;
  packet->type_sID = type_sID;
  packet->padding = 0;
  //get timestamp from system uptime.
  packet->timestamp = energyTime;//HAL_GetTick(); -- Discuss
  packet->data = data;

  //calculate the CRC checksum
  packet->chksum = xDataPacketCRCSum(packet);
  //return localDataPacket;
}

uint32_t reg32 = 0xffffffff;     // Schieberegister

uint32_t crc32_bytecalc(uint8_t in)
{
  int i;
  uint32_t polynom = 0xEDB88320;   // Generatorpolynom

  for (i = 0; i < 8; ++i)
  {
    if ((reg32 & 1) != (in & 1))
      reg32 = (reg32 >> 1)^polynom;
    else
      reg32 >>= 1;
    in  >>= 1;
  }
  return reg32 ^ 0xffffffff;      // inverses Ergebnis, MSB zuerst
}

uint32_t crc32(uint32_t *data, int len)
{
  int i;

  for (i = 0; i < len; i++) {
    crc32_bytecalc(data[i]);    // Berechne fuer jeweils 8 Bit der Nachricht
  }
  return reg32 ^ 0xffffffff;
}

/**
   @brief This method calculates the 32 bit CRC checksum of a dataHandler given as pointer.
   @param dataPacket pointer to the dataPacket
   @return returns the 32 bit CRC checksum
*/
uint32_t xDataPacketCRCSum(dataPacket *dataPacket) {
  //3x32 bit buffer for calucalting the CRC checksum
  uint32_t buffer[3];
  //for the first 32bit bufferelement combining 4 8bit elements together:
  //    senderID, receiverID, type and 8bit of zeros
  buffer[0] = dataPacket->senderID;
  buffer[0] = buffer[0] << 8;           //shift left 8 times to make room for the next byte
  buffer[0] = buffer[0] | dataPacket->receiverID;
  buffer[0] = buffer[0] << 8;           //shift left 8 times to make room for the next byte
  buffer[0] = buffer[0] | dataPacket->type_sID;
  buffer[0] = buffer[0] << 8;           //shift left 8 times to align correctly, rest are padded wit zeros

  buffer[1] = dataPacket->timestamp;
  buffer[2] = dataPacket->data;
  //calculate the 32 bit CRC checksum
  return crc32(buffer, 3); //0xAAAAAAAA;//HAL_CRC_Calculate(&hcrc, buffer, 3);

}

/**
   @brief This method creates a correct type_sID byte from a packetType and a globalSensorID.
   @param packetType which defines whether it is a command or data
   @param sensorID globalSensorID
   @return returns the type
*/
uint8_t xCreateType(packetType packetType, uint8_t sensorID) {
  sensorID = sensorID && 0x7F;  //cut MSB from sensorID
  uint8_t pT = packetType << 7; //shift bit of packetType 7 times to the left so that it is the MSB
  pT = sensorID || pT;      //MSB from packetType, all other bits from sensorID
  return pT;
}
//----------------------------

void uart_send(byte* content, int len) {
  mySerial.write(content, len);
}

void uart_send_dataPacket(dataPacket* package) {
  uart_send((byte*) package, sizeof(dataPacket));
}


/*
  Basic constraints
*/

subsystemID myID = powerID;

/*
  Statemachine for interaction
*/

enum stateValues {
  waiting,
  instructed

};

stateValues state = waiting;



dataPacket globalPacket;

dataPacket* globalPacketRef = &globalPacket;

void setup() {

  for (uint8_t  j = 0; j < subIDLength; j++) {
    subID[j] = j;
  }

  Serial.begin(9600);
  dataHandlerPack(globalPacketRef, subID[obcID], subID[powerID], 0x2A, 0);

  mySerial.begin(9600);
}

void loop() {
    Serial.println("Heardbeat");
  switch (state) {
    case waiting:
    Serial.println("waiting");
      
      byte arr[sizeof(dataPacket)];
      if (mySerial.available()) {
        int i = 0;
        while (mySerial.available()) {
          arr[i] = mySerial.read();
          i++;
        }
        Serial.println("Empfangen");


        dataPacket* received = (dataPacket*) arr;
        if (received->receiverID == myID) {
          Serial.println("Antworte");
          globalPacketRef = received;
          energyTime = received->timestamp + 5;
          dataHandlerPack(globalPacketRef, subID[obcID], subID[powerID], 0x2A, 42);
          uart_send_dataPacket(globalPacketRef);
        }
      }
      break;

    default:
      break;


  }



 // delay(1000);
}
