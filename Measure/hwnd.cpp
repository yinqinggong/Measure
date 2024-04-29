#include "pch.h"
#include "hwnd.h"

CHwnd::CHwnd()
{
	m_nHwnd = 0;
	m_cbWnd = NULL;
	m_pUser = NULL;
}

CHwnd::~CHwnd()
{

}

void CHwnd::InitHwnd(HWND nHwnd, PlayWndCallBack cb, void *pUser)
{
	m_nHwnd = nHwnd;
	m_cbWnd = cb;
	m_pUser = pUser;
}

void CHwnd::UnitHwnd()
{

}
void CHwnd::PlayVideo(unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int nHeight, unsigned int nWidth)
{
	if (m_nHwnd <= 0)
	{
		return;
	}

	if (m_cbWnd != NULL && m_pUser != NULL)
	{
		m_cbWnd(pY, pU, pV, nHeight, nWidth, m_pUser);
		return;
	}

	if ( !m_d3dPlayer.GetInitState() )
	{
		m_d3dPlayer.InitPlayer(m_nHwnd, nWidth, nHeight);
	}

	if ( m_d3dPlayer.GetInitState() )
	{
		m_d3dPlayer.DrawYV12(pY, pU, pV, nWidth, nHeight);
	}
}

HWND CHwnd::GetHwnd()
{
	return m_nHwnd;
}