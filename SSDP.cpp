/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 *
 * This file contains the SSDP service on YeeBox. This service is used to finding
 * the bridge by mobile APP.
 */

#include "SSDP.h"

#define SHORT_BUTTON_PUSH_DELAY 15000
#define SSDP_PORT 1900 // SSDP port

//Used for match the string "MAC:"
#define STR_MATCH_NULL 0
#define STR_MATCH_FST 1
#define STR_MATCH_SEC 2
#define STR_MATCH_THR 3
#define STR_MATCH_FOR 4
#define STR_MATCH_FIV 5
#define STR_MATCH_SUCC 6

//start SSDP instance
void SSDP::begin(CommonData *cmdata) {
	this->_udp.begin(SSDP_PORT);
	this->_cmdata = cmdata;
}

//used in main loop
void SSDP::loop() {

	if(this->_cmdata->_mode != BOXMODE_NORMAL){
		return;
	}

	// if there's data available, read a packet
	int packetSize = this->_udp.parsePacket();

	if (packetSize) {
		char mac[E2DA_WHITE_LIST_MAC_LEN + 1];
		//Read the buffer and parse the mac address
		byte word;
		byte status = STR_MATCH_NULL;

		_cmdata->leds->setLocalFastFlash(500);

		// _cmdata->com->write("SSDP packet received\n");

		//Check if "yeebox" exists
		while(_udp.available()){
			word = _udp.read();
			if(status == STR_MATCH_NULL && word == 'y'){
				status = STR_MATCH_FST;
			}
			else if(status == STR_MATCH_FST && word == 'e'){
				status = STR_MATCH_SEC;
			}
			else if(status == STR_MATCH_SEC && word == 'e'){
				status = STR_MATCH_THR;
			}
			else if(status == STR_MATCH_THR && word == 'b'){
				status = STR_MATCH_FOR;
			}
			else if(status == STR_MATCH_FOR && word == 'o'){
				status = STR_MATCH_FIV;
			}
			else if(status == STR_MATCH_FIV && word == 'x'){
				_udp.read(mac, E2DA_WHITE_LIST_MAC_LEN);
				mac[E2DA_WHITE_LIST_MAC_LEN] = '\0';
				status = STR_MATCH_SUCC;
				break;
			}
			else{
				status = STR_MATCH_NULL;
			}
			
		}
		if(status != STR_MATCH_SUCC){
			return;
		}

		//if maximum APP local connection exceeds, drop some connnections
		_cmdata->mobile->dropMobileClent(2);

/*		//Check invalidity of MAC address
		for(int i=0; i<E2DA_WHITE_LIST_MAC_LEN; i++){
			if( (mac[i] >= 'A' && mac[i] <= 'F') || (mac[i] >= '0' && mac[i] <= '9') ){
				;
			}
			else{
				return;
			}
		}

		if(EepromManage::checkEepromWhiteListByMac(mac) == false){
			
			//Check if the button is pushed down, if not, do nothing.
			if((millis() - this->_cmdata->btn->lastShortBtnPushTime) >  SHORT_BUTTON_PUSH_DELAY){
				this->_cmdata->btn->lastShortBtnPushTime = 0; //reset this value
				return;
			}
			EepromManage::AddEepromMacToWhiteList(mac);

			//send new mac to server
			_cmdata->remote->sendMsgToServer(APP_AUTH_MAC);
			_cmdata->remote->sendMsgToServer((unsigned char*)mac, E2DA_WHITE_LIST_MAC_LEN);
			_cmdata->remote->sendMsgToServer("\n");
		}

		memset(mac, 0, E2DA_WHITE_LIST_MAC_LEN+1);*/
		
		// send a reply, to the IP address and port who sent us the packet
		this->_udp.beginPacket(this->_udp.remoteIP(), this->_udp.remotePort());
		this->_udp.print("HTTP/1.1 200 OK\r\n");
		this->_udp.print("ST: yeelink:yeebox\r\n");
		this->_udp.print("USN: uuid:f5f52821-8290-294a-aed9-7dd1a5ca2165\r\n");
		this->_udp.print("Ext:\r\n");
		this->_udp.print("Cache-Control: max-age = 1800\r\n");
		this->_udp.print("LOCATION: ");
		this->_udp.print(Ethernet.localIP());
		this->_udp.print("\r\nMAC: ");
		for(int i=0; i<(E2DA_WHITE_LIST_MAC_LEN/2); i++) {
			sprintf(&mac[i*2], "%02X", this->_cmdata->mac[i]);
		}
		this->_udp.print(mac);
		this->_udp.print("\r\nBridgeVer: ");
		this->_udp.print((char*)this->_cmdata->version);
		this->_udp.print("\r\n");
		this->_udp.print("Content-Length: 0");
		this->_udp.print("\r\n\r\n");
		this->_udp.endPacket();
	}
}
