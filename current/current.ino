#include "cubesatConstants.h"
#include <stdint.h>
extern "C" {
#include "crc32MPEG2.h"
}

int pinA = A0;
int pinB = A1;
int pinC = A2;



uint32_t fold(uint32_t a, uint32_t b, uint32_t c) {
  uint32_t akku = 0;//0x80000000;

  akku |= a << 20;
  akku |= b << 10;
  akku |= c;
  uint32_t dummy = akku;
  uint32_t sum = 0;
  for (int i = 0; i < 30; i++) {
    sum = (sum + (dummy & 0x01)) % 4;
    dummy = dummy >> 1;
  }
  akku = akku | (sum << 30);

  return akku;
}

uint32_t getVoltage() {
  uint32_t a = analogRead(pinA);
  uint32_t b = analogRead(pinB);
  uint32_t c = analogRead(pinC);

  return fold(a, b, c);

}


dataPacket globalPacket;

dataPacket* globalPacketRef = &globalPacket;


/**
   @brief This method calculates the 32 bit CRC checksum of a dataHandler given as pointer.
   @param dataPacket pointer to the dataPacket
   @return returns the 32 bit CRC checksum
*/
uint32_t xDataPacketCRCSum(dataPacket *dataPacket) {
  //32 bit buffer for calucalting the CRC checksum
  uint32_t buffer_[3];
  //for the first 32bit bufferelement combining 4 8bit elements together:
  //    senderID, receiverID, type and 8bit of zeros
  buffer_[0] = dataPacket->senderID;
  buffer_[0] = buffer_[0] << 8;           //shift left 8 times to make room for the next byte
  buffer_[0] = buffer_[0] | dataPacket->receiverID;
  buffer_[0] = buffer_[0] << 8;           //shift left 8 times to make room for the next byte
  buffer_[0] = buffer_[0] | dataPacket->type_sID;
  buffer_[0] = buffer_[0] << 8;           //shift left 8 times to align correctly, rest are padded wit zeros

  buffer_[1] = dataPacket->timestamp;
  buffer_[2] = dataPacket->data;
  //calculate the 32 bit CRC checksum
  uint8_t* buffer2 = (uint8_t*) buffer_;
  return crc32B(buffer2, 12);

}

void dataHandlerPack(dataPacket* packet, subsystemID senderID, subsystemID receiverID, uint8_t type_sID, uint32_t data) {
  //packing the dataPacket with all known values.
  dataPacket *localDataPacket = packet;
  //as this is a microcontroller, it may be bedder to have a global
  localDataPacket->senderID = senderID;
  localDataPacket->receiverID = receiverID;
  localDataPacket->type_sID = type_sID;
  localDataPacket->padding = 0x00;
  //get timestamp from system uptime.
  localDataPacket->timestamp = 0x60;//millis();//HAL_GetTick(); -- Discuss
  localDataPacket->data = data;

  //calculate the CRC checksum
  localDataPacket->chksum = xDataPacketCRCSum(localDataPacket);
}


void printPack(dataPacket* ref) {
  Serial.print("SenderID:");
  Serial.println(ref->senderID);
  Serial.print("ReceiverID:");
  Serial.println(ref->receiverID);
  Serial.print("TypeID:");
  Serial.println(ref->type_sID);
  Serial.print("Padding:");
  Serial.println(ref->padding);
  Serial.print("Timestamp:                 ");
  Serial.println(ref->timestamp);
  Serial.print("Data:");
  Serial.println(ref->data, HEX);
  Serial.print("chksum:");
  Serial.print(ref->chksum,HEX);
  Serial.print("\n");
}
void printPackHEX(dataPacket* ref) {
  byte *dataPacketD = (byte*) ref;
  int i = 0;
  for (i = 0; i < 16; i++) {
    Serial.println(dataPacketD[i], HEX);
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(pinC, INPUT);
}

uint8_t a = 0xEE;
uint32_t d = 0xA0A1A2A3;

void loop() {
  dataHandlerPack(globalPacketRef,powerID,powerID,0,d);
  delay(1000);
  printPack(globalPacketRef);
  //printPackHEX(globalPacketRef);
}
