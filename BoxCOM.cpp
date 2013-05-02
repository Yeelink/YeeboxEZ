/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 *
 * This file contains the SSDP service on YeeBox. This service is used to finding
 * the bridge by mobile APP.
 */

#include "BoxCOM.h"

//do setup here
void BoxCOM::begin(CommonData* cmdata) {
	this->_cmdata = cmdata;
//	this->_lastHBTime = 0;
	Serial.begin(BOX_COM_BAUD);

	//Check zigbee is alive
	this->_cmdata->com->write("HB\n");
	this->_cmdata->_waitStartTime = millis();
}

//used in main loop
void BoxCOM::loop() {
//	if(millis() - this->_lastHBTime > HB_TIMEOUT) {
		//now we lost HB with CC2530
		//there is an issue that when run long time, Arduino UART will in a
		//unknown state, and can not send/recv data correctly, we should reset
		//Serial to do a workaround
//		Serial.end();
//		Serial.begin(BOX_COM_BAUD);
//	}
	
	while (Serial.available()) {
		char c = Serial.read();
		if ((c == '\n') && this->_rxSize < (BOXCOM_RX_BUF_MAX_SIZE - 2)) {
			this->_rxBuf[this->_rxSize] = c;
			this->_rxSize++;
			// a message has been get out, handle it
			this->processMessage();
			//reset buffer pointer
			this->_rxSize = 0;
			break;
		}
		else if(this->_rxSize >= (BOXCOM_RX_BUF_MAX_SIZE - 2)) {
			//buffer overflow, clear buffer
			this->_rxSize = 0;
			this->_rxBuf[this->_rxSize] = c;
			this->_rxSize++;
		}
		else {
			this->_rxBuf[this->_rxSize] = c;
			this->_rxSize++;
		}
	}
}

int BoxCOM::write(const char *str) {
//	Serial.flush();
	return Serial.write(str);
}

int BoxCOM::write(const char *buffer, unsigned int size) {
//	Serial.flush();
	return Serial.write((uint8_t*)buffer, size);
}

void BoxCOM::processMessage() {
	uint8_t p = 0;

	// message is splited by blank space to 2 parts:
	// 1: message method
	// 2: message parameter
	while(p < _rxSize){
		if(_rxBuf[p] == 0x20) {
			p++;
			break;
		}
		else {
			p++;
		}
	}

	// Status
	if(0 == strncmp((char*)_rxBuf, "S ", 2)) {
		//this->_cmdata->com->write("SB\n");
		onStatusMsg((char*)&_rxBuf[p], this->_rxSize - p, false);
//		this->_lastHBTime = millis();

		return;
	}

	// Heartbeat
	else if(0 == strncmp((char*)_rxBuf, "HB", 2)) {
		_cmdata->com->write("HACK\n");
//		this->_lastHBTime = millis();
		return;
	}

	else if( (0 == strncmp((char*)_rxBuf, "TB", 2)) || (0 == strncmp((char*)_rxBuf, "RTB", 3))){
		_cmdata->mobile->sendMessageToClient(_rxBuf, _rxSize);
		return;
	}

	// Heartbeat ACK
	else if(0 == strncmp((char*)_rxBuf, "HACK", 4)) {
//		this->_lastHBTime = millis();
		if(_cmdata->_mode == BOXMODE_INIT){
			//During box setup, if receives HB ACK, send "R" to get the current zigbee status.
			_cmdata->com->write("R\n");

			// When reveives zigbee status in BOXMODE_INIT, it indicates yeebox setup is
			// completed. Change the box status to BOXMODE_NORMAL.
			_cmdata->_mode = BOXMODE_INIT_WAIT;
			_cmdata->_waitStartTime = millis();
		}
		return;
	}

	// New
	else if(0 == strncmp((char*)_rxBuf, "NEW", 3)) {
		onStatusMsg((char*)&_rxBuf[p], _rxSize - p, true);
//		this->_lastHBTime = millis();
		return;
	}
}

bool BoxCOM::onStatusMsg(char *msg, uint8_t len, bool addNewFlag) {
	uint8_t p = 0;
	uint8_t hardwareType = 0;

	char *mac = NULL;
	char *type = NULL;
	char *online = NULL;
	char *R = NULL;
	char *G = NULL;
	char *B = NULL;
	char *L = NULL;
	char *effect = NULL;
	char *powerStatus = NULL;
	char *LQI = NULL;

	//get mac addr str
	mac = msg;
	while((p < len) && (msg[p] != ',')) { p++; }
	msg[p] = '\0'; p++;
	if(p >= len) return false;
	//get type
	type = &msg[p];
	while((p < len) && (msg[p] != ',')) { p++; }
	msg[p] = '\0'; p++;
	if(p >= len) return false;
	//get online
	online = &msg[p];
	while((p < len) && (msg[p] != ',')) { p++; }
	msg[p] = '\0'; p++;
	if(p >= len) return false;

	hardwareType = (unsigned short)atoi(type);
	if(*online == '1'){
		switch(hardwareType) {
		case HARDWARE_RGBLED:
			//get R
			R = &msg[p];
			while((p < len) && (msg[p] != ',')) { p++; }
			msg[p] = '\0'; p++;
			if(p >= len) return false;
			//get G
			G = &msg[p];
			while((p < len) && (msg[p] != ',')) { p++; }
			msg[p] = '\0'; p++;
			if(p >= len) return false;
			//get B
			B = &msg[p];
			while((p < len) && (msg[p] != ',')) { p++; }
			msg[p] = '\0'; p++;
			if(p >= len) return false;
			//get L
			L = &msg[p];
			while((p < len) && (msg[p] != ',')) { p++; }
			msg[p] = '\0';
			if(p >= len) return false;
			//get efftct
			effect = &msg[p];
			while((p < len) && (msg[p] != ',')) { p++; }
			msg[p] = '\0'; p++;
			if(p >= len) return false;
			//get LQI
			LQI = &msg[p];
			while((p < len) && (msg[p] != '\n')) { p++; }
			msg[p] = '\0'; p++;
			break;
		case HARDWARE_POWERSOCKET:
			//get socket status
			powerStatus = &msg[p];
			while((p < len) && (msg[p] != ',')) { p++; }
			msg[p] = '\0'; p++;
			if(p >= len) return false;
			//get LQI
			LQI = &msg[p];
			while((p < len) && (msg[p] != '\n')) { p++; }
			msg[p] = '\0'; p++;
			break;
		default:
			return false;
			break;
		}
	}
	else { //online == '0'
		;
	}

	//Add new device if required.
	if(addNewFlag){
		if(this->_cmdata->addNewDevice(mac, hardwareType) == false) {
			//already exists, only update status
			this->_cmdata->logic->updateDeviceStatus(mac, hardwareType, online, R, G, B, L, powerStatus, LQI, false);
			return false;
		}
	}
	
	this->_cmdata->logic->updateDeviceStatus(mac, hardwareType, online, R, G, B, L, powerStatus, LQI, addNewFlag);
	return true;
}

//void BoxCOM::onCtrlBackMsg(char *msg, uint8_t len) {
//	this->_cmdata->mobile->sendMessageToClient((uint8_t*)msg, len);
//}
