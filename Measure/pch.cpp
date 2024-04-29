// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。
void WaitAndTermThread(HANDLE hThread, DWORD dwTimeSec)
{
	if (hThread != NULL && hThread != INVALID_HANDLE_VALUE)
	{
		try
		{
			DWORD dwRet = WaitForSingleObject(hThread, (dwTimeSec == INFINITE) ? INFINITE : dwTimeSec);
			if (dwRet == WAIT_OBJECT_0)
			{
				return;
			}
			else if (dwRet == WAIT_TIMEOUT)
			{
				::TerminateThread(hThread, -1);
				return;
			}
			else if (dwRet == WAIT_FAILED)
			{
				return;
			}
		}
		catch (...)
		{
			//WriteLog(_T("---------------------结束线程捕获异常"));
		}
	}
}

BOOL FolderExist(CString strPath)   // 检查目录是否存在，存在为真，不存在为假
{
	WIN32_FIND_DATA   wfd;
	BOOL rValue = FALSE;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = TRUE;
	}
	FindClose(hFind);
	return rValue;
}

CString GetCurrentPath()
{
	WCHAR szCurrentPath[MAX_PATH];
	memset(szCurrentPath, 0, MAX_PATH);
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);

	CString  strFolderPath = (WCHAR*)szCurrentPath;
	strFolderPath = strFolderPath.Left(strFolderPath.ReverseFind('\\'));
	strFolderPath += "\\";

	return strFolderPath;
}

CString GetAppdataPath()
{
	CString strDir = GetCurrentPath();
	strDir.Append(_T("\\logs"));
	if (!FolderExist(strDir))
	{
		BOOL bRet = CreateDirectory(strDir, NULL);
		if (!bRet)
		{
			return _T("");
		}
	}

	strDir.Append(_T("\\"));

	return strDir;
	/*#ifdef DEBUG
		return GetCurrentPath();
	#else
		WCHAR	szPath[MAX_PATH] = { 0 };
		HRESULT hr = SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, 1);
		if (SUCCEEDED(hr))
		{
			CString strDir = (WCHAR*)szPath;
			strDir.Append(_T("\\Measure"));
			if (!FolderExist(strDir))
			{
				BOOL bRet = CreateDirectory(strDir, NULL);
				if (!bRet)
				{
					return _T("");
				}
			}

			strDir.Append(_T("\\"));

			return strDir;
		}
		return _T("");
	#endif*/
}