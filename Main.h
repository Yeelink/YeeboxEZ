/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 */

#ifndef YeeboxEZ_H_
#define YeeboxEZ_H_
#include "Defines.h"
#include <SPI.h>
#include <Ethernet.h>
#include <avr/wdt.h>
#include "CommonData.h"
#include "BoxLogic.h"
#include "SSDP.h"
#include "Mobile.h"
#include "BoxCOM.h"
#include "ButtonPushCheck.h"

//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

extern void statusLedInit(CommonData *cmdata);

//Do not add code below this line
#endif /* YeeboxEZ_H_ */
