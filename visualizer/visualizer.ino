#include <SoftwareSerial.h>

#include <Keyboard.h>

#include "cubesatConstants.h"


//Sender Code

SoftwareSerial mySerial(5, 6);

byte i = 0;
byte arr[dataPacketSize];
int count = 0 ;


dataPacket globalPacket;

dataPacket* globalPacketRef = &globalPacket;
void setup() {
  //  Serial.begin(9600);
  //  mySerial.begin(9600);
  Serial.begin(57600);
  mySerial.begin(9600);
  for (int i = 0; i < 10; i++) {
//    Serial.println(sizeof(subsystemID));
//    Serial.println(sizeof(dataPacket));
  }
  count = 0;
}


void loop() {
  if (mySerial.available()) {
    Serial.println(count);
    i = 0;
    while (mySerial.available()) {
      arr[i] = mySerial.read();
      i++;
      //   Serial.println(i);
    }
    /* for (int f = 0; f < dataPacketSize; f++) {
       Serial.print(arr[f]);
       Serial.print(" ");
       Serial.print(arr[f],HEX);
       Serial.print(" ");
       Serial.println(arr[f],BIN);
      }
    */

    dataPacket* ref = (dataPacket*) arr;
    count++;
    int type = HEX;
    Serial.println(ref->senderID, type);
    Serial.println(ref->receiverID, type);
    Serial.println(ref->type_sID, type);
    Serial.println(ref->padding, type);
    Serial.println(ref->timestamp, type);
    Serial.println(ref->data, type);
    Serial.println(ref->chksum, type);
    Serial.println();


    delay(1000);
  }
  //  Serial.println("Heardbeat");
}
