// ----------------------------------------------------------------------- //
//
// MODULE  : ProjectileTypes.cpp
//
// PURPOSE : Projectile classs - implementation
//
// CREATED : 10/3/97
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "ProjectileTypes.h"
#include "ServerUtilities.h"
#include "DamageTypes.h"
#include "SurfaceFunctions.h"
#include "SoundMgr.h"
#include "iltphysics.h"
#include "Spawner.h"
#include "DebrisFuncs.h"
#include "Character.h"
#include "PlayerObj.h"
#include "CharacterHitBox.h"
#include "Body.h"
#include "ObjectMsgs.h"
#include "ServerSoundMgr.h"
#include "GameServerShell.h"
#include "AIStimulusMgr.h"
#include "WeaponFireInfo.h"
#include "AIUtils.h"
#include "FXButeMgr.h"
#include "CharacterMgr.h"
#include "AI.h"
#include "WeaponItems.h"

//광식
#include "ServerMissionMgr.h"

LINKFROM_MODULE( ProjectileTypes );


#define DEFAULT_GRENADE_DAMPEN_PERCENT	0.35f
#define DEFAULT_GRENADE_MIN_VELOCITY	10.0f
#define DEFAULT_TIMEBOMB_DEFUSETIME		5.0f

CTList<CGrenade*> g_lstGrenades;
CTList<CFlashBang*> g_lstFlashBang;
CTList<CSmokeGrenade*>g_lstSmokeGrenade;

#pragma force_active on
BEGIN_CLASS(CGrenade)
END_CLASS_DEFAULT_FLAGS(CGrenade, CProjectile, NULL, NULL, CF_HIDDEN)

BEGIN_CLASS(CLipstickProx)
END_CLASS_DEFAULT_FLAGS(CLipstickProx, CGrenade, NULL, NULL, CF_HIDDEN)

BEGIN_CLASS(CGrenadeImpact)
END_CLASS_DEFAULT_FLAGS(CGrenadeImpact, CGrenade, NULL, NULL, CF_HIDDEN)

BEGIN_CLASS(CSpear)
END_CLASS_DEFAULT_FLAGS(CSpear, CProjectile, NULL, NULL, CF_HIDDEN)

BEGIN_CLASS(CCameraDisabler)
END_CLASS_DEFAULT_FLAGS(CCameraDisabler, CProjectile, NULL, NULL, CF_HIDDEN)

BEGIN_CLASS(CCoin)
END_CLASS_DEFAULT_FLAGS(CCoin, CGrenade, NULL, NULL, CF_HIDDEN)

#ifdef USE_AI
BEGIN_CLASS(CKitty)
END_CLASS_DEFAULT_FLAGS(CKitty, CProjectile, NULL, NULL, CF_HIDDEN)
#endif

BEGIN_CLASS(CFlashBang)
END_CLASS_DEFAULT_FLAGS(CFlashBang, CProjectile, NULL, NULL, CF_HIDDEN)

BEGIN_CLASS(CSmokeGrenade)
END_CLASS_DEFAULT_FLAGS(CSmokeGrenade, CProjectile, NULL, NULL, CF_HIDDEN)

BEGIN_CLASS(CBearTrap)
END_CLASS_DEFAULT_FLAGS(CBearTrap, CProjectile, NULL, NULL, CF_HIDDEN)

BEGIN_CLASS(CBanana)
END_CLASS_DEFAULT_FLAGS(CBanana, CGrenade, NULL, NULL, CF_HIDDEN)

BEGIN_CLASS(CProjectileSpawner)
END_CLASS_DEFAULT_FLAGS(CProjectileSpawner, CProjectile, NULL, NULL, CF_HIDDEN)
// 광식
//------------------------------------------------------------------------------
BEGIN_CLASS(CProjectileWeapon)
END_CLASS_DEFAULT_FLAGS(CProjectileWeapon, BaseClass, NULL, NULL, CF_ALWAYSLOAD | CF_HIDDEN)
//------------------------------------------------------------------------------
#pragma force_active off

// 광식
//------------------------------------------------------------------------------
namespace
{
#define UPDATE_DELTA				0.1f
#define WEAPONITEM_PICKUP_SOUND		"WeaponItemPickup"
#define WEAPONITEM_RESPAWN_SOUND	"WeaponItemRespawn"
static const char s_szDefaultRS[] = "RS\\Default.ltb";
#define WEAPON_LIFE_TIME			1000
//#define WEAPON_LIFE_TIME			500
#define WEAPON_PICKUP_TIME			50
}
//------------------------------------------------------------------------------

LTBOOL g_bOnce = LTFALSE;

static bool MoveToFloorFilterFn(HOBJECT hTest, void *pUserData)
{
	// First filter out objects in the list...

	if (ObjListFilterFn(hTest, pUserData))
	{
		// Okay, now make sure we're not placed on top of any non-solid
		// ray-hit true objects...

		uint32 dwFlags;
		g_pCommonLT->GetObjectFlags( hTest, OFT_Flags, dwFlags );
		if (dwFlags & FLAG_SOLID)
		{
			return true;
		}
	}

    return false;
}

// 광식
// ----------------------------------------------------------------------- //
// Weapon 던지기
// ----------------------------------------------------------------------- //
CProjectileWeapon::CProjectileWeapon ()
: GameBase (OT_MODEL)
{
	m_IsUpdate = LTTRUE ;
	m_nLifeTime = 0 ;
	m_nWeaponClass = 0;
	//------------------------------------------
	m_bSpinWeapon		= LTTRUE;
	m_bUpdating			= LTTRUE;
	m_eContainerCode	= CC_NO_CONTAINER;
	m_eLastHitSurface	= ST_UNKNOWN;
	m_fPitch			= 0.0f;
	m_fYaw				= 0.0f;
	m_fRoll				= 0.0f;
	m_fPitchVel			= 0.0f;
	m_fYawVel			= 0.0f;
	m_fRollVel			= 0.0f;
	m_bRotatedToRest	= LTFALSE;
	m_cBounces			= 0;
	m_hBounceSnd		= LTNULL;

	m_vDims.Init(5.0f, 5.0f, 5.0f);

	m_bNotify			= LTFALSE;

	ResetRotationVel();
	//------------------------------------------
}

CProjectileWeapon::~CProjectileWeapon ()
{
	if( m_hObject )
		g_pLTServer->RemoveObject(m_hObject);

	m_hObject = LTNULL;
}

uint32 CProjectileWeapon::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
	/*
	GameBase::EngineMessageFn(messageID, pData, fData);

	switch (messageID)
    {
    case MID_PRECREATE:
        return PreCreate(pData, fData);
    case MID_INITIALUPDATE:
        return InitialUpdate(pData, fData);
	case MID_TOUCHNOTIFY:
        return TouchNotify(pData, fData);
    case MID_UPDATE:
		m_nLifeTime++;
        return Update(pData, fData);
        
    default:
        break;
    }
	*/
	GameBase::EngineMessageFn(messageID, pData, fData);

	switch (messageID)
    {
    case MID_PRECREATE:
        return PreCreate(pData, fData);
    case MID_INITIALUPDATE:
        return InitialUpdate(pData, fData);
	case MID_TOUCHNOTIFY:
        return TouchNotify(pData, fData);
    case MID_UPDATE:
		m_nLifeTime++;
        return Update(pData, fData);
        
    default:
        break;
    }
    
    // Pass the message along to parent class.
    return (1);
}

void CProjectileWeapon::WeaponInfoSetUp(PROJECTILEWEAPON_INFO WeaponInfo)
{
	m_WeaponInfo	= WeaponInfo;

	m_WeaponInfo.m_vDir.Normalize();

	// Gravity
	g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags, FLAG_GRAVITY, FLAG_GRAVITY);
	
	LTVector vDims;
	g_pCommonLT->GetModelAnimUserDims( m_hObject, &vDims, g_pLTServer->GetModelAnimation(m_hObject));

	g_pPhysicsLT->SetObjectDims(m_hObject, &vDims, 0);

	g_pLTServer->SetBlockingPriority(m_hObject, 0);
	g_pPhysicsLT->SetForceIgnoreLimit(m_hObject, 0.0f);
	g_pPhysicsLT->SetMass( m_hObject, m_WeaponInfo.m_fMass );

	LTVector	vU, vR;
	if( (1.0f == m_WeaponInfo.m_vDir.y) || (-1.0f == m_WeaponInfo.m_vDir.y) )
	{
		vR = m_WeaponInfo.m_vDir.Cross( LTVector( 1.0f, 0.0f, 0.0f ) );
	}
	else
	{
		vR = m_WeaponInfo.m_vDir.Cross( LTVector( 0.0f, 1.0f, 0.0f) );
	}

	vU = vR.Cross( m_WeaponInfo.m_vDir );
	vU.Normalize();

	LTRotation rRot( m_WeaponInfo.m_vDir, vU );
	g_pLTServer->SetObjectRotation(m_hObject, &rRot);

	LTVector vVel;
	vVel = m_WeaponInfo.m_vDir * m_WeaponInfo.m_fVelocity;
	g_pPhysicsLT->SetVelocity(m_hObject, &vVel);	
}

uint32 CProjectileWeapon::PreCreate(void *pData, float fData)
{
	g_pCommonLT->SetObjectFlags(m_hObject, OFT_User, NULL, NULL);

	//------------------------------------------------------------
	ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;
	if (pStruct)
	{
		pStruct->m_NextUpdate = UPDATE_NEXT_FRAME;
	}

    // Return default of 1
    return 1;
}

uint32 CProjectileWeapon::InitialUpdate(void *pData, float fData)
{	
	// 광식 : world로 던진 무기들을 Mission에서 관리하다가 Restart 시 모두 삭제.
	g_pServerMissionMgr->AddManagedWeapon(m_hObject);

	/*
    g_pPhysicsLT->SetForceIgnoreLimit( m_hObject , 0.0f );
	g_pLTServer->SetNextUpdate (m_hObject, 0.01f) ;
	*/

	// 여기부터 새로 추가 된 부분..
	g_pLTServer->SetNetFlags(m_hObject, NETFLAG_POSUNGUARANTEED|NETFLAG_ANIM_THROWMODEL);
	//g_pLTServer->SetNetFlags(m_hObject, NETFLAG_POSUNGUARANTEED|NETFLAG_ROTUNGUARANTEED|NETFLAG_ANIMUNGUARANTEED);

	if( (int)fData == INITIALUPDATE_SAVEGAME ) return 0;

	g_pCommonLT->SetObjectFlags(m_hObject, OFT_User, USRFLG_MOVEABLE, USRFLG_MOVEABLE);
    //[MURSUM] g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags2, FLAG2_SERVERDIMS, FLAG2_SERVERDIMS);
	g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags2, 0, FLAG2_SERVERDIMS);

	SetNextUpdate(UPDATE_NEXT_FRAME);

	g_pPhysicsLT->SetObjectDims(m_hObject, &m_vDims, 0);

	return 1;
}

uint32 CProjectileWeapon::Update(void *pData, float fData)
{
	/*
	if (m_IsUpdate)
	{
		g_pLTServer->GetObjectRotation(m_hObject, &m_rRot);
    	m_vVel = m_rRot.Up() * -100.0f;
		g_pPhysicsLT->SetVelocity(m_hObject, &m_vVel);
	}

	if( m_nLifeTime >= WEAPON_LIFE_TIME )
	{
		g_pServerMissionMgr->RemoveManagedWeapon(m_hObject);
		g_pLTServer->RemoveObject(m_hObject);
		m_nLifeTime = WEAPON_LIFE_TIME + 1;
		m_IsUpdate  = LTFALSE ;
	}
	else
	{
		g_pLTServer->SetNextUpdate (m_hObject, 0.01f) ;
	}
	
    return 1 ;
	*/
	LTVector vVel;
	g_pPhysicsLT->GetVelocity(m_hObject, &vVel);
	
	if( vVel.MagSqr() <= 0.1 )
	{
		g_pCommonLT->SetObjectFlags( m_hObject, OFT_User, USRFLG_SPY_VISION, USRFLG_SPY_VISION );
	}

	if( m_nLifeTime >= WEAPON_LIFE_TIME )
	{
		g_pServerMissionMgr->RemoveManagedWeapon(m_hObject);
		g_pLTServer->RemoveObject(m_hObject);
		m_nLifeTime = WEAPON_LIFE_TIME + 1;
		m_IsUpdate  = LTFALSE ;
		return 1;
	}

	if( m_nLifeTime >= WEAPON_PICKUP_TIME )
	{
		m_bNotify = LTTRUE;
	}

	LTVector vPos;
	g_pLTServer->GetObjectPos(m_hObject, &vPos);

	uint32 nIgnoreFlags = (CC_ALL_FLAG & ~ ::GetLiquidFlags());
    m_eContainerCode = ::GetContainerCode(vPos, nIgnoreFlags);

	// Update grenade spin...
	if (m_bSpinWeapon && m_bUpdating)
	{
		if (m_fPitchVel != 0 || m_fYawVel != 0 || m_fRollVel != 0)
		{
            LTFLOAT fDeltaTime = g_pLTServer->GetFrameTime();

			m_fPitch += m_fPitchVel * fDeltaTime;
			m_fYaw   += m_fYawVel * fDeltaTime;
			m_fRoll  += m_fRollVel * fDeltaTime;

            LTRotation rRot(m_fPitch, m_fYaw, m_fRoll);
			g_pLTServer->SetObjectRotation(m_hObject, &rRot);
		}
	}

	if (m_hBounceSnd)
	{
        bool bIsDone;
        if (g_pLTServer->SoundMgr()->IsSoundDone(m_hBounceSnd, bIsDone) != LT_OK || bIsDone)
		{
            g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
            m_hBounceSnd = LTNULL;
		}
	}

	//g_pLTServer->SetNextUpdate (m_hObject, 0.01f) ;
	SetNextUpdate(UPDATE_NEXT_FRAME);

	return 1;
}

uint32 CProjectileWeapon::TouchNotify(void *pData, float fData)
{
	HOBJECT hObj = (HOBJECT)pData;
	if( !hObj ) return 0;

	//if( IsPlayer(hObj) )//&& fData == -1.0f )
	if( IsPlayer(hObj) && m_bNotify )
	{
		// Collision Check 시 문제가 됨. 총을 던질 때 수류탄 위치 정도로 밀어야 될 듯..
		//CCharacter* pCharObj = (CCharacter*)g_pLTServer->HandleToObject(hObj);
		CPlayerObj* pPlayerObj = dynamic_cast< CPlayerObj* >( g_pLTServer->HandleToObject( hObj ));

		//if(pCharObj && !pCharObj->IsDead() )
		if( pPlayerObj && pPlayerObj->m_bWeaponClass[m_nWeaponClass] == LTFALSE )
		{
			CAutoMessage cMsg;
			cMsg.Writeuint32(MID_ADDWEAPON);
			cMsg.Writeuint8(m_nWeaponId);
            cMsg.Writeuint8(m_nAmmoId);
            cMsg.Writeint32(m_nAmmo);
			cMsg.Writeuint8(m_nAmmoInClip);
			cMsg.Writebool(false);
			g_pLTServer->SendToObject(cMsg.Read(), m_hObject, hObj, MESSAGE_GUARANTEED);
			g_pLTServer->RemoveObject(m_hObject);
			g_pServerMissionMgr->RemoveManagedWeapon(m_hObject);

			SetNextUpdate (UPDATE_NEVER) ;
			m_bUpdating = LTFALSE ;

			pPlayerObj->m_bWeaponClass[m_nWeaponClass] = LTTRUE;
		}
	}

	HandleImpact(hObj);
	/*
	HOBJECT hObj = (HOBJECT)pData;
	if( !hObj ) return 0;

	CollisionInfo Info;
    memset(&Info, 0, sizeof(Info));
    g_pLTServer->GetLastCollision(&Info);

	LTRESULT result = g_pPhysicsLT->IsWorldObject( hObj );
	if (result == LT_YES)
	{
		if (m_IsUpdate)
		{
			m_rRot.Rotate(m_rRot.Forward (), PI/2);
			g_pLTServer->SetObjectRotation(m_hObject, &m_rRot);
		}		

		m_vVel.x = m_vVel.y = m_vVel.z = .0f ;
		g_pPhysicsLT->SetVelocity(m_hObject, &m_vVel);

		g_pLTServer->SetNextUpdate (m_hObject, 0.01f) ;
		m_IsUpdate = LTFALSE ;
	}	

	if( IsPlayer(hObj) && fData == -1.0f )
	{
		CCharacter* pCharObj = (CCharacter*)g_pLTServer->HandleToObject(hObj);

		if(pCharObj && !pCharObj->IsDead() )
		{
			CAutoMessage cMsg;
			cMsg.Writeuint32(MID_ADDWEAPON);
			cMsg.Writeuint8(m_nWeaponId);
            cMsg.Writeuint8(m_nAmmoId);
            cMsg.Writeint32(m_nAmmo);
			cMsg.Writebool(false);
			g_pLTServer->SendToObject(cMsg.Read(), m_hObject, hObj, MESSAGE_GUARANTEED);
			g_pLTServer->RemoveObject(m_hObject);
			g_pServerMissionMgr->RemoveManagedWeapon(m_hObject);

			SetNextUpdate (UPDATE_NEVER) ;
			m_IsUpdate = LTFALSE ;
		}
	}
	*/
	return 1;
}

