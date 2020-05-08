// ----------------------------------------------------------------------- //
//
// MODULE  : WeaponFX.cpp
//
// PURPOSE : Weapon special FX - Implementation
//
// CREATED : 2/22/98
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "WeaponFX.h"
#include "iltclient.h"
#include "ClientUtilities.h"
#include "WeaponFXTypes.h"
#include "GameClientShell.h"
#include "MarkSFX.h"
#include "ParticleShowerFX.h"
#include "DynamicLightFX.h"
#include "BulletTrailFX.h"
#include "MsgIDs.h"
#include "ShellCasingFX.h"
#include "ParticleExplosionFX.h"
#include "BaseScaleFX.h"
#include "DebrisFX.h"
#include "RandomSparksFX.h"
#include "iltphysics.h"
#include "MuzzleFlashFX.h"
#include "SurfaceFunctions.h"
#include "VarTrack.h"
#include "PolyDebrisFX.h"
#include "CharacterFX.h"
#include "CMoveMgr.h"
#include "ClientWeaponBase.h"
#include "ClientMultiplayerMgr.h"
#include "../SA/SAHUDMgr.h"

//광식
#include "PlayerCamera.h"
#include "ClientWeapon.h"
#include "ClientWeaponMgr.h"

static uint32 s_nNumShells = 0;

VarTrack	g_cvarShowFirePath;
VarTrack	g_cvarLightBeamColorDelta;
VarTrack	g_cvarImpactPitchShift;
VarTrack	g_cvarFlyByRadius;
VarTrack	g_vtBloodSplatsMinNum;
VarTrack	g_vtBloodSplatsMaxNum;
VarTrack	g_vtBloodSplatsMinLifetime;
VarTrack	g_vtBloodSplatsMaxLifetime;
VarTrack	g_vtBloodSplatsMinScale;
VarTrack	g_vtBloodSplatsMaxScale;
VarTrack	g_vtBloodSplatsRange;
VarTrack	g_vtBloodSplatsPerturb;
VarTrack	g_vtCreatePolyDebris;
VarTrack	g_vtWeaponFXMinImpactDot;
VarTrack	g_vtWeaponFXMinFireDot;
VarTrack	g_vtWeaponFXUseFOVPerformance;
VarTrack	g_vtWeaponFXMaxFireDist;
VarTrack	g_vtWeaponFXMaxImpactDist;
VarTrack	g_vtWeaponFXMaxMultiImpactDist;
VarTrack	g_vtMultiDing;

LTBOOL		g_bCanSeeImpactPos	= LTTRUE;
LTBOOL		g_bCanSeeFirePos	= LTTRUE;
LTBOOL		g_bDistantFirePos	= LTFALSE;
LTBOOL		g_bDistantImpactPos	= LTFALSE;

#define BULLETTRAIL_STARTPOS_OFFSET 3.5f

