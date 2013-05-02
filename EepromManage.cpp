/*
 * Copyright (c) 2012 Yeelink.net by gordon
 */

#include "EepromManage.h"
#include <avr/EEPROM.h>

/*
void EepromManage::getEepromLogo(unsigned char* logo)
{
	eeprom_read_block(logo, (void*)E2DA_LOGO_ADDR, E2DA_LOGO_LEN);
}*/

void EepromManage::getEepromVersion(unsigned char* version)
{
	eeprom_read_block(version, (void*)E2DA_VERSION_ADDR, E2DA_VERSION_LEN);
}

void EepromManage::getEepromMac(unsigned char* mac)
{
	eeprom_read_block(mac, (void*)E2DA_MAC_ADDR, E2DA_BOX_MAC_LEN);
}

/*
void EepromManage::getEepromBoxId(unsigned char* id)
{
	eeprom_read_block(id, (void*)E2DA_BOX_ID_ADDR, E2DA_BOX_ID_LEN);
}
*/
void EepromManage::getEepromBoxKey(unsigned char* key)
{
	eeprom_read_block(key, (void*)E2DA_BOX_KEY_ADDR, E2DA_BOX_KEY_LEN);
}

/*
void EepromManage::getEepromFaceBook(unsigned char* facebook)
{
	eeprom_read_block(facebook, (void*)E2DA_FACEBOOK_ADDR, E2DA_FACEBOOK_LEN);
}
*/
byte EepromManage::getEepromWhiteListNum()
{
	byte num = 0;
	eeprom_read_block(&num, (void*)E2DA_WHITE_LIST_LEN_ADDR, E2DA_WHITE_LIST_NUM_LEN);
	if(num == 0xFF){
		return 0;
	}
	return num;
}

void EepromManage::getEepromWhiteListMacByIndex(char* mac, uint8_t index)
{
	if(mac == NULL){
		return;
	}

	int src = E2DA_WHITE_LIST_ADDR + (index*E2DA_WHITE_LIST_MAC_LEN);
	eeprom_read_block(mac, (void*)src, E2DA_WHITE_LIST_MAC_LEN);
	return;
}

void EepromManage::setEepromWhiteListNum(byte num)
{
	eeprom_write_block(&num, (void*)E2DA_WHITE_LIST_LEN_ADDR, E2DA_WHITE_LIST_NUM_LEN);
}


bool EepromManage::checkEepromWhiteListByMac(char* mac)
{
	if(mac == NULL){
		return false;
	}
	byte num = getEepromWhiteListNum();
	char devMac[E2DA_WHITE_LIST_MAC_LEN];
	int src = E2DA_WHITE_LIST_ADDR;
	byte i;

	if(num > MAX_WHITE_LIST_NUM){
		num = MAX_WHITE_LIST_NUM;
	}
	
	for(i=0; i<num; i++)
	{
		eeprom_read_block(devMac, (void*)src, E2DA_WHITE_LIST_MAC_LEN);
		if(strncmp(mac, devMac, E2DA_WHITE_LIST_MAC_LEN) == 0)
		{
			return true;
		}
		src = src + E2DA_WHITE_LIST_MAC_LEN;
	}
	return false;
}

/* Add mac white list to EEPROM. We can only store 10 mac in white list. If more than
 * 10 is added, replace the existing one in order, so we store the number range from
 * 0 to 20. e.g. if now the number is exceed to 20, we treat it as 10.
 */
void EepromManage::AddEepromMacToWhiteList(char* mac)
{
	int src = E2DA_WHITE_LIST_ADDR;
	byte num = getEepromWhiteListNum();
	byte i;

	if(checkEepromWhiteListByMac(mac)){
		return;
	}

	//if the number is more than MAX_WHITE_LIST_NUM, replace the existing in order. 
	 for(i=0; i<num; i++)
	 {
	 	src = src + E2DA_WHITE_LIST_MAC_LEN;
		if(i == MAX_WHITE_LIST_NUM){
			src = E2DA_WHITE_LIST_ADDR;
		}
	 }

	eeprom_write_block(mac, (void*)src, E2DA_WHITE_LIST_MAC_LEN);

	num++;

	// Record the number range 0~20, if more than 20, just decrease 10
	if(num >= MAX_WHITE_LIST_NUM*2){
		num = num - MAX_WHITE_LIST_NUM;
	}
	setEepromWhiteListNum(num);
}

unsigned short EepromManage::getEepromDeviceNum()
{
	unsigned short num = 0;
	eeprom_read_block(&num, (void*)E2DA_DEV_NUM_ADDR, E2DA_DEV_NUM_LEN);
	if(num == 0xFFFF){
		return 0;
	}
	return num;
}

