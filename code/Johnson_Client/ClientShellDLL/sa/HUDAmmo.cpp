// ----------------------------------------------------------------------- //
//
// MODULE  : HUDAmmo.cpp
//
// PURPOSE : HUDItem to display player ammo
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "SAHUDMgr.h"
#include "SAPlayerStats.h"
#include "SAInterfaceMgr.h"
#include "ClientWeaponBase.h"
#include "ClientWeaponMgr.h"
#include "PlayerMgr.h"
#include "GameClientShell.h"


//******************************************************************************************
//**
//** HUD Ammo display
//**
//******************************************************************************************

CHUDAmmo::CHUDAmmo()
{
	m_UpdateFlags = kHUDAmmo;
	m_bDraw = LTFALSE;
	m_bInfinite = LTFALSE;

	m_bUseBar = LTFALSE;
	m_bUseText = LTTRUE;

	memset(m_LNum,0,sizeof(m_LNum));
	memset(m_MNum,0,sizeof(m_LNum));
	m_LNum[2] = '0';
	m_MNum[2] = '0';
}

LTBOOL CHUDAmmo::Init()
{
	UpdateLayout();

	uint32 fadeColor;
	fadeColor = SET_ARGB(0xFF,0xFF,0xFF,0xFF);
	g_pDrawPrim->SetRGBA(&m_Poly[1],fadeColor);

	char str[128];

	for(int i=0;i<10;i++)
	{		
		sprintf(str , "SA_Interface\\Hud\\Bar\\Num_AmmoS%d.dtx" , i);
		m_hMNumber[i] = g_pInterfaceResMgr->GetTexture(str);

		sprintf(str , "SA_Interface\\Hud\\Bar\\Num_AmmoL%d.dtx" , i);
		m_hLNumber[i] = g_pInterfaceResMgr->GetTexture(str);
	}

//	m_hSection = g_pInterfaceResMgr->GetTexture("SA_Interface\\Hud\\Bar\\Num_section.dtx");
//	g_pDrawPrim->SetRGBA(&m_Section,argbWhite);
//	SetupQuadUVs(m_Section, m_hSection, 0.0f,0.0f,1.0f,1.0f);

	for( i=0;i<3;i++)
	{	
		g_pDrawPrim->SetRGBA(&m_LNumPoly[i],argbWhite);
		SetupQuadUVs(m_LNumPoly[i], m_hLNumber[0], 0.0f,0.0f,1.0f,1.0f);
		m_LNum[i] = -1;

		g_pDrawPrim->SetRGBA(&m_MNumPoly[i],argbWhite);
		SetupQuadUVs(m_MNumPoly[i], m_hMNumber[0], 0.0f,0.0f,1.0f,1.0f);
		m_MNum[i] = -1;		
	}

	g_pDrawPrim->SetRGBA(&m_GameHiBulletPoly,argbWhite);
	m_GameHiBulletIcon = g_pInterfaceResMgr->GetTexture("SA_Interface\\Hud\\Bar\\bullet_bar.dtx");
	SetupQuadUVs(m_GameHiBulletPoly, m_GameHiBulletIcon, 0.0f, 0.0f, 1.0f, 1.0f);

	return LTTRUE;
}

void CHUDAmmo::Term()
{
}

void CHUDAmmo::Render()
{
	//[MURSUM] 포커스가 없어서 업뎃을 뒤로 미룬경우
	if( m_bNeedUpdate && g_pGameClientShell->IsMainWindowFocused() )
	{
		Update();
		SetNeedUpdate( LTFALSE );
	}

	bool bWeaponsEnabled = g_pPlayerMgr->GetClientWeaponMgr()->WeaponsEnabled();
	IClientWeaponBase* pClientWeapon =
	                g_pPlayerMgr->GetCurrentClientWeapon();
	if (!pClientWeapon || !bWeaponsEnabled) return;

	if (!m_bDraw || pClientWeapon->GetState() == W_DESELECT) return;

	g_pDrawPrim->SetTexture(m_GameHiBulletIcon);
	g_pDrawPrim->DrawPrim(&m_GameHiBulletPoly);

	for( int i=0;i<3;i++)
	{	
		if(m_LNum[i] >= 0)
		{		
			g_pDrawPrim->SetTexture(m_hLNumber[m_LNum[i]]);
			g_pDrawPrim->DrawPrim(&m_LNumPoly[i]);			
		}
	}

//	g_pDrawPrim->SetTexture(m_hSection);
//	g_pDrawPrim->DrawPrim(&m_Section);

	for( i=0;i<3;i++)
	{	
		if(m_MNum[i] >= 0)
		{		
			g_pDrawPrim->SetTexture(m_hMNumber[m_MNum[i]]);
			g_pDrawPrim->DrawPrim(&m_MNumPoly[i]);			
		}
	}

	
// ==> SPIKE 처리해야할 부분 (텍스쳐 교체)
	//render icon here
	if (m_hIcon)
	{
		g_pDrawPrim->SetTexture(m_hIcon);
		SetupQuadUVs(m_Poly[1], m_hIcon, 0.0f, 0.0f, 1.0f, 1.0f);
		g_pDrawPrim->DrawPrim(&m_Poly[1]);
	}
// <==
}

