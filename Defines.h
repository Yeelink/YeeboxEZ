/*
 * Copyright (c) 2012 Yeelink.net by dapingliu <dapingliu@yeelink.net>
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include "Arduino.h"

//add your function definitions for the project YeeboxEZ here
#define MAX_SUPPORTED_LAMPS 	8

#define MOBILEINTPORT 10003


#define DEVICE_MAC_LEN		16
#define DEVICE_IP_LEN		4

#define BOX_COM_BAUD		38400
#define MAX_EEPROM_SIZE		1024

//Used for button pin check
#define BUTTON_PIN 		A7
#define BUTTON_LOW_RANGE_VALUE    200

#define MAX_WHITE_LIST_NUM 10

#define APP_AUTH_MAC "FROM_YEEBOX_AUTH_MAC:"
#define YBOX_LOCAL_MAC "FROM_YEEBOX_LOCAL_MAC:"

typedef enum eeprom_data_len {
	E2DA_LOGO_LEN = 10,
	E2DA_VERSION_LEN = 10,
	E2DA_BOX_MAC_LEN = 6,
	E2DA_BOX_ID_LEN = 6,
	E2DA_BOX_KEY_LEN = 6,
	E2DA_FACEBOOK_KEY_PRES_LEN = 1,
	E2DA_FACEBOOK_STATUS_LEN = 1,
	E2DA_FACEBOOK_LEN = (128-E2DA_BOX_ID_LEN-E2DA_BOX_KEY_LEN),
	E2DA_DEV_NUM_LEN = 2,
	E2DA_DEV_TYPE_LEN = 2,
	E2DA_WHITE_LIST_NUM_LEN = 1,
	E2DA_WHITE_LIST_MAX_LEN = 120,
	E2DA_WHITE_LIST_MAC_LEN = 12,
	E2DA_ZIGBEE_MAC_LEN = 4 // zigbee mac length
} eeprom_data_len_t;


typedef enum eeprom_data_addr {
	//box identifier
	E2DA_START_ADDR = 0, //0
	E2DA_LOGO_ADDR = E2DA_START_ADDR + 0, //0x000
	E2DA_VERSION_ADDR = E2DA_LOGO_ADDR + E2DA_LOGO_LEN, //0x00A
	E2DA_MAC_ADDR = E2DA_VERSION_ADDR + E2DA_VERSION_LEN, //0x014

	E2DA_BOX_ID_ADDR = E2DA_MAC_ADDR + E2DA_BOX_MAC_LEN, //0x01A
	E2DA_BOX_KEY_ADDR = E2DA_BOX_ID_ADDR + E2DA_BOX_ID_LEN, //0x020

	//face book key
	E2DA_FACEBOOK_KEY_PRES_ADDR = E2DA_MAC_ADDR + E2DA_BOX_KEY_LEN, //0x036
	E2DA_FACEBOOK_STATUS = E2DA_FACEBOOK_KEY_PRES_ADDR + E2DA_FACEBOOK_KEY_PRES_LEN, //0x01B
	E2DA_FACEBOOK_ADDR = E2DA_FACEBOOK_STATUS + E2DA_FACEBOOK_STATUS_LEN, //0x01C

	//phone white list
	E2DA_WHITE_LIST_LEN_ADDR = E2DA_FACEBOOK_ADDR + E2DA_FACEBOOK_LEN,//0x09C
	E2DA_WHITE_LIST_ADDR = E2DA_WHITE_LIST_LEN_ADDR + E2DA_WHITE_LIST_NUM_LEN, //0x09D

	//device list
	E2DA_DEV_NUM_ADDR = E2DA_WHITE_LIST_ADDR + E2DA_WHITE_LIST_MAX_LEN, //0x115
	E2DA_DEV_DATA_ADDR = E2DA_DEV_NUM_ADDR + E2DA_DEV_NUM_LEN //0x117
	// Left 745 byte
} eeprom_data_addr_t;


typedef struct eeprom_device_list{
	unsigned char mac[E2DA_ZIGBEE_MAC_LEN];
	unsigned short type;
}eeprom_device_list_t;

typedef struct device_node {
	uint8_t type;
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t L;
	uint8_t effect;
	bool online;
	bool powerStatus;
	char MAC[DEVICE_IP_LEN];
	uint8_t LQI; //zigbee link quality indicator
} device_node_t;

typedef enum boxMode {
	BOXMODE_INIT = 0,
	BOXMODE_INIT_WAIT,
	BOXMODE_NORMAL,
	BOXMODE_REFRESH
} boxMode_t;

typedef enum ledStatus {
	LED_OFF = 0,	//always off
	LED_ON = 1,			//always on
	LED_FAST_FLASH,	//fast flash
	LED_MIDDLE_FLASH,	//middle flash
	LED_SLOW_FLASH	//slow flash
} ledStatus_t;

#define HARDWARE_BRIDGE			0
#define HARDWARE_RGBLED			1
#define HARDWARE_POWERSOCKET	2

#endif /* DEFINES_H_ */
