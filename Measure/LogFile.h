#pragma once
#include <stdio.h>

#ifndef _WIN32
#include <pthread.h>
#else
#include <Windows.h>
#endif

#define LOG_NAME_LEN 256
#define	LOG_FILE_SIZE		20		//日志文件最大值 单位 M

#define WriteLog		_classOTrace.WriteLogFile

class CLogFile
{
public:
	CLogFile();
	virtual ~CLogFile();

	void WriteLogFile(int nLogLevel, WCHAR* pLogFormat, ...);

private:
	void GetLogFileName(WCHAR szName[]);
	int    GetLogFileSizeM(FILE * pFile, int &iSize);
private:
	WCHAR m_szLogFileName[LOG_NAME_LEN];

	FILE	* m_pFile;
};

class OTrace
{
public:
	OTrace();
	virtual ~OTrace();
	void WriteLogFile(WCHAR * fmt, ...);

private:
	void LockFile();
	void UnLockFile();

	CLogFile m_log;

#ifndef _WIN32
	pthread_mutex_t m_mtx;
#else
	CRITICAL_SECTION m_mtx;
#endif	

};

extern OTrace _classOTrace;