//#include "stdafx.h"
#include "pch.h"
#include "Device.h"
//#include "LogFile.h"
//#include "FileProcess.h"
//#include "json/json.h"

std::string GetNameByChannel(int channel, std::vector<DeviceInfo> &deviceList)
{
	std::string name = "";
	for (size_t i = 0; i < deviceList.size(); i++)
	{
		if (channel == deviceList[i].device_id)
		{
			name = deviceList[i].device_name;
		}
	}
	return name;
}
CDevice::CDevice()
: main_user_(NULL)
, main_hwnd_(NULL)
{
	
}


CDevice::~CDevice()
{
}

int CDevice::StartRealPlay(HWND hWnd)
{
	return 0;
}
int CDevice::StopRealPlay()
{
	return 0;
}

void CDevice::RegisterCB(LONG main_user, HWND main_hwnd)
{
	main_user_ = main_user;
	main_hwnd_ = main_hwnd;
}

bool CDevice::GetbPlaying()
{
	return true;
}

int CDevice::GetIndex()
{
	return -1;
}

int CDevice::GetDeviceID()
{
	return 0;
}