//Function to handle filtering of the intersect segment calls needed by the blood splats
bool BloodSplatFilterFn(HOBJECT hTest, void *pUserData)
{
	// Check for the object type. We only want to be blocked by world models 
	uint32 nObjType;
	if(g_pLTClient->Common()->GetObjectType(hTest, &nObjType) != LT_OK)
		return false;

	if(nObjType != OT_WORLDMODEL)
		return false;

    return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::Init
//
//	PURPOSE:	Init the weapon fx
//
// ----------------------------------------------------------------------- //

LTBOOL CWeaponFX::Init(HLOCALOBJ hServObj, ILTMessage_Read *pMsg)
{
    if (!CSpecialFX::Init(hServObj, pMsg)) return LTFALSE;
    if (!pMsg) return LTFALSE;

	WCREATESTRUCT w;
	memset( &w, 0, sizeof(WCREATESTRUCT) );

	w.hServerObj	= hServObj;
	//w.hObjectHit	= pMsg->ReadObject();
    /////////////w.hFiredFrom    = pMsg->ReadObject();		// 광식
	int nShooter	= pMsg->Readuint8();
	CPlayerInfoMgr* pInfoMgr = g_pInterfaceMgr->GetPlayerInfoMgr();
	if( pInfoMgr )
	{
		if( nShooter == 255 )
		{
			w.hFiredFrom = LTNULL;
		}
		else
		{
			HOBJECT hShooterObj = pInfoMgr->GetHandleByID(nShooter);
			if( !hShooterObj ) return LTFALSE;

			w.hFiredFrom = hShooterObj;
		}
	}

    w.nWeaponId     = pMsg->Readuint8();

    //[MURSUM] REM 
	//w.nAmmoId       = pMsg->Readuint8();
    //w.nSurfaceType  = pMsg->Readuint8();
    /////////////w.wIgnoreFX     = pMsg->Readuint16();		// 광식
	WEAPON const* pWeapon = g_pWeaponMgr->GetWeapon(w.nWeaponId);
	if( pWeapon )
	{
		if( pWeapon->nAniType == 6 )	// Time bomb
		{
			w.wIgnoreFX     = 0;
		}
		else
		{
			w.wIgnoreFX     = 24064;
			int nRandom = GetRandom(0,3);
			if( nRandom != 3)
				w.wIgnoreFX |= WFX_TRACER;
		}
	}
	else
	{
		w.wIgnoreFX     = 24064;
		int nRandom = GetRandom(0,3);
		if( nRandom != 3)
			w.wIgnoreFX |= WFX_TRACER;
	}

    //w.nShooterId    = pMsg->Readuint8();	
    //w.vFirePos		= pMsg->ReadCompLTVector();
    //w.vPos			= pMsg->ReadCompLTVector();
    //w.vSurfaceNormal	= pMsg->ReadCompLTVector();
	// 임시 Test	
	//w.vFirePos		= pMsg->ReadCompPos();
	g_pLTClient->GetObjectPos(w.hFiredFrom, &w.vFirePos);
	/*
    if( pWeapon )
	{
		if( pWeapon->nAniType == 3 || pWeapon->nAniType == 6 )
		{
			w.vPos			= pMsg->ReadCompPos();
		}
		else
		{
			w.vPos			= pMsg->ReadLTVector();
		}
	}
	else
	{
		w.vPos			= pMsg->ReadCompPos();	
	}
	*/
	w.vPos			= pMsg->ReadCompPos();
    //w.vSurfaceNormal	= pMsg->ReadCompPos();
	//---------------------------------------------------------------------------------------------------
	SurfaceType eTempType;
	CClientWeaponMgr* pWeaponMgr;
	IClientWeaponBase* pWeaponBase;
	pWeaponMgr  = g_pPlayerMgr->GetClientWeaponMgr();
	if( !pWeaponMgr ) return LTFALSE;
	pWeaponBase = pWeaponMgr->GetCurrentClientWeapon();
	if( !pWeaponBase ) return LTFALSE;

	pWeaponBase->GetImpactSurface(w.hFiredFrom,
								  w.nWeaponId,
								  w.vFirePos,
								  w.vPos,
								  w.vSurfaceNormal,
								  eTempType, 
								  w.hObjectHit);

	if( eTempType == ST_HEAD || eTempType == ST_ARMOR || eTempType == ST_SKY )
	{
		w.wIgnoreFX |= WFX_MARK;
	}

	w.nSurfaceType = eTempType;	

	//[SPIKE] 투척무기일때 SurfaceType 알아오기=================================
	if(g_pWeaponMgr->GetWeapon(w.nWeaponId)->m_nFireType == 2)
	{
		// throw an intersect segment to determine where we really hit
		IntersectInfo iInfo;
		IntersectQuery qInfo;

		qInfo.m_Flags = INTERSECT_HPOLY | INTERSECT_OBJECTS | IGNORE_NONSOLID;
		qInfo.m_From	  = w.vPos;
		LTVector vUp(0.0f, -1.0f, 0.0f), vTo;
		vTo = w.vPos + (vUp * 100.0f);
		qInfo.m_To		  = vTo;
		if (g_pLTClient->IntersectSegment(&qInfo, &iInfo))
		{
			w.nSurfaceType   = GetSurfaceType(iInfo);
			w.vSurfaceNormal = iInfo.m_Plane.m_Normal;
		}
	}
	//===========================================================================

	uint32 nLocalId = 0;
	g_pLTClient->GetLocalClientID(&nLocalId);
	HOBJECT hLocalObj = g_pLTClient->GetClientObject();
	if( hLocalObj != w.hFiredFrom )
		w.nShooterId = nLocalId + 1;
	else
		w.nShooterId = nLocalId;
	//---------------------------------------------------------------------------------------------------

//    w.eImpactType	= static_cast< IMPACT_TYPE >( pMsg->Readuint8() );	
    w.eImpactType	= IMPACT_TYPE_IMPACT;	

	// make sure the impact type is valid
    ASSERT( ( 0 <= w.eImpactType ) && ( IMPACT_TYPE_COUNT > w.eImpactType ) );

	return Init(&w);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::Init
//
//	PURPOSE:	Init the weapon fx
//
// ----------------------------------------------------------------------- //

LTBOOL CWeaponFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CSpecialFX::Init(psfxCreateStruct)) return LTFALSE;

	WCREATESTRUCT* pCS = (WCREATESTRUCT*)psfxCreateStruct;

	m_nWeaponId		= pCS->nWeaponId;
	//[MURSUM] REM
	//m_nAmmoId		= pCS->nAmmoId;
	m_eSurfaceType	= (SurfaceType)pCS->nSurfaceType;
	m_wIgnoreFX		= pCS->wIgnoreFX;

	m_hObjectHit	 = pCS->hObjectHit;
    m_hFiredFrom     = pCS->hFiredFrom; // LTNULL
	m_vFirePos		 = pCS->vFirePos;
	m_vPos			 = pCS->vPos;
	m_vSurfaceNormal = pCS->vSurfaceNormal;
	m_vSurfaceNormal.Normalize();

	m_eCode			= CC_NO_CONTAINER;
	m_eFirePosCode	= CC_NO_CONTAINER;

	m_pWeapon = g_pWeaponMgr->GetWeapon(m_nWeaponId);
    if (!m_pWeapon) return LTFALSE;

	//[MURSUM] REM
	//m_pAmmo = g_pWeaponMgr->GetAmmo(m_nAmmoId);
    //if (!m_pAmmo) return LTFALSE;

	if( !m_pWeapon->aAmmoIds ) return LTFALSE;
	m_nAmmoId = (*m_pWeapon->aAmmoIds);
	m_pAmmo = g_pWeaponMgr->GetAmmo(m_nAmmoId);
    if (!m_pAmmo) return LTFALSE;

    m_fInstDamage   = (LTFLOAT) m_pAmmo->nInstDamage;
    m_fAreaDamage   = (LTFLOAT) m_pAmmo->nAreaDamage;

	m_nShooterId	= pCS->nShooterId;
	m_bLocal		= pCS->bLocal;

	// type of impact FX to play
	m_eImpactType	= pCS->eImpactType;

	if (!g_cvarShowFirePath.IsInitted())
	{
		g_cvarShowFirePath.Init(g_pLTClient, "ShowFirePath", NULL, -1.0f);
    }

	if (!g_cvarLightBeamColorDelta.IsInitted())
	{
		g_cvarLightBeamColorDelta.Init(g_pLTClient, "LightBeamColorDelta", NULL, 50.0f);
	}

	if (!g_cvarImpactPitchShift.IsInitted())
	{
		g_cvarImpactPitchShift.Init(g_pLTClient, "PitchShiftImpact", NULL, -1.0f);
	}

	if (!g_cvarFlyByRadius.IsInitted())
	{
		g_cvarFlyByRadius.Init(g_pLTClient, "FlyByRadius", NULL, 600.0f);
	}

	if (!g_vtBloodSplatsMinNum.IsInitted())
	{
		g_vtBloodSplatsMinNum.Init(g_pLTClient, "BloodSplatsMinNum", NULL, 3.0f);
	}

	if (!g_vtBloodSplatsMaxNum.IsInitted())
	{
		g_vtBloodSplatsMaxNum.Init(g_pLTClient, "BloodSplatsMaxNum", NULL, 5.0f);
	}

	if (!g_vtBloodSplatsMinLifetime.IsInitted())
	{
		g_vtBloodSplatsMinLifetime.Init(g_pLTClient, "BloodSplatsMinLifetime", NULL, 2.0f);
	}

	if (!g_vtBloodSplatsMaxLifetime.IsInitted())
	{
		g_vtBloodSplatsMaxLifetime.Init(g_pLTClient, "BloodSplatsMaxLifetime", NULL, 5.0f);
	}

	if (!g_vtBloodSplatsMinScale.IsInitted())
	{
		g_vtBloodSplatsMinScale.Init(g_pLTClient, "BloodSplatsMinScale", NULL, 0.08f);
	}

	if (!g_vtBloodSplatsMaxScale.IsInitted())
	{
		g_vtBloodSplatsMaxScale.Init(g_pLTClient, "BloodSplatsMaxScale", NULL, 0.16f);
	}

	if (!g_vtBloodSplatsRange.IsInitted())
	{
		g_vtBloodSplatsRange.Init(g_pLTClient, "BloodSplatsRange", NULL, 200.0f);
	}

	if (!g_vtBloodSplatsPerturb.IsInitted())
	{
		g_vtBloodSplatsPerturb.Init(g_pLTClient, "BloodSplatsPerturb", NULL, 200.0f);
	}

	if (!g_vtCreatePolyDebris.IsInitted())
	{
		g_vtCreatePolyDebris.Init(g_pLTClient, "CreatePolyDebris", NULL, 1.0f);
	}

	if (!g_vtWeaponFXMinFireDot.IsInitted())
	{
		g_vtWeaponFXMinFireDot.Init(g_pLTClient, "WeaponFXMinFireDot", NULL, 0.6f);
	}

	if (!g_vtWeaponFXMinImpactDot.IsInitted())
	{
		g_vtWeaponFXMinImpactDot.Init(g_pLTClient, "WeaponFXMinImpactDot", NULL, 0.6f);
	}

	if (!g_vtWeaponFXUseFOVPerformance.IsInitted())
	{
		g_vtWeaponFXUseFOVPerformance.Init(g_pLTClient, "WeaponFXUseFOVPerformance", NULL, 1.0f);
	}

	if (!g_vtWeaponFXMaxFireDist.IsInitted())
	{
		g_vtWeaponFXMaxFireDist.Init(g_pLTClient, "WeaponFXMaxFireDist", NULL, 1000.0f);
	}

	if (!g_vtWeaponFXMaxImpactDist.IsInitted())
	{
		g_vtWeaponFXMaxImpactDist.Init(g_pLTClient, "WeaponFXMaxImpactDist", NULL, 1000.0f);
	}

	if (!g_vtWeaponFXMaxMultiImpactDist.IsInitted())
	{
		g_vtWeaponFXMaxMultiImpactDist.Init(g_pLTClient, "WeaponFXMaxMultiImpactDist", NULL, 300.0f);
	}

	if (!g_vtMultiDing.IsInitted())
	{
		g_vtMultiDing.Init(g_pLTClient, "WeaponFXMultiImpactDing", NULL, 1.0f);
	}

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateObject
//
//	PURPOSE:	Create the various fx
//
// ----------------------------------------------------------------------- //

LTBOOL CWeaponFX::CreateObject(ILTClient* pClientDE)
{
    if (!CSpecialFX::CreateObject(pClientDE) || !g_pWeaponMgr) return LTFALSE;

	CGameSettings* pSettings = g_pInterfaceMgr->GetSettings();
    if (!pSettings) return LTFALSE;

	// Make sure the parent has updated their attachments.
	g_pLTClient->ProcessAttachments( m_hFiredFrom );
	
	// Set up our data members...

	// Set the local client id...
    uint32 dwId;
    g_pLTClient->GetLocalClientID(&dwId);
    m_nLocalId = (uint8)dwId;

	m_nDetailLevel = pSettings->SpecialFXSetting();

	// Fire pos may get tweaked a little...
	m_vFirePos = CalcFirePos(m_vFirePos);

	m_vDir = m_vPos - m_vFirePos;
	m_fFireDistance = m_vDir.Length();
	m_vDir.Normalize();
	m_rSurfaceRot = LTRotation(m_vSurfaceNormal, LTVector(0.0f, 1.0f, 0.0f));
	m_rDirRot = LTRotation(m_vDir, LTVector(0.0f, 1.0f, 0.0f));

	SetupExitInfo();


	// Calculate if the camera can see the fire position and the impact
	// position...

	g_bCanSeeImpactPos	= LTTRUE;
	g_bCanSeeFirePos	= LTTRUE;
	g_bDistantImpactPos	= LTFALSE;
	g_bDistantFirePos	= LTFALSE;

	if (g_vtWeaponFXUseFOVPerformance.GetFloat())
	{
		HOBJECT hCamera = g_pPlayerMgr->GetCamera();
		LTVector vCameraPos, vF, vDir;
		LTRotation rCameraRot;
		g_pLTClient->GetObjectPos(hCamera, &vCameraPos);
		g_pLTClient->GetObjectRotation(hCamera, &rCameraRot);
		vF = rCameraRot.Forward();

		vDir = m_vPos - vCameraPos;
		LTFLOAT fImpactDist = vDir.Length();
		LTFLOAT fMaxTemp = g_vtWeaponFXMaxImpactDist.GetFloat();

		if (fImpactDist > g_vtWeaponFXMaxImpactDist.GetFloat())
		{
			g_bDistantImpactPos = LTTRUE;
		}

		vDir.Normalize();

		LTFLOAT fMul = vDir.Dot(vF);
		g_bCanSeeImpactPos = (fMul < g_vtWeaponFXMinImpactDot.GetFloat() ? LTFALSE : LTTRUE);

		//[SPIKE] 투척무기일때는 예외! 뒤에서 터져도 느껴져야 하기 때문에 ================
		if(g_pWeaponMgr->GetWeapon(m_nWeaponId)->m_nFireType == 2) 
			g_bCanSeeImpactPos = LTTRUE;
		//================================================================================

		// In multiplayer we need to account for impacts that occur around
		// our camera that we didn't cause (this is also an issue in single
		// player, but due to the singler player gameplay dynamics it isn't
		// as noticeable)...

		if (!g_bCanSeeImpactPos && IsMultiplayerGame())
		{
			// Somebody else shot this...if the impact is close enough, we 
			// "saw" it...
			if (m_nLocalId != m_nShooterId && fImpactDist <= g_vtWeaponFXMaxMultiImpactDist.GetFloat())
			{
				g_bCanSeeImpactPos = LTTRUE;
			}
		}
		vDir = m_vFirePos - vCameraPos;

		if (vDir.Length() > g_vtWeaponFXMaxFireDist.GetFloat())
		{
			g_bDistantFirePos = LTTRUE;
		}
		vDir.Normalize();

		fMul = vDir.Dot(vF);
		float fTemp = g_vtWeaponFXMinFireDot.GetFloat();
		g_bCanSeeFirePos = (fMul < g_vtWeaponFXMinFireDot.GetFloat() ? LTFALSE : LTTRUE);

	
//[SPIKE] MuzzleFlush 2개 보이는 버그 수정=================== 
// FirePos와 CameraPos가 같으면... FALSE로 세팅 		
		if(vDir.Length() == 0.0f) g_bCanSeeFirePos = LTFALSE;
//==========================================================
	}

	// Determine what container the sfx is in...
	HLOCALOBJ objList[1];
	LTVector vTestPos = m_vPos;  // Test a little closer...
    uint32 dwNum = ::GetPointContainers(vTestPos, objList, 1, ::GetLiquidFlags());

	if (dwNum > 0 && objList[0])
	{
        uint16 dwCode;
        if (g_pLTClient->GetContainerCode(objList[0], &dwCode))
		{
			m_eCode = (ContainerCode)dwCode;
		}
	}

	// Determine if the fire point is in liquid
	vTestPos = m_vFirePos + m_vDir;  // Test a little further in...
    dwNum = ::GetPointContainers(vTestPos, objList, 1, ::GetLiquidFlags());

	if (dwNum > 0 && objList[0])
	{
        uint16 dwCode;
        if (g_pLTClient->GetContainerCode(objList[0], &dwCode))
		{
			m_eFirePosCode = (ContainerCode)dwCode;
		}
	}

	if (IsLiquid(m_eCode))
	{
		m_wImpactFX	= m_pAmmo->pUWImpactFX ? m_pAmmo->pUWImpactFX->nFlags : 0;	
	}
	else
	{
		m_wImpactFX	= m_pAmmo->pImpactFX ? m_pAmmo->pImpactFX->nFlags : 0;
	}

	m_wFireFX = m_pAmmo->pFireFX ? m_pAmmo->pFireFX->nFlags : 0;

	// Assume alt-fire, silenced, and tracer...these will be cleared by

	// IgnoreFX if not used...
 	m_wFireFX |= WFX_ALTFIRESND | WFX_SILENCED | WFX_TRACER | WFX_MUZZLE;

	// Assume impact ding, it will be cleared if not used...
	m_wImpactFX |= WFX_IMPACTDING;

	// Clear all the fire fx we want to ignore...
	m_wFireFX &= ~m_wIgnoreFX;
	m_wImpactFX &= ~m_wIgnoreFX;

	// See if this is a redundant weapon fx (i.e., this client shot the
	// weapon so they've already seen this fx)...

	/*
	if ( g_pClientMultiplayerMgr->IsConnectedToRemoteServer( ))
	{
		if (m_pAmmo->eType != PROJECTILE)
		{
			if (!m_bLocal && m_nLocalId >= 0 && m_nLocalId == m_nShooterId)
			{
				if (m_wImpactFX & WFX_IMPACTDING)
				{
					if (g_vtMultiDing.GetFloat())
					{
						PlayImpactDing();
					}
				}

                return LTFALSE;
			}
		}
	}
	*/
	if( !m_bLocal &&
		m_nLocalId >= 0 &&
		m_nLocalId == m_nShooterId &&
		m_pAmmo->eType == VECTOR && 
		m_eSurfaceType != ST_FLESH &&
		m_eSurfaceType != ST_HEAD &&
		m_eSurfaceType != ST_ARMOR &&
		g_pClientMultiplayerMgr->IsConnectedToRemoteServer() ) return LTFALSE;

	// Show the fire path...(debugging...)
	if (g_cvarShowFirePath.GetFloat() > 0)
	{
		PLFXCREATESTRUCT pls;
		pls.vStartPos			= m_vFirePos;
		pls.vEndPos				= m_vPos;
        pls.vInnerColorStart    = LTVector(GetRandom(127.0f, 255.0f), GetRandom(127.0f, 255.0f), GetRandom(127.0f, 255.0f));
		pls.vInnerColorEnd		= pls.vInnerColorStart;
        pls.vOuterColorStart    = LTVector(0, 0, 0);
        pls.vOuterColorEnd      = LTVector(0, 0, 0);
		pls.fAlphaStart			= 1.0f;
		pls.fAlphaEnd			= 1.0f;
		pls.fMinWidth			= 0;
		pls.fMaxWidth			= 10;
		pls.fMinDistMult		= 1.0f;
		pls.fMaxDistMult		= 1.0f;
		pls.fLifeTime			= 10.0f;
		pls.fAlphaLifeTime		= 10.0f;
		pls.fPerturb			= 0.0f;
        pls.bAdditive           = LTFALSE;
		pls.nWidthStyle			= PLWS_CONSTANT;
		pls.nNumSegments		= 2;

		CSpecialFX* pFX = g_pGameClientShell->GetSFXMgr()->CreateSFX(SFX_POLYLINE_ID, &pls);
		if (pFX) pFX->Update();
	}

	/*
	if( m_eSurfaceType == ST_SKY )
	{
		if(g_pWeaponMgr->GetWeapon(m_nWeaponId)->m_nFireType == 2 ||	//투척
			g_pWeaponMgr->GetWeapon(m_nWeaponId)->m_nFireType == 3 )	//C4
		{
			//[MURSUM] 땜빵!!
			m_eSurfaceType = ST_AIR;
		}
	}
	*/

	if (IsLiquid(m_eCode) || m_eSurfaceType != ST_SKY || (m_wImpactFX & WFX_IMPACTONSKY) || DT_GADGET_TIME_BOMB == m_pAmmo->eInstDamageType)
	{
		CreateWeaponSpecificFX();

		if (g_bCanSeeImpactPos)
		{
			if ((m_wImpactFX & WFX_MARK) && ShowsMark(m_eSurfaceType) && (LTBOOL)GetConsoleInt("MarkShow", 1))
			{
				LTBOOL bCreateMark = LTTRUE;
				if (g_bDistantImpactPos && m_nLocalId == m_nShooterId)
				{
					// Assume we'll see the mark if we're zoomed in ;)
					bCreateMark = g_pPlayerMgr->IsZoomed();
				}

				if (bCreateMark)
				{
					CreateMark(m_vPos, m_vSurfaceNormal, m_rSurfaceRot, m_eSurfaceType);
				}
			}
			CreateSurfaceSpecificFX();
		}

		PlayImpactSound();
	}


	if (IsBulletTrailWeapon())
	{	
		if (m_nDetailLevel != RS_LOW)
		{
			if( m_bLocal )
			{
				//[SPIKE] 물속성은 내부에서 검사 
				LTVector vRealFirePos;
				LTRotation rCameraRot;
				g_pLTClient->GetObjectRotation(g_pPlayerMgr->GetCamera(), &rCameraRot);
				vRealFirePos = m_vFirePos + (rCameraRot.Right()*BULLETTRAIL_STARTPOS_OFFSET);
				vRealFirePos += rCameraRot.Forward()*BULLETTRAIL_STARTPOS_OFFSET*8.0f; 
				vRealFirePos -= rCameraRot.Up()*BULLETTRAIL_STARTPOS_OFFSET;

				CreateBulletTrail(vRealFirePos); 
			}
			else
			{
				CreateBulletTrail(m_vFirePos);
				PlayUWBulletFlyBySound();
			}
			//===============================================
		}
	}


	// No tracers under water...
 	if ((LTBOOL)GetConsoleInt("Tracers", 1) && (m_wFireFX & WFX_TRACER) && !IsLiquid(m_eCode))
	{
		CreateTracer();
	}

	// 광식 [04.11.05] 탄피가 안튀어서... 앞으로 뛸 때 탄피 안튀게 되어 있음
	//if (g_bCanSeeFirePos)
	{	
	//[SPIKE] 샷건류 Muzzle은 중복되지 않게 =================== 
	if( m_wFireFX & WFX_MUZZLE ) CreateMuzzleFX();
	//==========================================================

		if (!g_bDistantFirePos && (LTBOOL)GetConsoleInt("ShellCasings", 1) && (m_wFireFX & WFX_SHELL))
		{
			CreateShell();
		}
	}

	if ((m_wFireFX & WFX_FIRESOUND) || (m_wFireFX & WFX_ALTFIRESND) || (m_wFireFX & WFX_SILENCED))
	{
		if( !(m_wIgnoreFX&WFX_MUZZLE) )	PlayFireSound();
	}
	// Only do fly-by sounds for weapons that leave bullet trails...that
	// we didn't fire ;)
    return LTFALSE;  // Just delete me, I'm done :)
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::SetupExitInfo
//
//	PURPOSE:	Setup our exit info
//
// ----------------------------------------------------------------------- //

void CWeaponFX::SetupExitInfo()
{
	m_eExitSurface	= ST_UNKNOWN;
	m_vExitPos		= m_vFirePos;
	m_vExitNormal	= m_vDir;
	m_eExitCode		= CC_NO_CONTAINER;
	if (m_nDetailLevel == RS_LOW) return;

	// Determine if there is an "exit" surface...
	IntersectQuery qInfo;
	IntersectInfo iInfo;

	qInfo.m_From = m_vFirePos + m_vDir;
	qInfo.m_To   = m_vFirePos - m_vDir;
	qInfo.m_Flags = INTERSECT_OBJECTS | IGNORE_NONSOLID | INTERSECT_HPOLY;

    if (g_pLTClient->IntersectSegment(&qInfo, &iInfo))
	{
		m_eExitSurface	= GetSurfaceType(iInfo);
		m_vExitNormal	= iInfo.m_Plane.m_Normal;
		m_vExitPos		= iInfo.m_Point + m_vDir;

		// Determine what container the sfx is in...
		HLOCALOBJ objList[1];
        LTVector vTestPos = m_vExitPos + m_vExitNormal;  // Test a little closer...
        uint32 dwNum = ::GetPointContainers(vTestPos, objList, 1, ::GetLiquidFlags());

		if (dwNum > 0 && objList[0])
		{
			uint16 dwCode;
            if (g_pLTClient->GetContainerCode(objList[0], &dwCode))
			{
				m_eExitCode = (ContainerCode)dwCode;
			}
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateExitDebris
//
//	PURPOSE:	Create any exit debris
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateExitDebris()
{
	// Create the surface specific exit fx...
	SURFACE* pSurf = g_pSurfaceMgr->GetSurface(m_eExitSurface);
	if (!pSurf || !pSurf->bCanShootThrough) return;

	if (IsLiquid(m_eExitCode))
	{
		// Create underwater fx...
		CLIENTFX_CREATESTRUCT	fxCS( pSurf->szUWExitFXName, 0, m_vPos );
		fxCS.m_vTargetNorm = m_vSurfaceNormal;
		g_pClientFXMgr->CreateClientFX(NULL, fxCS, LTTRUE );
	}
	else
	{
		// Create normal fx...
		CLIENTFX_CREATESTRUCT	fxCS( pSurf->szExitFXName, 0, m_vPos );
		fxCS.m_vTargetNorm = m_vSurfaceNormal;
		g_pClientFXMgr->CreateClientFX(NULL, fxCS, LTTRUE );
	}

	// Determine if we should create a beam of light through the surface...
	// CreateLightBeamFX(pSurf);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateLightBeamFX
//
//	PURPOSE:	Create a light beam (if appropriate)
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateLightBeamFX(SURFACE* pSurf)
{
	if (!pSurf) return;

    LTVector vEnterColor, vExitColor;
    if (g_pLTClient->GetPointShade(&m_vExitPos, &vExitColor) == LT_OK)
	{
		// Get the EnterColor value...
        LTVector vEnterPos = m_vExitPos - (m_vExitNormal * float(pSurf->nMaxShootThroughThickness + 1));

        if (g_pLTClient->GetPointShade(&vEnterPos, &vEnterColor) == LT_OK)
		{
			// Calculate the difference in light value...
            LTFLOAT fMaxEnter = Max(vEnterColor.x, vEnterColor.y);
			fMaxEnter = Max(fMaxEnter, vEnterColor.z);

            LTFLOAT fMaxExit = Max(vExitColor.x, vExitColor.y);
			fMaxExit = Max(fMaxExit, vExitColor.z);

			if (fabs((double)(fMaxExit - fMaxEnter)) >= g_cvarLightBeamColorDelta.GetFloat())
			{
                LTVector vStartPoint, vDir;
				if (fMaxEnter > fMaxExit)
				{
					vStartPoint = m_vExitPos;
					vDir = m_vDir;
				}
				else
				{
					vStartPoint = vEnterPos;
					vDir = -m_vDir;
				}

				PLFXCREATESTRUCT pls;
				pls.vStartPos			= vStartPoint;
				pls.vEndPos				= vStartPoint + (vDir * GetRandom(100.0, 150.0f));
                pls.vInnerColorStart    = LTVector(230, 230, 230);
				pls.vInnerColorEnd		= pls.vInnerColorStart;
                pls.vOuterColorStart    = LTVector(0, 0, 0);
                pls.vOuterColorEnd      = LTVector(0, 0, 0);
				pls.fAlphaStart			= 0.5f;
				pls.fAlphaEnd			= 0.0f;
				pls.fMinWidth			= 0;
				pls.fMaxWidth			= 10;
				pls.fMinDistMult		= 1.0f;
				pls.fMaxDistMult		= 1.0f;
				pls.fLifeTime			= 10.0f;
				pls.fAlphaLifeTime		= 10.0f;
				pls.fPerturb			= 0.0f;
                pls.bAdditive           = LTFALSE;
				pls.nWidthStyle			= PLWS_CONSTANT;
				pls.nNumSegments		= 1;

				CSpecialFX* pFX = g_pGameClientShell->GetSFXMgr()->CreateSFX(SFX_POLYLINE_ID, &pls);
				if (pFX) pFX->Update();
			}
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateExitMark
//
//	PURPOSE:	Create any exit surface marks
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateExitMark()
{
	if (m_eExitSurface != ST_UNKNOWN && ShowsMark(m_eExitSurface))
	{
        LTRotation rNormRot(m_vExitNormal, LTVector(0.0f, 1.0f, 0.0f));

		CreateMark(m_vExitPos, m_vExitNormal, rNormRot, m_eExitSurface);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateMark
//
//	PURPOSE:	Create a mark fx
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateMark(const LTVector &vPos, const LTVector &vNorm, const LTRotation &rRot,
						   SurfaceType eType)
{
	IMPACTFX* pImpactFX = m_pAmmo->pImpactFX;

	if (IsLiquid(m_eCode))
	{
		pImpactFX = m_pAmmo->pUWImpactFX;
	}

	if (!pImpactFX) return;

	CSFXMgr* psfxMgr = g_pGameClientShell->GetSFXMgr();
	if (!psfxMgr) return;

	MARKCREATESTRUCT mark;
	mark.m_vPos = vPos;
	mark.m_Rotation = rRot;


//[SPIKE] 보는 방향에 맞게 Mark에 회전을~	==========================================

// Randomly rotate the bullet hole...

// mark.m_Rotation.Rotate(vNorm, GetRandom(0.0f, MATH_CIRCLE));

	if(fabs(vNorm.y) > 0.1f)
	{		
		float fRoll = 0.0f;

		LTVector vDir = m_vDir;
		vDir.y = 0.0f;
		vDir.Normalize();

		LTVector vX(1.0f, 0.0f, 0.0f);
		fRoll = (LTFLOAT)acos(vDir.Dot(vX)); 
		if(vDir.z > 0.0f ) fRoll *= -1;
		if(vNorm.y < 0.0f) fRoll *= -1;

		mark.m_Rotation.Rotate(vNorm, fRoll);
	}
//=========================================================================
	mark.m_fScale		= pImpactFX->fMarkScale;
	mark.nAmmoId		= m_nAmmoId;
	mark.nSurfaceType   = eType;

	psfxMgr->CreateSFX(SFX_MARK_ID, &mark);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateBulletTrail
//
//	PURPOSE:	Create a bullet trail fx
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateBulletTrail(const LTVector &vStartPos)
{
	CSFXMgr* psfxMgr = g_pGameClientShell->GetSFXMgr();
	if (!psfxMgr) return;

    LTVector vColor1, vColor2;
	vColor1.Init(200.0f, 200.0f, 200.0f);
	vColor2.Init(255.0f, 255.0f, 255.0f);

	BTCREATESTRUCT bt;
	bt.vStartPos		= vStartPos;
	bt.vDir				= m_vDir;
	bt.vColor1			= vColor1;
	bt.vColor2			= vColor2;
	bt.fLifeTime		= 0.5f;
	bt.fFadeTime		= 0.3f;
	bt.fRadius			= 400.0f;
	bt.fGravity			= 0.0f;
	bt.fNumParticles	= (m_nDetailLevel == RS_MED) ? 15.0f : 30.0f;

	CSpecialFX* pFX = psfxMgr->CreateSFX(SFX_BULLETTRAIL_ID, &bt);
	// Let each bullet trail do its initial update...
	if (pFX) pFX->Update();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateTracer
//
//	PURPOSE:	Create a tracer fx
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateTracer()
{
	if (!m_pAmmo || !m_pAmmo->pTracerFX) return;

	CCharacterFX* pFX = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(m_hFiredFrom);
	if( pFX && pFX->IsBodyInLiquid() )
	{
		return;
	}

	CSFXMgr* psfxMgr = g_pGameClientShell->GetSFXMgr();
	if (!psfxMgr) return;

	//if (m_nDetailLevel != RS_HIGH && GetRandom(1, 2) == 1) return;

	if (m_nLocalId >= 0 && m_nLocalId == m_nShooterId) return;

	// Create tracer...
	if (m_fFireDistance > 100.0f)
	{
		TRCREATESTRUCT tracer;
		// Make tracer start in front of gun a little ways...
		tracer.vStartPos	= m_vFirePos + (m_vDir * 50.0f);
		tracer.vEndPos		= m_vPos;
		tracer.pTracerFX	= m_pAmmo->pTracerFX;

		CSpecialFX* pFX = psfxMgr->CreateSFX(SFX_TRACER_ID, &tracer);
		if (pFX) pFX->Update();
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateWeaponSpecificFX()
//
//	PURPOSE:	Create weapon specific fx
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateWeaponSpecificFX()
{
	// Do fire fx beam fx...
	if (m_pAmmo->pFireFX && m_pAmmo->pFireFX->szBeamFXName )
	{
		CLIENTFX_CREATESTRUCT fxInit( m_pAmmo->pFireFX->szBeamFXName, 0, m_vFirePos );
		fxInit.m_bUseTargetData = true;
		fxInit.m_vTargetPos = m_vPos;
		g_pClientFXMgr->CreateClientFX( LTNULL, fxInit, LTTRUE );	
	}

	// Only do impact fx if the client can see the impact position
	// or the impact fx may last a little while...
	if (g_bCanSeeImpactPos || m_pAmmo->fProgDamage > 0.0f || m_pAmmo->nAreaDamage > 0)
	{
		IFXCS cs;
		cs.eCode		= m_eCode;
		cs.eSurfType	= m_eSurfaceType;
		cs.rSurfRot		= m_rSurfaceRot;
		cs.vDir			= m_vDir;
		cs.vPos			= m_vPos;
		cs.vSurfNormal	= m_vSurfaceNormal;
		cs.fBlastRadius = (LTFLOAT) m_pAmmo->nAreaDamageRadius;
		cs.fTintRange   = (LTFLOAT) (m_pAmmo->nAreaDamageRadius * 5);

		//determine if the object we hit is a model 
		bool bHitMovable = false;

		if(m_hObjectHit)
		{
			//we hit an object, assume it is movable
			bHitMovable = true;

			uint32 nType;
			if(g_pLTClient->Common()->GetObjectType(m_hObjectHit, &nType) == LT_OK)
			{
				//we only want to hit non-movable world models
				if(nType == OT_WORLDMODEL)
				{
					uint32 nUserFlags;
					if(g_pLTClient->Common()->GetObjectFlags(m_hObjectHit, OFT_User, nUserFlags) == LT_OK)
					{
						//see if it is movable
						if(!(nUserFlags & USRFLG_MOVEABLE))
						{
							bHitMovable = false;
						}
					}
				}
			}				
		}

		if(bHitMovable && m_pAmmo->pMoveableImpactOverrideFX)
		{
			// Create the model hit effect
			g_pFXButeMgr->CreateImpactFX(m_pAmmo->pMoveableImpactOverrideFX, cs);
		}
		else if (IsLiquid(m_eCode))
		{
			// Create underwater weapon fx...
			g_pFXButeMgr->CreateImpactFX(m_pAmmo->pUWImpactFX, cs);
		}
		else if ( IMPACT_TYPE_RICOCHET == m_eImpactType )
		{
			/*
			// Create a ricochet weapon fx...
			ASSERT( 0 != m_pAmmo->pProjectileFX );
			IMPACTFX *pRicochetFX =
				g_pFXButeMgr->GetImpactFX(
					m_pAmmo->pProjectileFX->szRicochetFXName
				);
			g_pFXButeMgr->CreateImpactFX(pRicochetFX, cs);
			*/
		}
		else if ( IMPACT_TYPE_BLOCKED == m_eImpactType )
		{
			// Create a blocked weapon fx...
			g_pFXButeMgr->CreateImpactFX(m_pAmmo->pBlockedFX, cs);
		}
		else if ( IMPACT_TYPE_IMPACT == m_eImpactType )
		{
			// Create a ricochet weapon fx...
			g_pFXButeMgr->CreateImpactFX(m_pAmmo->pImpactFX, cs);
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateSurfaceSpecificFX()
//
//	PURPOSE:	Create surface specific fx
//
// ----------------------------------------------------------------------- //


void CWeaponFX::CreateSurfaceSpecificFX()
{
	CUserProfile* pProfile = g_pProfileMgr->GetCurrentProfile();
	if (!pProfile) return;

	// Only check for visibility if we actually have an object...
	if( m_hObjectHit )
	{
		uint32 dwFlags;
		g_pCommonLT->GetObjectFlags( m_hObjectHit, OFT_Flags, dwFlags );

		// Don't do surface fx on an invisible object...
		if( !(dwFlags & FLAG_VISIBLE) )
		{
			//return;
		}
	}

	if (m_eSurfaceType == ST_FLESH || m_eSurfaceType == ST_ARMOR || m_eSurfaceType == ST_HEAD)
	{
		// Only do gore if specified...
		if (pProfile->m_bGore && !g_GameDoc.IsLowViolence())
		{
			if ( m_pAmmo->eType == VECTOR &&
				(m_pAmmo->eInstDamageType == DT_BULLET || 
				 m_pAmmo->eInstDamageType == DT_MELEE ||
				 m_pAmmo->eInstDamageType == DT_SWORD) )
			{
				CreateBloodSplatFX();
			}
			/*//[MURSUM]=================================================
			else if ( m_pAmmo->eType == PROJECTILE &&		// 수류탄
				(m_pAmmo->eInstDamageType == DT_BULLET || 
				 m_pAmmo->eInstDamageType == DT_MELEE ||
				 m_pAmmo->eInstDamageType == DT_SWORD) )
			{
				CreateBloodSplatFXFromProjectile();
			}
			//============================================================
			*/
		}

		/*
		if( stricmp( m_pAmmo->szName, "FlashBang" ) == 0 )
		{
			CreateFlashBangFX();
		}
		*/
	}
	else	//[SPIKE] 가까운 거리에서 쐈을땐 효과 NO!! // 피 보이는건 제외!!!
	{
		LTVector vMySelfPos, vTempUp(0.0f, 40.0f, 0.0f); 
		g_pLTClient->GetObjectPos(g_pLTClient->GetClientObject(), &vMySelfPos);
		LTVector vDist = m_vPos - (vMySelfPos + vTempUp); // 눈위치를 기준으로 계산 
		if(vDist.Length() < 130) return;  
	}

	//[SPIKE]
	if(m_pAmmo->eType == PROJECTILE )
	{
	//	CreatreProjectileMark();  //수류탄 Mark
	}

	if ((m_wFireFX & WFX_EXITMARK) && ShowsMark(m_eExitSurface))
	{
		CreateExitMark();
	}

	if (m_wFireFX & WFX_EXITDEBRIS)
	{
		CreateExitDebris();
	}

	if (!m_pAmmo->pImpactFX) return;
	if (!m_pAmmo->pImpactFX->bDoSurfaceFX) return;

	// Create the surface specific fx...
	SURFACE* pSurf = g_pSurfaceMgr->GetSurface(m_eSurfaceType);

	if (pSurf)
	{
		//[SPIKE] 구조 변경  
		uint8 nIndex = 0;
		if(*(pSurf->szImpactFXName[SRF_MAX_IMPACTFX_COUNT-1]))
		{
			nIndex = GetRandom(0,4);
		}
		CLIENTFX_CREATESTRUCT	fxCS( pSurf->szImpactFXName[nIndex], 0, m_vPos );
		LTRotation rRot;
		rRot.Init();
		if(!pSurf->m_bEnableBulletDir)
		{	
			fxCS.m_vTargetNorm = m_vSurfaceNormal;
		}
		else
		{
			fxCS.m_vTargetNorm	= -m_vDir;
		}

		switch(IsSpriteFX(m_eSurfaceType))
		{
		case FXST_WATER:
			{
				if(m_pAmmo->eType == PROJECTILE)
				{
					nIndex = 0;
					if(*(pSurf->szExplodeImpactFXName[SRF_MAX_IMPACTFX_COUNT-1]))
					{
						nIndex = GetRandom(0,4);
					}
					strcpy(fxCS.m_sName, pSurf->szExplodeImpactFXName[nIndex]);
				}

				rRot = LTRotation(LTVector(0.0f, 0.0f, 1.0f), LTVector(0.0f, 1.0f, 0.0f));
				fxCS.m_rRot			= rRot;
				fxCS.m_vPos			= m_vPos;// + rRot.Forward()*100.0f;
				fxCS.m_hParent		= LTNULL;
				fxCS.m_vTargetNorm.Init();	
				g_pClientFXMgr->CreateClientFX(NULL, fxCS, LTTRUE );
			}
			break;

		case FXST_METAL:
			{
				if(m_pAmmo->eType != PROJECTILE)
				{
					rRot = LTRotation(m_vSurfaceNormal, LTVector(1.0f, 1.0f, 1.0f));
					fxCS.m_rRot			= rRot;
					fxCS.m_vPos			= m_vPos + rRot.Forward()*40.0f;
					fxCS.m_hParent		= LTNULL;
					fxCS.m_vTargetNorm.Init();	
					g_pClientFXMgr->CreateClientFX(NULL, fxCS, LTTRUE );
				}
			}
			break;

		default:
			{	
				if(m_pAmmo->eType != PROJECTILE && !IsLiquid(m_eCode))					
				{
					if(m_eSurfaceType == ST_FLESH || m_eSurfaceType == ST_ARMOR || m_eSurfaceType == ST_HEAD)
					{
						fxCS.m_vPos += m_vDir * 20.0f;  // 효과나오는 위치 조정

						if(m_eSurfaceType != ST_HEAD)
						{
							char szTemp[32]={0};
							sprintf(szTemp, "%d", GetRandom(0,3));						
							strcat(fxCS.m_sName, szTemp);							
						}				
						// 15세버젼이면서 피 효과일때
						if(g_GameDoc.IsLowViolence())
						{
							strcat(fxCS.m_sName, "_T");
						}
					}
					g_pClientFXMgr->CreateClientFX(NULL, fxCS, LTTRUE );
				}
				else if(IsLiquid(m_eCode))
				{
					strcpy(fxCS.m_sName, pSurf->szUWImpactFXName);

					// 15세버젼이면서 피 효과일때
					if(g_GameDoc.IsLowViolence() && (m_eSurfaceType == ST_FLESH || m_eSurfaceType == ST_ARMOR || m_eSurfaceType == ST_HEAD))
					{
						strcat(fxCS.m_sName, "_T");
					}
					//=======================================
					g_pClientFXMgr->CreateClientFX(NULL, fxCS, LTTRUE );	
				}

				/*[SPIKE] 임시주석 
				//[MURSUM]====================================================
				// 어딘가를 뚫고 나왔다.
				// 연기는 총알의 방향
				if( m_wIgnoreFX & WFX_IMPACTOBJ )	
				{
					nIndex = 0;
					if(*(pSurf->szImpactFXName[SRF_MAX_IMPACTFX_COUNT-1]))
					{
						nIndex = GetRandom(0,4);
					}
					memset( &fxCS, 0, sizeof(CLIENTFX_CREATESTRUCT) ); 
					LTStrCpy( fxCS.m_sName, pSurf->szImpactFXName[nIndex], sizeof(fxCS.m_sName) );
					fxCS.m_dwFlags		= 0;
					fxCS.m_vPos			= m_vFirePos;
					fxCS.m_vTargetNorm	= m_vDir;
					fxCS.m_rRot.Init();
					g_pClientFXMgr->CreateClientFX(NULL, fxCS, LTTRUE );

				}	*/
				//=============================================================
			}
			break;
		}		
	}
	else
	{
		//[SPIKE LOG]
		g_pLTClient->CPrint("[SPIKE]WeaponFx.cpp: Missing SurfaceImpactFX!!! <SurfaceType: '%d'>\n", m_eSurfaceType);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateMuzzleLight()
//
//	PURPOSE:	Create a muzzle light associated with this fx
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateMuzzleLight()
{
	// Check to see if we have the silencer...
	if (m_wFireFX & WFX_SILENCED) return;

	if (m_nLocalId != m_nShooterId || !g_pPlayerMgr->IsFirstPerson())
	{
		MUZZLEFLASHCREATESTRUCT mf;
		mf.pWeapon	= m_pWeapon;
		mf.hParent	= m_hFiredFrom;
		mf.vPos		= m_vFirePos;
		mf.rRot		= m_rDirRot;

		CSFXMgr* psfxMgr = g_pGameClientShell->GetSFXMgr();
		if (!psfxMgr) return;

		psfxMgr->CreateSFX(SFX_MUZZLEFLASH_ID, &mf);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::IsBulletTrailWeapon()
//
//	PURPOSE:	See if this weapon creates bullet trails in liquid
//
// ----------------------------------------------------------------------- //

LTBOOL CWeaponFX::IsBulletTrailWeapon()
{
	return (m_pAmmo->eInstDamageType == DT_BULLET);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::PlayImpactSound()
//
//	PURPOSE:	Play a surface impact sound if appropriate
//
// ----------------------------------------------------------------------- //

void CWeaponFX::PlayImpactSound()
{
	IMPACTFX* pImpactFX = m_pAmmo->pImpactFX;

	if (IsLiquid(m_eCode))
	{
		pImpactFX = m_pAmmo->pUWImpactFX;
	}

	if (!pImpactFX) return;

	if (m_pAmmo->eType == VECTOR)
	{
		if ((m_nDetailLevel == RS_LOW) && GetRandom(1, 2) != 1) return;
		else if ((m_nDetailLevel == RS_MED) && GetRandom(1, 3) == 1) return;
	}

	char* pSnd = GetImpactSound(m_eSurfaceType, m_nAmmoId);
    LTFLOAT fSndRadius = (LTFLOAT) pImpactFX->nSoundRadius;

	// 광식 [sound] : 거리를 Check 해서 너무 먼 곳이면 아에 sound play 차체를 하지 않으려고..
	HOBJECT hLocalObj = g_pLTClient->GetClientObject();
	if( hLocalObj )
	{
		LTVector vPos;
		LTVector vResult;
		float	 Mag;
		g_pLTClient->GetObjectPos( hLocalObj, &vPos);
		vResult = vPos - m_vPos;
		Mag = vResult.Mag();
		int random = GetRandom(0, 1);
		if( fSndRadius >= Mag && random )
		{
			if( pSnd && pSnd[0] )
			{
				uint32 dwFlags = 0;
				float fPitchShift = 1.0f;
				if (g_cvarImpactPitchShift.GetFloat() > 0.0f)
				{
					dwFlags |= PLAYSOUND_CTRL_PITCH;
				}

				uint8 nVolume = IsLiquid(m_eCode) ? 50 : 100;
				g_pClientSoundMgr->PlaySoundFromPos((LTVector)m_vPos, pSnd, fSndRadius,
					(m_eSurfaceType==ST_HEAD) ? SOUNDPRIORITY_MISC_HIGH : SOUNDPRIORITY_MISC_LOW,
					dwFlags, nVolume, fPitchShift);
			}
		}
	}
	/*
	if( pSnd && pSnd[0] )
	{
		uint32 dwFlags = 0;
		float fPitchShift = 1.0f;
		if (g_cvarImpactPitchShift.GetFloat() > 0.0f)
		{
			dwFlags |= PLAYSOUND_CTRL_PITCH;
		}

        uint8 nVolume = IsLiquid(m_eCode) ? 50 : 100;
		g_pClientSoundMgr->PlaySoundFromPos((LTVector)m_vPos, pSnd, fSndRadius,
			(m_eSurfaceType==ST_HEAD) ? SOUNDPRIORITY_MISC_HIGH : SOUNDPRIORITY_MISC_LOW,
			dwFlags, nVolume, fPitchShift);
	}
	*/
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateMuzzleFX()
//
//	PURPOSE:	Create muzzle specific fx
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateMuzzleFX()
{
	// 광식 : 내가 죽었을 때 다른 사람한테 붙었다면 효과를 만들면 안됨.
	//-------------------------------------------------------------------
	/*
	if( g_pPlayerMgr->IsPlayerDead() &&
		(m_hFiredFrom == g_pPlayerMgr->GetPlayerCamera()->GetAttachedObject() ||
		 g_pPlayerMgr->GetAttachState() == MID_DETACH_DISPLAY) )
	{
		return;
	}
	*/
	if( g_pPlayerMgr->IsPlayerDead() &&
		m_hFiredFrom == g_pPlayerMgr->GetPlayerCamera()->GetAttachedObject() )
	{
		return;
	}
	//-------------------------------------------------------------------

	// See who we are creating the effect for

	if (m_nLocalId != m_nShooterId && !IsLiquid(m_eFirePosCode))
	{
		//This is the AI shooting, we just need to create an effect as specified
		//by the hand held position
		if(m_pWeapon->szHHMuzzleFxName[0])
		{
			uint8 nIndex = 0;
			if(*(m_pWeapon->szHHMuzzleFxName[2]))
			{
				nIndex = GetRandom(0,2);
			}
			CLIENTFX_CREATESTRUCT	fxcs( m_pWeapon->szHHMuzzleFxName[nIndex], 0, m_vFirePos );

			// 광식
			//-------------------------------------------------------------
			char* szSocket = "Flash";
			HMODELSOCKET hSocket;
			LTransform transform;
			HOBJECT hWeaponObj = g_pGameClientShell->GetSFXMgr()->GetWeaponFromCharacter( m_hFiredFrom );						
			if( g_pModelLT->GetSocket(hWeaponObj, szSocket, hSocket) == LT_OK )
			{				
				if( g_pModelLT->GetSocketTransform(hWeaponObj, hSocket, transform, LTTRUE) == LT_OK )
				{
					//hWeaponObj = LTNULL;
					fxcs.m_rRot			= transform.m_Rot;
					fxcs.m_vPos			= transform.m_Pos + transform.m_Rot.Forward()*25;
					fxcs.m_hParent		= LTNULL;
					g_pClientFXMgr->CreateClientFX(NULL, fxcs, LTTRUE );

					CCharacterFX* pChar = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(m_hFiredFrom);
					if( pChar )	pChar->TurnOnFlashLight( &transform.m_Pos, &transform.m_Rot );
					
				}
			}
			/*
			//-------------------------------------------------------------			
			//fxcs.m_vTargetNorm	= m_vDir;
			//fxcs.m_hParent	= m_hFiredFrom;	// effect를 모델 기준으로 만들도록 변경 [광식]
			fxcs.m_hParent		= g_pGameClientShell->GetSFXMgr()->GetWeaponFromCharacter( m_hFiredFrom );
			g_pClientFXMgr->CreateClientFX(NULL, fxcs, LTTRUE );
			*/
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateShell()
//
//	PURPOSE:	Create shell casing
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateShell()
{
	// 광식
	if( g_pPlayerMgr->IsPlayerDead() &&
		IsRevivePlayerGameType() &&
		g_pPlayerMgr->GetAttachState() == MID_DETACH_DISPLAY )
	return;

	SHELLCREATESTRUCT sc;
	sc.rRot			= m_rDirRot;
	sc.vStartPos	= CalcBreachPos(m_vFirePos);
	sc.nWeaponId	= m_nWeaponId;
	sc.nAmmoId		= m_nAmmoId;
    sc.b3rdPerson	= LTTRUE;
	
	// See if this is our local client who fired and if we're in first
	// person...

	if (m_nLocalId == m_nShooterId && g_pPlayerMgr->IsFirstPerson())
	{
		sc.b3rdPerson = LTFALSE;

		// Add on the player's velocity...
		HOBJECT hObj = g_pPlayerMgr->GetMoveMgr()->GetObject();
		if (hObj)
		{
			g_pPhysicsLT->GetVelocity(hObj, &sc.vStartVel);
		}

		g_pGameClientShell->GetSFXMgr()->CreateSFX(SFX_SHELLCASING_ID, &sc);
	}

	//[MURSUM] g_pGameClientShell->GetSFXMgr()->CreateSFX(SFX_SHELLCASING_ID, &sc);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CreateBloodSplatFX
//
//	PURPOSE:	Create the blood splats, etc.
//
// ----------------------------------------------------------------------- //

void CWeaponFX::CreateBloodSplatFX()
{
	CSFXMgr* psfxMgr = g_pGameClientShell->GetSFXMgr();

	if (!psfxMgr) return;

	CSpecialFX* pFX = LTNULL;

	LTFLOAT fRange = g_vtBloodSplatsRange.GetFloat();

	// See if we should make some blood splats...
	ClientIntersectQuery iQuery;
	IntersectInfo iInfo;
	iQuery.m_From = m_vPos;

	// Create some blood splats...
	int nNumSplats = GetRandom((int)g_vtBloodSplatsMinNum.GetFloat(), (int)g_vtBloodSplatsMaxNum.GetFloat());

	LTVector vU, vR;
	vU = m_rDirRot.Up();
	vR = m_rDirRot.Right();

	for (int i=0; i < nNumSplats; i++)
	{
		LTVector vDir = m_vDir;

		// Perturb direction after first splat...
		if (i > 0)
		{
			float fPerturb = g_vtBloodSplatsPerturb.GetFloat();
			float fRPerturb = (GetRandom(-fPerturb, fPerturb))/1000.0f;
			float fUPerturb = (GetRandom(-fPerturb, fPerturb))/1000.0f;

			vDir += (vR * fRPerturb);
			vDir += (vU * fUPerturb);
		}

		iQuery.m_To			= vDir * fRange + m_vPos;
		iQuery.m_Flags		= IGNORE_NONSOLID | INTERSECT_HPOLY | INTERSECT_OBJECTS;
		iQuery.m_FilterFn	= BloodSplatFilterFn;
		iQuery.m_pUserData	= NULL;

		if (g_pLTClient->IntersectSegment(&iQuery, &iInfo) && IsMainWorld(iInfo.m_hObject))
		{
			SurfaceType eType = GetSurfaceType(iInfo);
			SURFACE* pSurface = g_pSurfaceMgr->GetSurface(eType);

			// Don't leave blood on surfaces that can't show marks...
			if (!pSurface || !pSurface->bShowsMark)
			{
				return;
			}

			//find the largest splat we can place and still have it fit onto the polygon
			float fMaxRadius;
			g_pLTClient->GetMaxRadiusInPoly(iInfo.m_hPoly, iInfo.m_Point, fMaxRadius);

			//only bother creating it if it is above some size
			if(fMaxRadius > 0.1f)
			{
				// Create a blood splat...
				BSCREATESTRUCT sc;
				sc.rRot = LTRotation(iInfo.m_Plane.m_Normal, LTVector(0.0f, 1.0f, 0.0f));

				// Randomly rotate the blood splat
				sc.rRot.Rotate(iInfo.m_Plane.m_Normal, GetRandom(0.0f, MATH_CIRCLE));
				LTVector vTemp = vDir * -2.0f;
				sc.vPos = iInfo.m_Point + vTemp;  // Off the wall a bit
				sc.vVel.Init(0.0f, 0.0f, 0.0f);


				sc.vInitialScale.Init(1.0f, 1.0f, 1.0f);
				sc.vInitialScale.x	= GetRandom(g_vtBloodSplatsMinScale.GetFloat(), g_vtBloodSplatsMaxScale.GetFloat());
			
				//if (bBigBlood) sc.vInitialScale.x *= g_vtBigBloodSizeScale.GetFloat();
				//clamp it to the maximum size
				static const uint32	knBloodSize = 256;
				sc.vInitialScale.x  = LTMIN(sc.vInitialScale.x, fMaxRadius / knBloodSize);

				sc.vInitialScale.y	= sc.vInitialScale.x;
				sc.vFinalScale		= sc.vInitialScale;

				sc.dwFlags			= FLAG_VISIBLE | FLAG_ROTATEABLESPRITE | FLAG_NOLIGHT;
				sc.fLifeTime		= GetRandom(g_vtBloodSplatsMinLifetime.GetFloat(), g_vtBloodSplatsMaxLifetime.GetFloat());

				//if (bBigBlood) sc.fLifeTime *= g_vtBigBloodLifeScale.GetFloat();
				sc.fInitialAlpha	= 1.0f;
				sc.fFinalAlpha		= 1.0f;
				sc.nType			= OT_SPRITE;
				sc.bPausable		= LTTRUE;

				if(!g_GameDoc.IsLowViolence())  // 18세이상 피!!
				{
					char* pBloodFiles[] =
					{					
						"FX\\Test\\Blood\\Spr\\BloodL1.spr",
						"FX\\Test\\Blood\\Spr\\BloodL2.spr",
						"FX\\Test\\Blood\\Spr\\BloodL3.spr",
						"FX\\Test\\Blood\\Spr\\BloodL4.spr"
					};				

					sc.pFilename = pBloodFiles[GetRandom(0,3)];
				}
				else   // 15세용 피!!!
				{
					char* pBloodFiles_Teen[] =
					{				
						"FX\\Test\\Blood\\Spr\\BloodL1_T.spr",
						"FX\\Test\\Blood\\Spr\\BloodL2_T.spr",
						"FX\\Test\\Blood\\Spr\\BloodL3_T.spr",
						"FX\\Test\\Blood\\Spr\\BloodL4_T.spr"
					};				

					sc.pFilename = pBloodFiles_Teen[GetRandom(0,3)];
				}

				pFX = psfxMgr->CreateSFX(SFX_SCALE_ID, &sc);
				if (pFX) pFX->Update();
			}
		}
		else if (i==0)
		{
			// Didn't hit anything straight back, do don't bother to
			// do anymore...
			return;
		}
	}
}


//[MURSUM]==========================================================================
void CWeaponFX::CreateFlashBangFX( HOBJECT hDamager, const LTVector& vFirePos )
{
	if( hDamager != g_pLTClient->GetClientObject() || LTNULL == m_pAmmo ) return;

	if( g_pPlayerMgr->IsPlayerDead() &&
		IsRevivePlayerGameType() &&
		g_pPlayerMgr->GetAttachState() == MID_DETACH_DISPLAY ) return;

	HOBJECT hCamera = g_pPlayerMgr->GetCamera();
	LTVector vCameraPos, vF, vDir;
	LTRotation rCameraRot;
	g_pLTClient->GetObjectPos(hCamera, &vCameraPos);
	g_pLTClient->GetObjectRotation(hCamera, &rCameraRot);
	vF = rCameraRot.Forward();

	vDir = vFirePos - vCameraPos;
	LTFLOAT fImpactDist = vDir.Length();

	if( m_pAmmo->nAreaDamageRadius <= 0 || fImpactDist > m_pAmmo->nAreaDamageRadius ) return;	

	vDir.Normalize();
	LTFLOAT fMul = vDir.Dot(vF) + 1.0f;	
	// 정면  fMul = 2
	// 90도  fMul = 1
	// 180도 fMul = 0

	float	fBlindTime = 0.0f;	//최소값

	fBlindTime	+= (m_pAmmo->nAreaDamageRadius-fImpactDist)/m_pAmmo->nAreaDamageRadius;	// 0 - 1
	fBlindTime	*= 4.0f;

	if( fMul < 0.75f )
	{
		fBlindTime	= 0.0f;
	}
	else
	{
		fBlindTime	*= fMul*0.5f;
	}

	g_pHUDFlashBangFX->SetVisible( true, fBlindTime );	
}

void CWeaponFX::CreateProjectileFX( HOBJECT hDamager, const LTVector& vFirePos, uint8 nAmmoID )
{
	AMMO const* pAmmo;

	pAmmo = g_pWeaponMgr->GetAmmo( "Timed Grenade" );
	if( pAmmo && pAmmo->nId == nAmmoID && !g_GameDoc.IsLowViolence())
	{
		m_pAmmo = pAmmo;
		CreateBloodSplatFXFromProjectile( hDamager, vFirePos );
		return;
	}

	pAmmo = g_pWeaponMgr->GetAmmo( "FlashBang" );
	if( pAmmo && pAmmo->nId == nAmmoID )
	{
		m_pAmmo = pAmmo;
		CreateFlashBangFX( hDamager, vFirePos );
		return;
	}
}

void CWeaponFX::CreateBloodSplatFXFromProjectile( HOBJECT hDamager, const LTVector& vFirePos )
{
	CSFXMgr* psfxMgr = g_pGameClientShell->GetSFXMgr();
	if (!psfxMgr) return;
	CSpecialFX* pFX = LTNULL;

	CUserProfile* pProfile = g_pProfileMgr->GetCurrentProfile();
	if (!pProfile || !pProfile->m_bGore) return;


	LTFLOAT fRange = g_vtBloodSplatsRange.GetFloat();

	// See if we should make some blood splats...
	ClientIntersectQuery iQuery;
	IntersectInfo iInfo;

	g_pLTClient->GetObjectPos( hDamager, &m_vPos );
	m_vPos.y += 50.0f;	// 위치가 땅으로 잡히면 피가 조금 찍힘...올려주자~!
	iQuery.m_From = m_vPos;

	// Create some blood splats...
	int nNumSplats = GetRandom((int)g_vtBloodSplatsMinNum.GetFloat(), (int)g_vtBloodSplatsMaxNum.GetFloat());

	LTVector vU, vR;
	vU.x = -1.0f;
	vU.y = 0.0f;
	vU.z = 0.0f;

	vR.x = 0.0f;
	vR.y = 0.0f;
	vR.z = 1.0f;

	for (int i=0; i < nNumSplats; i++)
	{
		// 방행 벡터는 아래쪽으로 랜덤하게 해서 바닥을 피바다로 만든다.
		LTVector vDir;		
		vDir.x = GetRandom(-0.8f, 0.8f);
		vDir.y = -1.0f;
		vDir.z = GetRandom(-0.8f, 0.8f);
		vDir.Normalize();

		// Perturb direction after first splat...
		if (i > 0)
		{
			float fPerturb = g_vtBloodSplatsPerturb.GetFloat();

			float fRPerturb = (GetRandom(-fPerturb, fPerturb))/1000.0f;
			float fUPerturb = (GetRandom(-fPerturb, fPerturb))/1000.0f;

			vDir += (vR * fRPerturb);
			vDir += (vU * fUPerturb);
		}

		iQuery.m_To			= vDir * fRange + m_vPos;
		iQuery.m_Flags		= IGNORE_NONSOLID | INTERSECT_HPOLY | INTERSECT_OBJECTS;
		iQuery.m_FilterFn	= BloodSplatFilterFn;
		iQuery.m_pUserData	= NULL;

		if (g_pLTClient->IntersectSegment(&iQuery, &iInfo) && IsMainWorld(iInfo.m_hObject))
		{
			SurfaceType eType = GetSurfaceType(iInfo);
			SURFACE* pSurface = g_pSurfaceMgr->GetSurface(eType);

			// Don't leave blood on surfaces that can't show marks...
			if (!pSurface || !pSurface->bShowsMark)
			{
				return;
			}

			//find the largest splat we can place and still have it fit onto the polygon
			float fMaxRadius;
			g_pLTClient->GetMaxRadiusInPoly(iInfo.m_hPoly, iInfo.m_Point, fMaxRadius);

			//only bother creating it if it is above some size
			if(fMaxRadius > 0.1f)
			{
				// Create a blood splat...
				BSCREATESTRUCT sc;
				sc.rRot = LTRotation(iInfo.m_Plane.m_Normal, LTVector(0.0f, 1.0f, 0.0f));

				// Randomly rotate the blood splat
				sc.rRot.Rotate(iInfo.m_Plane.m_Normal, GetRandom(0.0f, MATH_CIRCLE));

				LTVector vTemp = vDir * -2.0f;
				sc.vPos = iInfo.m_Point + vTemp;  // Off the wall a bit
				sc.vVel.Init(0.0f, 0.0f, 0.0f);

				sc.vInitialScale.Init(1.0f, 1.0f, 1.0f);
				sc.vInitialScale.x	= 2.0f*GetRandom(g_vtBloodSplatsMinScale.GetFloat(), g_vtBloodSplatsMaxScale.GetFloat());

				//clamp it to the maximum size
				static const uint32	knBloodSize = 256;
				sc.vInitialScale.x  = LTMIN(sc.vInitialScale.x, fMaxRadius / knBloodSize);

				sc.vInitialScale.y	= sc.vInitialScale.x;
				sc.vFinalScale		= sc.vInitialScale;

				sc.dwFlags			= FLAG_VISIBLE | FLAG_ROTATEABLESPRITE | FLAG_NOLIGHT;
				sc.fLifeTime		= GetRandom(g_vtBloodSplatsMinLifetime.GetFloat(), g_vtBloodSplatsMaxLifetime.GetFloat());

				sc.fInitialAlpha	= 1.0f;
				sc.fFinalAlpha		= 1.0f;
				sc.nType			= OT_SPRITE;
				sc.bPausable		= LTTRUE;
				
				if(!g_GameDoc.IsLowViolence())  // 18세이상 피!!
				{
					char* pBloodFiles[] =
					{					
						"FX\\Test\\Blood\\Spr\\BloodL1.spr",
						"FX\\Test\\Blood\\Spr\\BloodL2.spr",
						"FX\\Test\\Blood\\Spr\\BloodL3.spr",
						"FX\\Test\\Blood\\Spr\\BloodL4.spr"
					};				

					sc.pFilename = pBloodFiles[GetRandom(0,3)];
				}
				else   // 15세용 피!!!
				{
					char* pBloodFiles_Teen[] =
					{					
						"FX\\Test\\Blood\\Spr\\BloodL1_T.spr",
						"FX\\Test\\Blood\\Spr\\BloodL2_T.spr",
						"FX\\Test\\Blood\\Spr\\BloodL3_T.spr",
						"FX\\Test\\Blood\\Spr\\BloodL4_T.spr"
					};				

					sc.pFilename = pBloodFiles_Teen[GetRandom(0,3)];
				}
				pFX = psfxMgr->CreateSFX(SFX_SCALE_ID, &sc);
				if (pFX) pFX->Update();
			}
		}
		else if (i==0)
		{
			// Didn't hit anything straight back, do don't bother to
			// do anymore...
			return;
		}
	}
}
//=============================================================================


//[SPIKE] 폭탄 흔적 
void CWeaponFX::CreatreProjectileMark()
{
	CSFXMgr* psfxMgr = g_pGameClientShell->GetSFXMgr();
	if (!psfxMgr) return;

	CSpecialFX* pFX = LTNULL;

	LTFLOAT fRange = g_vtBloodSplatsRange.GetFloat();

	// See if we should make some blood splats...
	ClientIntersectQuery iQuery;
	IntersectInfo iInfo;
	g_pLTClient->GetObjectPos( m_hObjectHit, &m_vPos );
	m_vPos.y += 50.0f;	// 위치가 땅으로 잡히면 피가 조금 찍힘...올려주자~!
	iQuery.m_From = m_vPos;

	// Create some blood splats...
	// Mark 양  
	int nNumSplats = GetRandom((int)g_vtBloodSplatsMinNum.GetFloat(), (int)g_vtBloodSplatsMaxNum.GetFloat());

	LTVector vU, vR;
	vU.x = -1.0f;
	vU.y = 0.0f;
	vU.z = 0.0f;

	vR.x = 0.0f;
	vR.y = 0.0f;
	vR.z = 1.0f;

	for (int i=0; i < nNumSplats; i++)
	{
		// 방행 벡터는 아래쪽으로 랜덤하게 해서 바닥을 피바다로 만든다.
		LTVector vDir;		
		vDir.x = GetRandom(-0.8f, 0.8f);
		vDir.y = -1.0f;
		vDir.z = GetRandom(-0.8f, 0.8f);
		vDir.Normalize();

		// Perturb direction after first splat...
		if (i > 0)
		{
			float fPerturb = g_vtBloodSplatsPerturb.GetFloat();

			float fRPerturb = (GetRandom(-fPerturb, fPerturb))/1000.0f;
			float fUPerturb = (GetRandom(-fPerturb, fPerturb))/1000.0f;

			vDir += (vR * fRPerturb);
			vDir += (vU * fUPerturb);
		}

		iQuery.m_To			= vDir * fRange + m_vPos;
		iQuery.m_Flags		= IGNORE_NONSOLID | INTERSECT_HPOLY | INTERSECT_OBJECTS;
		iQuery.m_FilterFn	= BloodSplatFilterFn;
		iQuery.m_pUserData	= NULL;

		if (g_pLTClient->IntersectSegment(&iQuery, &iInfo) && IsMainWorld(iInfo.m_hObject))
		{
			SurfaceType eType = GetSurfaceType(iInfo);
			SURFACE* pSurface = g_pSurfaceMgr->GetSurface(eType);

			// Don't leave blood on surfaces that can't show marks...
			if (!pSurface || !pSurface->bShowsMark)
			{
				return;
			}

			//find the largest splat we can place and still have it fit onto the polygon
			float fMaxRadius;
			g_pLTClient->GetMaxRadiusInPoly(iInfo.m_hPoly, iInfo.m_Point, fMaxRadius);
			//only bother creating it if it is above some size
			if(fMaxRadius > 0.1f)
			{
				// Create a blood splat...
				BSCREATESTRUCT sc;
				sc.rRot = LTRotation(iInfo.m_Plane.m_Normal, LTVector(0.0f, 1.0f, 0.0f));

				// Randomly rotate the blood splat
				sc.rRot.Rotate(iInfo.m_Plane.m_Normal, GetRandom(0.0f, MATH_CIRCLE));
				LTVector vTemp = vDir * -2.0f;
				sc.vPos = iInfo.m_Point + vTemp;  // Off the wall a bit
				sc.vVel.Init(0.0f, 0.0f, 0.0f);
				sc.vInitialScale.Init(0.7f, 0.7f, 0.7f);
			//	sc.vInitialScale.x	= 2.0f*GetRandom(g_vtBloodSplatsMinScale.GetFloat(), g_vtBloodSplatsMaxScale.GetFloat());

				//clamp it to the maximum size
			//	static const uint32	knBloodSize = 256;
			//	sc.vInitialScale.x  = LTMIN(sc.vInitialScale.x, fMaxRadius / knBloodSize);

				sc.vInitialScale.y	= sc.vInitialScale.x;
				sc.vFinalScale		= sc.vInitialScale;
				sc.dwFlags			= FLAG_VISIBLE | FLAG_ROTATEABLESPRITE | FLAG_NOLIGHT;
				sc.fLifeTime		= GetRandom(g_vtBloodSplatsMinLifetime.GetFloat(), g_vtBloodSplatsMaxLifetime.GetFloat());
				sc.fInitialAlpha	= 1.0f;
				sc.fFinalAlpha		= 0.0f;
				sc.nType			= OT_SPRITE;
				sc.bPausable		= LTTRUE;

				char* pBloodFiles[] =
				{					
					"FX\\Effect_Sprite\\bombmark001.spr",
					"FX\\Effect_Sprite\\bombmark001.spr",
					"FX\\Effect_Sprite\\bombmark001.spr",
					"FX\\Effect_Sprite\\bombmark001.spr",
				};				
				sc.pFilename = pBloodFiles[GetRandom(0,3)];

				pFX = psfxMgr->CreateSFX(SFX_SCALE_ID, &sc);
				if (pFX) pFX->Update();
			}
		}
		else if (i==0)
		{
			// Didn't hit anything straight back, do don't bother to
			// do anymore...

			return;
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::PlayFireSound
//
//	PURPOSE:	Play the fire sound
//
// ----------------------------------------------------------------------- //

void CWeaponFX::PlayFireSound()
{
	if (m_nLocalId >= 0 && m_nLocalId == m_nShooterId)
	{
		return;  // This client already heard the sound ;)
	}

	PlayerSoundId eSoundId = PSI_FIRE;

	if (m_wFireFX & WFX_SILENCED)
	{
		eSoundId = PSI_SILENCED_FIRE;
	}
	else if (m_wFireFX & WFX_ALTFIRESND)
	{
		eSoundId = PSI_ALT_FIRE;
	}

	// 광식 [sound] : 너무 멀리 있는 녀석들은 아에 Sound Play 자체를 하지 않으려고...
	//---------------------------------------------------------------------------------
	/*
	HOBJECT hLocalObj = g_pLTClient->GetClientObject();
	if( hLocalObj )
	{
		LTVector vPos;
		LTVector vResult;
		g_pLTClient->GetObjectPos( hLocalObj, &vPos);
		vResult = vPos - m_vFirePos;
		float Mag = vResult.Mag();
		int nRandom = GetRandom(0, 2); // 2/3만 사운드 play
		if( Mag <= m_pWeapon->nFireSoundRadius && nRandom )
		{
			::PlayWeaponSound(m_pWeapon, m_vFirePos, eSoundId);
		}
	}
	//---------------------------------------------------------------------------------
	*/

	// 미션 게임 중 죽어서 다른 사람에게 붙은 경우
	if( g_pPlayerMgr->IsPlayerDead() &&
		IsRevivePlayerGameType() &&
		g_pPlayerMgr->GetAttachState() == MID_ATTACH_THIRD )
	{
		LTVector vPos;
		HOBJECT hLocalObj = g_pLTClient->GetClientObject();
		g_pLTClient->GetObjectPos( hLocalObj, &vPos );
		m_vFirePos = vPos;

		/*
		if( g_pPlayerMgr->GetThirdAttach() )
		{
			CPlayerInfoMgr* pMgr = g_pInterfaceMgr->GetPlayerInfoMgr();
			if( pMgr )
			{
				uint32 nPlayer = pMgr->GetNumPlayerInGame();
				for( int i = 0; i < nPlayer; i++ )
				{
					T_PLAYER_INFOptr pInfo = pMgr->GetPlayerByIndex(i);
					if( pInfo->hObject == g_pPlayerMgr->GetThirdAttach() )
					{
						g_pLTClient->GetObjectPos(g_pPlayerMgr->GetThirdAttach(), &vPos);
						g_pLTClient->SetObjectPos(g_pMoveMgr->GetObject(), &vPos);
					}
				}
			}
		}
		*/		
	}

	::PlayWeaponSound(m_pWeapon, m_vFirePos, eSoundId);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::CalcFirePos
//
//	PURPOSE:	Calculate the fire position based on the FireFrom object
//
// ----------------------------------------------------------------------- //

LTVector CWeaponFX::CalcFirePos(const LTVector &vFirePos)
{
 	if( !m_hFiredFrom || m_wIgnoreFX & WFX_MUZZLE ) return vFirePos;

	// See if this is our local client who fired, and if so
	// only calculate fire position if we are in 3rd person...

	if (m_nLocalId == m_nShooterId)
	{
		if (g_pPlayerMgr->IsFirstPerson()) return vFirePos;
	}

    LTVector vPos;
    LTRotation rRot;
	if (!GetAttachmentSocketTransform(m_hFiredFrom, "Flash", vPos, rRot))
	{
		vPos = vFirePos;
	}
	return vPos;
}

// ----------------------------------------------------------------------- //

//

//	ROUTINE:	CWeaponFX::CalcBreachPos

//

//	PURPOSE:	Calculate the breach position based on the FireFrom object

//

// ----------------------------------------------------------------------- //

LTVector CWeaponFX::CalcBreachPos(const LTVector &vBreachPos)
{
	if (!m_hFiredFrom) return vBreachPos;

	// See if this is our local client who fired, and if so

	// only calculate fire position if we are in 3rd person...

    LTVector vPos;
	if (m_nLocalId == m_nShooterId && g_pPlayerMgr->IsFirstPerson())
	{
 		IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
  		if ( pClientWeapon )
  		{
			pClientWeapon->GetShellEjectPos(&vPos);
			return vPos;
  		}
	}

    LTRotation rRot;
	if (!GetAttachmentSocketTransform(m_hFiredFrom, "Breach", vPos, rRot))
	{
		vPos = vBreachPos;
	}
	return vPos;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::PlayBulletFlyBySound()
//
//	PURPOSE:	Play bullet fly by sound (if appropriate)
//
// ----------------------------------------------------------------------- //



void CWeaponFX::PlayUWBulletFlyBySound()
{
	if (!m_pWeapon || !m_pAmmo) return;

	if (m_pAmmo->eType != VECTOR ||
		LTFALSE==g_pMoveMgr->IsHeadInLiquid()) return;

	// Camera pos
	HOBJECT hCamera = g_pPlayerMgr->GetCamera();
	LTVector vPos;
	g_pLTClient->GetObjectPos(hCamera, &vPos);



  	// We only play the flyby sound if we won't hear an impact...
  	if (m_pAmmo->pImpactFX)
  	{
  		LTVector vDist = m_vPos - vPos;
  		if ( vDist.Length() < (0.75f * float(m_pAmmo->pImpactFX->nSoundRadius)) ) 
			return;
  	}

	// See if the camera is close enough to the bullet path to hear the

	// bullet...



	LTFLOAT fRadius = g_cvarFlyByRadius.GetFloat();
	LTVector vDir = m_vDir;
	const LTVector vRelativePos = vPos - m_vFirePos;
    const LTFLOAT fRayDist = vDir.Dot(vRelativePos);
	LTVector vBulletDir = (vDir*fRayDist - vRelativePos);

    const LTFLOAT fDistSqr = vBulletDir.MagSqr();

	if (fDistSqr < fRadius*fRadius)
	{
		char szSound[MAX_CS_FILENAME_LEN];

		strcpy( szSound, "sa_weapons\\snd\\" );

		char* FlyBySounds[] =  { "UWFlyBy1.wav", "UWFlyBy2.wav", "UWFlyBy3.wav" };
		int nSize = (sizeof(FlyBySounds)/sizeof(FlyBySounds[0])) - 1;
		strcat(szSound, FlyBySounds[GetRandom(0, nSize)]);

		vPos += vBulletDir;
		g_pClientSoundMgr->PlaySoundFromPos(vPos, szSound, 500.0f, SOUNDPRIORITY_MISC_HIGH );
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeaponFX::PlayImpactDing()
//
//	PURPOSE:	Play a impact ding sound if appropriate
//
// ----------------------------------------------------------------------- //

void CWeaponFX::PlayImpactDing()
{
}