void CProjectileWeapon::ReadProps(ObjectCreateStruct* pStruct)
{
	//g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
}

void CProjectileWeapon::HandleImpact(HOBJECT hObj)
{
	if ( IsCharacter(hObj) || IsCharacterHitBox(hObj) || IsBody(hObj))
	{
		return;
	}

	LTVector vVel;
	g_pPhysicsLT->GetVelocity(m_hObject, &vVel);
	
	// 물에 빠졌는지 Check
	LTBOOL bEnteringLiquid = LTFALSE;
    uint16 code;
    if (g_pLTServer->GetContainerCode(hObj, &code))
	{
		if (IsLiquid((ContainerCode)code))
		{
            bEnteringLiquid = LTTRUE;
		}
		else
		{
			return;
		}
	}

	if( (ContainerCode)code == CC_TIMEBOMB_AREA ) return;

	CollisionInfo info;
    g_pLTServer->GetLastCollision(&info);

	// world와 충돌 check 계산
	if (IsMainWorld(hObj))
	{
		LTVector vPos, vCurVel, vP0, vP1;
        g_pLTServer->GetObjectPos(m_hObject, &vPos);

		vP1 = vPos;
        vCurVel = vVel * g_pLTServer->GetFrameTime();
		vP0 = vP1 - vCurVel;
		vP1 += vCurVel;

        LTFLOAT fDot1 = VEC_DOT(info.m_Plane.m_Normal, vP0) - info.m_Plane.m_Dist;
        LTFLOAT fDot2 = VEC_DOT(info.m_Plane.m_Normal, vP1) - info.m_Plane.m_Dist;

		if (fDot1 < 0.0f && fDot2 < 0.0f || fDot1 > 0.0f && fDot2 > 0.0f)
		{
			vPos = vP1;
		}
		else
		{
            LTFLOAT fPercent = -fDot1 / (fDot2 - fDot1);
			VEC_LERP(vPos, vP0, vP1, fPercent);
		}

		// 계산 된 새로운 Position
		g_pLTServer->SetObjectPos(m_hObject, &vPos);
	}

	// 만약 물이 아닌 곳에 충돌 했다면 바운딩 시킨다
	if (!bEnteringLiquid)
	{
		//vVel += (info.m_vStopVel * 2.0f);
		vVel += (info.m_vStopVel * 1.5f);
	}
	else //[SPIKE]
	{
		vVel *= 9.5f;
	}

	// Surface Type에 따라 총의 가속도를 줄인다.
	// 나중에 총의 Dampen변수 정의해서 써야쥐~
	LTFLOAT fDampenPercent = DEFAULT_GRENADE_DAMPEN_PERCENT;
	m_eLastHitSurface = GetSurfaceType(info);
	SURFACE* pSurf = g_pSurfaceMgr->GetSurface(m_eLastHitSurface);
	
	if( pSurf && pSurf->eType != ST_UNKNOWN )
	{
		// 충돌 면과 관련 된 Sound Play
		//if ( ShouldPlayBounceSound(pSurf) )
		if ( ShouldPlayBounceSound(pSurf) && m_cBounces < 3 )
		{
			uint32 dwFlags = PLAYSOUND_GETHANDLE | PLAYSOUND_TIME;

			int nVolume	= IsLiquid(m_eContainerCode) ? 50 : 100;

			LTVector vPos;
            g_pLTServer->GetObjectPos(m_hObject, &vPos);

			if (m_hBounceSnd)
			{
                g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
                m_hBounceSnd = LTNULL;
			}
			else
			{
				//g_pAIStimulusMgr->RegisterStimulus( kStim_EnemyDisturbanceSound, 1, m_hFiredFrom, m_hObject, vPos, pSurf->fGrenadeSndRadius );
			}

			m_hBounceSnd = g_pServerSoundMgr->PlaySoundFromPos(vPos, (char*)GetBounceSound(pSurf),
				pSurf->fGrenadeSndRadius, SOUNDPRIORITY_MISC_MEDIUM, dwFlags, nVolume);
		}
		fDampenPercent = (1.0f - pSurf->fHardness);
	}

	fDampenPercent = fDampenPercent > 1.0f ? 1.0f : (fDampenPercent < 0.0f ? 0.0f : fDampenPercent);
	vVel *= (1.0f - fDampenPercent);
	
	if (m_cBounces > 5)
	{
		// Object나 땅 위에 있다면 멈춘다.
		CollisionInfo standingInfo;
        g_pLTServer->GetStandingOn(m_hObject, &standingInfo);

		CollisionInfo* pInfo = standingInfo.m_hObject ? &standingInfo : &info;

		if( pInfo->m_hObject )
		{
			if( pInfo->m_Plane.m_Normal.y > 0.75f )
			{				
				m_bNotify = LTTRUE;
				vVel.Init();
				g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_GRAVITY | /*FLAG_TOUCH_NOTIFY |*/ FLAG_SOLID );
				RotateToRest();

				HMODELANIM dwAni = g_pLTServer->GetAnimIndex( m_hObject, "World" );
				if( dwAni != INVALID_ANI )
				{
					g_pModelLT->SetCurAnim(m_hObject, 0xFF, dwAni);
					g_pModelLT->SetCurAnimTime(m_hObject, 0xFF, 0);
					//g_pLTServer->SetModelAnimation( m_hObject, dwAni );
				}				
			}
		}
	}
	//--------------------------------------------------------------

	// 바운딩 된 것에 따라서 Rotation velocity 를 줄인다.
	ResetRotationVel(&vVel, pSurf);

	vVel -= info.m_vStopVel;

	g_pPhysicsLT->SetVelocity(m_hObject, &vVel);

	m_cBounces++;
}

void CProjectileWeapon::RotateToRest()
{
	m_bRotatedToRest = LTTRUE;

	// 모델이 애니를 가지고 있따면 애니를 끈다
	ANIMTRACKERID nTracker;
	if( LT_OK == g_pModelLT->GetMainTracker( m_hObject, nTracker ))
	{
		g_pModelLT->SetPlaying(m_hObject, nTracker, LTFALSE);
	}

	if( !m_bSpinWeapon ) return;

	LTRotation rRot(0.0f, m_fYaw, 0.0f);
	g_pLTServer->SetObjectRotation(m_hObject, &rRot);

    m_bUpdating = LTFALSE;
}

LTBOOL CProjectileWeapon::ShouldPlayBounceSound(SURFACE* pSurface)
{
	return ( pSurface->eType != ST_LIQUID && !!pSurface->szProjectileImpactSnds[0] );
}

const char* CProjectileWeapon::GetBounceSound(SURFACE* pSurface)
{
	return pSurface->szProjectileImpactSnds[GetRandom(0, SRF_MAX_IMPACT_SNDS-1)];
}

