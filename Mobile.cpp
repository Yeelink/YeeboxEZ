/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 *
 * This file contains the interface service Mobile APP. This service is used
 * to commucate with APP for recieving control commands and sending lights
 * status.
 */

#include "Mobile.h"
#include "EepromManage.h"
#include <avr/EEPROM.h>

Mobile::~Mobile() {
	delete this->_mobileIntf;
	this->_mobileIntf = NULL;
}

//do setup here
void Mobile::begin(CommonData* cmdata) {
	this->cmdata = cmdata;
	this->_mobileIntf = new EthernetServer(MOBILEINTPORT);
	this->_mobileIntf->begin();
	this->_rxSize = 0;
//	connHBTime = 0;
}

//used in main loop
void Mobile::loop() {
	EthernetClient _conn = this->_mobileIntf->available();
	if (_conn) {
		if (_conn.connected()) {
			while (_conn.available()) {
				//read on char each time
				char c = _conn.read();
				if ((c == '\n') || (this->_rxSize >= (MOBILE_RX_BUF_SIZE-2))) {
					this->_rxBuf[this->_rxSize] = c;
					this->_rxSize++;

					this->cmdata->leds->setLocalFastFlash(500);
					// a message has been get out, handle it
					this->processMessage(&_conn, this->_rxBuf, this->_rxSize);
					//reset buffer pointer
					this->_rxSize = 0;
				}
				else {
					this->_rxBuf[this->_rxSize] = c;
					this->_rxSize++;
				}

			}
			//no more data in W5100 buffer
			this->_rxSize = 0;;
		}
		else {
		    _conn.stop();
		}
	}
}

// handle one message in buffer
void Mobile::processMessage(EthernetClient* conn, uint8_t* buf, uint8_t buf_size){
	uint8_t p = 0;

	// message is splited by blank space to 2 parts:
	// 1: message method
	// 2: message parameter
	while(p < buf_size){
		if(buf[p] == 0x20) {
			break;
		}
		else {
			p++;
		}
	}

	// Heart beat
	if(0 == strncmp((char*)buf, "HB", 2)) {
		conn->print("HACK\r\n");
		return;
	}

	if(0 == strncmp((char*)buf, "T ", 2)) {
		cmdata->com->write((char*)buf);
//		conn->print("TB\r\n");
		return;
	}

	if(0 == strncmp((char*)buf, "RT", 2)) {
		cmdata->com->write((char*)buf);
//		conn->print("RTB 0001,10,0\r\n");
		return;
	}

	// Get List
	if(0 == strncmp((char*)buf, "GL", 2)) {
		this->_printDeviceList(conn);
		return;
	}

	// Control
	if(0 == strncmp((char*)buf, "C ", 2)) {
		uint8_t len = buf_size - p;

		char *mac = NULL;
		char *R = NULL;
		char *G = NULL;
		char *B = NULL;
		char *L = NULL;
		char *effect = NULL;

		// send message to COM
		this->cmdata->com->write((char*)buf, buf_size);
		// replay with Control Back
		conn->print("CB\r\n");

		//get mac addr str
		mac = (char*)&buf[++p];
		while((p < len) && (buf[p] != ',')) { p++; }
		buf[p] = '\0'; p++;
		//get R
		R = (char*)&buf[p];
		while((p < len) && (buf[p] != ',')) { p++; }
		buf[p] = '\0'; p++;
		//get G
		G = (char*)&buf[p];
		while((p < len) && (buf[p] != ',')) { p++; }
		buf[p] = '\0'; p++;
		//get B
		B = (char*)&buf[p];
		while((p < len) && (buf[p] != ',')) { p++; }
		buf[p] = '\0'; p++;
		//get L
		L = (char*)&buf[p];
		while((p < len) && (buf[p] != ',')) { p++; }
		buf[p] = '\0'; p++;
		//get effect
		effect = (char*)&buf[p];
		while((p < len) && (buf[p] != ',')) { p++; }
		buf[p] = '\0'; p++;
		//store status into deviceList
		for(int i=0; i<this->cmdata->deviceCount; i++) {
			if(0 == strncmp(mac, this->cmdata->deviceList[i].MAC, DEVICE_IP_LEN)) {
				switch(this->cmdata->deviceList[i].type)
				{
				case HARDWARE_RGBLED:
					if (*R && *G && *B) {
						this->cmdata->deviceList[i].R = atoi(R);
						this->cmdata->deviceList[i].G = atoi(G);
						this->cmdata->deviceList[i].B = atoi(B);
					}
					if(*L) { this->cmdata->deviceList[i].L = atoi(L); }
					if(*effect) { this->cmdata->deviceList[i].effect = atoi(effect); }
					break;
				case HARDWARE_POWERSOCKET:
					if(*L == '0') {
						this->cmdata->deviceList[i].powerStatus = false;
					}
					else {
						this->cmdata->deviceList[i].powerStatus = true;
					}
					break;
				default:
					break;
				}
				break;
			}
		}

		return;
	}

	// Discover
	// when get discover message from APP, send a DSC message to zigbee
	// coordinator for light searching
	if(0 == strncmp((char*)buf, "D", 1)) {
		this->cmdata->com->write("DSC\n");
		conn->print("DB\r\n");
		return;
	}

	// When receives "ADD" msg, try to add the new device.
/*	if(0 == strncmp((char*)buf, "ADD", 3)) {
		char *mac;
		eeprom_device_list_t deviceList;
		char buffer[25];
		char *type;

		// If max lamps exceed, just return
		if(this->cmdata->deviceCount > MAX_SUPPORTED_LAMPS-1 )
			return;

		//get mac
		mac = (char*)&buf[++p];
		while((buf[p] != ',')) { p++; }
		buf[p] = '\0'; p++;
		//get type
		type = (char*)&buf[p++];
		buf[p] = '\0';

		// If the mac is already exists, just return
		for(int i=0; i<this->cmdata->deviceCount; i++) {
			if(0 == strncmp(mac, this->cmdata->deviceList[i].MAC, DEVICE_IP_LEN)) {
				return;
			}
		}

		(void)strncpy((char*)deviceList.mac, mac, E2DA_ZIGBEE_MAC_LEN);
		(void)strncpy(this->cmdata->deviceList[this->cmdata->deviceCount].MAC, mac, E2DA_ZIGBEE_MAC_LEN);

		//get type
		deviceList.type = atoi(type);
		this->cmdata->deviceList[this->cmdata->deviceCount].type = deviceList.type;

		//Init other field
		this->cmdata->deviceList[this->cmdata->deviceCount].R = 255;
		this->cmdata->deviceList[this->cmdata->deviceCount].G = 255;
		this->cmdata->deviceList[this->cmdata->deviceCount].B = 255;
		this->cmdata->deviceList[this->cmdata->deviceCount].L = 100;
		this->cmdata->deviceList[this->cmdata->deviceCount].effect = 0;
		this->cmdata->deviceList[this->cmdata->deviceCount].online = false;
		this->cmdata->deviceList[this->cmdata->deviceCount].powerStatus = 0;
		this->cmdata->deviceList[this->cmdata->deviceCount].LQI = 0;
		this->cmdata->deviceCount++;

		// Add new device to EEPROM
		EepromManage::AddEepromDeviceList(deviceList);

		
		sprintf(buffer, "DMADD %s\n", mac);
		this->cmdata->com->write(buffer);
	//	conn->print(mac);
		conn->print("AB\r\n");
		return;
	}*/

	// EEPROMDUMP
	// dump 1024 bytes EEPROM data and output to connection by hexes
/*f(0 == strncmp((char*)buf, "EEPROMDUMP", 10)) {
		for(uint16_t i=0; i<MAX_EEPROM_SIZE; i++) {
			char data[2];
			sprintf(data, "%02X", eeprom_read_byte((uint8_t*)i));
			conn->print(data);
		}
		conn->print("\n");
		return;
	}

	// Clear EEPROM data
	if(0 == strncmp((char*)buf, "EEPROMRESET", 11)) {
			for(uint16_t i=0; i<MAX_EEPROM_SIZE; i++) {
				eeprom_write_byte((unsigned char*)i, 0xff);
			}
			return;
	}*/

	// Refresh
	// When get a R message, box will send a DSC to get all devices and
	// wait for 4s to get all the S messages from COM, then add MAC address
	// into EEPROM if it does not been
	if(0 == strncmp((char*)buf, "R", 1)) {
		this->cmdata->com->write("R\n");
		return;
	}

	return;
}

