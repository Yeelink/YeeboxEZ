/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 */

#ifndef SSDP_H_
#define SSDP_H_

#include <Ethernet.h>
#include <EthernetUdp.h>
#include "CommonData.h"

class SSDP {
private:
	class CommonData *_cmdata;
	EthernetUDP _udp;
public:
	void begin(CommonData *cmdata);
	void loop();
};

#endif /* SSDP_H_ */
