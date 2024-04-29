#include "pch.h"
//#ifdef _WIN32

#define	THISFILE "videoplayerd3d.cpp"
#include "videoplayerd3d.h"

//#define WriteLog

#pragma comment(lib, "d3d9.lib")

CVideoPlayerD3D::CVideoPlayerD3D()
{
	m_pD3D = NULL;
	m_pd3dDevice = NULL;
	m_pd3dSurface = NULL;//D3D绘图用变量

	m_hHandle = 0;
	m_iVideoWidth = 0;
	m_iVideoHeight = 0;

	m_bInit = false;
}

CVideoPlayerD3D::~CVideoPlayerD3D()
{
	UnInitPlayer();
}

bool CVideoPlayerD3D::InitPlayer(HWND hVideoWnd, int nVideoWidth, int nVideoHeight)
{
	char szBuffer[128] = {0};
	//sprintf(szBuffer, "InitPlayer width = %d, height = %d", nVideoWidth, nVideoHeight);
	OutputDebugStringA(szBuffer);

	m_bInit = false;

	m_hHandle = hVideoWnd;
	m_iVideoWidth = nVideoWidth;
	m_iVideoHeight = nVideoHeight;

	m_rcRect.left = 0;
	m_rcRect.top = 0;
	m_rcRect.right = nVideoWidth;
	m_rcRect.bottom = nVideoHeight;

	//WriteLog("%s : Line %d, Init Player VideoHeight = %d, VideoWidth = %d", THISFILE, __LINE__, nVideoHeight, nVideoWidth);

	if( m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
	if( m_pd3dDevice != NULL )
	{
		m_pd3dDevice->Release();
		m_pd3dDevice = NULL;
	}
	if( m_pd3dSurface != NULL )
	{
		m_pd3dSurface->Release();
		m_pd3dSurface = NULL;
	}

	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( m_pD3D == NULL )
	{
		//sprintf(szBuffer, "Direct3DCreate9 Failed, ErrorCode = %d!", GetLastError());
		//OutputDebugStringA(szBuffer);
		//WriteLog("Error %s %d Create D3D Failed!", THISFILE, __LINE__ );
		goto errorcode;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferHeight = m_iVideoHeight;
	d3dpp.BackBufferWidth = m_iVideoWidth;
	//d3dpp.hDeviceWindow = (HWND)m_hHandle;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER ;

	RECT    rtClient;
	::GetClientRect( (HWND)hVideoWnd, &rtClient);
	m_rtViewport = rtClient;

	HRESULT hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)m_hHandle, 
											/*D3DCREATE_SOFTWARE_VERTEXPROCESSING*/D3DCREATE_HARDWARE_VERTEXPROCESSING,
											&d3dpp, &m_pd3dDevice );
	if ( FAILED(hr) )
	{		
		//sprintf(szBuffer, "m_pD3D->CreateDevice Failed, ErrorCode = %x!", hr);
		//OutputDebugStringA(szBuffer);

		//WriteLog("Error %s : %d Use D3DCREATE_HARDWARE_VERTEXPROCESSING Failed hr = 0x%x", THISFILE, __LINE__, hr);
		hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)m_hHandle, 
														D3DCREATE_SOFTWARE_VERTEXPROCESSING,
														&d3dpp, &m_pd3dDevice 
													  );
	}
	if( FAILED( hr ) )
	{
		//WriteLog("Error %s : %d Create Device Failed hr = 0x%x", THISFILE, __LINE__, hr);
		goto errorcode;;
	}

	hr = m_pd3dDevice->CreateOffscreenPlainSurface(
											nVideoWidth, nVideoHeight,
											(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
											D3DPOOL_DEFAULT,
											&m_pd3dSurface,
											NULL );
	if( FAILED( hr ) )
	{		
		//sprintf(szBuffer, "m_pd3dDevice->CreateOffscreenPlainSurface, ErrorCode = %d!", GetLastError());
		//OutputDebugStringA(szBuffer);

		//WriteLog( "Error %s : %d Create Offscreen Failed hr = 0x%x", THISFILE, __LINE__, hr );
		goto errorcode;
	}

	m_bInit = true;

	return m_bInit;

errorcode:
	m_bInit = false;
	Sleep(1);
	return false;
}

void CVideoPlayerD3D::DrawImage()
{
	char szBuffer[256] = {0};

	if (m_pd3dDevice != NULL)
	{
		m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

		m_pd3dDevice->BeginScene();

		IDirect3DSurface9 * pBackBuffer = NULL;

		m_pd3dDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
		
		m_pd3dDevice->StretchRect( m_pd3dSurface, /*NULL*/&m_rcRect, pBackBuffer, NULL, D3DTEXF_LINEAR );
		//sprintf( szBuffer,"rcRect left = %d, top = %d, right = %d, bottom = %d", m_rcRect.left, m_rcRect.top, m_rcRect.right, m_rcRect.bottom);
		//OutputDebugStringA(szBuffer);

		m_pd3dDevice->EndScene();

		HRESULT hr = m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//sprintf( szBuffer, "%s : %d line Present Failed 0x%x", THISFILE, __LINE__, hr );
		//OutputDebugStringA( szBuffer );

		if ( FAILED(hr) )
		{
			ResetDevice();
		}
		
		if ( pBackBuffer != NULL )
		{
			pBackBuffer->Release();
		}
	}
}