void CHUDAmmo::Update()
{
	//[MURSUM] 포커스가 없으면 업뎃을 뒤로 미룬다.
	if( LTFALSE == g_pGameClientShell->IsMainWindowFocused() )
	{
		SetNeedUpdate( LTTRUE );
		return;
	}

	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(g_pPlayerStats->GetCurrentWeapon());
	AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(g_pPlayerStats->GetCurrentAmmo());

	m_bDraw = (pWeapon && pAmmo);

	if (pAmmo && (pAmmo->eInstDamageType == DT_MELEE || pAmmo->eInstDamageType == DT_SWORD) )
		m_bDraw = LTFALSE;

	if (!m_bDraw)
	{
		memset(m_LNum,0,sizeof(m_LNum));
		memset(m_MNum,0,sizeof(m_LNum));
		m_LNum[2] = '0';
		m_MNum[2] = '0';
	//	return;
	}
	
	IClientWeaponBase* pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon( );
	if( !pClientWeapon ) return;
	int nAmmoInClip = pClientWeapon ? pClientWeapon->GetAmmoInClip() : 0;
	int nAmmo = g_pPlayerStats->GetCurrentAmmoCount() - nAmmoInClip;
	m_nFull = (uint8)nAmmoInClip;
	m_nEmpty = (uint8)pWeapon->nShotsPerClip - m_nFull;

	m_bInfinite = pWeapon->bInfiniteAmmo;

	if(m_bDraw)
	{
		int temp = nAmmoInClip;
		int n100 , n10 , n1;

		n100 = temp / 100;
		n10  = (temp % 100) / 10;
		n1   = (temp % 100) % 10 ;

		if(n100 == 0) m_LNum[0] = -1;
		else m_LNum[0] = n100;

		if(n100 == 0 && n10 ==0) m_LNum[1] = -1;
		else m_LNum[1] = n10;
	
		m_LNum[2] = n1;


		temp = nAmmo < 0 ? 0 : nAmmo;

		n100 = temp / 100;
		n10  = (temp % 100) / 10;
		n1   = (temp % 100) % 10 ;

		if(n100 == 0) m_MNum[0] = -1;
		else m_MNum[0] = n100;

		if(n100 == 0 && n10 ==0) m_MNum[1] = -1;
		else m_MNum[1] = n10;
	
		m_MNum[2] = n1;	
	}

	
	float x = (float)(m_BasePos.x + m_IconOffset.x) * g_pInterfaceResMgr->GetXRatio();
	float y = (float)(m_BasePos.y + m_IconOffset.y) * g_pInterfaceResMgr->GetYRatio();
	float w = (float)m_IconSize.x * g_pInterfaceResMgr->GetXRatio();
	float h = (float)m_IconSize.y * g_pInterfaceResMgr->GetYRatio();

	// Added By Bestline ==>
	// Time : 2004-06-21 오후 8:22:40
	// HUD에 지금 선택 중인 무기 아이콘 출력
	if( pAmmo )
	{	
		std::string icon = pAmmo->GetNormalIcon();
		m_hIcon = g_pInterfaceResMgr->GetTexture(icon.c_str());
		g_pDrawPrim->SetXYWH(&m_Poly[1],x,y,w,h);
	}
	// Added By Bestline <==
	
	for(int i=0;i<3;i++)
	{	
		x = (float)(m_BasePos.x + m_TextOffset.x + 13 * i ) * g_pInterfaceResMgr->GetXRatio();		
		y = (float)(m_BasePos.y + m_TextOffset.y) * g_pInterfaceResMgr->GetYRatio();
		w = (float)(16) * g_pInterfaceResMgr->GetXRatio();
		h = (float)(32) * g_pInterfaceResMgr->GetYRatio();
		g_pDrawPrim->SetXYWH(&m_LNumPoly[i],x,y,w,h);
	}

	for(i=0;i<3;i++)
	{	
		x = (float)(m_BasePos.x + m_TextOffset.x + 13 * i + 53) * g_pInterfaceResMgr->GetXRatio();		
	//	y = (float)(m_BasePos.y + m_TextOffset.y + 14) * g_pInterfaceResMgr->GetXRatio();
		y = (float)(m_BasePos.y + m_TextOffset.y) * g_pInterfaceResMgr->GetYRatio();
		w = (float)(16) * g_pInterfaceResMgr->GetXRatio();
		h = (float)(32) * g_pInterfaceResMgr->GetYRatio();
		g_pDrawPrim->SetXYWH(&m_MNumPoly[i],x,y,w,h);
	}

//	x = (float)(m_BasePos.x + m_TextOffset.x + 63 ) * g_pInterfaceResMgr->GetXRatio();		
//	y = (float)(m_BasePos.y + m_TextOffset.y + 14 ) * g_pInterfaceResMgr->GetXRatio();
//	y = (float)(m_BasePos.y + m_TextOffset.y + 10 ) * g_pInterfaceResMgr->GetXRatio();
//	w = (float)(16) * g_pInterfaceResMgr->GetXRatio();
//	h = (float)(16) * g_pInterfaceResMgr->GetYRatio();
//	g_pDrawPrim->SetXYWH(&m_Section,x,y,w,h);

	x = (float)(m_GameHiBulletBasePos.x) * g_pInterfaceResMgr->GetXRatio();
	y = (float)(m_GameHiBulletBasePos.y) * g_pInterfaceResMgr->GetYRatio();
	w = (float)(m_GameHiBulletSize.x) * g_pInterfaceResMgr->GetXRatio();
	h = (float)(m_GameHiBulletSize.y) * g_pInterfaceResMgr->GetYRatio();
	g_pDrawPrim->SetXYWH(&m_GameHiBulletPoly,x,y,w,h);

}

