/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 */

#ifndef MOBILE_H_
#define MOBILE_H_

#include <Ethernet.h>
#include <EthernetUdp.h>
#include "CommonData.h"

#define MOBILE_RX_BUF_SIZE		128
#define MOBILE_TX_BUF_SIZE		128

class Mobile {
private:
	class CommonData* cmdata;
	EthernetServer* _mobileIntf;
//	unsigned long connHBTime;

	void _printDeviceList(EthernetClient*);
public:
	uint8_t _rxBuf[MOBILE_RX_BUF_SIZE];
	uint8_t _rxSize;
	~Mobile();
	void begin(CommonData*);
	void loop();
	void processMessage(EthernetClient*, uint8_t* buf, uint8_t buf_size);
	void sendMessageToClient(uint8_t *msg, uint8_t len);
	void dropMobileClent(byte num);
};

#endif /* MOBILE_H_ */