bool CVideoPlayerD3D::DrawYV12(unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int width, unsigned int height)
{
	if( m_pd3dSurface == NULL )
	{
		//WriteLog( "Error %s : %d Param is NULL", THISFILE, __LINE__ );
		return false;
	}

	D3DLOCKED_RECT d3d_rect;
	HRESULT hr;
	/*do
	{
	hr = m_pd3dSurface->LockRect( &d3d_rect, NULL, D3DLOCK_DONOTWAIT );
	}while ( D3DERR_WASSTILLDRAWING == hr );*/
	if( FAILED( m_pd3dSurface->LockRect( &d3d_rect, NULL, D3DLOCK_DONOTWAIT ) ) )
	{
		OutputDebugStringA("lock rect failed");
		return false;
	}

	int nX = 0;
	int nW = m_iVideoWidth;
	int nH = m_iVideoHeight;
	int nBufSize = m_iVideoWidth * m_iVideoHeight * sizeof(char);

	const int stride = d3d_rect.Pitch;
	char* to = (char *)d3d_rect.pBits;
	unsigned char* from = pY;

	if(m_iVideoWidth != stride)
	{
		for (nX = 0; nX < m_iVideoHeight; nX++) 
		{
			memcpy(to, from, m_iVideoWidth);
			to += stride;
			from += m_iVideoWidth;
		}
	}
	else
	{
		memcpy( d3d_rect.pBits, from, nBufSize);
		to += nBufSize;
		from += nBufSize;
	}

	nBufSize /= 4;
	nW /= 2;
	nH /= 2;

	from = pU;
	if(m_iVideoWidth != stride/2)
	{
		for (nX = 0; nX < nH; nX++)
		{
			memcpy(to, from, nW);
			to += stride/2;
			from += nW;
		}
	}
	else
	{
		memcpy( d3d_rect.pBits, from, nBufSize);		
		to += nBufSize;
		from += nBufSize;
	}

	from = pV;
	if(m_iVideoWidth != stride/2)
	{
		for (nX = 0; nX < nH; nX++)
		{
			memcpy(to, from, nW);
			to += stride/2;
			from += nW;
		}
	} 
	else
	{
		memcpy( d3d_rect.pBits, from, nBufSize );
	}
	
	hr = m_pd3dSurface->UnlockRect();
	if( FAILED( hr ) )
	{
		return false;
	}

	DrawImage();

	return false;
}

bool		CVideoPlayerD3D::GetInitState()
{
	return m_bInit;
}

void		CVideoPlayerD3D::UnInitPlayer()
{
	if( m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
	if( m_pd3dDevice != NULL )
	{
		m_pd3dDevice->Release();
		m_pd3dDevice = NULL;
	}
	if( m_pd3dSurface != NULL )
	{
		m_pd3dSurface->Release();
		m_pd3dSurface = NULL;
	}
}

HRESULT CVideoPlayerD3D::ResetDevice()
{
	// Check device state
	HRESULT hr = m_pd3dDevice->TestCooperativeLevel() ;

	// Device can be reset now
	if (SUCCEEDED(hr) || hr == D3DERR_DEVICENOTRESET)
	{
		D3DPRESENT_PARAMETERS d3dpp; 
		ZeroMemory( &d3dpp, sizeof(d3dpp) );
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.BackBufferHeight = m_iVideoHeight;
		d3dpp.BackBufferWidth = m_iVideoWidth;

		// Release resource allocated as D3DPOOL_DEFAULT
		// Reset device
		hr = m_pd3dDevice->Reset(&d3dpp) ;
		if ( SUCCEEDED(hr) )
		{
			//WriteLog("Warning %s : %d Reset DeviceLost Ok!", THISFILE, __LINE__);	
		}
		else// Reset device failed, show error box
		{
			m_bInit = false;	//重新初始化
			//WriteLog("Warning %s : %d Reset DeviceLost Failed hr = %x!", THISFILE, __LINE__, hr);
			Sleep(1);
		}
	}
	// Device is still in lost state, wait
	else if (hr == D3DERR_DEVICELOST)
	{
		//WriteLog("Warning %s : %d DeviceLost and Can not be Reset Now!", THISFILE, __LINE__);
		Sleep(1) ;
	}
	else// Other error, Show error box
	{
		//WriteLog( "Warning %s : %d TestCooperativeLevel Faile hr = 0x%x!", THISFILE, __LINE__, hr );
		Sleep(1);
	}

	return hr ;
}

/*
nArray[0]:left
nArray[1]:top 
nArray[2]:right 
nArray[3]:bottom
nArray[4]:窗口高度
nArray[5]:窗口宽度
*/
void CVideoPlayerD3D::SetZoomRect(int * pArray)
{
	if ( (0 == pArray[0]) && (0 == pArray[1]) && (0 == pArray[3]) && (0 == pArray[3]) )
	{
		m_rcRect.top = 0;
		m_rcRect.left = 0;
		m_rcRect.right = m_iVideoWidth;
		m_rcRect.bottom = m_iVideoHeight;
	}
	else
	{
		float iWindowWidth = pArray[5];
		float iWindowHeight = pArray[4];

		m_rcRect.left = ( (float)pArray[0]/iWindowWidth )*(float)m_iVideoWidth;
		m_rcRect.top = ( (float)pArray[1]/iWindowHeight )*(float)m_iVideoHeight;
		
		m_rcRect.right = ( (float)pArray[2]/iWindowWidth )*(float)m_iVideoWidth;
		m_rcRect.bottom = ( (float)pArray[3]/iWindowHeight )*(float)m_iVideoHeight;

		if ( (m_rcRect.left%2) != 0 )
		{
			m_rcRect.left = m_rcRect.left + 1;
		}

		if ( (m_rcRect.top%2) != 0 )
		{
			m_rcRect.top = m_rcRect.top + 1;
		}

		if ( (m_rcRect.right%2) != 0 )
		{
			m_rcRect.right = m_rcRect.right + 1;
		}

		if ( (m_rcRect.bottom%2) != 0 )
		{
			m_rcRect.bottom = m_rcRect.bottom + 1;
		}
	}
}

//#endif //_WIN32