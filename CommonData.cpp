/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 *
 * This file contains the class for common data storage. The common data
 * is used in the whole project to store public data and give access to
 * all the components; also component can share data each other via this
 * method.
 */

#include "CommonData.h"
#include <avr/EEPROM.h>

void CommonData::setup() {
	//Read devicelist number from EEPROM
	this->deviceCount = EepromManage::getEepromDeviceNum();
	EepromManage::getEepromMac(this->mac);
	EepromManage::getEepromBoxKey(this->key);
	EepromManage::getEepromVersion(this->version);

	//Update devicelist from EEPROM
	if(this->deviceCount > 0){
		eeprom_device_list_t deviceList[this->deviceCount];
		EepromManage::getEepromDeviceList(deviceList, this->deviceCount);
		for(unsigned short i=0; i<this->deviceCount; i++){
				this->deviceList[i].type = deviceList[i].type;
				strncpy(this->deviceList[i].MAC, (char*)(deviceList[i].mac), DEVICE_IP_LEN);
				this->deviceList[i].R = 255;
				this->deviceList[i].G = 255;
				this->deviceList[i].B = 255;
				this->deviceList[i].L = 100;
				this->deviceList[i].effect = 0;
				this->deviceList[i].online = false;
				this->deviceList[i].powerStatus = 0;
				this->deviceList[i].LQI = 0;
		}
	}

	this->_waitStartTime = 0;
}

bool CommonData::addNewDevice(char* mac, uint8_t type){
	eeprom_device_list_t deviceList;

	// If max lamps exceed or invalid mac, just return
	if(this->deviceCount >= MAX_SUPPORTED_LAMPS || mac == NULL)
		return false;

	// If the mac is already exists, just return
	for(int i=0; i<this->deviceCount; i++) {
		if(0 == strncmp(mac, this->deviceList[i].MAC, DEVICE_IP_LEN)) {
			return false;
		}
	}

	// Add new device in DeviceList
	(void)strncpy(this->deviceList[this->deviceCount].MAC, mac, E2DA_ZIGBEE_MAC_LEN);
	this->deviceList[this->deviceCount].type = type;
	this->deviceCount++;	

	// Add new device to EEPROM
	deviceList.type = (unsigned short)type;
	(void)strncpy((char*)deviceList.mac, mac, E2DA_ZIGBEE_MAC_LEN);

	EepromManage::AddEepromDeviceList(deviceList);
	return true;
}