// send message to available connection
void Mobile::sendMessageToClient(uint8_t* msg, uint8_t len) {
	this->_mobileIntf->write(msg, len);
}

//check if exceeds 2 local connection, drop them
void Mobile::dropMobileClent(byte num) {
	byte connSock[MAX_SOCK_NUM], j=0;
	bool maxConnFlag = false;

	for (byte sock = 0; sock < MAX_SOCK_NUM; sock++) {
    	EthernetClient client(sock);
    	if (EthernetClass::_server_port[sock] == MOBILEINTPORT) {
			if(client.connected()){
				connSock[j++] = sock;
				if(j==2) {
					maxConnFlag = true;
					break;
				}
			}
		}
	}
	if(maxConnFlag) {
		for(j=0;j<num;j++){
			EthernetClient client(connSock[j]);
			client.stop();
		}
	}
}

void Mobile::_printDeviceList(EthernetClient* conn) {
	conn->print("GLB ");
	for(int i=0; i<this->cmdata->deviceCount; i++) {
		conn->write((uint8_t*)this->cmdata->deviceList[i].MAC, DEVICE_IP_LEN);
		conn->print(",");
		conn->print(this->cmdata->deviceList[i].type);
		conn->print(",");
		conn->print(this->cmdata->deviceList[i].online ? "1" : "0");
		conn->print(",");
		conn->print(this->cmdata->deviceList[i].LQI);
		conn->print(",");
		switch(this->cmdata->deviceList[i].type) {
		case HARDWARE_RGBLED:
			conn->print(this->cmdata->deviceList[i].R);
			conn->print(",");
			conn->print(this->cmdata->deviceList[i].G);
			conn->print(",");
			conn->print(this->cmdata->deviceList[i].B);
			conn->print(",");
			conn->print(this->cmdata->deviceList[i].L);
			conn->print(",");
			conn->print(this->cmdata->deviceList[i].effect);
			conn->print(";");
			break;
		case HARDWARE_POWERSOCKET:
			conn->print(this->cmdata->deviceList[i].powerStatus);
			conn->print(";");
			break;
		default:
			conn->print("0,0,0,0,0;");
			break;
		}
	}
	conn->print("\r\n");
}
