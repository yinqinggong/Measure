#ifndef __HWND_H__
#define __HWND_H__

#ifdef _WIN32
#include "videoplayerd3d.h"
#endif

typedef void(*PlayWndCallBack)(unsigned char * pY, unsigned char *pU, unsigned char *pV, unsigned int nHeight, unsigned int nWidth, void * pUser);

class CHwnd
{
public:
	CHwnd();
	~CHwnd();
public:
	void InitHwnd(HWND nHwnd, PlayWndCallBack cb = NULL, void *pUser = NULL);
	void UnitHwnd();
	void PlayVideo(unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int nHeight, unsigned int nWidth);
	HWND  GetHwnd();
private:
	PlayWndCallBack m_cbWnd;
	void *m_pUser;
	HWND m_nHwnd;
	CVideoPlayerD3D m_d3dPlayer;
};
#endif