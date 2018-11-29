#include "cubesatConstants.h"
#include <SoftwareSerial.h>

SoftwareSerial serialA(8, 9); // RX, TX
#include <stdint.h>

dataPacket globalPacket;

dataPacket* globalPacketRef = &globalPacket;




void dataHandlerPack(dataPacket* packet,subsystemID senderID, subsystemID receiverID, uint8_t type_sID, uint32_t data){
  //packing the dataPacket with all known values.
  dataPacket *localDataPacket = packet;
  //as this is a microcontroller, it may be bedder to have a global
  localDataPacket->senderID = senderID;
  localDataPacket->receiverID= receiverID;
  localDataPacket->type_sID= type_sID;
  localDataPacket->padding=0;
  //get timestamp from system uptime.
  localDataPacket->timestamp = millis();//HAL_GetTick(); -- Discuss
  localDataPacket->data = data;

  //calculate the CRC checksum
  localDataPacket->chksum= 0;//xDataPacketCRCSum(localDataPacket);
  //return localDataPacket;
}


void setup() {
  // put your setup code here, to run once:
  serialA.begin(9600);
  Serial.begin(9600);
}
uint8_t a = 4;
  uint32_t d = 0xF0F0F0F0;
void loop() {
  // put your main code here, to run repeatedly:
  
  dataHandlerPack(globalPacketRef, powerID, obcID,a,d);

  serialA.write((byte*) globalPacketRef,sizeof(dataPacket));
  //Serial.write((byte*) globalPacketRef,sizeof(dataPacket));
  Serial.println(sizeof(dataPacket));

  Serial.print("Heartbeat");
  Serial.println(d);

  
  Serial.print("Time:");
  Serial.println(millis());
//serialA.println(globalPacket.senderID);
//  serialA.println(globalPacket.receiverID);

  delay(500);
  d++;
}
