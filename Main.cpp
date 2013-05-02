/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 *
 * This is the main project file for YeeBox. This version implements
 * YeeBox Ethernet-Zigbee solution, so called YeeboxEZ.
 * 	- Atmel Mega328P
 * 	- WIZnet W5100
 * 	- TI CC2530
 */

#include "Main.h"

//CommonData instace
CommonData cmdata;
//BoxLogic instance
BoxLogic logic;
//Create a SSDP instance
SSDP ssdp;
//Create a mobile interface instance
Mobile mobile;
//Create a COM instance
BoxCOM com;
//Create a LED status instance
StatusLED leds;
//Create Button Status check instance
ButtonPushCheck btn;

//IPAddress ip(192,168,1,2);

void (*app_start)(void) = 0x0000;

void wdtSetup() {
  cli();
  MCUSR = 0;
  WDTCSR |= B00011000;
  WDTCSR = B01100001;
  sei();
}

ISR(WDT_vect)
{
	MCUSR = 0;
    WDTCSR |= _BV(WDCE) | _BV(WDE);
    WDTCSR = 0;
    app_start();
}


//The setup function is called once at startup of the sketch
void setup() {
	leds.begin(&cmdata);
	cmdata._mode = BOXMODE_INIT;
	cmdata.leds = &leds;
	leds.setLocalOff();
	leds.setLocalMiddleFlash(60000); //after 60s(another 60s is triggered by dhcp lib),
								   //local led will be power off, notify user that
								   //a network issue should be checked
	leds.setGlobalOff();

	// initialize data
	cmdata.setup();
	cmdata.logic = &logic;
	cmdata.ssdp = &ssdp;
	cmdata.mobile = &mobile;
	cmdata.com = &com;
	cmdata.btn = &btn;

	// start the Ethernet and UDP:
	//Ethernet.begin(cmdata.mac, ip);
	if (Ethernet.begin(cmdata.mac) != 0) {
		logic.begin(&cmdata);
		ssdp.begin(&cmdata);
		mobile.begin(&cmdata);
		com.begin(&cmdata);
		btn.begin(&cmdata);

		cmdata.leds->setLocalOn();
		cmdata.leds->setLocalSlowFlash(15000);
	}
	else {
		cmdata.leds->setLocalOff();
	}

	//wdt_enable(WDTO_8S);
	wdtSetup();
}

// The loop function is called in an endless loop
void loop() {
	btn.loop();
	ssdp.loop();
	mobile.loop();
	com.loop();
	logic.loop();
	leds.loop();

	wdt_reset();
//	delay(6000);
}
