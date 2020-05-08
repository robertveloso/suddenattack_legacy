// HUDGameTime.cpp: implementation of the CHUDGameTime class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HUDGameTime.h"

//->Source 추가 Start.[추가되어진 요일:11.12.2003] Function name:-
#include "InterfaceMgr.h"
//->Source 추가 End.  [추가       사람:monk77]  [#|#] 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHUDGameTime::CHUDGameTime()
{

}

CHUDGameTime::~CHUDGameTime()
{
	
}

void CHUDGameTime::DrawLevelTimeRemain()
{
	char pBuff[64];

	LTFLOAT fLevelTime = 5.0f * 60.0f;

	if ( m_fServerTime != -1.0f )
	{
		// servertime은 서버에서의 경과한 시간임.
	
		float fT1 = g_pLTClient->GetGameTime() - g_pGameClientShell->m_fLevelStartTime;
		if ( fT1 < m_fServerTime )
			g_pGameClientShell->m_fLevelStartTime -= (m_fServerTime - fT1);

		m_fServerTime = -1.0f;
	}	

	LTFLOAT fTime = (g_pGameClientShell->m_fLevelStartTime + fLevelTime) - g_pLTClient->GetGameTime();

	int nMin = (int) (fTime / 60.0f);
	int nSec = (int) (fTime - nMin * 60.0f);
	sprintf(pBuff, "Time: %d:%d", nMin, nSec);

	m_TimeTxt.Show(0, pBuff, 581, 8, argbBlack);
	m_TimeTxt.Update();	
	m_TimeTxt.Render();
}
