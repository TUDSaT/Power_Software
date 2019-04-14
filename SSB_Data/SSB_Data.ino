
#include "cubesatConstants.h"
#include <stdint.h>
extern "C" {
#include "dataHandlerArduino.h"
}
  
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

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

void printPack(dataPacket* ref) {
  Serial.print("SenderID:");
  Serial.println(ref->senderID);
  Serial.print("receiveBufferrID:");
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

void printPackSS(dataPacket* ref) {
  byte *dataPacketD = (byte*) ref;
  int i = 0;
  for (i = 0; i < 16; i++) {
    mySerial.write(dataPacketD[i]);
  }
}

void printPackBytes(dataPacket* ref) {
  byte *dataPacketD = (byte*) ref;
  int i = 0;
  for (i = 0; i < 16; i++) {
    Serial.print(dataPacketD[i]);
  }
  Serial.println("-");
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


Serial.print(9600);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);

  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(pinC, INPUT); 
}

 
uint8_t a = 0xEE;
uint32_t d = 1;
dataPacket sendBuffer;
dataPacket* sendBufferRef = &sendBuffer;


dataPacket receiveBuffer;
dataPacket* refreceiveBuffer = &receiveBuffer;

int delta = 100;

void loop() { // run over and over

    dataHandlerPack(sendBufferRef,powerID,obcID,0,d);
 //   printPackHEX(sendBufferRef);
    
    d++;
      int after = mySerial.available();
      Serial.println(after);
    if(after == 16){
      
      byte* buff = (byte*) refreceiveBuffer;
      for(int i = 0;i < 16;i++){
        
        buff[i] = (uint8_t) mySerial.read();
      }
       printPack(refreceiveBuffer);
       printPackSS(sendBufferRef);//response
       
    Serial.println("Beat");
    }else{
      while(mySerial.available()){//drop buffer if filled with less than a dataPacket oder more than one
        mySerial.read();
      }
    }
    delay(100);
}
