// ----------------------------------------------------------------------- //
//
// MODULE  : HUDHealth.cpp
//
// PURPOSE : HUDItem to display player health
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "SAHUDMgr.h"
#include "PlayerStats.h"
#include "InterfaceMgr.h"
#include "HUDHealth.h"

extern CGameClientShell* g_pGameClientShell;
LTFLOAT g_fTempWidth = 0.0f;
//******************************************************************************************
//**
//** HUD Health display
//**
//******************************************************************************************

#define DAMAGED_NOTIFY_ELAPSE	0.7f

CHUDHealth::CHUDHealth()
{
	m_UpdateFlags	= kHUDHealth | kHUDArmor | kHUDAir;
	m_eLevel		= kHUDRenderGameOver;
	// NOVICE ---------------------------------------------------------------------------
	m_nLastHP		= 0;
	m_nLastAP		= 0;
	m_bHPDamaged	= LTFALSE;
	m_bAPDamaged	= LTFALSE;

	m_fLastHPNotifiedTime	= 0.0f;
	m_fLastAPNotifiedTime	= 0.0f;
	// ----------------------------------------------------------------------------------
	m_nLastAir		= 0;
	m_bAirDamaged	= LTFALSE;
	m_fLastAirNotifiedTime = 0.0f;
}

LTBOOL CHUDHealth::Init()
{
	UpdateLayout();

//GameHI Hud [2004.04.02]
	g_pDrawPrim->SetRGBA(&m_GameHiArmorPoly,argbWhite);
	g_pDrawPrim->SetRGBA(&m_GameHiHealthPoly,argbWhite);
	g_pDrawPrim->SetRGBA(&m_AirPoly[0],argbWhite);
	g_pDrawPrim->SetRGBA(&m_AirPoly[1],argbWhite);

	m_GameHiArmorIcon  = g_pInterfaceResMgr->GetTexture("SA_Interface\\Hud\\Bar\\Armor_bar.dtx");
	m_GameHiHealthIcon = g_pInterfaceResMgr->GetTexture("SA_Interface\\Hud\\Bar\\health_bar.dtx");
	m_AirIcon[0]	   = g_pInterfaceResMgr->GetTexture("SA_Interface\\Hud\\Bar\\o2_bar1.dtx");
	m_AirIcon[1]	   = g_pInterfaceResMgr->GetTexture("SA_Interface\\Hud\\Bar\\o2_bar2.dtx");

	SetupQuadUVs(m_GameHiArmorPoly, m_GameHiArmorIcon, 0.0f, 0.0f, 1.0f, 1.0f);
	SetupQuadUVs(m_GameHiHealthPoly, m_GameHiHealthIcon, 0.0f, 0.0f, 1.0f, 1.0f);

	SetupQuadUVs(m_AirPoly[0], m_AirIcon[0], 0.0f, 0.0f, 1.0f, 1.0f);
	SetupQuadUVs(m_AirPoly[1], m_AirIcon[1], 0.0f, 0.0f, 1.0f, 1.0f);
	
	char str[128];

	for(int i=0;i<10;i++)
	{	
		sprintf(str , "SA_Interface\\Hud\\Bar\\Num_s%d.dtx" , i);
		m_hNumber[i] = g_pInterfaceResMgr->GetTexture(str);
	}

	for( i=0;i<6;i++)
	{	
		g_pDrawPrim->SetRGBA(&m_NumPoly[i],argbWhite);
		SetupQuadUVs(m_NumPoly[i], m_hNumber[0], 0.0f,0.0f,1.0f,1.0f);
		m_nNum[i] = -1;
	}

//GameHI Hud [2004.04.02]
	
	return LTTRUE;
}

void CHUDHealth::Term()
{
}

