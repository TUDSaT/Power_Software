#include "cubesatConstants.h"
#include "dataHandler.h"
#include <stdint.h>
#include <stdio.h>

void dataHandlerPack(dataPacket* packet, uint8_t senderID, uint8_t receiverID,
		uint8_t type_sID, uint32_t data) {

	//as this is a microcontroller, it may be bedder to have it global
	packet->senderID = senderID;
	packet->receiverID = receiverID;
	packet->type_sID = type_sID;
	packet->padding = 0;
	//get timestamp from system uptime.
	packet->timestamp = 1; //HAL_GetTick(); -- Discuss
	packet->data = data;

	//calculate the CRC checksum
	packet->chksum = xDataPacketCRCSum(packet);
	//return localDataPacket;
}

uint32_t reg32 = 0xffffffff;     // Schieberegister

uint32_t crc32_bytecalc(uint8_t in) {
	int i;
	uint32_t polynom = 0xEDB88320;   // Generatorpolynom

	for (i = 0; i < 8; ++i) {
		if ((reg32 & 1) != (in & 1))
			reg32 = (reg32 >> 1) ^ polynom;
		else
			reg32 >>= 1;
		in >>= 1;
	}
	return reg32 ^ 0xffffffff;      // inverses Ergebnis, MSB zuerst
}

uint32_t crc32(uint32_t *data, int len) {
	int i;

	for (i = 0; i < len; i++) {
		crc32_bytecalc(data[i]);    // Berechne fuer jeweils 8 Bit der Nachricht
	}
	return reg32 ^ 0xffffffff;
}

const uint8_t  0x

uint32_t crcTWO(uint32_t *data, int len) {
	int i;

	for (i = 0; i < len; i++) {
		crc32_bytecalc(data[i]);    // Berechne fuer jeweils 8 Bit der Nachricht
	}
	return reg32 ^ 0xffffffff;
}

uint32_t xDataPacketCRCSum(dataPacket *dataPacket) {
	//3x32 bit buffer for calucalting the CRC checksum
	uint32_t buffer[3];
	//for the first 32bit bufferelement combining 4 8bit elements together:
	//    senderID, receiverID, type and 8bit of zeros
	buffer[0] = dataPacket->senderID;
	buffer[0] = buffer[0] << 8; //shift left 8 times to make room for the next byte
	buffer[0] = buffer[0] | dataPacket->receiverID;
	buffer[0] = buffer[0] << 8; //shift left 8 times to make room for the next byte
	buffer[0] = buffer[0] | dataPacket->type_sID;
	buffer[0] = buffer[0] << 8; //shift left 8 times to align correctly, rest are padded wit zeros

	buffer[1] = dataPacket->timestamp;
	buffer[2] = dataPacket->data;
	//calculate the 32 bit CRC checksum
	return crcTWO(buffer, 3); //0xAAAAAAAA;//HAL_CRC_Calculate(&hcrc, buffer, 3);

}

uint8_t xCreateType(packetType packetType, uint8_t sensorID) {
	sensorID = sensorID && 0x7F;  //cut MSB from sensorID
	uint8_t pT = packetType << 7; //shift bit of packetType 7 times to the left so that it is the MSB
	pT = sensorID || pT;     //MSB from packetType, all other bits from sensorID
	return pT;
}

dataPacket globalPacket;

dataPacket* globalPacketRef = &globalPacket;

void setup() {

	for (uint8_t j = 0; j < subIDLength; j++) {
		subID[j] = j;
	}

	dataHandlerPack(globalPacketRef, subID[obcID], subID[powerID], 0x2A, 0);

}

void main() { // run over and over
	setup();
	printf("%d\n", globalPacket.senderID);
	printf("%d\n", globalPacket.receiverID);
	printf("%d\n", globalPacket.type_sID);
	printf("%d\n", globalPacket.padding);
	printf("%d\n", globalPacket.timestamp);
	printf("%d\n", globalPacket.data);
	printf("%d\n", globalPacket.chksum);

}
