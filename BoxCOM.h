/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 */

#ifndef BOXCOM_H_
#define BOXCOM_H_

#include "Arduino.h"
#include "CommonData.h"

#define BOXCOM_RX_BUF_MAX_SIZE	64
//#define HB_TIMEOUT				10000

class BoxCOM {
private:
	class CommonData *_cmdata;
	uint8_t _rxBuf[BOXCOM_RX_BUF_MAX_SIZE];
	uint8_t _rxSize;
//	unsigned long _lastHBTime;
	void processMessage();
	bool onStatusMsg(char *msg, uint8_t len, bool addNewFlag);
public:
	void begin(CommonData *cmdata);
	void loop();
	int write(const char *str);
	int write(const char *buffer, unsigned int size);
};


#endif /* BOXCOM_H_ */