void CHUDHealth::RenderDamaged( HTEXTURE hTexture, LTPoly_GT4* pPoly, EN_DAMAGED_HUD edhType )
{
	int		nColor	= 0;
	LTFLOAT fColor	= 0.0f;

	LTFLOAT* pfLastNotifiedTime = LTNULL;

	switch ( edhType )
	{
	case EDH_HP:
		{
			m_fLastHPNotifiedTime += g_pLTClient->GetFrameTime();
			pfLastNotifiedTime = &m_fLastHPNotifiedTime;
		}
		break;
	case EDH_AP:
		{
			m_fLastAPNotifiedTime += g_pLTClient->GetFrameTime();
			pfLastNotifiedTime = &m_fLastAPNotifiedTime;
		}
		break;
	case EDH_AIR:
		{
			m_fLastAirNotifiedTime += g_pLTClient->GetFrameTime();
			pfLastNotifiedTime = &m_fLastAirNotifiedTime;
		}
		break;
	default:
		break;
	}

	g_pDrawPrim->SetTexture( hTexture );
	
	if ( *pfLastNotifiedTime < DAMAGED_NOTIFY_ELAPSE )
	{
		fColor = *pfLastNotifiedTime * 255.0f / DAMAGED_NOTIFY_ELAPSE;
		nColor = (int)fColor;
		nColor = 0xFFFF0000 | nColor << 8 | nColor;
		
		g_pDrawPrim->SetRGBA( pPoly, nColor );
	}
	else
	{
		*pfLastNotifiedTime = 0.0f;
		g_pDrawPrim->SetRGBA( pPoly, 0xFFFFFFFF );

		switch ( edhType )
		{
		case EDH_HP:	m_bHPDamaged  = LTFALSE;	break;
		case EDH_AP:	m_bAPDamaged  = LTFALSE;	break;
		case EDH_AIR:	m_bAirDamaged = LTFALSE;	break;
		default:	break;
		}
	}

	g_pDrawPrim->DrawPrim( pPoly );
}

void CHUDHealth::Render()
{
	if( g_pHUDPlayerState->IsAttachBarVisible() ) return;
	//GameHI Hud [2004.04.02]

	g_pDrawPrim->SetTexture(m_GameHiArmorIcon);
	g_pDrawPrim->DrawPrim(&m_GameHiArmorPoly);

	g_pDrawPrim->SetTexture(m_GameHiHealthIcon);
	g_pDrawPrim->DrawPrim(&m_GameHiHealthPoly);


	// NOVICE ---------------------------------------------------------------------------
	// 빨간색으로 만들고, 서서히 없애기
	if ( m_bHPDamaged ) 
	{
		RenderDamaged( m_GameHiHealthIcon, &m_GameHiHealthPoly, EDH_HP );
	}

	if ( m_bAPDamaged )
	{
		RenderDamaged( m_GameHiArmorIcon, &m_GameHiArmorPoly, EDH_AP );
	}

	
	// ----------------------------------------------------------------------------------

	for(int i=0;i<6;i++)
	{	
		if(m_nNum[i] >= 0)
		{		
			g_pDrawPrim->SetTexture(m_hNumber[m_nNum[i]]);
			g_pDrawPrim->DrawPrim(&m_NumPoly[i]);			
		}
	}

	
	//[SPIKE] Air	=================================================
	if(m_nLastAir < 100)
	{
		g_pDrawPrim->SetTexture(m_AirIcon[0]);
		g_pDrawPrim->DrawPrim(&m_AirPoly[0]);
		if (m_AirIcon[0]  && m_nLastAir == 0)
		{
			RenderDamaged( m_AirIcon[0], &m_AirPoly[0], EDH_AIR );
		}
		else
		{
			g_pDrawPrim->SetRGBA( &m_AirPoly[0], 0xFFFFFFFF );
		}		
		
		g_pDrawPrim->SetXYWH(&m_AirPoly[1],
							(LTFLOAT)m_AirBasePos[1].x*g_pInterfaceResMgr->GetXRatio(),
							(LTFLOAT)m_AirBasePos[1].y*g_pInterfaceResMgr->GetYRatio(), 
							g_fTempWidth*g_pInterfaceResMgr->GetXRatio(),
							(LTFLOAT)m_AirSize[1].y*g_pInterfaceResMgr->GetYRatio());

		g_pDrawPrim->SetTexture(m_AirIcon[1]);
		g_pDrawPrim->DrawPrim(&m_AirPoly[1]);
		
	/*	for(i=6; i<9; i++)
		{
			if(m_nNum[i] >= 0)
			{		
				g_pDrawPrim->SetTexture(m_hNumber[m_nNum[i]]);
				g_pDrawPrim->DrawPrim(&m_NumPoly[i]);			
			}
		}*/
	}
	//================================================================
}

