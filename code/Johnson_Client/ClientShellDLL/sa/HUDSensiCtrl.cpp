#include "StdAfx.h"
#include "SAHUDMgr.h"
#include "HUDSensiCtrl.h"
#include "SAPlayerStats.h"
#include "SAInterfaceMgr.h"
#include "GameClientShell.h"

CHUDSensiCtrl::CHUDSensiCtrl()
{
	m_UpdateFlags		= kHUDMissionText;
	m_eLevel			= kHUDRenderNone;
	m_bVisible			= LTFALSE;		
	m_pSensitivityText	= LTNULL;		
	m_fSensiDurationTime	= 0.0f;
	m_fSensiTextAlpha		= 0.0f;
}

CHUDSensiCtrl::~CHUDSensiCtrl()
{
}

LTBOOL CHUDSensiCtrl::Init()
{	
	// 마우스 민감도 Text ====================================================================
	m_pSensitivityText = new CSAD3DTextEx();		
	m_pSensitivityText->CreateD3DText( 256, 20, true );
	m_pSensitivityText->SetTextColor( RGB(200,200,200), RGB(0,0,0) );
	m_pSensitivityText->SetTransparent(true);
	m_pSensitivityText->DrawD3DText( "", 0 );
	// ========================================================================================	

	this->ScreenDimChanged();
	return LTTRUE;
}

VOID 
CHUDSensiCtrl::Term()
{
	SAFE_DELETE( m_pSensitivityText );
}

VOID
CHUDSensiCtrl::Update()
{
}


VOID 
CHUDSensiCtrl::Render()
{
	if( IsVisible() && m_pSensitivityText )
	{
		m_pSensitivityText->Render();

		if( m_fSensiDurationTime > 0.0f )
		{
			m_fSensiDurationTime -= g_pLTClient->GetFrameTime();
		}
		else 
		{
			if(m_fSensiTextAlpha > 0.0f)
			{
				m_fSensiTextAlpha -= g_pLTClient->GetFrameTime() * 255.0f;
				if(m_fSensiTextAlpha < 0.0f) m_fSensiTextAlpha = 0.0f;
				m_pSensitivityText->SetAlpha( (uint8)m_fSensiTextAlpha );
			}
			else
			{
				Show(LTFALSE);
			}
		}
	}
}

VOID 
CHUDSensiCtrl::ScreenDimChanged()
{	
}

VOID
CHUDSensiCtrl::CalculateSensitivity(LTBOOL bLow_Or_High)
{
	Show(LTTRUE);
	m_fSensiDurationTime	= 1.0f;
	m_fSensiTextAlpha		= 255.0f;
	m_pSensitivityText->SetAlpha( (uint8)m_fSensiTextAlpha );

	char szTempText[128] = {LTNULL};
	int iCurrentSensi = g_pProfileMgr->GetCurrentProfile()->m_nSensitivity;
	
	if(bLow_Or_High) // Low
	{
		iCurrentSensi--;
		if(iCurrentSensi<0) iCurrentSensi = 0;
	}
	else // High
	{
		iCurrentSensi++;
		if(iCurrentSensi>100) iCurrentSensi = 100;
	}
	sprintf(szTempText, "Current MouseSensitivity: %d", iCurrentSensi);
	m_pSensitivityText->DrawD3DText(szTempText, strlen(szTempText), DT_LEFT, D3D_TEXT_SHADOW, false);
	
	// Dim설정 
	LTIntPt	BasePos;	
	BasePos.x = (int)(g_pInterfaceResMgr->GetScreenWidth() * 0.5f - (m_pSensitivityText->GetTextLength()/2));
	BasePos.y = (int)(g_pInterfaceResMgr->GetScreenHeight() * 0.8f);
	m_pSensitivityText->SetPosition( BasePos.x, BasePos.y );
	
	g_pProfileMgr->GetCurrentProfile()->m_nSensitivity = iCurrentSensi;
	g_pProfileMgr->GetCurrentProfile()->ApplyMouse();			// 적용 
	
	sprintf(szTempText, "%d", iCurrentSensi);
	g_pPopupOption->m_pEBSensi->SetText(szTempText);
	g_pPopupOption->m_pScrollSensi->SetPosXRatio(((float)g_pPopupOption->m_pEBSensi->GetInt())/100);
	g_pPopupOption->m_pFmGauge_Sensi->SetSize( ((g_pPopupOption->m_pScrollSensi->GetPos().x+8)-
												(g_pPopupOption->m_pFmGauge_Sensi->GetBasePos().x+2)), 15);

}
