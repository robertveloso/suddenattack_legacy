// HUDGameInfo.cpp: implementation of the CHUDGameInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HUDGameInfo.h"
#include "InterfaceMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHUDGameInfo::CHUDGameInfo()
{
	m_UpdateFlags = kHUDFrame;
	m_lpFont = NULL;
}

CHUDGameInfo::~CHUDGameInfo()
{
	
}

LTBOOL CHUDGameInfo::Init()
{

	UpdateLayout();

	return LTTRUE;
}

void CHUDGameInfo::Term()
{

}

void CHUDGameInfo::Render()
{
	if (m_lpFont)
	{
		RECT rc;
		SetRect(&rc, m_BasePos.x,m_BasePos.y,10,10);
		m_lpFont->Begin();
		m_lpFont->DrawText( m_sRemainTime.c_str(), -1, &rc, DT_SINGLELINE | DT_CALCRECT, 0 );
		m_lpFont->DrawText( m_sRemainTime.c_str(), -1, &rc, DT_SINGLELINE, 0xFFFFFFFF);
		m_lpFont->End();
	}
}

void CHUDGameInfo::Update()
{
	char pBuff[64];

	LTFLOAT fLevelTime = 5.0f * 60.0f;

/*
	if ( m_fServerTime != -1.0f )
	{
		// servertime은 서버에서의 경과한 시간임.
	
		float fT1 = g_pLTClient->GetGameTime() - g_pGameClientShell->m_fLevelStartTime;
		if ( fT1 < m_fServerTime )
			g_pGameClientShell->m_fLevelStartTime -= (m_fServerTime - fT1);

		m_fServerTime = -1.0f;
	}	

	LTFLOAT fTime = (g_pGameClientShell->m_fLevelStartTime + fLevelTime) - g_pLTClient->GetGameTime();
*/
	float fTime = g_pLTClient->GetGameTime() - m_fServerTime;

	int nMin = (int) (fTime / 60.0f);
	int nSec = (int) (fTime - nMin * 60.0f);
	sprintf(pBuff, m_szTimeFormat, nMin, nSec);

	m_sRemainTime = std::string(pBuff);
//	m_TimeTxt.Show(0, pBuff, 581, 8, argbBlack);
//	m_TimeTxt.Update();	
//	m_TimeTxt.Render();
}

void CHUDGameInfo::UpdateLayout()
{
	char *pTag = "HUDGameInfo";
	m_BasePos	= g_pLayoutMgr->GetPoint(pTag,"BasePos");
	g_pLayoutMgr->GetString(pTag, "TimeFormat", m_szTimeFormat, 128);


	char szFontFace[128];
	uint8 nFontSize;
	g_pLayoutMgr->GetString(pTag, "TimeFontFace", szFontFace, 128);
	nFontSize = g_pLayoutMgr->GetInt(pTag,"TimeFontSize");

	m_lpFont = g_pInterfaceResMgr->CreateDXFont(szFontFace, nFontSize, 0x1);

	m_fServerTime = g_pLTClient->GetGameTime();
}
