#ifdef _WIN32

#ifndef	__VIDEO_PLAYER_D3D_H__

#define __VIDEO_PLAYER_D3D_H__
#include <d3d9.h>
//#include "privatetypes.h"

class CVideoPlayerD3D
{
public:
	CVideoPlayerD3D();
	~CVideoPlayerD3D();

public:
	bool		InitPlayer(HWND hVideoWnd, int nVideoWidth, int nVideoHeight);
	bool		DrawYV12(unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int width, unsigned int height);
	bool		GetInitState();
	void		UnInitPlayer();

	//设置zoom区域
	void		SetZoomRect(int * pArray);

public:
	IDirect3D9 * m_pD3D;
	IDirect3DDevice9 * m_pd3dDevice;
	IDirect3DSurface9 * m_pd3dSurface;//D3D绘图用变量
	RECT m_rtViewport;//视频显示区域（要保持宽高比）

private:
	void DrawImage();
	HRESULT ResetDevice();

private:
	HWND	m_hHandle;
	int		m_iVideoWidth;
	int		m_iVideoHeight;
	bool    m_bInit;

	RECT	m_rcRect;
	
};



#endif//__VIDEO_PLAYER_D3D_H__

#endif//_WIN32