/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 */

#ifndef STATUSLED_H_
#define STATUSLED_H_

#include "Arduino.h"
#include "CommonData.h"
#include "MsTimer2.h"

class StatusLED {
private:
	class CommonData *_cmdata;

	unsigned int _local_flash_timer;
	unsigned long _local_flash_start_time;
	unsigned int _global_flash_timer;
	unsigned long _global_flash_start_time;

public:
	//pre-flash status, used to recover status after flash
	ledStatus_t preFlashLocalLedStatus;
	ledStatus_t preFlashGlobalLedStatus;

	//Led status
	ledStatus_t localLedStatus;
	ledStatus_t globalLedStatus;

	//functions
	void begin(CommonData *cmdata);
	void loop();

	void setLocalOn();
	void setLocalOff();
	void setLocalFastFlash(unsigned int time); //ms, if time==0, always flash
	void setLocalMiddleFlash(unsigned int time);
	void setLocalSlowFlash(unsigned int time);

	void setGlobalOn();
	void setGlobalOff();
	void setGlobalFastFlash(unsigned int time);
	void setGlobalMiddleFlash(unsigned int time);
	void setGlobalSlowFlash(unsigned int time);
};

#endif /* STATUSLED_H_ */
