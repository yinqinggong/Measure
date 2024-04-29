#include "pch.h"
#include "common.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")

using namespace std;

void UTF8ToUnicode(const char* strUTF8, CString &strUnicode) {
	int nLen = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCCH)strUTF8, -1, NULL, 0);  //返回需要的unicode长度,包括\0字符
	WCHAR * wszUnicode = new WCHAR[nLen];
	memset(wszUnicode, 0, nLen * 2);
	nLen = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)strUTF8, -1, wszUnicode, nLen);    //把utf8转成unicode
	strUnicode = wszUnicode;
	delete[] wszUnicode;
}
void  UnicodeToUtf8(const CString &unicode, char** strUTF8, int *strUTF8Len) {
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)unicode, -1, NULL, 0, NULL, NULL); //返回需要的utf8长度,包括\0字符
	char *szUtf8 = new char[len];
	memset(szUtf8, 0, len);
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)unicode, -1, szUtf8, len, NULL, NULL);
	*strUTF8 = szUtf8;
	*strUTF8Len = len;
}
void  AnsiToUnicode(const char* strAnsi, CString &strUnicode) {
	int nLen = ::MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, (LPCCH)strAnsi, -1, NULL, 0);  //返回需要的unicode长度,包括\0字符
	WCHAR * wszUnicode = new WCHAR[nLen];
	memset(wszUnicode, 0, nLen * 2);
	nLen = MultiByteToWideChar(CP_ACP, 0, (LPCCH)strAnsi, -1, wszUnicode, nLen);    //把utf8转成unicode
	strUnicode = wszUnicode;
	delete[] wszUnicode;
}
void  UnicodeToAnsi(const CString &unicode, char** strAnsi, int *strAnsiLen) {
	int len;
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)unicode, -1, NULL, 0, NULL, NULL); //返回需要的utf8长度,包括\0字符
	char *szAnsi = new char[len];
	memset(szAnsi, 0, len);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)unicode, -1, szAnsi, len, NULL, NULL);
	*strAnsi = szAnsi;
	*strAnsiLen = len;
}

// 返回磁盘剩余空间
long DiskFree(std::string strDevice)
{
	ULARGE_INTEGER nFreeBytesAvailable;
	ULARGE_INTEGER nTotalNumberOfBytes;
	ULARGE_INTEGER nTotalNumberOfFreeBytes;

	if (GetDiskFreeSpaceExA(strDevice.c_str(),
		&nFreeBytesAvailable,
		&nTotalNumberOfBytes,
		&nTotalNumberOfFreeBytes))
	{
		return ((long)(nFreeBytesAvailable.QuadPart / (1024 * 1024.0)));//MB
	}

	return 0;
}

// 查找文件
void FindDirectory(const char* pathName, std::vector<std::string> &files)
{
	files.clear();

	// build a string with wildcards
	std::string strWildcard = pathName;
	strWildcard += "\\*.*";

	std::string str;

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA(strWildcard.c_str(), &FindFileData);
	if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //is directory
	{
		while (FindNextFileA(hFind, &FindFileData) != 0)
		{
			if (FindFileData.cFileName[0] == '.')
				continue;
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //is directory
			{
				std::string path = pathName;
				path += "\\";
				str = path + std::string(FindFileData.cFileName);
				files.push_back(str);
			}
		}
	}

	std::sort(files.begin(), files.end());

	FindClose(hFind);
}

// 删除文件夹
BOOL DeleteFolder(LPCSTR lpszPath)
{
	BOOL bRet = FALSE;
	std::string strWildcard = lpszPath;
	strWildcard += "\\*.*";
	std::string str;
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA(strWildcard.c_str(), &FindFileData);
	if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //is directory
	{
		while (FindNextFileA(hFind, &FindFileData) != 0)
		{
			if (FindFileData.cFileName[0] == '.')
				continue;
			if (FindFileData.dwFileAttributes/* & FILE_ATTRIBUTE_DIRECTORY*/) //is directory
			{
				std::string path = lpszPath;
				path += "\\";
				str = path + std::string(FindFileData.cFileName);
				if (DeleteFileA(str.c_str()))
				{
					bRet = TRUE;
				}
			}
		}
	}
	FindClose(hFind);
	RemoveDirectoryA(lpszPath);

	return bRet;
}