void CHUDHealth::Update()
{
	if (m_bUseHealthText)
	{
		int temp = g_pPlayerStats->GetHealth();
		int n100 , n10 , n1;

		n100 = temp / 100;
		n10  = (temp % 100) / 10;
		n1   = (temp % 100) % 10 ;

		if(n100 == 0) m_nNum[0] = -1;
		else m_nNum[0] = n100;

		if(n100 == 0 && n10 ==0) m_nNum[1] = -1;
		else m_nNum[1] = n10;
	
		m_nNum[2] = n1;

		temp = g_pPlayerStats->GetArmor();

		n100 = temp / 100;
		n10  = (temp % 100) / 10;
		n1   = (temp % 100) % 10 ;

		if(n100 == 0) m_nNum[3] = -1;
		else m_nNum[3] = n100;

		if(n100 == 0 && n10 ==0) m_nNum[4] = -1;
		else m_nNum[4] = n10;
	
		m_nNum[5] = n1;

		// AirPoint ===================================
		temp = g_pPlayerStats->GetAirPoint();

		g_fTempWidth = (LTFLOAT)temp / 100.0f;
		g_fTempWidth = 140 * g_fTempWidth;

	/*	n100 = temp / 100;
		n10  = (temp % 100) / 10;
		n1   = (temp % 100) % 10 ;

		if(n100 == 0) m_nNum[6] = -1;
		else m_nNum[6] = n100;

		if(n100 == 0 && n10 ==0) m_nNum[7] = -1;
		else m_nNum[7] = n10;
	
		m_nNum[8] = n1;*/


		//==============================================

		// NOVICE ---------------------------------------------------------------------------
		// HP, AP 데미지 체크해서, 빨간색으로 만들기 위한 준비.
		// 빨간색으로 만드는건, Render 에서 한다.

		// 가장 최초값은, m_nLast?? = 0.., g_pPlayerStats->Get?????() = 100..
		// uint32 이기 때문에 if 에서는 참이다.
		if ( m_nLastHP - g_pPlayerStats->GetHealth() > 0 )
		{
			// 100 은 처음 값이라고 생각해서 데미지 처리 하지 않는다.
			if ( 100 != g_pPlayerStats->GetHealth() )
			{
				m_bHPDamaged = LTTRUE;
				m_fLastHPNotifiedTime = 0.0f;
			}

			m_nLastHP = g_pPlayerStats->GetHealth();
		}

		// 상동.
		if ( m_nLastAP - g_pPlayerStats->GetArmor() > 0 )
		{
			if ( 100 != g_pPlayerStats->GetArmor() )
			{
				m_bAPDamaged = LTTRUE;
				m_fLastAPNotifiedTime = 0.0f;
			}

			m_nLastAP = g_pPlayerStats->GetArmor();
		}
		// ----------------------------------------------------------------------------------
		
		// Air
		if ( m_nLastAir - g_pPlayerStats->GetAirPoint() > 0 )
		{
			if ( 100 != g_pPlayerStats->GetAirPoint() )
			{
				m_bAirDamaged = LTTRUE;
				m_fLastAirNotifiedTime = 0.0f;
			}

			m_nLastAir = g_pPlayerStats->GetAirPoint();
		}
		// ----------------------------------------------------------------------------------
		
		
	}

//GameHI Hud [2004.04.02]
	float fx , fy , fw ,fh;
	fx = (float)(m_GameHiHealthBasePos.x) * g_pInterfaceResMgr->GetXRatio();
	fy = (float)(m_GameHiHealthBasePos.y) * g_pInterfaceResMgr->GetYRatio();
	fw = (float)(m_GameHiHealthSize.x) * g_pInterfaceResMgr->GetXRatio();
	fh = (float)(m_GameHiHealthSize.y) * g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH(&m_GameHiHealthPoly,fx,fy,fw,fh);

	fx = (float)(m_GameHiArmorBasePos.x) * g_pInterfaceResMgr->GetXRatio();
	fy = (float)(m_GameHiArmorBasePos.y) * g_pInterfaceResMgr->GetYRatio();
	fw = (float)(m_GameHiArmorSize.x) * g_pInterfaceResMgr->GetXRatio();
	fh = (float)(m_GameHiArmorSize.y) * g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH(&m_GameHiArmorPoly,fx,fy,fw,fh);

	fx = (float)(m_AirBasePos[0].x) * g_pInterfaceResMgr->GetXRatio();
	fy = (float)(m_AirBasePos[0].y) * g_pInterfaceResMgr->GetYRatio();
	fw = (float)(m_AirSize[0].x) * g_pInterfaceResMgr->GetXRatio();
	fh = (float)(m_AirSize[0].y) * g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH(&m_AirPoly[0],fx,fy,fw,fh);

	for(int i=0;i<3;i++)
	{	
		fx = (float)(m_GameHiHealthBasePos.x + m_HealthTextOffset.x + 13 * i) * g_pInterfaceResMgr->GetXRatio();
		fy = (float)(m_GameHiHealthBasePos.y + m_HealthTextOffset.y) * g_pInterfaceResMgr->GetYRatio();
		fw = (float)(16) * g_pInterfaceResMgr->GetXRatio();
		fh = (float)(32) * g_pInterfaceResMgr->GetYRatio();
		g_pDrawPrim->SetXYWH(&m_NumPoly[i],fx,fy,fw,fh);
	}

	for(i=3;i<6;i++)
	{	
		fx = (float)(m_GameHiArmorBasePos.x + m_ArmorTextOffset.x + 13 * (i-3)) * g_pInterfaceResMgr->GetXRatio();
		fy = (float)(m_GameHiArmorBasePos.y + m_ArmorTextOffset.y) * g_pInterfaceResMgr->GetYRatio();
		fw = (float)(16) * g_pInterfaceResMgr->GetXRatio();
		fh = (float)(32) * g_pInterfaceResMgr->GetYRatio();
		g_pDrawPrim->SetXYWH(&m_NumPoly[i],fx,fy,fw,fh);		
	}

	//[SPIKE] Air
/*	for(i=6;i<9;i++)
	{	
		fx = (float)(m_AirBasePos[0].x + m_AirTextOffset.x + 13 * (i-6)) * g_pInterfaceResMgr->GetXRatio();
		fy = (float)(m_AirBasePos[0].y + m_AirTextOffset.y) * g_pInterfaceResMgr->GetYRatio();
		fw = (float)(16) * g_pInterfaceResMgr->GetXRatio();
		fh = (float)(32) * g_pInterfaceResMgr->GetYRatio();
		g_pDrawPrim->SetXYWH(&m_NumPoly[i],fx,fy,fw,fh);		
	}*/
//GameHI Hud [2004.04.02]
}

