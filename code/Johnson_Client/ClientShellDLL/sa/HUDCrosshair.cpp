// ----------------------------------------------------------------------- //
//
// MODULE  : HUDCrosshair.cpp
//
// PURPOSE : HUDItem to display crosshair
//
// (c) 2001-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "SAHUDMgr.h"
#include "HUDCrosshair.h"
#include "PlayerStats.h"
#include "SurfaceFunctions.h"
#include "CharacterFX.h"
#include "BodyFX.h"
#include "SAGameClientShell.h"
#include "ClientWeaponBase.h"
#include "ClientWeaponMgr.h"
#include "TargetMgr.h"
#include "CMoveMgr.h"
#include "VehicleMgr.h"
#include "PopupScreenOption.h"
#include "MissionMgr.h"

#include "GameText.h"
#include "GameMessage.h"

namespace
{
	VarTrack g_vtScopeLRGap;
	VarTrack g_vtScopeUDGap;
	VarTrack g_vtScopeLRRadius;
	VarTrack g_vtScopeUDRadius;
#define MAX_PERTURB_TEX	10
	HTEXTURE	g_hBaccurate[MAX_PERTURB_TEX] = {0, };
	int			g_nCurrentBacc	= 0;
}

//******************************************************************************************
//** HUD crosshair
//******************************************************************************************
CHUDCrosshair::CHUDCrosshair()
{
	m_UpdateFlags = kHUDFrame;
	m_bEnabled = true;
	m_bArmed = false;
	
	m_fScale = 0.0f;
	m_style = 0;
	m_bCanActivateTarget = true;
	m_nTargetTeam = INVALID_TEAM;
	//m_eLevel = kHUDRenderDead;
	m_eLevel = kHUDRenderText;

	// Added By kwangsik ==>
	// Time : 2004-07-19 오전 11:25:55
	m_pFont				= LTNULL;
	// Added By kwangsik <==
}
	

LTBOOL CHUDCrosshair::Init()
{
	// Added By kwangsik ==>
	// Time : 2004-07-19 오전 11:28:34
	m_pFont		= new CSAD3DText();
	m_pFont->CreateD3DText( 256, 16, true );
	m_pFont->SetTextColor( RGB(0,0,0) );
	m_pFont->SetTransparent(true);
	// Added By kwangsik <==


	for(int i=0; i<MAXPOLYCOUNT; i++)
		g_pDrawPrim->SetRGBA(&m_Poly[i],argbWhite);

	g_vtScopeLRGap.Init(g_pLTClient, "ScopeLRGap", NULL, 32.0f);
    g_vtScopeUDGap.Init(g_pLTClient, "ScopeUPGap", NULL, 32.0f);
    g_vtScopeLRRadius.Init(g_pLTClient, "ScopeLRRadius", NULL, 0.377f);
    g_vtScopeUDRadius.Init(g_pLTClient, "ScopeUDRadius", NULL, 0.34f);

	uint8 style = (uint8)GetConsoleInt("CrosshairStyle",0);
	SetStyle(style);

//---------------------------------------------------------------------------------------
// Gamehi 2004.04.06 bestline
	//[SPIKE]
	m_hScpe[0] = g_pInterfaceResMgr->GetTexture("\\SA_INTERFACE\\HUD\\SCOPE\\SCOPE.DTX");	
	SetupQuadUVs(m_Poly[2], m_hScpe[0], 0.0f, 0.0f, 1.0f, 1.0f);
	m_hScpe[1] = g_pInterfaceResMgr->GetTexture("\\SA_INTERFACE\\HUD\\SCOPE\\Scope_WidthLine.dtx");	
	SetupQuadUVs(m_Poly[3], m_hScpe[1], 0.0f, 0.0f, 0.5f, 1.0f);
	m_hScpe[2] = g_pInterfaceResMgr->GetTexture("\\SA_INTERFACE\\HUD\\SCOPE\\Scope_HightLine.dtx");	
	SetupQuadUVs(m_Poly[4], m_hScpe[2], 0.0f, 0.0f, 1.0f, 0.5f);
	m_hScpe[3] = g_pInterfaceResMgr->GetTexture("\\SA_INTERFACE\\HUD\\SCOPE\\Scope_RedSpot.dtx");	
	SetupQuadUVs(m_Poly[5], m_hScpe[3], 0.0f, 0.0f, (3.0f/8.0f), (3.0f/8.0f));
//---------------------------------------------------------------------------------------

	UpdateLayout();	

	ScreenDimChanged();
	
	return LTTRUE;
}

