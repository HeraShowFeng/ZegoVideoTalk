#pragma once

#define mConfig theApp.GetConfig()
#define mBase theApp.GetBase()

//demo�汾��ö������
typedef enum _Version
{
	ZEGO_PROTOCOL_UDP = 0,
	ZEGO_PROTOCOL_UDP_INTERNATIONAL = 1,
	ZEGO_PROTOCOL_CUSTOM = 2
}Version;