void CProjectileWeapon::ResetRotationVel(LTVector* pvNewVel, SURFACE* pSurf)
{
	if (!m_bSpinWeapon) return;

	LTFLOAT fVal  = MATH_CIRCLE/2.0f;
    LTFLOAT fVal2 = MATH_CIRCLE;
	m_fPitchVel	 = GetRandom(-fVal, fVal);
	m_fYawVel	 = GetRandom(-fVal2, fVal2);
	m_fRollVel   = GetRandom(-fVal2, fVal2);
}
// ----------------------------------------------------------------------- //


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::CGrenade
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CGrenade::CGrenade() : CProjectile()
{
	// Uncomment these lines to use normal, non-point collide physics.
	// (point collide physics is faster and should be used whenever
	// possible).
	//m_dwFlags &= ~FLAG_POINTCOLLIDE;
	//m_dwFlags |= FLAG_SOLID | FLAG_CLIENTNONSOLID;

	m_vDims.Init(5.0f, 5.0f, 5.0f);

    m_bSpinGrenade  = LTTRUE;
    m_bUpdating     = LTTRUE;

	m_fPitch	= 0.0f;
	m_fYaw		= 0.0f;
	m_fRoll		= 0.0f;
	m_fPitchVel	= 0.0f;
	m_fYawVel	= 0.0f;
	m_fRollVel	= 0.0f;

	m_cBounces = 0;

    m_hBounceSnd = LTNULL;
	m_eContainerCode = CC_NO_CONTAINER;
	m_eLastHitSurface = ST_UNKNOWN;

	m_bRotatedToRest = LTFALSE;
	m_bAddToGrenadeList = LTTRUE;

	if ( m_bAddToGrenadeList )
	{
		g_lstGrenades.Add(this);
	}

	ResetRotationVel();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::~CGrenade
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CGrenade::~CGrenade()
{
	// Just to be safe...
	if( m_bAddToGrenadeList )
		g_lstGrenades.Remove(this);

	if (m_hBounceSnd)
	{
        g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
        m_hBounceSnd = LTNULL;
	}
}
// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::Detonate
//
//	PURPOSE:	Go boom
//
// ----------------------------------------------------------------------- //

void CGrenade::Detonate(HOBJECT hObj)
{
	// We're blowing up, take us out of the grenade list
	if( m_bAddToGrenadeList )
		g_lstGrenades.Remove(this);

	CProjectile::Detonate(hObj);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::HandleImpact
//
//	PURPOSE:	Handle bouncing off of things
//
// ----------------------------------------------------------------------- //

void CGrenade::HandleImpact(HOBJECT hObj)
{
	// [KLS 7/28/02] - If we hit a character detonate. 
	// (i.e., only act like a timed grenade if we don't hit someone)

//[SPIKE] 몸에 맞으면 터지지 않게 하기 위한 주석 
	if ( IsCharacter(hObj) || IsCharacterHitBox(hObj) || IsBody(hObj))
	{
		if( g_pServerMissionMgr->IsNetFriendlyFire() || !IsMyTeam( hObj ))
		{
	//		Detonate(hObj);
	//		return;
		}	
	}

    LTVector vVel;
	g_pPhysicsLT->GetVelocity(m_hObject, &vVel);

	// See if we are impacting on liquid...

    LTBOOL bEnteringLiquid = LTFALSE;
    uint16 code;
    if (g_pLTServer->GetContainerCode(hObj, &code))
	{
		if (IsLiquid((ContainerCode)code))
		{
            bEnteringLiquid = LTTRUE;
		}
		else
		{
			return;
		}
	}


	CollisionInfo info;
    g_pLTServer->GetLastCollision(&info);
	//[SPIKE] 투척무기들 사람 몸에 맞으면 팅기게...
/*	LTBOOL bHitCharactor = LTFALSE;
	if ( (IsCharacter(hObj) || IsCharacterHitBox(hObj) || IsBody(hObj) ) && !g_bOnce)
	{
		g_bOnce = LTTRUE;
		IntersectInfo iInfo;
		IntersectQuery qInfo;
		
		qInfo.m_Flags = INTERSECT_HPOLY | INTERSECT_OBJECTS | IGNORE_NONSOLID;
		LTVector vTemp;
		vTemp = vVel;
		vTemp.Normalize();
		qInfo.m_From	  = vPos - (vTemp*50.0f);
		qInfo.m_To		  = vPos + (vTemp*50.0f);
		if (g_pLTServer->IntersectSegment(&qInfo, &iInfo))
		{
			info.m_hObject = iInfo.m_hObject;
			info.m_hPoly   = iInfo.m_hPoly;
			info.m_Plane   = iInfo.m_Plane;
			info.m_vStopVel = (info.m_Plane.m_Normal * vVel.Length());
			bHitCharactor = LTTRUE;
		}
	}
	else
	{
		g_bOnce = LTFALSE;
	}
	*/
	// Calculate where we really hit the world...
    if (IsMainWorld(hObj)/* || bHitCharactor*/)
	{
		LTVector vPos, vCurVel, vP0, vP1;
		g_pLTServer->GetObjectPos(m_hObject, &vPos);
	
		vP1 = vPos;
        vCurVel = vVel * g_pLTServer->GetFrameTime();
		vP0 = vP1 - vCurVel;
		vP1 += vCurVel;


        LTFLOAT fDot1 = VEC_DOT(info.m_Plane.m_Normal, vP0) - info.m_Plane.m_Dist;
        LTFLOAT fDot2 = VEC_DOT(info.m_Plane.m_Normal, vP1) - info.m_Plane.m_Dist;

		if (fDot1 < 0.0f && fDot2 < 0.0f || fDot1 > 0.0f && fDot2 > 0.0f)
		{
			vPos = vP1;
		}
		else
		{
            LTFLOAT fPercent = -fDot1 / (fDot2 - fDot1);
			VEC_LERP(vPos, vP0, vP1, fPercent);
		}

		// Set our new "real" pos...

        g_pLTServer->SetObjectPos(m_hObject, &vPos);
	}

	// Do the bounce, if the object we hit isn't liquid...

	if (!bEnteringLiquid)
	{
		vVel += (info.m_vStopVel * 1.6f);
	}
	else //[SPIKE]
	{
		vVel *= 9.5f;
	}


	// Dampen the grenade's new velocity based on the surface type...

    LTFLOAT fDampenPercent = DEFAULT_GRENADE_DAMPEN_PERCENT;

	m_eLastHitSurface = GetSurfaceType(info);
	SURFACE* pSurf = g_pSurfaceMgr->GetSurface(m_eLastHitSurface);
	if (pSurf)
	{
		// Play a bounce sound (based on the surface type) if one isn't
		// already playing...

		// 광식 : bounding sound는 2 번만.
		//if ( ShouldPlayBounceSound(pSurf) )
		if ( ShouldPlayBounceSound(pSurf) && m_cBounces < 3 )
		{
            uint32 dwFlags = PLAYSOUND_GETHANDLE | PLAYSOUND_TIME;

			int nVolume	= IsLiquid(m_eContainerCode) ? 50 : 100;

            LTVector vPos;
            g_pLTServer->GetObjectPos(m_hObject, &vPos);

			// Only play one sound at a time...
			
			if (m_hBounceSnd)
			{
                g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
                m_hBounceSnd = LTNULL;
			}
#ifdef USE_AI
			else {
				// Register a stimulus for the bounce sound, if this is a new bounce sound.

				g_pAIStimulusMgr->RegisterStimulus( kStim_EnemyDisturbanceSound, 1, m_hFiredFrom, m_hObject, vPos, pSurf->fGrenadeSndRadius );
			}
#endif

			m_hBounceSnd = g_pServerSoundMgr->PlaySoundFromPos(vPos, (char*)GetBounceSound(pSurf),
				pSurf->fGrenadeSndRadius, SOUNDPRIORITY_MISC_MEDIUM, dwFlags, nVolume);
		}

		fDampenPercent = (1.0f - pSurf->fHardness);
	}

	fDampenPercent = fDampenPercent > 1.0f ? 1.0f : (fDampenPercent < 0.0f ? 0.0f : fDampenPercent);

	vVel *= (1.0f - fDampenPercent);


	// See if we should come to a rest...

    LTVector vTest = vVel;
	vTest.y = 0.0f;

	if (vTest.Mag() < DEFAULT_GRENADE_MIN_VELOCITY)
	{
		// If we're on the ground (or an object), stop movement...

		CollisionInfo standingInfo;
        g_pLTServer->GetStandingOn(m_hObject, &standingInfo);

		CollisionInfo* pInfo = standingInfo.m_hObject ? &standingInfo : &info;

		if (pInfo->m_hObject)
		{
			// Don't stop on walls...

			if (pInfo->m_Plane.m_Normal.y > 0.75f)
			{
				vVel.Init();

				// Turn off gravity, solid, and touch notify....

				g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_GRAVITY | FLAG_TOUCH_NOTIFY | FLAG_SOLID );

				// Rotate to rest...

				RotateToRest();
			}
		}
	}


	// Reset rotation velocities due to the bounce...

	ResetRotationVel(&vVel, pSurf);


	// We need to subtact this out because the engine will add it back in,
	// kind of a kludge but necessary...

	vVel -= info.m_vStopVel;


    g_pPhysicsLT->SetVelocity(m_hObject, &vVel);

	m_cBounces++;
}

// ----------------------------------------------------------------------- //

LTBOOL CGrenade::ShouldPlayBounceSound(SURFACE* pSurface)
{
	return ( pSurface->eType != ST_LIQUID && !!pSurface->szGrenadeImpactSnd[0] );
}

// ----------------------------------------------------------------------- //

const char* CGrenade::GetBounceSound(SURFACE* pSurface)
{
	return pSurface->szGrenadeImpactSnd;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::UpdateGrenade()
//
//	PURPOSE:	Update the grenade...
//
// ----------------------------------------------------------------------- //

void CGrenade::UpdateGrenade()
{
	// Update my container code...

    LTVector vPos;
	g_pLTServer->GetObjectPos(m_hObject, &vPos);

	// Get our container code, but ignore everything except for liquid containers...

	uint32 nIgnoreFlags = (CC_ALL_FLAG & ~ ::GetLiquidFlags());
    m_eContainerCode = ::GetContainerCode(vPos, nIgnoreFlags);


	// Update grenade spin...

	if (m_bSpinGrenade && m_bUpdating)
	{
		if (m_fPitchVel != 0 || m_fYawVel != 0 || m_fRollVel != 0)
		{
            LTFLOAT fDeltaTime = g_pLTServer->GetFrameTime();

			m_fPitch += m_fPitchVel * fDeltaTime;
			m_fYaw   += m_fYawVel * fDeltaTime;
			m_fRoll  += m_fRollVel * fDeltaTime;

            LTRotation rRot(m_fPitch, m_fYaw, m_fRoll);
			g_pLTServer->SetObjectRotation(m_hObject, &rRot);
		}
	}


	// See if the bounce sound is done playing...

	if (m_hBounceSnd)
	{
        bool bIsDone;
        if (g_pLTServer->SoundMgr()->IsSoundDone(m_hBounceSnd, bIsDone) != LT_OK || bIsDone)
		{
            g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
            m_hBounceSnd = LTNULL;
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::RotateToRest()
//
//	PURPOSE:	Rotate the grenade to its rest position...
//
// ----------------------------------------------------------------------- //

void CGrenade::RotateToRest()
{
	// Record this grenade for AI's to be wary of
	// At rest

	m_bRotatedToRest = LTTRUE;

	// Turn off any animations that are playing (e.g., spin)...

	ANIMTRACKERID nTracker;
	if( LT_OK == g_pModelLT->GetMainTracker( m_hObject, nTracker ))
	{
		g_pModelLT->SetPlaying(m_hObject, nTracker, LTFALSE);
	}

	// Spin it if necessary

	if (!m_bSpinGrenade) return;

    LTRotation rRot(0.0f, m_fYaw, 0.0f);
	g_pLTServer->SetObjectRotation(m_hObject, &rRot);

    m_bUpdating = LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::ResetRotationVel()
//
//	PURPOSE:	Update the grenade...
//
// ----------------------------------------------------------------------- //

void CGrenade::ResetRotationVel(LTVector* pvNewVel, SURFACE* pSurf)
{
	if (!m_bSpinGrenade) return;

	// (TO DO: base this on new velocity and surface hardness?)

    LTFLOAT fVal  = MATH_CIRCLE/2.0f;
    LTFLOAT fVal2 = MATH_CIRCLE;
	m_fPitchVel	 = GetRandom(-fVal, fVal);
	m_fYawVel	 = GetRandom(-fVal2, fVal2);
	m_fRollVel   = GetRandom(-fVal2, fVal2);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::EngineMessageFn
//
//	PURPOSE:	Handle engine messages
//
// ----------------------------------------------------------------------- //

uint32 CGrenade::EngineMessageFn(uint32 messageID, void *pData, LTFLOAT fData)
{
	switch(messageID)
	{
		case MID_UPDATE:
		{
			UpdateGrenade();
		}
		break;

		default : break;
	}

	return CProjectile::EngineMessageFn(messageID, pData, fData);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::Save
//
//	PURPOSE:	Save the object
//
// ----------------------------------------------------------------------- //

void CGrenade::Save(ILTMessage_Write *pMsg, uint32 dwSaveFlags)
{
	if (!pMsg) return;

	CProjectile::Save(pMsg, dwSaveFlags);

	SAVE_BOOL(m_bSpinGrenade);
    SAVE_BOOL(m_bUpdating);
    SAVE_BYTE(m_eContainerCode);
    SAVE_BYTE(m_eLastHitSurface);

    SAVE_FLOAT(m_fPitch);
    SAVE_FLOAT(m_fYaw);
    SAVE_FLOAT(m_fRoll);
    SAVE_FLOAT(m_fPitchVel);
    SAVE_FLOAT(m_fYawVel);
    SAVE_FLOAT(m_fRollVel);
	SAVE_BOOL(m_bRotatedToRest);
	SAVE_INT(m_cBounces);
	SAVE_BOOL(m_bAddToGrenadeList);
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::Load
//
//	PURPOSE:	Load the object
//
// ----------------------------------------------------------------------- //

void CGrenade::Load(ILTMessage_Read *pMsg, uint32 dwLoadFlags)
{
	if (!pMsg) return;

	CProjectile::Load(pMsg, dwLoadFlags);

    LOAD_BOOL(m_bSpinGrenade);
    LOAD_BOOL(m_bUpdating);
    LOAD_BYTE_CAST(m_eContainerCode, ContainerCode);
    LOAD_BYTE_CAST(m_eLastHitSurface, SurfaceType);

    LOAD_FLOAT(m_fPitch);
    LOAD_FLOAT(m_fYaw);
    LOAD_FLOAT(m_fRoll);
    LOAD_FLOAT(m_fPitchVel);
    LOAD_FLOAT(m_fYawVel);
    LOAD_FLOAT(m_fRollVel);
	LOAD_BOOL(m_bRotatedToRest);
	LOAD_INT(m_cBounces);
	LOAD_BOOL(m_bAddToGrenadeList);

	if ( m_bRotatedToRest && m_bAddToGrenadeList )
	{
		g_lstGrenades.Add(this);
	}
}

void CGrenade::RemoveGrenade()
{
	this->RemoveObject();
}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLipstickProx::Setup()
//
//	PURPOSE:	Setup the grenade...
//
// ----------------------------------------------------------------------- //

LTBOOL CLipstickProx::Setup(CWeapon const* pWeapon, WeaponFireInfo const& info)
{
	if( !CGrenade::Setup(pWeapon, info) )
		return LTFALSE;

	if (!m_pAmmoData || !m_pAmmoData->pProjectileFX ||
		!m_pAmmoData->pProjectileFX->pClassData) return LTFALSE;

	m_pClassData = (PROXCLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLipstickProx::UpdateGrenade()
//
//	PURPOSE:	Update the grenade...
//
// ----------------------------------------------------------------------- //

void CLipstickProx::UpdateGrenade()
{
	CGrenade::UpdateGrenade();

	// If we're doing normal (in-air) updates, don't do any more
	// processing...

	if (m_bUpdating || !m_pClassData) return;

	// Waiting to go boom...

	if (m_bActivated && m_DetonateTime.Stopped())
	{
        Detonate(LTNULL);
		return;
	}


	// See if it is time to arm yet...

	if (!m_bArmed && m_ArmTime.Stopped())
	{
        m_bArmed = LTTRUE;

		// Play armed sound...

		if (m_pClassData->szArmSound[0])
		{
			int nVolume = IsLiquid(m_eContainerCode) ? 50 : 100;

            LTVector vPos;
			g_pLTServer->GetObjectPos(m_hObject, &vPos);

			g_pServerSoundMgr->PlaySoundFromPos(vPos, m_pClassData->szArmSound,
                (LTFLOAT)m_pClassData->nArmSndRadius, SOUNDPRIORITY_MISC_MEDIUM,
				0, nVolume);
		}
	}


	// Is there anything close enough to cause us to go boom?

	if (!m_bActivated && m_bArmed)
	{
		if (!m_pAmmoData) return;

        LTFLOAT fRadius = (LTFLOAT) m_pClassData->nActivateRadius;

		// NEED TO FIGURE OUT A BETTER WAY TO DO THIS!!!

        LTVector vPos;
		g_pLTServer->GetObjectPos(m_hObject, &vPos);
        ObjectList* pList = g_pLTServer->FindObjectsTouchingSphere(&vPos, fRadius);
		if (!pList) return;

		ObjectLink* pLink = pList->m_pFirstLink;
		while (pLink)
		{
			if( false == g_pServerMissionMgr->IsNetFriendlyFire() && IsMyTeam( pLink->m_hObject ))
			{
				//go to next obj
				pLink = pLink->m_pNext;
				continue;
			}

			if (IsCharacter(pLink->m_hObject))
			{
                m_bActivated = LTTRUE;

                LTFLOAT fDelay = m_pClassData->fActivateDelay;

				m_DetonateTime.Start(fDelay);

				// Play activation sound...

				if (m_pClassData->szActivateSound[0])
				{
					int nVolume = IsLiquid(m_eContainerCode) ? 50 : 100;

                    LTVector vPos;
                    g_pLTServer->GetObjectPos(m_hObject, &vPos);

					g_pServerSoundMgr->PlaySoundFromPos(vPos, m_pClassData->szActivateSound,
                        (LTFLOAT) m_pClassData->nActivateSndRadius,
						SOUNDPRIORITY_MISC_MEDIUM, 0, nVolume);
				}

				break;
			}

			pLink = pLink->m_pNext;
		}
        g_pLTServer->RelinquishList(pList);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLipstickProx::HandleImpact
//
//	PURPOSE:	Handle bouncing off of things
//
// ----------------------------------------------------------------------- //

void CLipstickProx::HandleImpact(HOBJECT hObj)
{
	CGrenade::HandleImpact(hObj);

	// See if we should stick to the object we just hit...

	SURFACE* pSurf = g_pSurfaceMgr->GetSurface(m_eLastHitSurface);
	if (pSurf)
	{
		// Does this surface support magnatism?  If so, stick...

		if (pSurf->bMagnetic)
		{
			// Need to set velocity to 0.0f but account for stoping vel
			// being added back in...

			CollisionInfo info;
            g_pLTServer->GetLastCollision(&info);

			LTVector vVel(0, 0, 0);
			vVel -= info.m_vStopVel;
			g_pPhysicsLT->SetVelocity(m_hObject, &vVel);

			m_vSurfaceNormal.Init(0, 1, 0);
			m_vSurfaceNormal = info.m_Plane.m_Normal;


			// Turn off gravity, solid, and touch notify....
			// And turn on go-thru-world so it doesn't reflect from the ending position

			g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags, FLAG_GOTHRUWORLD, FLAG_GRAVITY | FLAG_TOUCH_NOTIFY | FLAG_SOLID | FLAG_GOTHRUWORLD);


			// Rotate to rest...

			RotateToRest();
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLipstickProx::RotateToRest()
//
//	PURPOSE:	Rotate the grenade to its rest position...
//
// ----------------------------------------------------------------------- //

void CLipstickProx::RotateToRest()
{
	CGrenade::RotateToRest();

	// Okay, rotated based on the surface normal we're on...
	LTRotation rRot(m_vSurfaceNormal, LTVector(0.0f, 1.0f, 0.0f));
	g_pLTServer->SetObjectRotation(m_hObject, &rRot);

	// Arm the grenade after a few...

    LTFLOAT fDelay = m_pClassData->fArmDelay;

	m_ArmTime.Start(fDelay);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLipstickProx::Save
//
//	PURPOSE:	Save the object
//
// ----------------------------------------------------------------------- //

void CLipstickProx::Save(ILTMessage_Write *pMsg, uint32 dwSaveFlags)
{
	if (!pMsg) return;

	CGrenade::Save(pMsg, dwSaveFlags);

	m_DetonateTime.Save(pMsg);
	m_ArmTime.Save(pMsg);

	SAVE_VECTOR(m_vSurfaceNormal);
	SAVE_BOOL(m_bArmed);
	SAVE_BOOL(m_bActivated);
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLipstickProx::Load
//
//	PURPOSE:	Load the object
//
// ----------------------------------------------------------------------- //

void CLipstickProx::Load(ILTMessage_Read *pMsg, uint32 dwLoadFlags)
{
	if (!pMsg) return;

	CGrenade::Load(pMsg, dwLoadFlags);

	m_DetonateTime.Load(pMsg);
	m_ArmTime.Load(pMsg);

    LOAD_VECTOR(m_vSurfaceNormal);

    LOAD_BOOL(m_bArmed);
    LOAD_BOOL(m_bActivated);

    m_pClassData = LTNULL;
	if (m_pAmmoData && m_pAmmoData->pProjectileFX)
	{
		m_pClassData = (PROXCLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSpear::CSpear
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CSpear::CSpear()
:	CProjectile		( ),
	m_pClassData	( LTNULL )
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSpear::~CSpear
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CSpear::~CSpear()
{
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CSpear::Setup
//
//  PURPOSE:	Setup the spear...
//
// ----------------------------------------------------------------------- //

LTBOOL CSpear::Setup( const CWeapon *pWeapon, const WeaponFireInfo &info )
{
	// Let the base class setup first...

	if( !CProjectile::Setup( pWeapon, info ))
		return LTFALSE;

	// Get the class data...

	if( !m_pAmmoData || !m_pAmmoData->pProjectileFX ||
		!m_pAmmoData->pProjectileFX->pClassData ) return LTFALSE;

	m_pClassData = (SPEARCLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSpear::HandleImpact
//
//	PURPOSE:	Handle bouncing off of things
//
// ----------------------------------------------------------------------- //

void CSpear::HandleImpact(HOBJECT hObj)
{

	// [KLS 8/18/02] - Get the object's position and rotation before calling
	// parent's HandleImpact as it may change these values...
    LTVector vPos, vVel;
    g_pLTServer->GetObjectPos(m_hObject, &vPos);

    LTRotation rRot;
    g_pLTServer->GetObjectRotation(m_hObject, &rRot);


	// [RP] 7/29/02 Call the parent projectile's HandleImapct() first so it will process the damage.
	//		This will enable us to see if the projectile has killed a character if it impacted on one.
	//		Calling this will remove the projectile object but since it won't actually get removed untill
	//		the next update getting the objects data is still valid...

	CProjectile::HandleImpact(hObj);


	if (!m_pAmmoData || !m_pAmmoData->pProjectileFX || !m_pClassData )
	{
		return;
	}


	CollisionInfo info;
    g_pLTServer->GetLastCollision(&info);


	// Should we break the spear?

	enum SpearAction
	{
		eSpearActionBreak,
		eSpearActionStickWorld,
		eSpearActionStickAI,
		eSpearActionStickPlayer,
		eSpearActionStickBody
	};

	SpearAction eSpearAction = eSpearActionBreak;


	// Optimization:  AI spears always break...
	if (IsPlayer(m_hFiredFrom))
	{

		bool bDoNormalChecks = true;

		// First see if we hit a character and killed them in a "wall stick able" way
		// If so we want to keep the spear stuck in them so it has a chance to stick
		// them to a wall..

		if (m_pClassData->bCanWallStick && IsCharacter(hObj))
		{
			CCharacter* pCharacter = dynamic_cast<CCharacter*>(g_pLTServer->HandleToObject(hObj));
			if (pCharacter && pCharacter->IsDead())
			{
				ModelSkeleton eModelSkeleton = pCharacter->GetModelSkeleton();
				ModelNode eModelNode = pCharacter->GetModelNodeLastHit();
				if (NODEFLAG_WALLSTICK & g_pModelButeMgr->GetSkeletonNodeFlags(eModelSkeleton, eModelNode))
				{
					bDoNormalChecks = false;
					eSpearAction = eSpearActionStickPlayer;
				}
			}
		}
		
		if (bDoNormalChecks)
		{
			if (GetRandom(0.0, 1.0f) > m_pClassData->fStickPercent )
			{
				// Randomly break even if we could sometimes stick...
		
				eSpearAction = eSpearActionBreak;
			}
			else if (IsMainWorld(hObj))
			{
 				// Calculate where we really hit the world...

				LTVector vCurVel, vP0, vP1;
				g_pPhysicsLT->GetVelocity(m_hObject, &vVel);

				vP1 = vPos;
				vCurVel = vVel * g_pLTServer->GetFrameTime();
				vP0 = vP1 - vCurVel;
				vP1 += vCurVel;

				LTFLOAT fDot1 = VEC_DOT(info.m_Plane.m_Normal, vP0) - info.m_Plane.m_Dist;
				LTFLOAT fDot2 = VEC_DOT(info.m_Plane.m_Normal, vP1) - info.m_Plane.m_Dist;

				if (fDot1 < 0.0f && fDot2 < 0.0f || fDot1 > 0.0f && fDot2 > 0.0f)
				{
					vPos = vP1;
				}
				else
				{
					LTFLOAT fPercent = -fDot1 / (fDot2 - fDot1);
					VEC_LERP(vPos, vP0, vP1, fPercent);
				}

				// Set our new "real" pos...

				g_pLTServer->SetObjectPos(m_hObject, &vPos);

				eSpearAction = eSpearActionStickWorld;
			}
			else if (IsMoveable(hObj))
			{
				if (IsPlayer(hObj))
				{
					// Attach to a Player
					eSpearAction = eSpearActionStickPlayer;
				}
				else if (IsBody(hObj))
				{
					// Attach to a body
 					eSpearAction = eSpearActionStickBody;
				}
				else
				{
					// Could probably come up with a way to attach to moveable
					// non-character objects (like doors), but it is much easier
					// to just break it ;)...

					eSpearAction = eSpearActionBreak;
				}
			}
		}
	}



	// If the surface is too hard, the spear will just break when
	// it hits it...

	SurfaceType eSurf = GetSurfaceType(info);
	SURFACE* pSurf = g_pSurfaceMgr->GetSurface(eSurf);
	if ((eSpearActionBreak == eSpearAction) || ((eSpearActionStickWorld == eSpearAction) && pSurf && pSurf->fHardness > 0.5))
	{
		// Create spear debris...

		DEBRIS* pDebris = g_pDebrisMgr->GetDebris(m_pAmmoData->szName);
		if (pDebris)
		{
			vVel.Normalize();
            CreatePropDebris(vPos, -vVel, pDebris->nId);
		}

		return;
	}

	// Create the Spear powerup...

	char szSpawn[512];
	sprintf(szSpawn, "AmmoBox MPRespawn 0;AmmoType1 %s;AmmoCount1 1;Filename %s;Skin %s",
		m_pAmmoData->szName, m_pAmmoData->pProjectileFX->szModel,
		m_pAmmoData->pProjectileFX->szSkin);

	LTVector vScale = m_pAmmoData->pProjectileFX->vModelScale;

	// Make sure the spear sticks out a little ways...

	vVel.Normalize();
	vPos -= (vVel * vScale.z/2.0f);

	BaseClass* pClass = SpawnObject(szSpawn, LTVector(-10000,-10000,-10000), rRot);

	if (pClass)
	{
		g_pLTServer->ScaleObject(pClass->m_hObject, &vScale);

		LTVector vDims;
		g_pPhysicsLT->GetObjectDims(pClass->m_hObject, &vDims);
		vDims.x *= vScale.x;
		vDims.y *= vScale.y;
		vDims.z *= vScale.z;

		vDims.x *= m_pClassData->vDimsScale.x;
		vDims.y *= m_pClassData->vDimsScale.y;
		vDims.z *= m_pClassData->vDimsScale.z;

		// Make sure our dims get propagated to the clients...
		//[MURSUM] g_pCommonLT->SetObjectFlags(pClass->m_hObject, OFT_Flags2, FLAG2_SERVERDIMS, FLAG2_SERVERDIMS);
		g_pCommonLT->SetObjectFlags(pClass->m_hObject, OFT_Flags2, 0, FLAG2_SERVERDIMS);
		g_pPhysicsLT->SetObjectDims(pClass->m_hObject, &vDims, 0);

		// We don't want our pickups animating
		
		ANIMTRACKERID	nTracker;
		if( LT_OK == g_pModelLT->GetMainTracker( pClass->m_hObject, nTracker ))
		{
			g_pModelLT->SetPlaying( pClass->m_hObject, nTracker, LTFALSE );
		}

		if ( eSpearActionStickAI == eSpearAction || eSpearActionStickPlayer == eSpearAction )
		{
			g_pCommonLT->SetObjectFlags(pClass->m_hObject, OFT_User, 0, USRFLG_GLOW | USRFLG_CAN_ACTIVATE);
			g_pCommonLT->SetObjectFlags(pClass->m_hObject, OFT_Flags, 0, FLAG_TOUCH_NOTIFY);

			if ( eSpearActionStickPlayer == eSpearAction )
			{
				g_pCommonLT->SetObjectFlags(pClass->m_hObject, OFT_User, USRFLG_ATTACH_HIDE1SHOW3, USRFLG_ATTACH_HIDE1SHOW3);
			}

			// Try to attach it to the character

			CCharacter* pCharacter = dynamic_cast<CCharacter*>(g_pLTServer->HandleToObject(hObj));
			if( pCharacter && !pCharacter->AddSpear(pClass->m_hObject, pCharacter->GetModelNodeLastHit(), rRot, m_pClassData->bCanWallStick ))
			{
				// Attaching the spear failed so create some debris...

				DEBRIS* pDebris = g_pDebrisMgr->GetDebris(m_pAmmoData->szName);
				if( pDebris )
				{
					vVel.Normalize();
					CreatePropDebris(vPos, -vVel, pDebris->nId);
				}
			}
		}
		else if ( eSpearActionStickBody == eSpearAction )
		{
			g_pCommonLT->SetObjectFlags(pClass->m_hObject, OFT_User, 0, USRFLG_GLOW | USRFLG_CAN_ACTIVATE);
			g_pCommonLT->SetObjectFlags(pClass->m_hObject, OFT_Flags, 0, FLAG_TOUCH_NOTIFY);

			// Attach it to the body

			Body* pBody = (Body*)g_pLTServer->HandleToObject(hObj);
			pBody->AddSpear(pClass->m_hObject, rRot);
		}
		else // ( eSpearActionStickWorld == eSpearAction )
		{
			// Move it to the right position in the world
			g_pLTServer->SetObjectPos(pClass->m_hObject, &vPos);
		}
	}
}



// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CCameraDisabler::CCameraDisabler
//
//  PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CCameraDisabler::CCameraDisabler()
:	CProjectile()
{

}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CCameraDisabler::~CCameraDisabler
//
//  PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CCameraDisabler::~CCameraDisabler()
{

}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CCameraDisabler::HandleImpact
//
//  PURPOSE:	Handle colliding with geometry and objects, specificly a Security Camera
//
// ----------------------------------------------------------------------- //

void CCameraDisabler::HandleImpact( HOBJECT hObj )
{
	if( !m_pAmmoData )
	{
		CProjectile::HandleImpact( hObj );
		return;
	}
	
	// Check to see if we hit a security camera that can be disabled...

	uint32 dwUserFlags;
	g_pCommonLT->GetObjectFlags( hObj, OFT_User, dwUserFlags );
	
	if( dwUserFlags & USRFLG_GADGET_CAMERA_DISABLER && m_pAmmoData->eInstDamageType == DT_CAMERA_DISABLER )
	{
		// Let the camera handle what to do...

		char buf[100];
		sprintf(buf, "Gadget %d", m_pAmmoData->nId);
		SendTriggerMsgToObject(this, hObj, LTFALSE, buf);
	}
	else
	{
		// Break it!!

		DEBRIS* pDebris = g_pDebrisMgr->GetDebris(m_pAmmoData->szName);
		if (pDebris)
		{
			LTVector	vVel, vPos;
			
			g_pPhysicsLT->GetVelocity( m_hObject, &vVel );
			g_pLTServer->GetObjectPos( m_hObject, &vPos );

			vVel.Normalize();
            LTVector vNegVel = -vVel;
            CreatePropDebris(vPos, vNegVel, pDebris->nId);
		}
	}

	CProjectile::HandleImpact( hObj );
}


// ----------------------------------------------------------------------- //

LTBOOL CCoin::ShouldPlayBounceSound(SURFACE* pSurface)
{
	return (m_cBounces == 0);
}

// ----------------------------------------------------------------------- //

const char* CCoin::GetBounceSound(SURFACE* pSurface)
{
	// Hardcoded SoundBute names.  see soundbutes.txt

	if( pSurface->eType == ST_LIQUID )
		return "CoinBounceLiquid";

	return "CoinBounce";
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCoin::RotateToRest()
//
//	PURPOSE:	Rotate the grenade to its rest position...
//
// ----------------------------------------------------------------------- //

void CCoin::RotateToRest()
{
	if ( !m_bRotatedToRest )
	{
		char szSpawn[1024];
		sprintf(szSpawn, "WeaponItem Gravity 0;AmmoAmount 1;MPRespawn 0;WeaponType Coin;AmmoType Coin");

		LTVector vPos;
		g_pLTServer->GetObjectPos(m_hObject, &vPos);
		vPos.y += 2.0f; // This offsets us from the floor a bit so we don't pop through when WeaponItem sets its dims.

		LTRotation rRot;

		BaseClass* pObj = SpawnObject(szSpawn, vPos, rRot);

		if ( pObj && pObj->m_hObject )
		{
			g_pPhysicsLT->SetVelocity(pObj->m_hObject, &LTVector(0,0,0));
		}

		g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_VISIBLE);
		g_pLTServer->RemoveObject(m_hObject);
	}

	CGrenade::RotateToRest();

	if ( IsCharacter(m_hFiredFrom) )
	{
		LTVector vPosition;
		g_pLTServer->GetObjectPos(m_hObject, &vPosition);

        CCharacter* pCharacter = (CCharacter*)g_pLTServer->HandleToObject(m_hFiredFrom);

		LTFLOAT fVolume;
		SURFACE* pSurf = g_pSurfaceMgr->GetSurface(m_eLastHitSurface);
		_ASSERT(pSurf);
		if (pSurf)
		{
			fVolume = pSurf->fMovementNoiseModifier;
		}
		else
		{
			fVolume = 1.0f;
		}

		ASSERT(m_pAmmoData && m_pAmmoData->pImpactFX);
		if ( (!m_pAmmoData) || (!m_pAmmoData->pImpactFX) ) return;
		

		// Get the Distance that the impact noise carries
		float fWeaponImpactNoiseDistance = fVolume * (float)m_pAmmoData->pImpactFX->nAISoundRadius;
#ifdef USE_AI
		// Register EnemyCoinSound stimulus.
		g_pAIStimulusMgr->RegisterStimulus(kStim_EnemyCoinSound, m_hFiredFrom, vPosition, fWeaponImpactNoiseDistance);
#endif
	}
}


#ifdef USE_AI
// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::CKitty
//
//  PURPOSE:	Constructor...
//
// ----------------------------------------------------------------------- //

CKitty::CKitty()
:	CProjectile		( ),
	m_bArmed		( LTFALSE ),
	m_bActivated	( LTFALSE ),
	m_hActivator	( LTNULL ),
	m_hSpawnedKitty	( LTNULL ),
	m_pClassData	( LTNULL ),
	m_eStimID		( kStimID_Unset ),
	m_hArmSound		( NULL )
{
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::~CKitty
//
//  PURPOSE:	Destructor...
//
// ----------------------------------------------------------------------- //

CKitty::~CKitty()
{
	if( m_hSpawnedKitty )
	{
		g_pLTServer->RemoveObject( m_hSpawnedKitty );
	}
	if( m_eStimID != kStimID_Unset )
	{
		g_pAIStimulusMgr->RemoveStimulus( m_eStimID );
	}

	if( m_hArmSound )
	{
		g_pLTServer->SoundMgr()->KillSound( m_hArmSound );
		m_hArmSound = LTNULL;
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::Setup
//
//  PURPOSE:	Setup the kitty... meow!
//
// ----------------------------------------------------------------------- //

LTBOOL CKitty::Setup(CWeapon const* pWeapon, WeaponFireInfo const& info)
{
	// Let the base class setup first...

	if( !CProjectile::Setup( pWeapon, info ))
		return LTFALSE;

	// Get the class data...

	if( !m_pAmmoData || !m_pAmmoData->pProjectileFX ||
		!m_pAmmoData->pProjectileFX->pClassData ) return LTFALSE;

	m_pClassData = (KITTYCLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;

	// Set the Kitty on the ground...

	HOBJECT hFilter[] = { m_hObject, m_hFiredFrom, LTNULL };
	MoveObjectToFloor( m_hObject, hFilter, MoveToFloorFilterFn );

	// Rotate the Kitty so it's standing up... (cats always land on their feet ;)

	LTVector	vF, vR;

	if( (1.0f == m_vDir.y) || (-1.0f == m_vDir.y) )
	{
		vR = m_vDir.Cross( LTVector( 1.0f, 0.0f, 0.0f ));
	}
	else
	{
		vR = m_vDir.Cross( LTVector( 0.0f, 1.0f, 0.0f ));
	}

	vF = vR.Cross( LTVector(0, -1, 0) );
	vF.Normalize();
	
    LTRotation rRot(vF, LTVector(0, 1, 0) );
	g_pLTServer->SetObjectRotation(m_hObject, &rRot);
	
	char szSpawn[1024];
	sprintf(szSpawn, "WeaponItem Gravity 0;AmmoAmount 1;MPRespawn 0;WeaponType %s;AmmoType %s; MoveToFloor 0, DMTouchPickup 0; Team %s",
		m_pWeaponData->szName, m_pAmmoData->szName, TeamIdToString( GetTeamId( )));

	LTVector vPos;
	g_pLTServer->GetObjectPos( m_hObject, &vPos );

	BaseClass* pObj = SpawnObject(szSpawn, vPos, rRot);
	if( pObj && pObj->m_hObject )
	{
		g_pPhysicsLT->SetVelocity(pObj->m_hObject, &LTVector(0,0,0));
		m_hSpawnedKitty = pObj->m_hObject;

		// Since we now have a duplicate object that can be picked up set the actual kitty invisible...

		g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_VISIBLE );

		// Enlarge the dims of the Kitty object to make sure it is hit by raycasts before the spawned WeaponItem...
		
		LTVector vDims;
		g_pPhysicsLT->GetObjectDims( pObj->m_hObject, &vDims );
		vDims *= 1.1f;
		g_pPhysicsLT->SetObjectDims( m_hObject, &vDims, 0 );
	}
	
	// Decide when the kitty should be armed...

	LTFLOAT fDelay = m_pClassData->fArmDelay;

	m_ArmTimer.Start( fDelay );

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::CreateDangerStimulus
//
//  PURPOSE:	Create a stimulus for AI to notice...
//
// ----------------------------------------------------------------------- //

LTBOOL CKitty::CreateDangerStimulus(HOBJECT hFiredFrom)
{
	LTVector vPos;
	g_pLTServer->GetObjectPos( m_hObject, &vPos );
	m_eStimID = g_pAIStimulusMgr->RegisterStimulus( kStim_EnemyDangerVisible, hFiredFrom, m_hObject, vPos, 1.f, 1.f );

	return LTTRUE; 
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::EngineMessageFn
//
//  PURPOSE:	Handle messages from the engine...
//
// ----------------------------------------------------------------------- //

uint32 CKitty::EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData )
{
	switch( messageID )
	{
		case MID_UPDATE :
		{
			UpdateKitty( );
		}

		default : break;
	}

	return CProjectile::EngineMessageFn( messageID, pData, fData );
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::HandleImpact
//
//  PURPOSE:	What did we hit?
//
// ----------------------------------------------------------------------- //

void CKitty::HandleImpact( HOBJECT hObj )
{
	// Do nothing...
	// The prjectile base will Detonate so just override with an empty call.
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::UpdateKitty
//
//  PURPOSE:	Update the kitty...
//
// ----------------------------------------------------------------------- //

void CKitty::UpdateKitty( )
{
	if( !m_pClassData ) return;

	// Have we lost all nine lives...

	if( ShouldDetonate() )
	{	
		Detonate( LTNULL );
		return;
	}

	if( m_bActivated )
	{
		UpdateMovement( );
		return;
	}

	// See if it is time to arm yet...

	if( !m_bArmed && m_ArmTimer.Stopped() )
	{
        m_bArmed = LTTRUE;

		// Play armed sound...

		if( m_pClassData->pArmSound[0] )
		{
			LTVector vPos;
            g_pLTServer->GetObjectPos( m_hObject, &vPos );

			uint32 nIgnoreFlags = (CC_ALL_FLAG & ~ ::GetLiquidFlags());
			int nVolume = IsLiquid( ::GetContainerCode(vPos, nIgnoreFlags) ) ? 50 : 100;

			uint32 dwFlags = m_pClassData->bLoopArmSound ? PLAYSOUND_LOOP | PLAYSOUND_GETHANDLE : 0;

			m_hArmSound = g_pServerSoundMgr->PlaySoundFromObject( m_hObject, m_pClassData->pArmSound,
							(LTFLOAT)m_pClassData->nSoundRadius, SOUNDPRIORITY_MISC_MEDIUM,
							dwFlags, nVolume );
		}
		
		// Play armed ClientFX...

		if( m_hSpawnedKitty )
		{
			WeaponItem *pSpawnedKitty = dynamic_cast<WeaponItem*>(g_pLTServer->HandleToObject( m_hSpawnedKitty ));
			if( pSpawnedKitty )
			{
				if( IsTeamGameType() && m_pClassData->pArmedFXBlue[0] && m_pClassData->pArmedFXRed[0] )
				{
					if( m_nTeamId == 0 )
					{
						pSpawnedKitty->SetClientFX( m_pClassData->pArmedFXBlue );
					}
					else
					{
						pSpawnedKitty->SetClientFX( m_pClassData->pArmedFXRed );
					}
				}
				else if( m_pClassData->pArmedFX[0] )
				{
					pSpawnedKitty->SetClientFX( m_pClassData->pArmedFX );
				}
			}
		}
	}

	if( !m_hSpawnedKitty )
	{
		// If our spawned weapon item was picked up reomve the kitty...

		RemoveObject();
	}

	// Are there any Characters close enough to go active?

	if( !m_bActivated && m_bArmed )
	{
		CheckActivation( );
	}
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::UpdateMovement
//
//  PURPOSE:	Continue to track and chase the character who activated us...
//
// ----------------------------------------------------------------------- //

void CKitty::UpdateMovement( )
{
	if( !m_hActivator )
		return;

	// Get the direction we need to go...

	LTVector	vActPos;
	g_pLTServer->GetObjectPos( m_hActivator, &vActPos );
	
	LTVector	vPos;
	g_pLTServer->GetObjectPos( m_hObject, &vPos );

	LTVector	vDir = vActPos - vPos;
	vDir.Normalize();

	// Intersect with the world to find the poly we're standing on...

    LTVector vDims;
	g_pPhysicsLT->GetObjectDims( m_hObject, &vDims );

	LTVector vDown(0.0f, -10000.0f, 0.0f);

	IntersectQuery IQuery;
	IntersectInfo IInfo;

	IQuery.m_From		= vPos;
	IQuery.m_To			= vPos + vDown;
	IQuery.m_Flags		= IGNORE_NONSOLID | INTERSECT_OBJECTS | INTERSECT_HPOLY;
	IQuery.m_FilterFn	= NULL;
	IQuery.m_pUserData	= NULL;

    if( g_pLTServer->IntersectSegment( &IQuery, &IInfo ))
	{
        LTFLOAT fDist = vPos.y - IInfo.m_Point.y;
		
		// Put us in the correct pos;
		
		vPos.y -= (fDist - (vDims.y + 0.1f));
		g_pLTServer->SetObjectPos( m_hObject, &vPos );
	}


	// Get the normal of any plane we are intersecting...
	
	LTVector vPlaneNormal( 0, 1, 0 );
	if( IInfo.m_hObject )
	{
		vPlaneNormal = IInfo.m_Plane.m_Normal;
	}

	// Rotate the Kitty so it's standing up... (cats always land on their feet ;)

	LTVector	vF, vR;

	if( (1.0f == vDir.y) || (-1.0f == vDir.y) )
	{
		vR = vDir.Cross( LTVector( 1.0f, 0.0f, 0.0f ));
	}
	else
	{
		vR = vDir.Cross( vPlaneNormal );
	}

	vF = vR.Cross( -vPlaneNormal );
	vF.Normalize();
	
    LTRotation rRot(vF, vPlaneNormal );
	g_pLTServer->SetObjectRotation( m_hObject, &rRot );

	
	LTVector vVel = vF * m_pClassData->fChaseVelocity;
	g_pPhysicsLT->SetVelocity( m_hObject, &vVel );
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::ShouldDetonate
//
//  PURPOSE:	Check to see if we should blow up...
//
// ----------------------------------------------------------------------- //

bool CKitty::ShouldDetonate( )
{
	// If we weren't active we shouldn't blow up...

	if( !m_bActivated )
		return false;

	// Has it been long enough...

	if( m_DetonateTimer.Stopped() )
		return true;

	// If the object we were chasing gets removed just blow up...

	if( !m_hActivator )
		return true;

	// Is any character within our detonation range...

	float fRadius = (float)m_pClassData->nDetonateRadius;
	
	LTVector vPos;
	g_pLTServer->GetObjectPos( m_hObject, &vPos );

	LTVector vDims;
	g_pPhysicsLT->GetObjectDims( m_hObject, &vDims );

	// Move the center of the radius check up so it is closer to the character pos...

	vPos.y += vDims.y;

	if( g_pCharacterMgr->FindCharactersWithinRadius( LTNULL, vPos, fRadius ))
	{
		return true;
	}

	return false;
}

void CKitty::CheckActivation( )
{
	float fRadius = (float)m_pClassData->nActivateRadius;

	LTVector vPos;
	g_pLTServer->GetObjectPos( m_hObject, &vPos );

	CTList<CCharacter*>	lstChars;
	CCharacter			*pChar = LTNULL;

	if( g_pCharacterMgr->FindCharactersWithinRadius( &lstChars, vPos, fRadius ))
	{
		pChar = *(lstChars.GetItem( TLIT_FIRST ));
		if( pChar )
		{
			// Don't activate if it's our owner/team...
			if( IsTeamGameType( ))
			{
				if( IsMyTeam( pChar->m_hObject ))
					return;
			}
			else if( pChar->m_hObject == m_hFiredFrom )
			{
				return;	
			}

			// Make sure the kitty can actually see the character...

			IntersectQuery	IQuery;
			IntersectInfo	IInfo;

			IQuery.m_Flags		= CHECK_FROM_POINT_INSIDE_OBJECTS | INTERSECT_OBJECTS | INTERSECT_HPOLY | IGNORE_NONSOLID;
			
			HOBJECT hFilterList[] = { m_hObject, m_hSpawnedKitty, LTNULL };
			IQuery.m_FilterFn	= ObjListFilterFn;
			IQuery.m_pUserData	= hFilterList;

			IQuery.m_From		= vPos;
			g_pLTServer->GetObjectPos( pChar->m_hObject, &IQuery.m_To );

			if( g_pLTServer->IntersectSegment( &IQuery, &IInfo ))
			{
				if( IInfo.m_hObject != pChar->m_hObject &&
					IInfo.m_hObject != pChar->GetHitBox() )
				{
					// The Kitty can't actually see the character so don't activate yet...
					
					return;
				}
			}

			// Go active and save the object we want to chase...

			m_bActivated = LTTRUE;
			m_hActivator = pChar->m_hObject;

			m_DetonateTimer.Start( m_pClassData->fDetonateTime );

			// Play activation sound...

			if( m_pClassData->pActivateSound[0] )
			{
				LTVector vPos;
				g_pLTServer->GetObjectPos( m_hObject, &vPos );

				uint32 nIgnoreFlags = (CC_ALL_FLAG & ~ ::GetLiquidFlags());
				int nVolume = IsLiquid( ::GetContainerCode(vPos, nIgnoreFlags) ) ? 50 : 100;

				g_pServerSoundMgr->PlaySoundFromPos( vPos, m_pClassData->pActivateSound,
					(LTFLOAT)m_pClassData->nSoundRadius,
					SOUNDPRIORITY_MISC_MEDIUM, 0, nVolume);
			}

			// Once activated remove the weapon item pickup and set the actuall kitty visible again...

			if( m_hSpawnedKitty )
			{
				g_pLTServer->RemoveObject( m_hSpawnedKitty );
			}

			g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE );

			// Set the walk animation for the kitty...

			HMODELANIM dwAni = g_pLTServer->GetAnimIndex( m_hObject, "Walk" );
			if( dwAni != INVALID_ANI )
			{
				g_pLTServer->SetModelAnimation( m_hObject, dwAni );
			}

		}
	}
}




// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::Save
//
//  PURPOSE:	Save the kitty object...
//
// ----------------------------------------------------------------------- //

void CKitty::Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags )
{
	if( !pMsg ) return;

	CProjectile::Save( pMsg, dwSaveFlags );

	m_DetonateTimer.Save( pMsg );
	m_ArmTimer.Save( pMsg );

	SAVE_bool( m_bArmed );
	SAVE_bool( m_bActivated );
	
	SAVE_HOBJECT( m_hActivator );

	SAVE_HOBJECT( m_hSpawnedKitty );
	SAVE_DWORD(m_eStimID);
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CKitty::Load
//
//  PURPOSE:	Load the Kitty object...
//
// ----------------------------------------------------------------------- //

void CKitty::Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags )
{
	if( !pMsg ) return;

	CProjectile::Load( pMsg, dwLoadFlags );

	m_DetonateTimer.Load( pMsg );
	m_ArmTimer.Load( pMsg );

	LOAD_bool( m_bArmed	);
	LOAD_bool( m_bActivated	);

	LOAD_HOBJECT( m_hActivator );

	LOAD_HOBJECT( m_hSpawnedKitty );
	LOAD_DWORD_CAST(m_eStimID, EnumAIStimulusID);

	m_pClassData = LTNULL;
	if( m_pAmmoData && m_pAmmoData->pProjectileFX &&
		m_pAmmoData->pProjectileFX->pClassData )
	{
		m_pClassData = (KITTYCLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;
	}
}

#endif //USE_AI



// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBearTrap::CBearTrap
//
//  PURPOSE:	Constructor....
//
// ----------------------------------------------------------------------- //

CBearTrap::CBearTrap( )
:	CProjectile			( ),
	m_bArmed			( LTFALSE ),
	m_hSpawnedBearTrap	( LTNULL ),
	m_pClassData		( LTNULL )
{
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBearTrap::~CBearTrap
//
//  PURPOSE:	Destructor...
//
// ----------------------------------------------------------------------- //

CBearTrap::~CBearTrap( )
{

}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBearTrap::EngineMessageFn
//
//  PURPOSE:	Handle messages from the engine...
//
// ----------------------------------------------------------------------- //

uint32 CBearTrap::EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData )
{
	switch( messageID )
	{
		case MID_UPDATE :
		{
			UpdateTrap( );
		}

		default : break;
	}

	return CProjectile::EngineMessageFn( messageID, pData, fData );
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBearTrap::UpdateTrap
//
//  PURPOSE:	Update
//
// ----------------------------------------------------------------------- //

void CBearTrap::UpdateTrap( )
{
	if( !m_pClassData ) return;

	// See if it is time to arm yet...

	if( !m_bArmed && m_ArmTimer.Stopped() )
	{
        m_bArmed = LTTRUE;
	}
	
	if( !m_hSpawnedBearTrap )
	{
		RemoveObject();
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBearTrap::Setup
//
//  PURPOSE:	Lay the trap down at the players feet...
//
// ----------------------------------------------------------------------- //

LTBOOL CBearTrap::Setup( const CWeapon *pWeapon, const WeaponFireInfo &info )
{
	// Let the base class setup first...

	if( !CProjectile::Setup( pWeapon, info ))
		return LTFALSE;

	// Get the class data...

	if( !m_pAmmoData || !m_pAmmoData->pProjectileFX ||
		!m_pAmmoData->pProjectileFX->pClassData ) return LTFALSE;

	m_pClassData = (BEARTRAPCLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;

	// Let the object that fired us take damage from us.

	m_bCanTouchFiredFromObj = LTTRUE;

	// Set the world animation...

	HMODELANIM dwAni = g_pLTServer->GetAnimIndex( m_hObject, "World" );
	if( dwAni != INVALID_ANI )
	{
		g_pLTServer->SetModelAnimation( m_hObject, dwAni );
	}

	// Set the Trap to the ground...

	HOBJECT hFilter[] = { m_hObject, m_hFiredFrom, LTNULL };
	MoveObjectToFloor( m_hObject, hFilter, MoveToFloorFilterFn );

	// Rotate the Trap so it's face up...

	LTVector	vF, vR;

	if( (1.0f == m_vDir.y) || (-1.0f == m_vDir.y) )
	{
		vR = m_vDir.Cross( LTVector( 1.0f, 0.0f, 0.0f ));
	}
	else
	{
		vR = m_vDir.Cross( LTVector( 0.0f, 1.0f, 0.0f ));
	}

	vF = vR.Cross( LTVector(0, -1, 0) );
	vF.Normalize();
	
    LTRotation rRot(vF, LTVector(0, 1, 0) );
	g_pLTServer->SetObjectRotation(m_hObject, &rRot);

	char szSpawn[1024];
	sprintf(szSpawn, "WeaponItem Gravity 0;AmmoAmount 1;MPRespawn 0;WeaponType %s;AmmoType %s; MoveToFloor 0, DMTouchPickup 0; Team %s",
		m_pWeaponData->szName, m_pAmmoData->szName, TeamIdToString( GetTeamId( )));

	LTVector vPos;
	g_pLTServer->GetObjectPos( m_hObject, &vPos );

	// Set the direction and fired from position so when a character steps on us the information 
	// is accurate for testing against the characters nodes...

	m_vDir.Init( 0.0f, 1.0f, 0.0f );
	m_vFirePos = vPos;

	BaseClass* pObj = SpawnObject(szSpawn, vPos, rRot);
	if( pObj && pObj->m_hObject )
	{
		g_pPhysicsLT->SetVelocity(pObj->m_hObject, &LTVector(0,0,0));
		m_hSpawnedBearTrap = pObj->m_hObject;


		// Reset the model file to the projectile model...
		
		ObjectCreateStruct ocs;
		LTStrCpy( ocs.m_Filename, m_pAmmoData->pProjectileFX->szModel, ARRAY_LEN( ocs.m_Filename ));
		g_pCommonLT->SetObjectFilenames( m_hSpawnedBearTrap, &ocs );

		// Set the world animation...

		HMODELANIM dwAni = g_pLTServer->GetAnimIndex( m_hSpawnedBearTrap, "World" );
		if( dwAni != INVALID_ANI )
		{
			g_pLTServer->SetModelAnimation( m_hSpawnedBearTrap, dwAni );
		}

		// Since we now have a duplicate object that can be picked up set the actual kitty invisibel...

		g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_VISIBLE );
	}


	// Start our armming timer so our owner has a chance to move...

	m_ArmTimer.Start( m_pClassData->fArmDelay );

	// Turn off point collision so we use box physics to get better collision detection against characters...

	g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags, FLAG_TOUCH_NOTIFY, FLAG_TOUCH_NOTIFY | FLAG_SOLID | FLAG_POINTCOLLIDE );

	return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBearTrap::HandleImpact
//
//  PURPOSE:	NONE
//
// ----------------------------------------------------------------------- //

void CBearTrap::HandleImpact( HOBJECT hObj )
{
	if( IsCharacter( hObj ) || IsCharacterHitBox( hObj ) )
	{
		CCharacter *pChar = dynamic_cast<CCharacter*>(g_pLTServer->HandleToObject( m_hFiredFrom ));
		if( pChar && !m_bArmed)
		{
			// Don't detonate on the owner or it's hit box untill it's armed....
			
			if( (hObj == m_hFiredFrom) || (hObj == pChar->GetHitBox() ))
			{
				return;
			}
		}

		// Check if it is a player object or check the hitbox model object for a player...

		if( IsPlayer( hObj ) || IsCharacterHitBox( hObj ) )
		{
			CPlayerObj *pPlayerObj = LTNULL;
			
			if( IsPlayer( hObj ) )
			{
				pPlayerObj = dynamic_cast<CPlayerObj*>(g_pLTServer->HandleToObject( hObj ));
			}
			else
			{
				CCharacterHitBox* pHitBox = dynamic_cast<CCharacterHitBox*>(g_pLTServer->HandleToObject( hObj ));
				if( pHitBox && pHitBox->GetModelObject() )
				{
					if( IsPlayer( pHitBox->GetModelObject() ))
					{
						pPlayerObj = dynamic_cast<CPlayerObj*>(g_pLTServer->HandleToObject( pHitBox->GetModelObject() ));
					}
				}
			}

			if( pPlayerObj )
			{
				// It's a player so make sure they aren't on a vehicle...

				if( pPlayerObj->GetPlayerPhysicsModel() != PPM_NORMAL )
				{
					return;
				}

				if( false == g_pServerMissionMgr->IsNetFriendlyFire() && IsMyTeam( pPlayerObj->m_hObject ))
					return;

			}
		}
	
		Detonate( hObj );

		if( m_hSpawnedBearTrap )
		{
			g_pLTServer->RemoveObject( m_hSpawnedBearTrap );
		}
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBearTrap::Detonate
//
//  PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CBearTrap::Detonate( HOBJECT hObj )
{
	// Show the beartrap snap shut but don't get rid of the pickup item so it can be reused...

	if( m_hSpawnedBearTrap )
	{
		HMODELANIM dwAni = g_pLTServer->GetAnimIndex( m_hSpawnedBearTrap, "Activate" );
		if( dwAni != INVALID_ANI )
		{
			g_pLTServer->SetModelAnimation( m_hSpawnedBearTrap, dwAni );
			g_pLTServer->SetModelPlaying( m_hSpawnedBearTrap, true );
			g_pLTServer->SetModelLooping( m_hSpawnedBearTrap, false );
		}
	}

	CProjectile::Detonate( hObj );
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBearTrap::Save
//
//  PURPOSE:	Save the Trap...
//
// ----------------------------------------------------------------------- //

void CBearTrap::Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags )
{
	if( !pMsg ) return;

	CProjectile::Save( pMsg, dwSaveFlags );
	
	m_ArmTimer.Save( pMsg );

	SAVE_bool( m_bArmed );
	SAVE_HOBJECT( m_hSpawnedBearTrap );
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBearTrap::Load
//
//  PURPOSE:	Load the Trap...
//
// ----------------------------------------------------------------------- //

void CBearTrap::Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags )
{
	if( !pMsg ) return;

	CProjectile::Load( pMsg, dwLoadFlags );

	m_ArmTimer.Load( pMsg );

	LOAD_bool( m_bArmed );
	LOAD_HOBJECT( m_hSpawnedBearTrap );

	m_pClassData = LTNULL;
	if( m_pAmmoData && m_pAmmoData->pProjectileFX &&
		m_pAmmoData->pProjectileFX->pClassData )
	{
		m_pClassData = (BEARTRAPCLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBanana::CBanana
//
//  PURPOSE:	Constructor....
//
// ----------------------------------------------------------------------- //

CBanana::CBanana()
:	CGrenade				( ),
	m_bArmed				( false ),
	m_pClassData			( LTNULL ),
	m_hSpawnedBanana		( LTNULL ),
	m_bMovedToSpawnedPos	( false )
{
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBanana::~CBanana
//
//  PURPOSE:	Destructor...
//
// ----------------------------------------------------------------------- //

CBanana::~CBanana()
{
	if( m_hSpawnedBanana )
		g_pLTServer->RemoveObject( m_hSpawnedBanana );

	m_hSpawnedBanana = LTNULL;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBanana::EngineMessageFn
//
//  PURPOSE:	Handle messages from the engine...
//
// ----------------------------------------------------------------------- //

uint32 CBanana::EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData )
{
	switch( messageID )
	{
		case MID_UPDATE :
		{
			UpdateBanana( );
		}

		default : break;
	}

	return CGrenade::EngineMessageFn( messageID, pData, fData );
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBanana::UpdateBanana
//
//  PURPOSE:	Update
//
// ----------------------------------------------------------------------- //

void CBanana::UpdateBanana( )
{
	if( !m_pClassData ) return;

	// If at rest we should have our spawned object, otherwise we need to go away...

	if( m_bRotatedToRest )
	{
		if( m_hSpawnedBanana && !m_bMovedToSpawnedPos )
		{
			LTVector vPos;
			g_pLTServer->GetObjectPos( m_hSpawnedBanana, &vPos );

			vPos.y += 32;

			g_pLTServer->SetObjectPos( m_hObject, &vPos );

			HOBJECT hFilter[] = { m_hObject, m_hFiredFrom, m_hSpawnedBanana, LTNULL	};
			m_bMovedToSpawnedPos = !!MoveObjectToFloor( m_hObject, hFilter, MoveToFloorFilterFn );
		}
	
		if( !m_hSpawnedBanana )
			RemoveObject();
	}

	// See if it is time to arm yet...

	if( !m_bArmed && m_ArmTimer.On() && m_ArmTimer.Stopped() )
	{
        m_bArmed = LTTRUE;
	} 
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBanana::Setup
//
//  PURPOSE:	NONE
//
// ----------------------------------------------------------------------- //

LTBOOL CBanana::Setup( const CWeapon *pWeapon, const WeaponFireInfo &info )
{
	// Let the base class setup first...

	if( !CGrenade::Setup( pWeapon, info ))
		return LTFALSE;

	// Get the class data...

	if( !m_pAmmoData || !m_pAmmoData->pProjectileFX ||
		!m_pAmmoData->pProjectileFX->pClassData ) return LTFALSE;

	m_pClassData = (BANANACLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;

	return LTTRUE;

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBanana::HandleImpact()
//
//	PURPOSE:	Handle touching the world and characters...
//
// ----------------------------------------------------------------------- //

void CBanana::HandleImpact( HOBJECT hObj )
{
	if (IsBody(hObj))
		return; 

	if( IsCharacter( hObj ) || IsCharacterHitBox( hObj ))
	{
		// Check if it is a player object or check the hitbox model object for a player...

		if( IsPlayer( hObj ) || IsCharacterHitBox( hObj ) )
		{
			CPlayerObj *pPlayerObj = LTNULL;
			
			if( IsPlayer( hObj ) )
			{
				pPlayerObj = dynamic_cast<CPlayerObj*>(g_pLTServer->HandleToObject( hObj ));
			}
			else
			{
				CCharacterHitBox* pHitBox = dynamic_cast<CCharacterHitBox*>(g_pLTServer->HandleToObject( hObj ));
				if( pHitBox && pHitBox->GetModelObject() )
				{
					if( IsPlayer( pHitBox->GetModelObject() ))
					{
						pPlayerObj = dynamic_cast<CPlayerObj*>(g_pLTServer->HandleToObject( pHitBox->GetModelObject() ));
					}
				}
			}

			if( pPlayerObj )
			{
				// It's a player so make sure they aren't on a vehicle...

				if( pPlayerObj->GetPlayerPhysicsModel() != PPM_NORMAL )
				{
					return;
				}

				if( false == g_pServerMissionMgr->IsNetFriendlyFire() && IsMyTeam( pPlayerObj->m_hObject ))
					return;

			}
		}

		
		// They stepped on us so slip them...

		Detonate( LTNULL );
	}
	else
	{
		// Bounce off the ground and come to a rest...
		
		CGrenade::HandleImpact(	hObj );
	}
}

void CBanana::Detonate( HOBJECT hObj )
{
	// Create banana debris...

	if( m_pAmmoData && !m_bDetonated )
	{
		DEBRIS *pDebris = g_pDebrisMgr->GetDebris(m_pAmmoData->szName);
		if( pDebris )
		{
			LTVector vPos;
			g_pLTServer->GetObjectPos( m_hObject, &vPos );

			::CreatePropDebris( vPos, LTVector( 0.0f, 1.0f, 0.0f ), pDebris->nId );
		}
	}

	CGrenade::Detonate( hObj );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBanana::ShouldPlayBounceSound()
//
//	PURPOSE:	Are we allowed to play a bounce sound...
//
// ----------------------------------------------------------------------- //

LTBOOL CBanana::ShouldPlayBounceSound(SURFACE* pSurface)
{
	return (m_cBounces == 0);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBanana::GetBounceSound()
//
//	PURPOSE:	Grab the hardcoded sound path...
//
// ----------------------------------------------------------------------- //

const char* CBanana::GetBounceSound(SURFACE* pSurface)
{
	// Hardcoded SoundBute names.  see soundbutes.txt

	if( pSurface->eType == ST_LIQUID )
		return "BananaBounceLiquid";

	return "BananaBounce";
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBanana::RotateToRest()
//
//	PURPOSE:	Rotate the grenade to its rest position...
//
// ----------------------------------------------------------------------- //

void CBanana::RotateToRest()
{
	// Go invisible since the spawned pickup is there and we still need to recieve touch notifies in order to detonate...

	g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags, FLAG_TOUCH_NOTIFY, FLAG_VISIBLE | FLAG_TOUCH_NOTIFY | FLAG_SOLID | FLAG_POINTCOLLIDE);

	if ( !m_bRotatedToRest )
	{
		if( !m_pWeaponData || !m_pAmmoData )
			return;

		char szSpawn[1024];
		sprintf(szSpawn, "WeaponItem Gravity 0;AmmoAmount 1;MPRespawn 0;WeaponType %s;AmmoType %s; MoveToFloor 0, DMTouchPickup 0; Team %s",
			m_pWeaponData->szName, m_pAmmoData->szName, TeamIdToString( GetTeamId( )));

		LTVector vPos;
		g_pLTServer->GetObjectPos(m_hObject, &vPos);
		vPos.y += 32.0f; // This offsets us from the floor a bit so we don't pop through when WeaponItem sets its dims.

		// Set the direction and fired from position so when a character slips on us the information 
		// is accurate for testing against the characters nodes...

		m_vDir.Init( 0.0f, 1.0f, 0.0f );
		m_vFirePos = vPos;

		// Randomize the yaw so the bananas dont all face the same direction...

		LTRotation rRot( 0.0f, GetRandom( -MATH_CIRCLE, MATH_CIRCLE ), 0.0f );

		BaseClass* pObj = SpawnObject(szSpawn, vPos, rRot);

		if ( pObj && pObj->m_hObject )
		{
			// Set it on the floor and zero out it's velocity...

			HOBJECT hFilter[] = { m_hObject, m_hFiredFrom, LTNULL	};
			MoveObjectToFloor( pObj->m_hObject, hFilter, MoveToFloorFilterFn );

			g_pPhysicsLT->SetVelocity(pObj->m_hObject, &LTVector(0,0,0));
			m_hSpawnedBanana = pObj->m_hObject;

			// Reset the model file to the projectile model...
		
			ObjectCreateStruct ocs;
			LTStrCpy( ocs.m_Filename, m_pAmmoData->pProjectileFX->szModel, ARRAY_LEN( ocs.m_Filename ));
			g_pCommonLT->SetObjectFilenames( m_hSpawnedBanana, &ocs );

			// Set the world animation...

			HMODELANIM dwAni = g_pLTServer->GetAnimIndex( m_hSpawnedBanana, "World" );
			if( dwAni != INVALID_ANI )
			{
				g_pLTServer->SetModelAnimation( m_hSpawnedBanana, dwAni );
			}
		}

		// Start our armming timer so our owner has a chance to move...

		m_ArmTimer.Start( m_pClassData->fArmDelay );

		// Let the object that fired us take damage from us.

		m_bCanTouchFiredFromObj = LTTRUE;
	}

	CGrenade::RotateToRest();

	if ( IsCharacter(m_hFiredFrom) )
	{
		LTVector vPosition;
		g_pLTServer->GetObjectPos(m_hObject, &vPosition);

        CCharacter* pCharacter = (CCharacter*)g_pLTServer->HandleToObject(m_hFiredFrom);

		LTFLOAT fVolume;
		SURFACE* pSurf = g_pSurfaceMgr->GetSurface(m_eLastHitSurface);
		_ASSERT(pSurf);
		if (pSurf)
		{
			fVolume = pSurf->fMovementNoiseModifier;
		}
		else
		{
			fVolume = 1.0f;
		}
		ASSERT(m_pAmmoData && m_pAmmoData->pImpactFX);
		if ( (!m_pAmmoData) || (!m_pAmmoData->pImpactFX) ) return;
		

		// Get the Distance that the impact noise carries
		float fWeaponImpactNoiseDistance = fVolume * (float)m_pAmmoData->pImpactFX->nAISoundRadius;

#ifdef USE_AI
		// Register EnemyDisturbanceSound stimulus.
		g_pAIStimulusMgr->RegisterStimulus(kStim_EnemyDisturbanceSound, m_hFiredFrom, vPosition, fWeaponImpactNoiseDistance);
#endif
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBanana::Save
//
//  PURPOSE:	Save the Banana...
//
// ----------------------------------------------------------------------- //

void CBanana::Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags )
{
	if( !pMsg ) return;

	CGrenade::Save( pMsg, dwSaveFlags );
	
	m_ArmTimer.Save( pMsg );

	SAVE_bool( m_bArmed );
	SAVE_HOBJECT( m_hSpawnedBanana );
	SAVE_bool( m_bMovedToSpawnedPos );
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CBanana::Load
//
//  PURPOSE:	Load the Banana...
//
// ----------------------------------------------------------------------- //

void CBanana::Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags )
{
	if( !pMsg ) return;

	CGrenade::Load( pMsg, dwLoadFlags );

	m_ArmTimer.Load( pMsg );

	LOAD_bool( m_bArmed );
	LOAD_HOBJECT( m_hSpawnedBanana );
	LOAD_bool( m_bMovedToSpawnedPos );

	m_pClassData = LTNULL;
	if( m_pAmmoData && m_pAmmoData->pProjectileFX &&
		m_pAmmoData->pProjectileFX->pClassData )
	{
		m_pClassData = (BANANACLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;
	}
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CProjectileSpawner::CProjectileSpawner
//
//  PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CProjectileSpawner::CProjectileSpawner()
: CProjectile	()
{

}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CProjectileSpawner::~CProjectileSpawner
//
//  PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CProjectileSpawner::~CProjectileSpawner()
{

}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CProjectileSpawner::Setup
//
//  PURPOSE:	Setup the spawner
//
// ----------------------------------------------------------------------- //

LTBOOL CProjectileSpawner::Setup(CWeapon const* pWeapon, WeaponFireInfo const& info)
{
	// Let the base class setup first

	if( !CProjectile::Setup( pWeapon, info ))
		return LTFALSE;

	if( !m_pAmmoData || !m_pAmmoData->pProjectileFX ||
		!m_pAmmoData->pProjectileFX->pClassData ) return LTFALSE;

	m_pClassData = (SPAWNCLASSDATA*)m_pAmmoData->pProjectileFX->pClassData;

	return LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CProjectileSpawner::HandleImpact
//
//  PURPOSE:	NONE
//
// ----------------------------------------------------------------------- //

void CProjectileSpawner::HandleImpact( HOBJECT hObj )
{
	LTVector vVel;
	g_pPhysicsLT->GetVelocity( m_hObject, &vVel );

	CollisionInfo CollInfo;
	g_pLTServer->GetLastCollision( &CollInfo );

	// Calculate where we really hit the world...
    
	if( IsMainWorld(hObj) || IsWorldModel(hObj) )
	{
        LTVector vPos, vCurVel, vP0, vP1;
        g_pLTServer->GetObjectPos(m_hObject, &vPos);

		vP1 = vPos;
        vCurVel = vVel * g_pLTServer->GetFrameTime();
		vP0 = vP1 - vCurVel;
		vP1 += vCurVel;

        LTFLOAT fDot1 = VEC_DOT(CollInfo.m_Plane.m_Normal, vP0) - CollInfo.m_Plane.m_Dist;
        LTFLOAT fDot2 = VEC_DOT(CollInfo.m_Plane.m_Normal, vP1) - CollInfo.m_Plane.m_Dist;

		if (fDot1 < 0.0f && fDot2 < 0.0f || fDot1 > 0.0f && fDot2 > 0.0f)
		{
			vPos = vP1;
		}
		else
		{
            LTFLOAT fPercent = -fDot1 / (fDot2 - fDot1);
			VEC_LERP(vPos, vP0, vP1, fPercent);
		}

		// Keep us there if we are supposed to stick to the world...

		if( m_pClassData->bStickToWorld )
		{
			vVel.Init();

			g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_GRAVITY | FLAG_TOUCH_NOTIFY | FLAG_SOLID | FLAG_VISIBLE);
			
			// Rotate us to be flat against the surface...

			LTRotation rObjRot;
			g_pLTServer->GetObjectRotation( m_hObject, &rObjRot );

			LTRotation rSurfaceRot( CollInfo.m_Plane.m_Normal, rObjRot.Up() );

			// Build the Spawn string from the ObjectName and list of properties...

			char szSpawn[512] = {0};
			sprintf( szSpawn, "%s ", m_pClassData->szSpawnObject );

			for( int i = 0; i < m_pClassData->blrObjectProps.GetNumItems(); ++i )
			{
				if( (strlen( szSpawn ) + strlen( m_pClassData->blrObjectProps.GetItem(i) ) + 1) > 512 )
				{
					ASSERT( LTFALSE && "Spawn string is larger than 512 chars" );
					break;
				}

				strcat( szSpawn, m_pClassData->blrObjectProps.GetItem(i) );
				strcat( szSpawn, ";" );
			}
			
			LTVector vScale = m_pAmmoData->pProjectileFX->vModelScale;

			BaseClass* pClass = SpawnObject( szSpawn, vPos, rSurfaceRot );

			if( pClass )
			{
				g_pLTServer->ScaleObject(pClass->m_hObject, &vScale);

				LTVector vDims;
				g_pPhysicsLT->GetObjectDims(pClass->m_hObject, &vDims);
				vDims.x *= vScale.x;
				vDims.y *= vScale.y;
				vDims.z *= vScale.z;

				g_pPhysicsLT->SetObjectDims(pClass->m_hObject, &vDims, 0);
				
			}
		}

	}

	g_pLTServer->RemoveObject( m_hObject );

}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashBang::CFlashBang
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CFlashBang::CFlashBang() : CProjectile()
{
	m_vDims.Init(5.0f, 5.0f, 5.0f);

    m_bSpinFlashBang  = LTTRUE;
    m_bUpdating     = LTTRUE;

	m_fPitch	= 0.0f;
	m_fYaw		= 0.0f;
	m_fRoll		= 0.0f;
	m_fPitchVel	= 0.0f;
	m_fYawVel	= 0.0f;
	m_fRollVel	= 0.0f;

	m_cBounces = 0;

    m_hBounceSnd = LTNULL;
	m_eContainerCode = CC_NO_CONTAINER;
	m_eLastHitSurface = ST_UNKNOWN;

	m_bRotatedToRest = LTFALSE;
	m_bAddToFlashBangList = LTTRUE;

	if( m_bAddToFlashBangList )
	{
		g_lstFlashBang.Add(this);
	}
	
	ResetRotationVel();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashBang::~CFlashBang
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CFlashBang::~CFlashBang()
{
	// Just to be safe...
	if( m_bAddToFlashBangList )
		g_lstFlashBang.Remove(this);

	if (m_hBounceSnd)
	{
        g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
        m_hBounceSnd = LTNULL;
	}
}
// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashBang::Detonate
//
//	PURPOSE:	Go boom
//
// ----------------------------------------------------------------------- //

void CFlashBang::Detonate(HOBJECT hObj)
{
	// We're blowing up, take us out of the grenade list
	if( m_bAddToFlashBangList )
		g_lstFlashBang.Remove(this);

	CProjectile::Detonate(hObj);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashBang::HandleImpact
//
//	PURPOSE:	Handle bouncing off of things
//
// ----------------------------------------------------------------------- //

void CFlashBang::HandleImpact(HOBJECT hObj)
{
	// [KLS 7/28/02] - If we hit a character detonate. 
	// (i.e., only act like a timed grenade if we don't hit someone)

//[SPIKE] 투척무기들 사람 몸에 맞아도 터지 않게....
/*	if ( IsCharacter(hObj) || IsCharacterHitBox(hObj) || IsBody(hObj))
	{
		Detonate(hObj);
		return;
	}
*/

    LTVector vVel;
	g_pPhysicsLT->GetVelocity(m_hObject, &vVel);

	// See if we are impacting on liquid...

    LTBOOL bEnteringLiquid = LTFALSE;
    uint16 code;
    if (g_pLTServer->GetContainerCode(hObj, &code))
	{
		if (IsLiquid((ContainerCode)code))
		{
            bEnteringLiquid = LTTRUE;
		}
		else
		{
			return;
		}
	}


	CollisionInfo info;
    g_pLTServer->GetLastCollision(&info);

	// Calculate where we really hit the world...
    if (IsMainWorld(hObj))
	{
        LTVector vPos, vCurVel, vP0, vP1;
        g_pLTServer->GetObjectPos(m_hObject, &vPos);

		vP1 = vPos;
        vCurVel = vVel * g_pLTServer->GetFrameTime();
		vP0 = vP1 - vCurVel;
		vP1 += vCurVel;

        LTFLOAT fDot1 = VEC_DOT(info.m_Plane.m_Normal, vP0) - info.m_Plane.m_Dist;
        LTFLOAT fDot2 = VEC_DOT(info.m_Plane.m_Normal, vP1) - info.m_Plane.m_Dist;

		if (fDot1 < 0.0f && fDot2 < 0.0f || fDot1 > 0.0f && fDot2 > 0.0f)
		{
			vPos = vP1;
		}
		else
		{
            LTFLOAT fPercent = -fDot1 / (fDot2 - fDot1);
			VEC_LERP(vPos, vP0, vP1, fPercent);
		}

		// Set our new "real" pos...

        g_pLTServer->SetObjectPos(m_hObject, &vPos);
	}

	// Do the bounce, if the object we hit isn't liquid...

	if (!bEnteringLiquid)
	{
		vVel += (info.m_vStopVel * 1.6f);
	}
	else //[SPIKE]
	{
		vVel *= 9.5f;
	}


	// Dampen the grenade's new velocity based on the surface type...

    LTFLOAT fDampenPercent = DEFAULT_GRENADE_DAMPEN_PERCENT;

	m_eLastHitSurface = GetSurfaceType(info);
	SURFACE* pSurf = g_pSurfaceMgr->GetSurface(m_eLastHitSurface);
	if (pSurf)
	{
		// Play a bounce sound (based on the surface type) if one isn't
		// already playing...

		if ( ShouldPlayBounceSound(pSurf) )
		{
            uint32 dwFlags = PLAYSOUND_GETHANDLE | PLAYSOUND_TIME;

			int nVolume	= IsLiquid(m_eContainerCode) ? 50 : 100;

            LTVector vPos;
            g_pLTServer->GetObjectPos(m_hObject, &vPos);

			// Only play one sound at a time...
			
			if (m_hBounceSnd)
			{
                g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
                m_hBounceSnd = LTNULL;
			}
#ifdef USE_AI
			else {
				// Register a stimulus for the bounce sound, if this is a new bounce sound.

				g_pAIStimulusMgr->RegisterStimulus( kStim_EnemyDisturbanceSound, 1, m_hFiredFrom, m_hObject, vPos, pSurf->fGrenadeSndRadius );
			}
#endif

			m_hBounceSnd = g_pServerSoundMgr->PlaySoundFromPos(vPos, (char*)GetBounceSound(pSurf),
				pSurf->fGrenadeSndRadius, SOUNDPRIORITY_MISC_MEDIUM, dwFlags, nVolume);
		}

		fDampenPercent = (1.0f - pSurf->fHardness);
	}

	fDampenPercent = fDampenPercent > 1.0f ? 1.0f : (fDampenPercent < 0.0f ? 0.0f : fDampenPercent);

	vVel *= (1.0f - fDampenPercent);


	// See if we should come to a rest...

    LTVector vTest = vVel;
	vTest.y = 0.0f;

	if (vTest.Mag() < DEFAULT_GRENADE_MIN_VELOCITY)
	{
		// If we're on the ground (or an object), stop movement...

		CollisionInfo standingInfo;
        g_pLTServer->GetStandingOn(m_hObject, &standingInfo);

		CollisionInfo* pInfo = standingInfo.m_hObject ? &standingInfo : &info;

		if (pInfo->m_hObject)
		{
			// Don't stop on walls...

			if (pInfo->m_Plane.m_Normal.y > 0.75f)
			{
				vVel.Init();

				// Turn off gravity, solid, and touch notify....

				g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_GRAVITY | FLAG_TOUCH_NOTIFY | FLAG_SOLID );

				// Rotate to rest...

				RotateToRest();
			}
		}
	}


	// Reset rotation velocities due to the bounce...

	ResetRotationVel(&vVel, pSurf);


	// We need to subtact this out because the engine will add it back in,
	// kind of a kludge but necessary...

	vVel -= info.m_vStopVel;


    g_pPhysicsLT->SetVelocity(m_hObject, &vVel);

	m_cBounces++;
}

// ----------------------------------------------------------------------- //

LTBOOL CFlashBang::ShouldPlayBounceSound(SURFACE* pSurface)
{
	return ( pSurface->eType != ST_LIQUID && !!pSurface->szGrenadeImpactSnd[0] );
}

// ----------------------------------------------------------------------- //

const char* CFlashBang::GetBounceSound(SURFACE* pSurface)
{
	return pSurface->szGrenadeImpactSnd;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::UpdateFlashBang()
//
//	PURPOSE:	Update the grenade...
//
// ----------------------------------------------------------------------- //

void CFlashBang::UpdateFlashBang()
{
	// Update my container code...

    LTVector vPos;
	g_pLTServer->GetObjectPos(m_hObject, &vPos);

	// Get our container code, but ignore everything except for liquid containers...

	uint32 nIgnoreFlags = (CC_ALL_FLAG & ~ ::GetLiquidFlags());
    m_eContainerCode = ::GetContainerCode(vPos, nIgnoreFlags);


	// Update grenade spin...

	if (m_bSpinFlashBang && m_bUpdating)
	{
		if (m_fPitchVel != 0 || m_fYawVel != 0 || m_fRollVel != 0)
		{
            LTFLOAT fDeltaTime = g_pLTServer->GetFrameTime();

			m_fPitch += m_fPitchVel * fDeltaTime;
			m_fYaw   += m_fYawVel * fDeltaTime;
			m_fRoll  += m_fRollVel * fDeltaTime;

            LTRotation rRot(m_fPitch, m_fYaw, m_fRoll);
			g_pLTServer->SetObjectRotation(m_hObject, &rRot);
		}
	}


	// See if the bounce sound is done playing...

	if (m_hBounceSnd)
	{
        bool bIsDone;
        if (g_pLTServer->SoundMgr()->IsSoundDone(m_hBounceSnd, bIsDone) != LT_OK || bIsDone)
		{
            g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
            m_hBounceSnd = LTNULL;
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::RotateToRest()
//
//	PURPOSE:	Rotate the grenade to its rest position...
//
// ----------------------------------------------------------------------- //

void CFlashBang::RotateToRest()
{
	m_bRotatedToRest = LTTRUE;

	// Turn off any animations that are playing (e.g., spin)...

	ANIMTRACKERID nTracker;
	if( LT_OK == g_pModelLT->GetMainTracker( m_hObject, nTracker ))
	{
		g_pModelLT->SetPlaying(m_hObject, nTracker, LTFALSE);
	}

	// Spin it if necessary

	if (!m_bSpinFlashBang) return;

    LTRotation rRot(0.0f, m_fYaw, 0.0f);
	g_pLTServer->SetObjectRotation(m_hObject, &rRot);

    m_bUpdating = LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::ResetRotationVel()
//
//	PURPOSE:	Update the grenade...
//
// ----------------------------------------------------------------------- //

void CFlashBang::ResetRotationVel(LTVector* pvNewVel, SURFACE* pSurf)
{
	if (!m_bSpinFlashBang) return;

	// (TO DO: base this on new velocity and surface hardness?)

    LTFLOAT fVal  = MATH_CIRCLE/2.0f;
    LTFLOAT fVal2 = MATH_CIRCLE;
	m_fPitchVel	 = GetRandom(-fVal, fVal);
	m_fYawVel	 = GetRandom(-fVal2, fVal2);
	m_fRollVel   = GetRandom(-fVal2, fVal2);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::EngineMessageFn
//
//	PURPOSE:	Handle engine messages
//
// ----------------------------------------------------------------------- //

uint32 CFlashBang::EngineMessageFn(uint32 messageID, void *pData, LTFLOAT fData)
{
	switch(messageID)
	{
		case MID_UPDATE:
		{
			UpdateFlashBang();
		}
		break;

		default : break;
	}

	return CProjectile::EngineMessageFn(messageID, pData, fData);
}

void CFlashBang::AddImpact( HOBJECT hObj,
                            const LTVector &vFirePos,
							const LTVector &vImpactPos,
							const LTVector &vSurfaceNormal,
							SurfaceType eType,
							IMPACT_TYPE eImpactType,
							bool bDeflected )
{
	CProjectile::AddImpact( hObj, vFirePos, vImpactPos, vSurfaceNormal, eType, eImpactType, bDeflected );

	AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(m_nAmmoId);
	if (!pAmmo) return;

	m_vPos	= vImpactPos;
	
    if( false == IsLiquid(m_eContainerCode) )
	{
		AreaDamageObjectsInSphere( vImpactPos, (LTFLOAT)pAmmo->nAreaDamageRadius );
	}	
}

void CFlashBang::AreaDamageObjectsInSphere( LTVector vPos, float fDamageRadius )
{
	ObjectList* pList = g_pLTServer->FindObjectsTouchingSphere(&vPos, fDamageRadius);
	if (!pList) return;

	ObjectLink* pLink = pList->m_pFirstLink;

	while (pLink)
	{
		AreaDamageObject(pLink->m_hObject,fDamageRadius);
		pLink = pLink->m_pNext;
	}

	g_pLTServer->RelinquishList(pList);
}

void CFlashBang::AreaDamageObject( HOBJECT hObj, float fDamageRadius )
{
	if (!hObj) return;

	HOBJECT hDamager = m_hFiredFrom ? m_hFiredFrom : m_hObject;

	LTVector vObjPos;
	g_pLTServer->GetObjectPos(hObj, &vObjPos);

    LTVector vDir = vObjPos - m_vPos;
    LTFLOAT fDist = vDir.Mag();

	if (fDist <= fDamageRadius)
	{
		// Make sure that Characters don't take damage if another object
		// is blocking them from the explosion...
	
		LTBOOL bIntersect1 = LTFALSE;
		LTBOOL bIntersect2 = LTFALSE;

		if (IsCharacter(hObj) || IsCharacterHitBox(hObj))
		{
			// To do this test, do an intersect segment both directions
			// (from the object to the explosion and from the explosion
			// to the object).  This will ensure that neither point
			// is inside a wall and that nothing is blocking the damage...

			IntersectInfo iInfo;
			IntersectQuery qInfo;

			qInfo.m_Flags	  = INTERSECT_HPOLY | INTERSECT_OBJECTS | IGNORE_NONSOLID;
			qInfo.m_FilterFn  = FlashBangFilterFn;

			qInfo.m_From = m_vPos + vDir/fDist;
			qInfo.m_To   = vObjPos;
			
			bIntersect1 = g_pLTServer->IntersectSegment(&qInfo, &iInfo);

			qInfo.m_From = vObjPos;
			qInfo.m_To   = m_vPos + vDir/fDist;

			bIntersect2 = g_pLTServer->IntersectSegment(&qInfo, &iInfo);
		}

		if (!bIntersect1 && !bIntersect2)
		{
			if( IsPlayer(hObj) )
			{
				uint8	nID = 0xFF;
				CPlayerObj* pPlayer = (CPlayerObj*)g_pLTServer->HandleToObject(hObj);
				if( pPlayer )
				{
					nID = (uint8)g_pLTServer->GetClientID( pPlayer->GetClient() );			
				}

				CAutoMessage cMsg;
				LTVector vPos;
				g_pLTServer->GetObjectPos(hObj, &vPos);
				
				cMsg.Writeuint8(MID_PROJECTILE_FX);
				cMsg.Writeuint8(m_nAmmoId);
				cMsg.Writeuint8(nID);					//Damager
				cMsg.WriteCompPos(m_vPos);				//FirePos
				g_pLTServer->SendToClient( cMsg.Read(), LTNULL, MESSAGE_GUARANTEED );
			}			
		}
	}
}

void CFlashBang::RemoveFlashBang()
{
	this->RemoveObject();
}

bool FlashBangFilterFn(HOBJECT hObj, void *pUserData)
{
	uint32 dwFlags;
	g_pCommonLT->GetObjectFlags(hObj, OFT_Flags, dwFlags);
	if (!(dwFlags & FLAG_SOLID))
	{
		return false;
	}
	else if (IsMainWorld(hObj) || (OT_WORLDMODEL == GetObjectType(hObj)))
	{
		return true;
	}

	return false;
}


//=====================================================================
//[SPIKE] 연막탄 
//=====================================================================

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSmokeGrenade::CSmokeGrenade
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //
CSmokeGrenade::CSmokeGrenade() : CProjectile()
{
	m_vDims.Init(5.0f, 5.0f, 5.0f);

    m_bSpinSmokeGrenade  = LTTRUE;
    m_bUpdating     = LTTRUE;

	m_fPitch	= 0.0f;
	m_fYaw		= 0.0f;
	m_fRoll		= 0.0f;
	m_fPitchVel	= 0.0f;
	m_fYawVel	= 0.0f;
	m_fRollVel	= 0.0f;

	m_cBounces = 0;

    m_hBounceSnd = LTNULL;
	m_eContainerCode = CC_NO_CONTAINER;
	m_eLastHitSurface = ST_UNKNOWN;

	m_bRotatedToRest = LTFALSE;
	m_bAddToSmokeGrenadeList = LTTRUE;

	if( m_bAddToSmokeGrenadeList )
	{
		g_lstSmokeGrenade.Add(this);
	}
	
	ResetRotationVel();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSmokeGrenade::~CSmokeGrenade
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CSmokeGrenade::~CSmokeGrenade()
{
	// Just to be safe...
	if( m_bAddToSmokeGrenadeList )
		g_lstSmokeGrenade.Remove(this);

	if (m_hBounceSnd)
	{
        g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
        m_hBounceSnd = LTNULL;
	}
}
// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSmokeGrenade::Detonate
//
//	PURPOSE:	Go boom
//
// ----------------------------------------------------------------------- //

void CSmokeGrenade::Detonate(HOBJECT hObj)
{
	// We're blowing up, take us out of the grenade list
	if( m_bAddToSmokeGrenadeList )
		g_lstSmokeGrenade.Remove(this);

	CProjectile::Detonate(hObj);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSmokeGrenade::HandleImpact
//
//	PURPOSE:	Handle bouncing off of things
//
// ----------------------------------------------------------------------- //

void CSmokeGrenade::HandleImpact(HOBJECT hObj)
{
	// [KLS 7/28/02] - If we hit a character detonate. 
	// (i.e., only act like a timed grenade if we don't hit someone)

//[SPIKE] 투척무기들 사람 몸에 맞아도 터지 않게....
/*	if ( IsCharacter(hObj) || IsCharacterHitBox(hObj) || IsBody(hObj))
	{
		Detonate(hObj);
		return;
	}
*/

    LTVector vVel;
	g_pPhysicsLT->GetVelocity(m_hObject, &vVel);

	// See if we are impacting on liquid...

    LTBOOL bEnteringLiquid = LTFALSE;
    uint16 code;
    if (g_pLTServer->GetContainerCode(hObj, &code))
	{
		if (IsLiquid((ContainerCode)code))
		{
            bEnteringLiquid = LTTRUE;
		}
		else
		{
			return;
		}
	}


	CollisionInfo info;
    g_pLTServer->GetLastCollision(&info);

	// Calculate where we really hit the world...
    if (IsMainWorld(hObj))
	{
        LTVector vPos, vCurVel, vP0, vP1;
        g_pLTServer->GetObjectPos(m_hObject, &vPos);

		vP1 = vPos;
        vCurVel = vVel * g_pLTServer->GetFrameTime();
		vP0 = vP1 - vCurVel;
		vP1 += vCurVel;

        LTFLOAT fDot1 = VEC_DOT(info.m_Plane.m_Normal, vP0) - info.m_Plane.m_Dist;
        LTFLOAT fDot2 = VEC_DOT(info.m_Plane.m_Normal, vP1) - info.m_Plane.m_Dist;

		if (fDot1 < 0.0f && fDot2 < 0.0f || fDot1 > 0.0f && fDot2 > 0.0f)
		{
			vPos = vP1;
		}
		else
		{
            LTFLOAT fPercent = -fDot1 / (fDot2 - fDot1);
			VEC_LERP(vPos, vP0, vP1, fPercent);
		}

		// Set our new "real" pos...

        g_pLTServer->SetObjectPos(m_hObject, &vPos);
	}

	// Do the bounce, if the object we hit isn't liquid...

	if (!bEnteringLiquid)
	{
		vVel += (info.m_vStopVel * 1.6f);
	}
	else //[SPIKE]
	{
		vVel *= 9.5f;
	}


	// Dampen the grenade's new velocity based on the surface type...

    LTFLOAT fDampenPercent = DEFAULT_GRENADE_DAMPEN_PERCENT;

	m_eLastHitSurface = GetSurfaceType(info);
	SURFACE* pSurf = g_pSurfaceMgr->GetSurface(m_eLastHitSurface);
	if (pSurf)
	{
		// Play a bounce sound (based on the surface type) if one isn't
		// already playing...

		if ( ShouldPlayBounceSound(pSurf) )
		{
            uint32 dwFlags = PLAYSOUND_GETHANDLE | PLAYSOUND_TIME;

			int nVolume	= IsLiquid(m_eContainerCode) ? 50 : 100;

            LTVector vPos;
            g_pLTServer->GetObjectPos(m_hObject, &vPos);

			// Only play one sound at a time...
			
			if (m_hBounceSnd)
			{
                g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
                m_hBounceSnd = LTNULL;
			}
#ifdef USE_AI
			else {
				// Register a stimulus for the bounce sound, if this is a new bounce sound.

				g_pAIStimulusMgr->RegisterStimulus( kStim_EnemyDisturbanceSound, 1, m_hFiredFrom, m_hObject, vPos, pSurf->fGrenadeSndRadius );
			}
#endif

			m_hBounceSnd = g_pServerSoundMgr->PlaySoundFromPos(vPos, (char*)GetBounceSound(pSurf),
				pSurf->fGrenadeSndRadius, SOUNDPRIORITY_MISC_MEDIUM, dwFlags, nVolume);
		}

		fDampenPercent = (1.0f - pSurf->fHardness);
	}

	fDampenPercent = fDampenPercent > 1.0f ? 1.0f : (fDampenPercent < 0.0f ? 0.0f : fDampenPercent);

	vVel *= (1.0f - fDampenPercent);


	// See if we should come to a rest...

    LTVector vTest = vVel;
	vTest.y = 0.0f;

	if (vTest.Mag() < DEFAULT_GRENADE_MIN_VELOCITY)
	{
		// If we're on the ground (or an object), stop movement...

		CollisionInfo standingInfo;
        g_pLTServer->GetStandingOn(m_hObject, &standingInfo);

		CollisionInfo* pInfo = standingInfo.m_hObject ? &standingInfo : &info;

		if (pInfo->m_hObject)
		{
			// Don't stop on walls...

			if (pInfo->m_Plane.m_Normal.y > 0.75f)
			{
				vVel.Init();

				// Turn off gravity, solid, and touch notify....

				g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_GRAVITY | FLAG_TOUCH_NOTIFY | FLAG_SOLID );

				// Rotate to rest...

				RotateToRest();
			}
		}
	}


	// Reset rotation velocities due to the bounce...

	ResetRotationVel(&vVel, pSurf);


	// We need to subtact this out because the engine will add it back in,
	// kind of a kludge but necessary...

	vVel -= info.m_vStopVel;


    g_pPhysicsLT->SetVelocity(m_hObject, &vVel);

	m_cBounces++;

	//[SPIKE] 멈추는 순간 터짐 
	vVel.y = 0.0f;
	if(vVel.Length() < 1.0f || m_cBounces > 20)
	{
		Detonate(LTNULL);
	}
	//====================================
}

// ----------------------------------------------------------------------- //

LTBOOL CSmokeGrenade::ShouldPlayBounceSound(SURFACE* pSurface)
{
	return ( pSurface->eType != ST_LIQUID && !!pSurface->szGrenadeImpactSnd[0] );
}

// ----------------------------------------------------------------------- //

const char* CSmokeGrenade::GetBounceSound(SURFACE* pSurface)
{
	return pSurface->szGrenadeImpactSnd;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::UpdateSmokeGrenade()
//
//	PURPOSE:	Update the grenade...
//
// ----------------------------------------------------------------------- //

void CSmokeGrenade::UpdateSmokeGrenade()
{
	// Update my container code...

    LTVector vPos;
	g_pLTServer->GetObjectPos(m_hObject, &vPos);

	// Get our container code, but ignore everything except for liquid containers...

	uint32 nIgnoreFlags = (CC_ALL_FLAG & ~ ::GetLiquidFlags());
    m_eContainerCode = ::GetContainerCode(vPos, nIgnoreFlags);


	// Update grenade spin...

	if (m_bSpinSmokeGrenade && m_bUpdating)
	{
		if (m_fPitchVel != 0 || m_fYawVel != 0 || m_fRollVel != 0)
		{
            LTFLOAT fDeltaTime = g_pLTServer->GetFrameTime();

			m_fPitch += m_fPitchVel * fDeltaTime;
			m_fYaw   += m_fYawVel * fDeltaTime;
			m_fRoll  += m_fRollVel * fDeltaTime;

            LTRotation rRot(m_fPitch, m_fYaw, m_fRoll);
			g_pLTServer->SetObjectRotation(m_hObject, &rRot);
		}
	}


	// See if the bounce sound is done playing...

	if (m_hBounceSnd)
	{
        bool bIsDone;
        if (g_pLTServer->SoundMgr()->IsSoundDone(m_hBounceSnd, bIsDone) != LT_OK || bIsDone)
		{
            g_pLTServer->SoundMgr()->KillSound(m_hBounceSnd);
            m_hBounceSnd = LTNULL;
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::RotateToRest()
//
//	PURPOSE:	Rotate the grenade to its rest position...
//
// ----------------------------------------------------------------------- //

void CSmokeGrenade::RotateToRest()
{
	m_bRotatedToRest = LTTRUE;

	// Turn off any animations that are playing (e.g., spin)...

	ANIMTRACKERID nTracker;
	if( LT_OK == g_pModelLT->GetMainTracker( m_hObject, nTracker ))
	{
		g_pModelLT->SetPlaying(m_hObject, nTracker, LTFALSE);
	}

	// Spin it if necessary

	if (!m_bSpinSmokeGrenade) return;

    LTRotation rRot(0.0f, m_fYaw, 0.0f);
	g_pLTServer->SetObjectRotation(m_hObject, &rRot);

    m_bUpdating = LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::ResetRotationVel()
//
//	PURPOSE:	Update the grenade...
//
// ----------------------------------------------------------------------- //

void CSmokeGrenade::ResetRotationVel(LTVector* pvNewVel, SURFACE* pSurf)
{
	if (!m_bSpinSmokeGrenade) return;

	// (TO DO: base this on new velocity and surface hardness?)

    LTFLOAT fVal  = MATH_CIRCLE/2.0f;
    LTFLOAT fVal2 = MATH_CIRCLE;
	m_fPitchVel	 = GetRandom(-fVal, fVal);
	m_fYawVel	 = GetRandom(-fVal2, fVal2);
	m_fRollVel   = GetRandom(-fVal2, fVal2);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGrenade::EngineMessageFn
//
//	PURPOSE:	Handle engine messages
//
// ----------------------------------------------------------------------- //

uint32 CSmokeGrenade::EngineMessageFn(uint32 messageID, void *pData, LTFLOAT fData)
{
	switch(messageID)
	{
		case MID_UPDATE:
		{
			UpdateSmokeGrenade();
			//[SPIKE] 연막탄은 시간으로 터지지 않기 때문에...
			return GameBase::EngineMessageFn(messageID, pData, fData);
			//=======================================================
		}
		break;

		default : break;
	}

	return CProjectile::EngineMessageFn(messageID, pData, fData);
}

void CSmokeGrenade::AddImpact( HOBJECT hObj,
                            const LTVector &vFirePos,
							const LTVector &vImpactPos,
							const LTVector &vSurfaceNormal,
							SurfaceType eType,
							IMPACT_TYPE eImpactType,
							bool bDeflected )
{
	CProjectile::AddImpact( hObj, vFirePos, vImpactPos, vSurfaceNormal, eType, eImpactType, bDeflected );

	AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(m_nAmmoId);
	if (!pAmmo) return;

	m_vPos	= vImpactPos;
	
    if( false == IsLiquid(m_eContainerCode) )
	{
		AreaDamageObjectsInSphere( vImpactPos, (LTFLOAT)pAmmo->nAreaDamageRadius );
	}	
}

void CSmokeGrenade::AreaDamageObjectsInSphere( LTVector vPos, float fDamageRadius )
{
	ObjectList* pList = g_pLTServer->FindObjectsTouchingSphere(&vPos, fDamageRadius);
	if (!pList) return;

	ObjectLink* pLink = pList->m_pFirstLink;

	while (pLink)
	{
		AreaDamageObject(pLink->m_hObject,fDamageRadius);
		pLink = pLink->m_pNext;
	}

	g_pLTServer->RelinquishList(pList);
}

void CSmokeGrenade::AreaDamageObject( HOBJECT hObj, float fDamageRadius )
{
	if (!hObj) return;

	HOBJECT hDamager = m_hFiredFrom ? m_hFiredFrom : m_hObject;

	LTVector vObjPos;
	g_pLTServer->GetObjectPos(hObj, &vObjPos);

    LTVector vDir = vObjPos - m_vPos;
    LTFLOAT fDist = vDir.Mag();

	if (fDist <= fDamageRadius)
	{
		// Make sure that Characters don't take damage if another object
		// is blocking them from the explosion...
	
		LTBOOL bIntersect1 = LTFALSE;
		LTBOOL bIntersect2 = LTFALSE;

		if (IsCharacter(hObj) || IsCharacterHitBox(hObj))
		{
			// To do this test, do an intersect segment both directions
			// (from the object to the explosion and from the explosion
			// to the object).  This will ensure that neither point
			// is inside a wall and that nothing is blocking the damage...

			IntersectInfo iInfo;
			IntersectQuery qInfo;

			qInfo.m_Flags	  = INTERSECT_HPOLY | INTERSECT_OBJECTS | IGNORE_NONSOLID;
			qInfo.m_FilterFn  = FlashBangFilterFn;

			qInfo.m_From = m_vPos + vDir/fDist;
			qInfo.m_To   = vObjPos;
			
			bIntersect1 = g_pLTServer->IntersectSegment(&qInfo, &iInfo);

			qInfo.m_From = vObjPos;
			qInfo.m_To   = m_vPos + vDir/fDist;

			bIntersect2 = g_pLTServer->IntersectSegment(&qInfo, &iInfo);
		}

		if (!bIntersect1 && !bIntersect2)
		{
			if( IsPlayer(hObj) )
			{
				uint8	nID = 0xFF;
				CPlayerObj* pPlayer = (CPlayerObj*)g_pLTServer->HandleToObject(hObj);
				if( pPlayer )
				{
					nID = (uint8)g_pLTServer->GetClientID( pPlayer->GetClient() );			
				}

				CAutoMessage cMsg;
				LTVector vPos;
				g_pLTServer->GetObjectPos(hObj, &vPos);
				
				cMsg.Writeuint8(MID_PROJECTILE_FX);
				cMsg.Writeuint8(m_nAmmoId);
				cMsg.Writeuint8(nID);					//Damager
				cMsg.WriteCompPos(m_vPos);				//FirePos
				g_pLTServer->SendToClient( cMsg.Read(), LTNULL, MESSAGE_GUARANTEED );
			}			
		}
	}
}

void CSmokeGrenade::RemoveSmokeGrenade()
{
	this->RemoveObject();
}