void CHUDAmmo::UpdateLayout()
{
	int nCurrentLayout = GetConsoleInt("HUDLayout",0);

	m_BasePos		= g_pLayoutMgr->GetAmmoBasePos(nCurrentLayout);

	m_bUseBar		= g_pLayoutMgr->GetUseAmmoBar(nCurrentLayout);
	m_BarOffset		= g_pLayoutMgr->GetAmmoBarOffset(nCurrentLayout);
    m_ClipOffset	= g_pLayoutMgr->GetAmmoClipOffset(nCurrentLayout);
    m_ClipUnitSize	= g_pLayoutMgr->GetAmmoClipUnitSize(nCurrentLayout);

	m_bUseText		= g_pLayoutMgr->GetUseAmmoText(nCurrentLayout);
	m_TextOffset	= g_pLayoutMgr->GetAmmoTextOffset(nCurrentLayout);
    
	m_IconOffset	= g_pLayoutMgr->GetAmmoIconOffset(nCurrentLayout);
	m_IconSize		= g_pLayoutMgr->GetAmmoIconSize(nCurrentLayout);
	m_TextColor		= g_pLayoutMgr->GetAmmoColor(nCurrentLayout);

	m_nBarHeight		= g_pLayoutMgr->GetBarHeight(nCurrentLayout);
	m_nTextHeight		= g_pLayoutMgr->GetTextHeight(nCurrentLayout);
	m_fBarScale			= g_pLayoutMgr->GetBarScale(nCurrentLayout);


	m_GameHiBulletBasePos  =  g_pLayoutMgr->GetGameHIBulletBarIconPos(nCurrentLayout);
	m_GameHiBulletSize     =  g_pLayoutMgr->GetGameHIBulletBarIconSize(nCurrentLayout);
}

void CHUDAmmo::ScreenDimChanged()
{
}