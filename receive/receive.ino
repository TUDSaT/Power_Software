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
  localDataPacket->timestamp = 0;//HAL_GetTick(); -- Discuss
  localDataPacket->data = data;

  //calculate the CRC checksum
  localDataPacket->chksum= 0;//xDataPacketCRCSum(localDataPacket);
  //return localDataPacket;
}

void printPack(dataPacket* ref){
  Serial.print("SenderID:");
  Serial.println(ref->senderID);
   Serial.print("ReceiverID:");
  Serial.println(ref->receiverID);
   Serial.print("TypeID:");
  Serial.println(ref->type_sID);
  Serial.print("Padding:");
  Serial.println(ref->padding);
   Serial.print("Timestamp:");
  Serial.println(ref->timestamp);
 Serial.print("Data:");
  Serial.println(ref->data);
 Serial.print("chksum:");
  Serial.println(ref->chksum);
}

void readPacket(dataPacket* ref){
  if(serialA.available() == sizeof(dataPacket)){
    int size_ = sizeof(dataPacket);
    int i = 0;
    byte*  buffer_ = (byte*) ref;
    while(i < size_){
      buffer_[i] = serialA.read();
      i++;
    }
  }
}
void setup() {
  serialA.begin(9600);
  Serial.begin(9600);
}

void loop() {
//  dataHandlerPack(globalPacketRef, powerID, obcID,a,d);
  readPacket(globalPacketRef);
  printPack(globalPacketRef);

  Serial.print("Heartbeat:");
  Serial.println(millis());
  delay(500);
}
