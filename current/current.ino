
#include "cubesatConstants.h"
#include <stdint.h>
extern "C" {
#include "dataHandlerArduino.h"
}

int pinA = A0;//Three pins to read from as dummy voltage 
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
  Serial.print(ref->timestamp,HEX);
  Serial.print("\n");
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
}
