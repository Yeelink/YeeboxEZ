/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 *
 * This is the main project file for YeeBox. This version implements
 * YeeBox Ethernet-Zigbee solution, so called YeeboxEZ.
 * 	- Atmel Mega328P
 * 	- WIZnet W5100
 * 	- TI CC2530
 */

#include "StatusLED.h"

// after v1.0.2, global & local leds swapped
#define GLOBAL_LED 	4
#define LOCAL_LED	3

#define TIMER2_INTERVAL			50
#define FAST_FLASH_INTERVAL		1	//1 timer2 intervals == 50ms
#define MIDDLE_FLASH_INTERVAL	5	//5 timer2 intervals == 250ms
#define SLOW_FLASH_INTERVAL		20	//20 timer2 intervals = 1s

class CommonData *_leds_cmdata;
unsigned short slowFlashCounter;
unsigned short middleFlashCounter;
unsigned short fastFlashCounter;

unsigned short localLEDOnOff = LOW;
unsigned short globalLEDOnOff = LOW;

void statusLedInit(CommonData *cmdata);
void timer2InterruptHandler();

//do setup here
void statusLedInit(CommonData *cmdata) {
	_leds_cmdata = cmdata;

	fastFlashCounter = 0;
	middleFlashCounter = 0;
	slowFlashCounter = 0;

	pinMode(GLOBAL_LED, OUTPUT);
	pinMode(LOCAL_LED, OUTPUT);

	//every 100ms will trigger
	MsTimer2::set(TIMER2_INTERVAL, timer2InterruptHandler);
	MsTimer2::start();
}


void timer2InterruptHandler() {
	fastFlashCounter++;
	middleFlashCounter++;
	slowFlashCounter++;

	//fast flash
	if(fastFlashCounter == FAST_FLASH_INTERVAL) {
		fastFlashCounter = 0;

		if(_leds_cmdata->leds->localLedStatus == LED_FAST_FLASH) {
			localLEDOnOff = (localLEDOnOff == LOW ? HIGH : LOW);
			digitalWrite(LOCAL_LED, localLEDOnOff);
		}

		if(_leds_cmdata->leds->globalLedStatus == LED_FAST_FLASH) {
			globalLEDOnOff = (globalLEDOnOff == LOW ? HIGH : LOW);
			digitalWrite(GLOBAL_LED, globalLEDOnOff);
		}
	}

	//middle flash
	if(middleFlashCounter == MIDDLE_FLASH_INTERVAL) {
		middleFlashCounter = 0;

		if(_leds_cmdata->leds->localLedStatus == LED_MIDDLE_FLASH) {
			localLEDOnOff = (localLEDOnOff == LOW ? HIGH : LOW);
			digitalWrite(LOCAL_LED, localLEDOnOff);
		}

		if(_leds_cmdata->leds->globalLedStatus == LED_MIDDLE_FLASH) {
			globalLEDOnOff = (globalLEDOnOff == LOW ? HIGH : LOW);
			digitalWrite(GLOBAL_LED, globalLEDOnOff);
		}
	}

	//slow flash
	if(slowFlashCounter == SLOW_FLASH_INTERVAL) {
		slowFlashCounter = 0;

		if(_leds_cmdata->leds->localLedStatus == LED_SLOW_FLASH) {
			localLEDOnOff = (localLEDOnOff == LOW ? HIGH : LOW);
			digitalWrite(LOCAL_LED, localLEDOnOff);
		}

		if(_leds_cmdata->leds->globalLedStatus == LED_SLOW_FLASH) {
			globalLEDOnOff = (globalLEDOnOff == LOW ? HIGH : LOW);
			digitalWrite(GLOBAL_LED, globalLEDOnOff);
		}
	}

	//on or off
	if(_leds_cmdata->leds->localLedStatus == LED_ON ||
			_leds_cmdata->leds->localLedStatus == LED_OFF) {
		digitalWrite(LOCAL_LED, _leds_cmdata->leds->localLedStatus);
	}
	if(_leds_cmdata->leds->globalLedStatus == LED_ON ||
			_leds_cmdata->leds->globalLedStatus == LED_OFF) {
		digitalWrite(GLOBAL_LED, _leds_cmdata->leds->globalLedStatus);
	}
}

//--------------------------------------------------------------------------