void CHUDHealth::UpdateLayout()
{
	int nCurrentLayout = GetConsoleInt("HUDLayout",0);

	m_bUseHealthText	= g_pLayoutMgr->GetUseHealthText(nCurrentLayout);
	
	m_GameHiHealthBasePos	= g_pLayoutMgr->GetGameHIHealthIconPos(nCurrentLayout);
	m_GameHiHealthSize		= g_pLayoutMgr->GetGameHIHealthIconSize(nCurrentLayout);
	m_HealthTextOffset		= g_pLayoutMgr->GetHealthTextOffset(nCurrentLayout);

	m_GameHiArmorBasePos	= g_pLayoutMgr->GetArmorIconPos(nCurrentLayout);
	m_GameHiArmorSize		= g_pLayoutMgr->GetArmorIconSize(nCurrentLayout);
	m_ArmorTextOffset		= g_pLayoutMgr->GetArmorTextOffset(nCurrentLayout);
	
	m_AirBasePos[0]			= g_pLayoutMgr->GetAirBasePos(nCurrentLayout);
	m_AirSize[0].x			= 256;
	m_AirSize[0].y			= 16;
	m_AirBasePos[1].x		= m_AirBasePos[0].x+36;
	m_AirBasePos[1].y		= m_AirBasePos[0].y+1;
	m_AirSize[1].x			= 140;
	m_AirSize[1].y			= 16;	
//	m_AirTextOffset			= g_pLayoutMgr->GetAirTextOffset(nCurrentLayout);
}

void CHUDHealth::ScreenDimChanged()
{
}