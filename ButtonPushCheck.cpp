/*
 * Copyright (c) 2012 Yeelink.net by gordon tang
 */

#include "ButtonPushCheck.h"

#define BUTTON_PUSH_DOWN_LONG 5000

void ButtonPushCheck::begin(CommonData *cmdata) {
	_cmdata = cmdata;

	this->buttonDownTime = 0;
//	this->lastShortBtnPushTime = 0;
}

void ButtonPushCheck::loop() {
	int val = analogRead(BUTTON_PIN);

	if(val > BUTTON_LOW_RANGE_VALUE){ //Button is not pushed
		if(this->buttonDownTime != 0){
			this->checkButtonPushLongTerm();

			//reset buttonDownTime.
			this->buttonDownTime = 0;
		}
	}
	else{
		if(this->buttonDownTime == 0){
			this->buttonDownTime = millis();
		}
	}
}

void ButtonPushCheck::checkButtonPushLongTerm()
{
	unsigned long currTime = millis();
	if( (currTime - this->buttonDownTime) > BUTTON_PUSH_DOWN_LONG ){ // push down button for a long time
		//ALL LEDS FAST FLASH
		this->_cmdata->leds->setLocalFastFlash(4000);
		this->_cmdata->leds->setGlobalFastFlash(4000);
		EepromManage::resetEepromData();
	}
/*	else{
		//push down button for a short time
		this->lastShortBtnPushTime = currTime;
	}*/
}