bool CheckDirectory(char* filePath)
{
	WIN32_FIND_DATAA wfd;
	HANDLE hFind = FindFirstFileA(filePath, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) &&
		(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
	{
		return true;
	}
	FindClose(hFind);

	if (!MakeSureDirectoryPathExists(filePath))
	{
		DWORD dwErrNo = ::GetLastError();
		LPSTR lpBuffer;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS
			| FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErrNo,
			LANG_NEUTRAL, (LPTSTR)& lpBuffer, 0, NULL);

		LocalFree(lpBuffer);
		return false;
	}

	return true;
}

std::string GetFormatTimeByTimestamp(int timestamp)
{
	if (timestamp < 0)
	{
		return "";
	}
	time_t tick = (time_t)timestamp;
	struct tm tm;
	char result[100] = { 0 };
	localtime_s(&tm, &tick);
	strftime(result, sizeof(result), "%Y-%m-%d %H:%M:%S", &tm);
	std::string retTime = result;
	return retTime;
}

std::string GetCurFormatTime()
{
	time_t tick = time(0);
	struct tm tm;
	char result[100] = { 0 };
	localtime_s(&tm, &tick);
	strftime(result, sizeof(result), "%Y%m%d_%H%M%S", &tm);
	std::string retTime = result;
	return retTime;
}

std::string GetCurFormatTimeDate()
{
	time_t tick = time(0);
	struct tm tm;
	char result[100] = { 0 };
	localtime_s(&tm, &tick);
	strftime(result, sizeof(result), "%Y年%m月%d日", &tm);
	std::string retTime = result;
	return retTime;
}

std::string GetCurFormatTimeTime()
{
	time_t tick = time(0);
	struct tm tm;
	char result[100] = { 0 };
	localtime_s(&tm, &tick);
	strftime(result, sizeof(result), "%H:%M:%S", &tm);
	std::string retTime = result;
	return retTime;
}