void CHUDCrosshair::Term()
{
	SAFE_DELETE( m_pFont );
}

void CHUDCrosshair::Render()
{
	if (g_pInterfaceMgr->GetScopeOn())
	{
		if (!g_pPlayerMgr->UsingCamera())
			RenderScope();
		return;
	}

	// 광식 : 죽었을 때는 Scope만 Rendering 하고 crosshair 는 skip..
	if( g_pPlayerMgr->IsPlayerDead() )
		return ;

	// See if we can show a crosshair or not...
	if (g_pMoveMgr->GetVehicleMgr()->IsVehiclePhysics() &&
		!g_pMoveMgr->GetVehicleMgr()->CanShowCrosshair())
	{
		return;
	}

	if (!m_bEnabled)
		return;

	// 스나이퍼 일때 Crosshair 제외...
	if(!g_pPlayerMgr->GetClientWeaponMgr() || !g_pPlayerMgr->GetClientWeaponMgr()->GetCurrentClientWeapon() ) return;
	if(g_pPlayerMgr->GetClientWeaponMgr()->GetCurrentClientWeapon()->
					 GetWeapon()->nAniType == ANITYPE_SNIPER)
		return;
	/////////////////////////////////////  SPIKE
	
	if (g_pPlayerMgr->IsPlayerDead())
	{
		//[MURSUM]=====================================
		//흰점 제거
		/*
		g_pDrawPrim->SetTexture(LTNULL);
		g_pDrawPrim->SetRGBA(&m_Poly[2],argbWhite);

		float x = 400.0f * g_pInterfaceResMgr->GetXRatio();
		float y = 300.0f * g_pInterfaceResMgr->GetYRatio();
		float w = 2.0f * g_pInterfaceResMgr->GetXRatio();

		g_pDrawPrim->SetXYWH(&m_Poly[2],x,y,w,w);
		g_pDrawPrim->DrawPrim(&m_Poly[2],1);
		*/
		//==============================================
	}
	else
	{
		//------------------------------------------------------------------------
		// 광식
		// 적군은 Target 되었을 때 무조건 빨간색으로 표시 됨
		//------------------------------------------------------------------------
		if( m_nTargetTeam == INVALID_TEAM )
		{
			if (GetConsoleInt("CrosshairDynamiac",1))
			{
				g_pDrawPrim->SetTexture(g_hBaccurate[g_nCurrentBacc]);
				g_pDrawPrim->DrawPrim(&m_Poly[1],1);
			}
		}
		else
		{
			//uint8 TargetID = g_pPlayerMgr->GetTargetMgr()->GetTargetTeam();
						
//[SPIKE] 기본 Crosshair 유지 -------------------------------------------------------------------
			if (GetConsoleInt("CrosshairDynamiac",1))
			{
				g_pDrawPrim->SetTexture(g_hBaccurate[g_nCurrentBacc]);
				g_pDrawPrim->DrawPrim(&m_Poly[1],1);
			}			
		}
		//------------------------------------------------------------------------
	}

	m_pFont->Render(); 
}


