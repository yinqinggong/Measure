#pragma once
#include <string>
#include <vector>
//#include <Wininet.h>
#include <Windows.h>
#include <atlstr.h>

static const int g_settings_btnId = 12000;//配置界面按钮ID
static const int g_login_btnId = 13000;//登录界面按钮ID
static const int g_report_btnId = 14000;//报表界面按钮ID
static const int g_home_btnId = 15000;//首页界面按钮ID
static const int g_history_btnId = 16000;//首页界面按钮ID
static const int g_yard_btnId = 17000;//首页界面按钮ID
static const int g_help_btnId = 18000;//首页界面按钮ID

#define MINI_BTN_PATH_NOR			_T("img\\btn\\min.png")
#define MINI_BTN_PATH_HOVER			_T("img\\btn\\min_h.png")
#define MINI_BTN_PATH_DOWN			_T("img\\btn\\min_d.png")

#define MAX_BTN_PATH_NOR			_T("img\\btn\\max.png")
#define MAX_BTN_PATH_HOVER			_T("img\\btn\\max_h.png")
#define MAX_BTN_PATH_DOWN			_T("img\\btn\\max_d.png")

#define RESTORE_BTN_PATH_NOR		_T("img\\btn\\restore.png")
#define RESTORE_BTN_PATH_HOVER		_T("img\\btn\\restore_h.png")
#define RESTORE_BTN_PATH_DOWN		_T("img\\btn\\restore_d.png")

#define QUIT_BTN_PATH_NOR			_T("img\\btn\\close.png")
#define QUIT_BTN_PATH_HOVER			_T("img\\btn\\close_h.png")
#define QUIT_BTN_PATH_DOWN			_T("img\\btn\\close_d.png")

//#define WINDOW_BACKGROUND			_T("img\\window\\back.png")
#define WINDOW_BACKGROUND			_T("img\\window\\bg.png")
#define PNG_BIG_SHIP                _T("img\\window\\ship.png")
#define WINDOW_LOGO					_T("img\\window\\logo.png")

typedef struct typeVideoData
{
	int channel;
	int create_time;
	std::string duration;
	int durationMillis;
	std::string path;
}VideoData;

typedef struct typeActionData
{
	int action_id;
	int start_time;
	int end_time;
	int channel;
	std::string direction;
	int amount;
	int flag;
	int record_type;
	std::string video_url;
}ActionData;

typedef struct typeRecordData
{
	int record_id;
	int ship_id;
	std::string ship_name;
	int truck_id;
	std::string plate_number;
	int device_id; 
	int camera_id;
	int start_time;
	int end_time;
	int start_image;
	int end_image;
	std::string direction;
	int edit_flag;
	int confirm_time;
	std::string video_url;
	int amount;
	int record_type;
	std::string record_type_str;
}RecordData;

typedef struct typeHttpInfo
{
	char httpIP[32];
	int httpPort;
	char channelDetailAPI[32];
	char actionAPI[32];
	char confirmAPI[32];
	char userLoginAPI[32];
	char recordStatisticAPI[32];
	char historyAPI[32];
	char clientRegAPI[32];
	char recordByUserAPI[32];
	char shipAPI[32];
	char truckAPI[32];
	char deviceAPI[32];
	char shipTruckDeviceAPI[32];
	char beatAPI[32];
	char landingBillAPI[32];
	char clientCamera[32];
	char recordAPI[32];
	char realTimeAPI[32];
	char singleSheetAPI[32];
	char dailySheetAPI[32];
	char shipReportAPI[32];
	char newDailySheetAPI[32];
	char newShipReportAPI[32];
	char lightStatusAPI[32];
	char shiftStatusAPI[32];
	char offlineAPI[32];
}HttpInfo;

typedef struct typeShipData
{
	int ship_id;
	std::string ship_name;
	int start_time;
	int end_time;
	int op_status;
	int checked;
	std::string goods;
	float amount;
	float weight;
	float volumns;
}ShipData;

typedef struct typeShipConfigData
{
	int id;
	int ship_id;
	std::string ship_name;
	int truck_id;
	std::string plate_number;
	int device_id;
	std::string device_name;
	int checked;
}ShipConfigData;

typedef struct typeTruckData
{
	int truck_id;
	std::string plate_number;
	std::string driver;
	int status;//0:未知  1:可用  9:不可用
}TruckData;

typedef struct typeDeviceData
{
	int device_id;
	std::string device_name;
	std::string ip;
	int status;//0:未知  1:可用  9:不可用
}DeviceData;

typedef struct typeBillData
{
	int bill_id;
	int ship_id;
	std::string ship_name;
	int amount;
	float volumns;
	float weight;
	std::string consigner;
	std::string goods_name;
	int tally_amount;
	int remain_amount;
}BillData;

typedef struct typeRealTimeShip
{
	int ship_id;
	std::string ship_name;
	std::string goods;
	int amount;
	float weight;
	float volumns;
	int start_time;
	int end_time;
	int tally_amount;
	int remain_amount;
	int light_status;
	int op_status;
	int shift_status;
	int shift_id;
	std::vector<BillData> landingbills;
}RealTimeShip;

typedef struct typeSingleSheet
{
	int id;
	int ship_id;
	std::string  ship_name;
	std::string cabin_type;
	std::string checkpoint_no;	
	std::string consignor;
	std::string wood_type;
	int large;
	int amount;
	int tally_time;
	std::string tally_user;
	std::string consignee;
	std::string plate_number;
	std::string driver;
}SingleSheet;

typedef struct typeDailySheet
{
	int id;
	int start_time;
	int end_time;
	std::string tally_user;
	float amount;
}DailySheet;

typedef struct typeShipSheet
{
	int ship_id;
	std::string ship_name;
	float amount;
	int start_time;
	int end_time;
	float tally_amount;
}ShipSheet;

typedef struct typeCabinWood
{
	std::string wood_type;
	float wood_num;
}CabinWood;

typedef struct typeShipCabin
{
	std::vector<CabinWood> cabinWood;
	float amount;
}ShipCabin;

void  UTF8ToUnicode(const char* strUTF8, CString &strUnicode);
void  UnicodeToUtf8(const CString &unicode, char** strUTF8, int *strUTF8Len);
void  AnsiToUnicode(const char* strAnsi, CString &strUnicode);
void  UnicodeToAnsi(const CString &unicode, char** strAnsi, int *strAnsiLen);

long DiskFree(std::string strDevice);
void FindDirectory(const char* pathName, std::vector<std::string> &files);
BOOL DeleteFolder(LPCSTR lpszPath);
bool CheckDirectory(char* filePath);
std::string GetFormatTimeByTimestamp(int timestamp);
std::string GetCurFormatTime();
std::string GetCurFormatTimeDate();
std::string GetCurFormatTimeTime();
bool FileExist(const char *fileName);
bool ReadPhotoFile(std::basic_string<char> strFileName, std::string &strData);
bool WritePhotoFile(std::basic_string<char> strFileName, std::string &strData);

//BOOL GetInternetConnectedState();
//bool GetBeginEndTimeStamps(CDateTimeCtrl &beginDateCtrl, CDateTimeCtrl &endDateCtrl, int &beginTime, int &endTime);

//  拼接完整路径
CString SplicFullFilePath(CString strExeModuleName);
std::string GetCurrentPathUTF8();
std::string GetImagePathUTF8();

CString GetImagePath();//获取图片的路径
CString GetCurrentPath();//获取当前路径
CString GetAppdataPath();//获取appdata路径
std::string GetAppdataPathUTF8();
CString GetLogsPath();//获取appdata路径