void EepromManage::getEepromDeviceList(eeprom_device_list_t* deviceList, unsigned short num)
{
	int src = E2DA_DEV_DATA_ADDR;
	unsigned short i;
	for(i=0; i<num; i++)
	{
		if((src + E2DA_ZIGBEE_MAC_LEN + E2DA_DEV_TYPE_LEN) <= MAX_EEPROM_SIZE)
		{
			eeprom_read_block(deviceList[i].mac, (void*)src, E2DA_ZIGBEE_MAC_LEN);
			src = src + E2DA_ZIGBEE_MAC_LEN;
			eeprom_read_block(&(deviceList[i].type), (void*)src, E2DA_DEV_TYPE_LEN);
			src = src + E2DA_DEV_TYPE_LEN;
		}
		else
		{
			break;
		}
	}
}

bool EepromManage::checkEepromDeviceByMac(unsigned char* mac)
{
	unsigned short num = getEepromDeviceNum();
	unsigned char devMac[E2DA_ZIGBEE_MAC_LEN];
	int src = E2DA_DEV_DATA_ADDR;
	unsigned short i;
	for(i=0; i<num; i++)
	{
		if((src + E2DA_ZIGBEE_MAC_LEN + E2DA_DEV_TYPE_LEN) <= MAX_EEPROM_SIZE)
		{
			eeprom_read_block(devMac, (void*)src, E2DA_ZIGBEE_MAC_LEN);
			if(strncmp((char*)mac, (char*)devMac, E2DA_ZIGBEE_MAC_LEN) == 0)
			{
				return true;
			}
			src = src + E2DA_ZIGBEE_MAC_LEN + E2DA_DEV_TYPE_LEN;
		}
		else
		{
			break;
		}
	}
	return false;
}

/*
void EepromManage::setEepromLogo(unsigned char* logo)
{
	eeprom_write_block(logo, (void*)E2DA_LOGO_ADDR, E2DA_LOGO_LEN);
}

void EepromManage::setEepromVersion(unsigned char* version)
{
	eeprom_write_block(version, (void*)E2DA_VERSION_ADDR, E2DA_VERSION_LEN);
}

void EepromManage::setEepromMac(unsigned char* mac)
{
	eeprom_write_block(mac, (void*)E2DA_MAC_ADDR, E2DA_BOX_MAC_LEN);
}

void EepromManage::setEepromFaceBook(unsigned char* facebook)
{
	eeprom_write_block(facebook, (void*)E2DA_FACEBOOK_ADDR, E2DA_FACEBOOK_LEN);
}
*/
void EepromManage::setEepromDeviceNum(unsigned short num)
{
	eeprom_write_block(&num, (void*)E2DA_DEV_NUM_ADDR, E2DA_DEV_NUM_LEN);
}
/*
void EepromManage::setEepromDeviceList(eeprom_device_list_t* deviceList, unsigned short num)
{
	int src = E2DA_DEV_DATA_ADDR;
	unsigned short i;
	for(i=0; i<num; i++)
	{
		eeprom_write_block(deviceList[i].mac, (void*)src, E2DA_ZIGBEE_MAC_LEN);
		src = src + E2DA_ZIGBEE_MAC_LEN;
		eeprom_write_block(&(deviceList[i].type), (void*)src, E2DA_DEV_TYPE_LEN);
		src = src + E2DA_DEV_TYPE_LEN;
	}
	setEepromDeviceNum(num);
}
*/
void EepromManage::AddEepromDeviceList(eeprom_device_list_t deviceList)
{
	int src = E2DA_DEV_DATA_ADDR;
	unsigned short num = getEepromDeviceNum();

	unsigned short i;
	for(i=0; i<num; i++)
	{
		src = src + E2DA_ZIGBEE_MAC_LEN + E2DA_DEV_TYPE_LEN;
	}

	//Check if mac is already exists.
	if(checkEepromDeviceByMac(deviceList.mac)){
		return;
	}

	if((src + E2DA_ZIGBEE_MAC_LEN + E2DA_DEV_TYPE_LEN) <= MAX_EEPROM_SIZE)
	{
		eeprom_write_block(deviceList.mac, (void*)src, E2DA_ZIGBEE_MAC_LEN);
		src = src + E2DA_ZIGBEE_MAC_LEN;
		eeprom_write_block(&(deviceList.type), (void*)src, E2DA_DEV_TYPE_LEN);

		setEepromDeviceNum(num+1);
	}
	else
	{
	   // not allowed to add new device, add debug log here
	}   
}
/*
void EepromManage::RemoveEepromDeviceFromList(unsigned char* mac)
{
	// TODO: 
}
*/
void EepromManage::resetEepromData()
{
	for(uint16_t i=E2DA_WHITE_LIST_LEN_ADDR; i<MAX_EEPROM_SIZE; i++) {
		eeprom_write_byte((unsigned char*)i, 0xff);
	}
	setEepromWhiteListNum(0);
	setEepromDeviceNum(0);
	return;
}