void CHUDCrosshair::Update()
{
	if (!m_bEnabled)
		return;


	
	IClientWeaponBase* pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();

    float fPerturb = 1.0f;

	// Just use the weapon's dynamic perturb to determine the amount the
	// crosshair should scale...
	if (pClientWeapon)
	{
		fPerturb = pClientWeapon->GetDynamicPerturb();
		
//		if(fPerturb > pClientWeapon->GetWeapon()->m_fPerturbMax)
//			fPerturb = pClientWeapon->GetWeapon()->m_fPerturbMax;

		//g_pLTClient->CPrint( "%f", fPerturb );
	}

	//[MURSUM]
	//게임중 이미지로딩은 하지 않음 ( 포커스 없을경우 깨짐 )
	/*
	uint8 style = (uint8)GetConsoleInt("CrosshairStyle",0);
	if (style != m_style)
		SetStyle(style);
	*/

	bool bDynamic = (!!GetConsoleInt("CrosshairDynamic",1));


	uint32 cr = (uint32)(GetConsoleInt("CrosshairRed",0x00));
	uint32 cg = (uint32)(GetConsoleInt("CrosshairGreen",0xFF));
	uint32 cb = (uint32)(GetConsoleInt("CrosshairBlue",0xFF));
	//uint32 ca = (uint32)( (1.0f - fPerturb) * 255.0f);
	uint32 ca = (uint32)( 0.0f);

	// 광식
	//----------------------------------------
	int tempPerturb = (int)(fPerturb*10);
	if( tempPerturb > 9 ) tempPerturb = 9;
	g_nCurrentBacc = tempPerturb;

	//g_pLTClient->CPrint( "%d", g_nCurrentBacc );
	//----------------------------------------
	
	if (!bDynamic)
		ca = 0xFF;

	uint32 crosscolor = SET_ARGB(ca,cr,cg,cb);
	if (bDynamic)
	{
		//ca = (uint32)( fPerturb * 255.0f);
		ca = (uint32)(255.0f);
		crosscolor = SET_ARGB(ca,cr,cg,cb);
		g_pDrawPrim->SetRGBA(&m_Poly[1],crosscolor);
	}

	m_bCanActivateTarget = g_pPlayerMgr->GetTargetMgr()->CanActivateTarget();
	m_nTargetTeam  = g_pPlayerMgr->GetTargetMgr()->GetTargetTeam();

	//[MURSUM]===========================================
	//Search 글씨 찍지말자~!
	/*
	if (g_pPlayerMgr->IsSearching())
	{
		m_pFont->DrawText(0,0, LoadTempString(IDS_TARGET_SEARCHING), RGB(0,0,0));
		return;
	}
	*/
	//===================================================

	HOBJECT hObj = g_pPlayerMgr->GetTargetMgr()->GetTargetObject();
	uint16  nID = g_pPlayerMgr->GetTargetMgr()->GetTargetStringID();
	
	//[MURSUM]=========================================
	//Zoom이면 이름을 안찍나?
	static bool bZoomed = false;
	if( g_pPlayerMgr->IsZooming() )
	{
		if (!bZoomed)
		{
			bZoomed = true;			
			m_pFont->DrawD3DText( "", 0 );
		}
		
		return;
	}
	//================================================
	
	bool btest = bZoomed;
	//if we have zoomed since our last full update, force an update
	if (!bZoomed && hObj == m_hObj && nID == m_nString)
		return;

	bZoomed = false;

	m_hObj = hObj;
	m_nString = nID;

	if( m_pFont )
	{
		if( m_bCanActivateTarget )
		{	
			if( m_nTargetTeam == INVALID_TEAM )	//PickUp , Search, Defusing Bomb...
			{
				m_pFont->SetTextColor( RGB(255,184,30),RGB(0,0,0) );
				m_pFont->DrawD3DText( g_pPlayerMgr->GetTargetMgr()->GetTargetString(),
									  strlen(g_pPlayerMgr->GetTargetMgr()->GetTargetString()),
									  DT_CENTER, D3D_TEXT_OUTLINE );
			}
			else
			{
				if( m_nTargetTeam == g_GameDoc.GetTeam() )	//우리 팀
				{
					if( LTFALSE == g_pNickList->IsVisible() )
					{
						m_pFont->SetTextColor( RGB(88,142,248),RGB(0,0,0) );
						m_pFont->DrawD3DText( g_pPlayerMgr->GetTargetMgr()->GetTargetString(),
							strlen(g_pPlayerMgr->GetTargetMgr()->GetTargetString()),
							DT_CENTER, D3D_TEXT_OUTLINE );
					}
				}
				else								//적
				{
					m_pFont->SetTextColor( RGB(255,0,0), RGB(0,0,0) );
					m_pFont->DrawD3DText( g_pPlayerMgr->GetTargetMgr()->GetTargetString(),
										  strlen(g_pPlayerMgr->GetTargetMgr()->GetTargetString()),
										  DT_CENTER, D3D_TEXT_OUTLINE );
				}
			}

			//[SPIKE] 해체를 위한 도움말 
			if( g_pMissionMgr->GetMissionState() == MI_BOMB_PLANTED && stricmp(g_pPlayerMgr->GetTargetMgr()->GetTargetString(), "BOMB") == 0)
			{
				char szDefuseHelpText[512] = {LTNULL};
				//"'%s'키를 누르면 폭탄을 해체할 수 있습니다."
				sprintf(szDefuseHelpText, NF_GetMsg(SA_TEXT_HELP_BOMB_DEFUSE_KEY), g_pPopupOption->GetCommandItem(16));
				g_pHelpMsgs->AddMessage( szDefuseHelpText, 14, kMsgHelpMsg, 3.0f );
			}
			///////////////////////////////////////////////////////////////////////////////////
		}
		else
		{
			m_pFont->SetTextColor( RGB(128,128,128),RGB(0,0,0) );
			m_pFont->DrawD3DText( g_pPlayerMgr->GetTargetMgr()->GetTargetString(),
								  strlen(g_pPlayerMgr->GetTargetMgr()->GetTargetString()),
								  DT_CENTER, D3D_TEXT_OUTLINE );
		}
	}		

	// Check for special case of camera-activate mode...

	bool bUsingCamera = false;
	AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(g_pPlayerStats->GetCurrentAmmo());
	if (pAmmo && pAmmo->eInstDamageType == DT_GADGET_CAMERA)
	{
		bUsingCamera = true;
	}
	if (!bUsingCamera || !g_pPlayerMgr->IsZoomed())
	{
		g_pInterfaceMgr->RemoveOverlay(OVM_CAMERA);
		g_pInterfaceMgr->RemoveOverlay(OVM_CAMERA_TARGET);
		return;
	}


	if (hObj)
	{
		uint32 dwUserFlags = 0;
		g_pCommonLT->GetObjectFlags(hObj, OFT_User, dwUserFlags);

		if ( (dwUserFlags & USRFLG_GADGET_CAMERA) && g_pPlayerMgr->InCameraGadgetRange( hObj ) )
		{
			g_pInterfaceMgr->CreateOverlay(OVM_CAMERA_TARGET);
			g_pInterfaceMgr->RemoveOverlay(OVM_CAMERA);
			return;

		}
	}

	g_pInterfaceMgr->CreateOverlay(OVM_CAMERA);
	g_pInterfaceMgr->RemoveOverlay(OVM_CAMERA_TARGET);


}





// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHUDCrosshair::RenderScope()
//
//	PURPOSE:	Draw the scope crosshair
//
// ----------------------------------------------------------------------- //
void CHUDCrosshair::RenderScope()
{
	g_pDrawPrim->SetTexture(LTNULL);

	float cx = 400.0f * g_pInterfaceResMgr->GetXRatio();
	float cy = 300.0f * g_pInterfaceResMgr->GetYRatio();

	float hR = g_vtScopeLRRadius.GetFloat() * cx * 2.0f;
	float hGap = g_vtScopeLRGap.GetFloat() * g_pInterfaceResMgr->GetXRatio();
	float vR = g_vtScopeUDRadius.GetFloat() * cx * 2.0f;
	float vGap = g_vtScopeUDGap.GetFloat() * g_pInterfaceResMgr->GetXRatio();


//[SPIKE] ////////////////////////////////////////////////////////////
	// Base
	g_pDrawPrim->SetXYWH(&m_Poly[2],0,0, cx*2.0f,cy*2.0f);
	g_pDrawPrim->SetTexture(m_hScpe[0]);	
	g_pDrawPrim->DrawPrim(&m_Poly[2],1);
	// Width_Line
	g_pDrawPrim->SetTexture(LTNULL);
	g_pDrawPrim->DrawPrim(&m_Poly[3],1);
	// Hight_Line
	g_pDrawPrim->SetTexture(LTNULL);	 
	g_pDrawPrim->DrawPrim(&m_Poly[4],1);
	// Red_Spot
	g_pDrawPrim->SetXYWH(&m_Poly[5],cx-2.0f,cy-2.0f,3.0f,3.0f);
	g_pDrawPrim->SetTexture(m_hScpe[3]);	
	g_pDrawPrim->DrawPrim(&m_Poly[5],1);	
//	g_pDrawPrim->SetXYWH(&m_Poly[2],200,150, cx, cy);
///////////////////////////////////////////////////////////////////////
}



