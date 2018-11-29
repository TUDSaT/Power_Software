#include <SoftwareSerial.h>

#include <Keyboard.h>

#include "cubesatConstants.h"


//Sender Code

SoftwareSerial mySerial(7, 8);

uint32_t clockTime = 0;


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
  packet->timestamp = clockTime;//HAL_GetTick(); -- Discuss
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
  dataPacket* localRef;
  
  dataHandlerPack(localRef, subID[obcID], subID[powerID], 0x2A, 0);

  uart_send_dataPacket(localRef);

  
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
    Serial.println(ref->timestamp, DEC);
    Serial.println(ref->data, type);
    Serial.println(ref->chksum, type);
    Serial.println();
   
  }


    Serial.println("Heardbeat-Vis"); 
    delay(1000);
    clockTime +=40;
}
