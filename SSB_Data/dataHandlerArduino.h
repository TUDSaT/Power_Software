#ifndef DATAHANDLERARDUINO_H
# define  DATAHANDLERARDUINO_H
#include "cubesatConstants.h"

uint32_t xDataPacketCRCSum(dataPacket *dataPacket);

void dataHandlerPack(dataPacket* packet, subsystemID senderID, subsystemID receiverID, uint8_t type_sID, uint32_t data);

uint8_t xCreateType(packetType packetType, uint8_t sensorID);

#endif