void CHUDCrosshair::UpdateLayout()
{
	int nCurrentLayout = GetConsoleInt("HUDLayout",0);

	LTVector vCol = g_pLayoutMgr->GetVector("Miscellaneous","Team1Color");
	uint8 nR = (uint8)vCol.x;
	uint8 nG = (uint8)vCol.y;
	uint8 nB = (uint8)vCol.z;

	vCol = g_pLayoutMgr->GetVector("Miscellaneous","Team2Color");
	nR = (uint8)vCol.x;
	nG = (uint8)vCol.y;
	nB = (uint8)vCol.z;
}



//[SPIKE] 
void CHUDCrosshair::SetStyle(uint8 style)
{
	char *szTag = "HUDCrosshair";
	char szAtt[32], szTempStyle[64]={LTNULL}, szTempIndex[8];

	for( int count = 0; count <MAX_PERTURB_TEX; count++ )
	{
		sprintf(szAtt, "Crosshair%d", style);  // 스타일에 맞게 주소 세팅 
		sprintf(szTempIndex, "%d", count);
		strcat(szAtt, szTempIndex);
		if(!g_pLayoutMgr->HasValue(szTag,szAtt))
			break;

		char szTexStyle[256];
		g_pLayoutMgr->GetString(szTag,szAtt,szTexStyle,sizeof(szTexStyle));

		uint32 tw,th;
		g_hBaccurate[count] = g_pInterfaceResMgr->GetTexture(szTexStyle);
		g_pTexInterface->GetTextureDims(g_hBaccurate[count],tw,th);
		m_fInaccurateSz = (float)tw;
		//setup the poly UV coords
		SetupQuadUVs(m_Poly[1], g_hBaccurate[count], 0.0f, 0.0f, 1.0f, 1.0f);
	}	
	m_style = style;
}

void CHUDCrosshair::ScreenDimChanged()
{
	m_pFont->SetPosition( g_pInterfaceResMgr->GetScreenWidth()*0.5f - 128.0f,//* g_pInterfaceResMgr->GetXRatio(),
						  g_pInterfaceResMgr->GetScreenHeight()*0.58f);

	g_pDrawPrim->SetXYWH(&m_Poly[3],0.0f, g_pInterfaceResMgr->GetScreenHeight()*0.5f - 1.0f, (float)g_pInterfaceResMgr->GetScreenWidth(), 1.0f);
	g_pDrawPrim->SetRGBA(&m_Poly[3], 0xFF000000);
	g_pDrawPrim->SetXYWH(&m_Poly[4],g_pInterfaceResMgr->GetScreenWidth()*0.5f - 1.0f, 0.0f, 1.0f, (float)g_pInterfaceResMgr->GetScreenHeight());
	g_pDrawPrim->SetRGBA(&m_Poly[4], 0xFF000000);

	if(m_fInaccurateSz < 64.0f) return;
	float x = g_pInterfaceResMgr->GetScreenWidth()*0.5f;
	float y = g_pInterfaceResMgr->GetScreenHeight()*0.5f;
	float fTemp = m_fInaccurateSz / 64.0f;
	x -= 32.0f;
	y -= 32.0f;
	g_pDrawPrim->SetXYWH(&m_Poly[1],x,y,m_fInaccurateSz/fTemp,m_fInaccurateSz/fTemp);	
}