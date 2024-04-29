#include "pch.h"
#include "LogFile.h"
#include <stdio.h>
#include <time.h>
#include <string>

#pragma warning (disable:4996)	//warning:_CRT_SECURE_NO_WARNINGS 字符串安全警告

#ifdef _WIN32
#include <Windows.h>
#else
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#endif

typedef	 struct taglogtime
{
	int nYear;
	int nMon;
	int nDay;
	int wDay;
	int nHour;
	int nMin;
	int nSec;
	int mSec;
}logtime;

void getlogtime(logtime &ltime)
{
#ifdef _WIN32
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);				//得到系统时间   

	ltime.nYear = sysTime.wYear;
	ltime.nMon = sysTime.wMonth;
	ltime.nDay = sysTime.wDay;
	ltime.wDay = sysTime.wDayOfWeek;
	ltime.nHour = sysTime.wHour;
	ltime.nMin = sysTime.wMinute;
	ltime.nSec = sysTime.wSecond;
	ltime.mSec = sysTime.wMilliseconds;
#else
	struct timeval tval;
	gettimeofday(&tval, NULL);

	struct tm * local_time;
	local_time = localtime((time_t*)&tval.tv_sec);

	ltime.nYear = local_time->tm_year + 1900;
	ltime.nMon = local_time->tm_mon + 1;
	ltime.nDay = local_time->tm_mday;
	ltime.wDay = local_time->tm_wday;
	ltime.nHour = local_time->tm_hour;
	ltime.nMin = local_time->tm_min;
	ltime.nSec = local_time->tm_sec;
	ltime.mSec = tval.tv_usec / 1000;
#endif
}

void CLogFile::GetLogFileName(WCHAR szName[])
{
	WCHAR szTmp[128] = { 0 };

	logtime lgt;
	getlogtime(lgt);

	wsprintf(szTmp, _T("%d-%02d-%02d-%02d-%02d-%02d"),
		lgt.nYear, lgt.nMon, lgt.nDay,
		lgt.nHour, lgt.nMin, lgt.nSec);//"2010-09-21-09-23-34"

	CString path = GetAppdataPath();
	WCHAR *wPath = path.GetBuffer(path.GetLength());
	path.ReleaseBuffer();

	CString version = VERSION_INFO;
	WCHAR *wVersion = version.GetBuffer(version.GetLength());
	version.ReleaseBuffer();

	wsprintf(szName, _T("%sMeasure_%s_%s.log"), wPath, wVersion, szTmp);
}

int CLogFile::GetLogFileSizeM(FILE * pFile, int &iSize)
{
	if (NULL == pFile)
	{
		return 0;
	}

	int currentPos = ftell(pFile);			//取得当前文件指针位置,可能已经移动了文件指针
	fseek(pFile, 0, SEEK_END);					//移动到文件的结尾
	iSize = (ftell(pFile)) / (1024 * 1024);	//获得文件大小	单位 : M
	fseek(pFile, currentPos, SEEK_SET);		//恢复到原来的文件指针位置	

	return 1;
}

CLogFile::CLogFile()
{
	m_pFile = NULL;
	memset(m_szLogFileName, 0, LOG_NAME_LEN);

	GetLogFileName(m_szLogFileName);

	//	m_pFile = fopen(m_szLogFileName, "a+");
}

CLogFile::~CLogFile()
{
	if (NULL != m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}
void CLogFile::WriteLogFile(int nLogLevel, WCHAR* pLogFormat, ...)
{
	// 	if ( NULL == m_pFile )
	// 	{
	// 		return ;
	// 	}

	if (NULL != m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	m_pFile = _wfopen(m_szLogFileName, _T("a+"));
	if (NULL != m_pFile)
	{
		int iFileSize = 0;
		GetLogFileSizeM(m_pFile, iFileSize);
		if (iFileSize >= LOG_FILE_SIZE)				//产生新日志文件
		{
			GetLogFileName(m_szLogFileName);

			fclose(m_pFile);
			m_pFile = NULL;

			m_pFile = _wfopen(m_szLogFileName, _T("a+"));
		}

		WCHAR szLogText[1024] = { 0 };
		va_list argptr;
		int cnt = 0;

		va_start(argptr, pLogFormat);
		cnt = wvsprintf(szLogText, pLogFormat, argptr);
		va_end(argptr);

		logtime lgt;
		getlogtime(lgt);
		_wsetlocale(0, L"chs");
		fwprintf(m_pFile, _T("%d-%d-%d %d:%d:%d.%d		%s\n"), lgt.nYear, lgt.nMon, lgt.nDay, lgt.nHour, lgt.nMin, lgt.nSec, lgt.mSec, szLogText);

		fclose(m_pFile);
		m_pFile = NULL;
	}
}

#ifndef _WIN32
OTrace::OTrace()
{
	pthread_mutex_init(&m_mtx, NULL);
}
OTrace::~OTrace()
{
	pthread_mutex_destroy(&m_mtx);
}

void OTrace::LockFile()
{
	pthread_mutex_lock(&m_mtx);
}

void OTrace::UnLockFile()
{
	pthread_mutex_unlock(&m_mtx);
}
#else
OTrace::OTrace()
{
	InitializeCriticalSection(&m_mtx);
}
OTrace::~OTrace()
{
	DeleteCriticalSection(&m_mtx);
}

void OTrace::LockFile()
{
	EnterCriticalSection(&m_mtx);
}

void OTrace::UnLockFile()
{
	LeaveCriticalSection(&m_mtx);
}
#endif

void OTrace::WriteLogFile(WCHAR * fmt, ...)
{
	LockFile();
	WCHAR s[256] = { 0 };

	va_list argptr;
	int cnt;
	va_start(argptr, fmt);
	cnt = wvsprintf(s, fmt, argptr);
	va_end(argptr);

	m_log.WriteLogFile(1, s, wcslen(s));

	UnLockFile();
}

OTrace _classOTrace;