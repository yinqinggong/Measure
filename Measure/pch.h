// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"


class CTempLock
{
public:
	CTempLock(CCriticalSection* pCS)
	{
		if (pCS != NULL)
		{
			pCS->Lock();
			m_pCS = pCS;
		}
	}
	~CTempLock()
	{
		if (m_pCS != NULL)
		{
			m_pCS->Unlock();
			m_pCS = NULL;
		}
	}
private:
	CCriticalSection* m_pCS;
};
const CString  VERSION_INFO = _T("1.0");

void WaitAndTermThread(HANDLE hThread, DWORD dwTimeSec = 5);

CString GetCurrentPath();//获取当前路径
CString GetAppdataPath();//获取appdata路径




#endif //PCH_H