void StatusLED::begin(CommonData *cmdata){
	this->_cmdata = cmdata;

	statusLedInit(cmdata);
}

void StatusLED::loop(){
	unsigned long now = millis();
	bool deviceOnline = false;

	if(this->localLedStatus == LED_FAST_FLASH ||
			this->localLedStatus ==	LED_MIDDLE_FLASH ||
			this->localLedStatus == LED_SLOW_FLASH) {
		if(now - this->_local_flash_start_time > this->_local_flash_timer) {
			this->localLedStatus = this->preFlashLocalLedStatus;
		}
	}

	if(this->globalLedStatus == LED_FAST_FLASH ||
			this->globalLedStatus == LED_MIDDLE_FLASH ||
			this->globalLedStatus == LED_SLOW_FLASH) {
		if(now - this->_global_flash_start_time > this->_global_flash_timer) {
			this->globalLedStatus = this->preFlashGlobalLedStatus;
		}
	}

	//Global LED shows
	for(int i=0; i<this->_cmdata->deviceCount; i++) {
		deviceOnline = (deviceOnline || this->_cmdata->deviceList[i].online);
	}
	if(deviceOnline == false) {
		this->setGlobalOff();
	}
	else {
		this->setGlobalOn();
	}
}

void StatusLED::setLocalOn() {
	this->localLedStatus = LED_ON;
}

void StatusLED::setLocalOff() {
	this->localLedStatus = LED_OFF;
}

void StatusLED::setLocalFastFlash(unsigned int time) {
	if((time < 0) || (time > 65000)) {
		return;
	}

	if(this->localLedStatus == LED_ON ||
			this->localLedStatus == LED_OFF) {
		this->preFlashLocalLedStatus = this->localLedStatus;
		this->_local_flash_timer = time;
		this->_local_flash_start_time = millis();
		this->localLedStatus = LED_FAST_FLASH;
	}
}

void StatusLED::setLocalMiddleFlash(unsigned int time) {
	if((time < 0) || (time > 65000)) {
		return;
	}

	if(this->localLedStatus == LED_ON ||
			this->localLedStatus == LED_OFF) {
		this->preFlashLocalLedStatus = this->localLedStatus;
		this->_local_flash_timer = time;
		this->_local_flash_start_time = millis();
		this->localLedStatus = LED_MIDDLE_FLASH;
	}
}

void StatusLED::setLocalSlowFlash(unsigned int time) {
	if((time < 0) || (time > 65000)) {
		return;
	}

	if(this->localLedStatus == LED_ON ||
			this->localLedStatus == LED_OFF) {
		this->preFlashLocalLedStatus = this->localLedStatus;
		this->_local_flash_timer = time;
		this->_local_flash_start_time = millis();
		this->localLedStatus = LED_SLOW_FLASH;
	}
}

void StatusLED::setGlobalOn() {
	this->globalLedStatus = LED_ON;
}

void StatusLED::setGlobalOff() {
	this->globalLedStatus = LED_OFF;
}

void StatusLED::setGlobalFastFlash(unsigned int time) {
	if((time < 0) || (time > 65000)) {
		return;
	}

	if(this->globalLedStatus == LED_ON ||
			this->globalLedStatus == LED_OFF) {
		this->preFlashGlobalLedStatus = this->globalLedStatus;
		this->_global_flash_timer = time;
		this->_global_flash_start_time = millis();
		this->globalLedStatus = LED_FAST_FLASH;
	}
}

void StatusLED::setGlobalMiddleFlash(unsigned int time) {
	if((time < 0) || (time > 65000)) {
		return;
	}

	if(this->globalLedStatus == LED_ON ||
			this->globalLedStatus == LED_OFF) {
		this->preFlashGlobalLedStatus = this->globalLedStatus;
		this->_global_flash_timer = time;
		this->_global_flash_start_time = millis();
		this->globalLedStatus = LED_MIDDLE_FLASH;
	}
}

void StatusLED::setGlobalSlowFlash(unsigned int time) {
	if((time < 0) || (time > 65000)) {
		return;
	}

	if(this->globalLedStatus == LED_ON ||
			this->globalLedStatus == LED_OFF) {
		this->preFlashGlobalLedStatus = this->globalLedStatus;
		this->_global_flash_timer = time;
		this->_global_flash_start_time = millis();
		this->globalLedStatus = LED_SLOW_FLASH;
	}
}

