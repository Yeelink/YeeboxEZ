/*
 * Copyright (c) 2012 Yeelink.net by gordon tang
 */

#ifndef BUTTON_PUSH_CHECK_H_
#define BUTTON_PUSH_CHECK_H_

#include <Ethernet.h>
#include <EthernetUdp.h>
#include "Arduino.h"
#include "CommonData.h"

class ButtonPushCheck {
private:
	class CommonData *_cmdata;
	unsigned long buttonDownTime;

public:
//	unsigned long lastShortBtnPushTime;
	void begin(CommonData *cmdata);
	void loop();
	void checkButtonPushLongTerm();
};

#endif
