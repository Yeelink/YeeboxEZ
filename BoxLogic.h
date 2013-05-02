/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 */

#ifndef BOXLOGIC_H_
#define BOXLOGIC_H_

#include "Defines.h"
#include "CommonData.h"

class BoxLogic {
private:
	class CommonData *_cmdata;
public:
	void begin(CommonData *cmdata);
	void loop();

	//when there is a device status message comes from zigbee network
	void updateDeviceStatus(char* mac, uint8_t type, char *online, char *R, char *G,
			char *B, char *L, char *powerStatus, char *LQI, bool isNewDevice);
	void deviceOnlineChange(uint8_t deviceIndex, bool online, bool isNewDevice);
};


#endif /* BOXLOGIC_H_ */