bool FileExist(const char * fileName)
{
	WIN32_FIND_DATAA wfd;
	if (FindFirstFileA(fileName, &wfd) == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	return true;
}
//int GetTimestamp()
//{
//	time_t st = time(0);
//}

std::string Encode(const char* Data, int DataByte)
{
	//编码表  
	const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值  
	string strEncode;
	unsigned char Tmp[4] = { 0 };
	int LineLength = 0;
	for (int i = 0; i<(int)(DataByte / 3); i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
		if (LineLength += 4, LineLength == 76) { strEncode += "\r\n"; LineLength = 0; }
	}
	//对剩余数据进行编码  
	int Mod = DataByte % 3;
	if (Mod == 1)
	{
		Tmp[1] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += "=";
	}

	return strEncode;
}

std::string Decode(const char* Data, int DataByte, int& OutByte)
{
	//解码表  
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'  
		0, 0, 0,
		63, // '/'  
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'  
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'  
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'  
	};
	//返回值  
	string strDecode;
	int nValue;
	int i = 0;
	while (i < DataByte)
	{
		if (*Data != '\r' && *Data != '\n')
		{
			nValue = DecodeTable[*Data++] << 18;
			nValue += DecodeTable[*Data++] << 12;
			strDecode += (nValue & 0x00FF0000) >> 16;
			OutByte++;
			if (*Data != '=')
			{
				nValue += DecodeTable[*Data++] << 6;
				strDecode += (nValue & 0x0000FF00) >> 8;
				OutByte++;
				if (*Data != '=')
				{
					nValue += DecodeTable[*Data++];
					strDecode += nValue & 0x000000FF;
					OutByte++;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过  
		{
			Data++;
			i++;
		}
	}
	return strDecode;
}


//以下是读写图片的调用代码：
bool ReadPhotoFile(std::basic_string<char> strFileName, std::string &strData)
{
	HANDLE hFile;
	hFile = CreateFileA(strFileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dFileSize = GetFileSize(hFile, NULL);
	char * pBuffer = new char[dFileSize + 1];

	if (pBuffer == NULL)
		return false;

	memset(pBuffer, 0, dFileSize);

	DWORD dReadSize(0);
	if (!ReadFile(hFile, pBuffer, dFileSize, &dReadSize, NULL))
	{
		delete[]pBuffer;
		CloseHandle(hFile);
		return false;
	}


	strData = "";
	strData = Encode((const char*)pBuffer, dReadSize);

	delete[]pBuffer;
	CloseHandle(hFile);
	return true;
}

bool WritePhotoFile(std::basic_string<char> strFileName, std::string &strData)
{
	HANDLE hFile;
	hFile = CreateFileA(strFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}


	int datalen(0);
	DWORD dwritelen(0);
	std::string strdcode = Decode(strData.data(), strData.size(), datalen);
	if (!WriteFile(hFile, strdcode.data(), datalen, &dwritelen, NULL))
	{
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);
	return true;
}

BOOL GetInternetConnectedState()
{
	DWORD   flags;//上网方式   
	BOOL   bOnline = TRUE;//是否在线    

	bOnline = InternetGetConnectedState(&flags, 0);
	if (bOnline)//在线     
	{
		if ((flags & INTERNET_CONNECTION_MODEM) == INTERNET_CONNECTION_MODEM)
		{
			//cout << "在线：拨号上网\n";
		}
		if ((flags & INTERNET_CONNECTION_LAN) == INTERNET_CONNECTION_LAN)
		{
			//cout << "在线：通过局域网\n";
		}
		if ((flags & INTERNET_CONNECTION_PROXY) == INTERNET_CONNECTION_PROXY)
		{
			//cout << "在线：代理\n";
		}
		if ((flags & INTERNET_CONNECTION_MODEM_BUSY) == INTERNET_CONNECTION_MODEM_BUSY)
		{
			//cout << "MODEM被其他非INTERNET连接占用\n";
		}
	}
	else
	{
		//cout << "不在线\n";
	}
	return bOnline;
}


bool GetBeginEndTimeStamps(CDateTimeCtrl &beginDateCtrl, CDateTimeCtrl &endDateCtrl, int &beginTime, int &endTime)
{
	COleDateTime beginOleDate, endOleDate;
	beginDateCtrl.GetTime(beginOleDate);
	beginOleDate.SetDateTime(beginOleDate.GetYear(), beginOleDate.GetMonth(), beginOleDate.GetDay(), 0, 0, 0);
	beginDateCtrl.SetTime(beginOleDate);

	endDateCtrl.GetTime(endOleDate);
	endOleDate.SetDateTime(endOleDate.GetYear(), endOleDate.GetMonth(), endOleDate.GetDay(), 23, 59, 59);
	endDateCtrl.SetTime(endOleDate);

	CTime  beginDate, endDate;
	beginDateCtrl.GetTime(beginDate);
	endDateCtrl.GetTime(endDate);
	beginTime = beginDate.GetTime();
	endTime = endDate.GetTime();
	
	return true;
}


//  拼接完整路径
CString SplicFullFilePath(CString strExeModuleName)
{
	// 提取当前路径
	// 准备写文件
	WCHAR strPath[MAX_PATH + 1] = { 0 };
	WCHAR * pTempPath;
	GetModuleFileName(NULL, strPath, MAX_PATH);
	pTempPath = strPath;
	while (wcschr(pTempPath, '\\'))
	{
		pTempPath = wcschr(pTempPath, '\\');
		pTempPath++;
	}
	*pTempPath = 0;

	CString strPathName;
	strPathName += strPath + strExeModuleName;
	return strPathName;
}