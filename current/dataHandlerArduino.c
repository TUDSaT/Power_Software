#include "dataHandlerArduino.h"
#include "cubesatConstants.h"
#include <stdint.h>
#include "crc32MPEG2.h"


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
  return crcB(buffer_, 3);

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


/**
 * @brief This method creates a correct type_sID byte from a packetType and a globalSensorID.
 * @param packetType which defines whether it is a command or data
 * @param sensorID globalSensorID
 * @return returns the type
*/
uint8_t xCreateType(packetType packetType, uint8_t sensorID){
	sensorID = sensorID && 0x7F; 	//cut MSB from sensorID
	uint8_t pT = packetType << 7;	//shift bit of packetType 7 times to the left so that it is the MSB
	pT = sensorID || pT;			//MSB from packetType, all other bits from sensorID
	return pT;
}
