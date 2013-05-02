/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 *
 * This file contains YeeBox logic handling functions
 */

#include "BoxLogic.h"

#define HB_DELAY_TIME 3000
#define BRING_UP_WAIT_TIME 5000

//start BoxLogic instance
void BoxLogic::begin(CommonData *cmdata) {
	this->_cmdata = cmdata;
}

//used in box setup 
void BoxLogic::loop() {
	unsigned long curTime = millis();

	// During the box initiation, detect if "HB" is timeout.
	if(this->_cmdata->_mode == BOXMODE_INIT && this->_cmdata->_waitStartTime != 0) {
		if( (curTime - this->_cmdata->_waitStartTime) > HB_DELAY_TIME) {
			//If timeout, it indicates CC2530 may be not initiated, sent "HB" again
			this->_cmdata->com->write("HB\n");
			this->_cmdata->_waitStartTime = curTime;
		}
	}

	//Wait 10s for zigbee initiation.
	if(this->_cmdata->_mode == BOXMODE_INIT_WAIT && this->_cmdata->_waitStartTime != 0){
		if( (curTime - this->_cmdata->_waitStartTime) > BRING_UP_WAIT_TIME) {
			this->_cmdata->_mode = BOXMODE_NORMAL;
			this->_cmdata->_waitStartTime = 0;
			this->_cmdata->leds->setLocalOn();
		}
	}
}

// when a device online status change
void BoxLogic::deviceOnlineChange(uint8_t deviceIndex, bool online, bool isNewDevice) {
	char msg[20];

	if(this->_cmdata->_mode != BOXMODE_NORMAL){
		return;
	}

	if(online) {
		//send header
		if(isNewDevice) {
			//NEW 0001,1,1,186,255,255,255,100,0\n
			sprintf(msg, "NEW %.4s,%d,1,%d,", this->_cmdata->deviceList[deviceIndex].MAC,
					this->_cmdata->deviceList[deviceIndex].type,
					this->_cmdata->deviceList[deviceIndex].LQI);
			this->_cmdata->mobile->sendMessageToClient((unsigned char*)msg, strlen(msg));
		}
		else {
			sprintf(msg, "S %.4s,%d,1,%d,", this->_cmdata->deviceList[deviceIndex].MAC,
					this->_cmdata->deviceList[deviceIndex].type,
					this->_cmdata->deviceList[deviceIndex].LQI);
			this->_cmdata->mobile->sendMessageToClient((unsigned char*)msg, strlen(msg));
		}
		//params
		switch(this->_cmdata->deviceList[deviceIndex].type) {
			case HARDWARE_RGBLED:
				sprintf(msg, "%d,%d,%d,%d,%d\r\n", this->_cmdata->deviceList[deviceIndex].R,
						this->_cmdata->deviceList[deviceIndex].G,
						this->_cmdata->deviceList[deviceIndex].B,
						this->_cmdata->deviceList[deviceIndex].L,
						this->_cmdata->deviceList[deviceIndex].effect);
				this->_cmdata->mobile->sendMessageToClient((unsigned char*)msg, strlen(msg));
				break;
			case HARDWARE_POWERSOCKET:
				sprintf(msg, "%d\r\n", this->_cmdata->deviceList[deviceIndex].powerStatus);
				this->_cmdata->mobile->sendMessageToClient((unsigned char*)msg, strlen(msg));
				break;
			default:
				break;
		}

	}
	else {
		//offline
		sprintf(msg, "S %.4s,,0,,,,,\r\n", this->_cmdata->deviceList[deviceIndex].MAC);
		this->_cmdata->mobile->sendMessageToClient((unsigned char*)msg, strlen(msg));
	}
}

// when there is a status update message from zigbee network, update
// status in box and notify app client
void BoxLogic::updateDeviceStatus(char* mac, uint8_t type, char *online, char *R, char *G,
			char *B, char *L, char *powerStatus, char *LQI, bool isNewDevice) {
	uint8_t deviceIndex = 0;

	//param check
	if(!mac || (strlen(mac) != DEVICE_IP_LEN)) return; //not valid MAC

	//look up mac in list
	for(deviceIndex=0; deviceIndex<this->_cmdata->deviceCount; deviceIndex++) {
		if(0 == strncmp(mac, this->_cmdata->deviceList[deviceIndex].MAC, DEVICE_IP_LEN)) {
			break;
		}
	}
	//not found, this device is not managed
	if(deviceIndex >= this->_cmdata->deviceCount) {
		if(this->_cmdata->_mode == BOXMODE_REFRESH) {
			//TODO: EEPROM remember devices;
		}
		return;
	}

	//start to handle device status
	if(online) {
		uint8_t state = (uint8_t)atoi(online);
		switch(state) {
		case 0:
			if(this->_cmdata->deviceList[deviceIndex].online == true) {
				this->_cmdata->deviceList[deviceIndex].online = false;
				this->deviceOnlineChange(deviceIndex, false, isNewDevice);
			}
			break;
		case 1:
			if(this->_cmdata->deviceList[deviceIndex].online == false) {
				this->_cmdata->deviceList[deviceIndex].online = true;
				this->deviceOnlineChange(deviceIndex, true, isNewDevice);
			}
			break;
		default:
			break;
		}
	}
	else {
		//wrong params
		return;
	}

	//signal quality
	if(LQI) {
		this->_cmdata->deviceList[deviceIndex].LQI = atoi(LQI);
	}

	if(type) {
		this->_cmdata->deviceList[deviceIndex].type = type;
	}
	else {
		return;
	}
	//this->_cmdata->com->write(type);
	//return;
	switch(type) {
		case HARDWARE_RGBLED: {
			if(R && G && B) {
				//this->_cmdata->com->write("Enter R G B\n");
				this->_cmdata->deviceList[deviceIndex].R = atoi(R);
				this->_cmdata->deviceList[deviceIndex].G = atoi(G);
				this->_cmdata->deviceList[deviceIndex].B = atoi(B);
			}
			else {
				return;
			}

			if(L) {
				//this->_cmdata->com->write("Enter L\n");
				this->_cmdata->deviceList[deviceIndex].L = atoi(L);
			}
			else {
				return;
			}

			break;
		}

		case HARDWARE_POWERSOCKET: {
			//powerStatus is used for socket status 0/1
			if(powerStatus) {
				//this->_cmdata->com->write("Enter powerStatus\n");
				this->_cmdata->deviceList[deviceIndex].powerStatus = ((*powerStatus == '1') ? true : false);
			}
			else {
				return;
			}

			break;
		}

		default: {
			this->_cmdata->deviceList[deviceIndex].R = 9;
			this->_cmdata->deviceList[deviceIndex].G = 9;
			this->_cmdata->deviceList[deviceIndex].B = 9;
			break;
		}
	} //switch
}
