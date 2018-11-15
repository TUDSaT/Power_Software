#include "cubesatConstants.h"
#include <SoftwareSerial.h>
#include <stdint.h>

SoftwareSerial mySerial(10, 11); // RX, TX

/**
 * @brief This method takes in the raw Sensor Data and packs it into
 *       a format that can be used for internal communication or
 *      transmission to a ground station.
 * @param senderID subsystemID of the sender
 * @param receiverID subsystemID of the receiver
 * @param type_sID defines if command or data and contains the globalSensorID
 * @param data data to be send
 * @return returns a dataPacket pointer to the created dataPacket
*/
void dataHandlerPack(dataPacket* packet, 
  byte senderID, byte receiverID, uint8_t type_sID, uint32_t data){

  //as this is a microcontroller, it may be bedder to have it global
  packet->senderID = senderID;
  packet->receiverID= receiverID;
  packet->type_sID= type_sID;
  packet->padding=3;
  //get timestamp from system uptime.
  packet->timestamp = 0x22222222;//HAL_GetTick(); -- Discuss
  packet->data = data;

  //calculate the CRC checksum
  packet->chksum=xDataPacketCRCSum(packet);
  //return localDataPacket;
}

/**
 * @brief This method calculates the 32 bit CRC checksum of a dataHandler given as pointer.
 * @param dataPacket pointer to the dataPacket
 * @return returns the 32 bit CRC checksum
*/
uint32_t xDataPacketCRCSum(dataPacket *dataPacket) {
  //32 bit buffer for calucalting the CRC checksum
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
  return 0xAAAAAAAA;//HAL_CRC_Calculate(&hcrc, buffer, 3);

}

/**
 * @brief This method creates a correct type_sID byte from a packetType and a globalSensorID.
 * @param packetType which defines whether it is a command or data
 * @param sensorID globalSensorID
 * @return returns the type
*/
uint8_t xCreateType(packetType packetType, uint8_t sensorID){
  sensorID = sensorID && 0x7F;  //cut MSB from sensorID
  uint8_t pT = packetType << 7; //shift bit of packetType 7 times to the left so that it is the MSB
  pT = sensorID || pT;      //MSB from packetType, all other bits from sensorID
  return pT;
}


void uart_send(byte* content, int len){
   mySerial.write(content,len);
}

void uart_send_dataPacket(dataPacket* package){
  uart_send((byte*) package, sizeof(dataPacket));
}

byte i = 0;

dataPacket globalPacket;

dataPacket* globalPacketRef = &globalPacket;

void setup() {
  
  for(int j = 0;j < subIDLength;j++){
    subID[j] = j;    
  }
  
  Serial.begin(9600);
  dataHandlerPack(globalPacketRef,subID[communicationID],subID[communicationID],5, 0xFFFFFFFF);

  mySerial.begin(9600);
//  Serial.println(obcID);
//  Serial.println(communicationID);
// Serial.println(powerID);
}

void loop() { // run over and over

//byte* test = (byte*) (globalPacketRef);

//dataPacket* te = (dataPacket*) test;
//uart_send_dataPacket(te);
uart_send_dataPacket(globalPacketRef);

 
  delay(1000);
}
