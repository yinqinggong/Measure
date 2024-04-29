#pragma once
#include <iostream>
#include <string>
#include <vector>

typedef struct typeCameraInfo
{
	int id;
	char rtsp[256];
	int device_id;
	int camera_type; 
	char recordPath[256];
}CameraInfo;
typedef struct tagDeviceInfo
{
	int index;//设备索引，区分与channel
	int channel;//0/1:播放短焦/长焦
	int online;//1:设备在线 0：设备下线
	int device_id;
	char device_name[32];
	char ip[32];
	int status;
	int truck_id;
	char plate_number[32];
	char controller_ip[32];
	int controller_port;
	std::vector<CameraInfo> cameras;
}DeviceInfo;

std::string GetNameByChannel(int channel, std::vector<DeviceInfo> &deviceList);

class CDevice
{
public:
	CDevice();
	virtual ~CDevice();
	virtual int StartRealPlay(HWND hWnd);
	virtual int StopRealPlay();

	virtual bool GetbPlaying();
	virtual int GetIndex();
	virtual int GetDeviceID();
public:
	LONG main_user_;
	HWND main_hwnd_;
	void RegisterCB(LONG main_user, HWND main_hwnd);
};

