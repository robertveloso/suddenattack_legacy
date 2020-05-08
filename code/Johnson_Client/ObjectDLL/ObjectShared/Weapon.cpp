// ----------------------------------------------------------------------- //
//
// MODULE  : Weapon.cpp
//
// PURPOSE : Weapon class - implementation
//
// CREATED : 9/25/97
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "weapon.h"
#include "ServerUtilities.h"
#include "iltserver.h"
#include "ltengineobjects.h"
#include "MsgIds.h"
#include "HHWeaponModel.h"
#include "Character.h"
#include "WeaponFXTypes.h"
#include "GameServerShell.h"
#include "PlayerObj.h"
#include "Weapons.h"
#include "AIStimulusMgr.h"
#include "WeaponFireInfo.h"
#include "AIUtils.h"
#include "Projectile.h"
#include "FXButeMgr.h"

extern LTBOOL g_bInfiniteAmmo;
extern CGameServerShell* g_pGameServerShell;

#ifdef USE_AI
extern CAIStimulusMgr* g_pAIStimulusMgr;
#else
#include "UberAssert.h"
#endif

#define INFINITE_AMMO_AMOUNT 1000

uint8 g_nRandomWeaponSeed = 255;


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::CWeapon
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CWeapon::CWeapon()
{
	m_eState			= W_IDLE;
    m_bHave             = LTFALSE;
	m_nAmmoInClip		= 1;
	m_fDamageFactor		= 1.0f;
	m_fRangeAdjust		= 1.0f;

	m_fLifeTime			= -1.0f;

	m_nWeaponId			= 0;

    m_pParent           = LTNULL;

	m_nCurTracer		= 0;

	m_bFirstSelection	= LTTRUE;

	m_nHandAni			= INVALID_ANI;
	m_nPreFireAni		= INVALID_ANI;
	m_nFireAni			= INVALID_ANI;
	m_nPostFireAni		= INVALID_ANI;

	m_fLastFireTime		= 0.0f;

	m_pWeaponData		= NULL;
	m_pAmmoData			= NULL;

	m_bHidden			= false;
	m_bSpecialModeOn    = LTFALSE;

	Drop();
}
	
void CWeapon::Drop()
{
    m_bHave = LTFALSE;
	if (m_pWeaponData)
		m_nAmmoId = m_pWeaponData->nDefaultAmmoId;
	else
		m_nAmmoId = 0;
	for( int i = 0; i < W_MAX_MODS; ++i )
	{
		m_Mods[i] = WMGR_INVALID_ID;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::~CWeapon
//
//	PURPOSE:	Deallocate memory
//
// ----------------------------------------------------------------------- //

CWeapon::~CWeapon()
{
	KillLoopSound();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::Init
//
//	PURPOSE:	Init object
//
// ----------------------------------------------------------------------- //

LTBOOL CWeapon::Init(CWeapons* pParent, HOBJECT hObj, int nWeaponId, int nAmmoId)
{
    if (!pParent || !hObj) return LTFALSE;

	m_pParent	= pParent;
	m_hObject   = hObj;
	m_nWeaponId	= nWeaponId;
	m_nAmmoId	= nAmmoId;

	m_pWeaponData = g_pWeaponMgr->GetWeapon(m_nWeaponId);
    if (!m_pWeaponData) return LTFALSE;

	m_pAmmoData = g_pWeaponMgr->GetAmmo(m_nAmmoId);
    if (!m_pAmmoData) return LTFALSE;

    return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::ReloadClip
//
//	PURPOSE:	Fill the clip
//
// ----------------------------------------------------------------------- //

void CWeapon::ReloadClip(LTBOOL bPlayReload, int nNewAmmo /*=-1*/, uint8 nAmmoId /*=WMGR_INVALID_ID*/ )
{
	if (!m_pParent) return;

	// See if we want to reload with a different ammo type...
	
	if( nAmmoId	!= WMGR_INVALID_ID && nAmmoId != m_nAmmoId )
	{
		SetAmmoId( nAmmoId );
		m_nAmmoInClip = 0;
	}

	int nAmmo = nNewAmmo >= 0 ? nNewAmmo : m_pParent->GetAmmoCount(m_nAmmoId);
	int nShotsPerClip = m_pWeaponData->nShotsPerClip;

	// Clip is full...
	LTBOOL bInfiniteAmmo = (g_bInfiniteAmmo || m_pWeaponData->bInfiniteAmmo);
	if (m_nAmmoInClip == nShotsPerClip && !bInfiniteAmmo )
	{
		return;
	}

	if (nAmmo > 0 && nShotsPerClip > 0)
	{
		m_nAmmoInClip = nAmmo < nShotsPerClip ? nAmmo : nShotsPerClip;
	}

	if (IsPlayer(m_hObject) && bPlayReload)
	{
        CPlayerObj* pPlayer = (CPlayerObj*) g_pLTServer->HandleToObject(m_hObject);
      
		//---------------------------------------------------------------------
		// 광식 : 총알 Reload 했을 때 나에게 붙어 있는 모든 유저에게 알린다.
		//---------------------------------------------------------------------
		int nTempNewAmmo = 0;
		for( int i = 0; i < MAX_ATTACH_PLAYER; i++ )
		{
			if( pPlayer->m_AttachPlayer[i].hAttachPlayer != LTNULL )
			{
				CHECK_PLAYEROBJ_BEGIN();
				CPlayerObj* pAttachPlayer = LTNULL;
				CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList().begin();
				while( iter != CPlayerObj::GetPlayerObjList().end() )
				{
					if( (*iter)->GetClient() == pPlayer->m_AttachPlayer[i].hAttachPlayer )
					{
						pAttachPlayer = (*iter);
						break;
					}					
					iter++;
				}
				CHECK_PLAYEROBJ_END();

				//CPlayerObj* pAttachPlayer = (CPlayerObj*)g_pLTServer->GetClientUserData(pPlayer->m_AttachPlayer[i].hAttachPlayer);
				if( pAttachPlayer && pAttachPlayer->m_nAttachState == MID_ATTACH_FIRST )
				{
					CAutoMessage cMsg;
					cMsg.Writeuint8( MID_WEAPON_RELOAD );
					cMsg.Writeuint8( nAmmoId );

					// 광식 [05.02.16]
					if( nNewAmmo > 0 )
					{
						nTempNewAmmo = nNewAmmo - 1;
					}
					else
					{
						nTempNewAmmo = m_pParent->GetAmmoCount(nAmmoId);
					}
					cMsg.Writeuint16( nTempNewAmmo );

					g_pLTServer->SendToClient( cMsg.Read(), pPlayer->m_AttachPlayer[i].hAttachPlayer, MESSAGE_GUARANTEED );
				}
			}
		}
		//---------------------------------------------------------------------
	}
	
#ifdef USE_AI
	// Add a sound stimulus for the reloading sound.
	LTVector vPos;
	g_pLTServer->GetObjectPos(m_hModelObject, &vPos);
	g_pAIStimulusMgr->RegisterStimulus(kStim_EnemyWeaponReloadSound, m_hObject, m_hModelObject, vPos, 1.0f, 1.0f);
#endif
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::UpdateWeapon
//
//  PURPOSE:    Update the weapon (fire if bFire == LTTRUE)
//
// ----------------------------------------------------------------------- //

WeaponState CWeapon::UpdateWeapon(WeaponFireInfo & fireInfo, LTBOOL bFire)
{
	WeaponState eRet = W_IDLE;

	if (bFire)
	{
		fireInfo.nSeed = GetRandom(2,255);
		// Put in a timestamp since the AI's use this and don't send us a timestamp
		m_fLastFireTime = g_pLTServer->GetTime();
		eRet = Fire(fireInfo);
	}

	return eRet;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::Fire
//
//	PURPOSE:	Fire the weapon
//
// ----------------------------------------------------------------------- //

WeaponState CWeapon::Fire(WeaponFireInfo & info, LTBOOL bForceDetonate/*SPIKE*/)
{
	if (!info.hFiredFrom || !g_pWeaponMgr || !m_pParent) return W_IDLE;

	// Make sure we always have ammo if we should...

	LTBOOL bInfiniteAmmo = (g_bInfiniteAmmo || m_pWeaponData->bInfiniteAmmo);
	int nAmmo = bInfiniteAmmo ? INFINITE_AMMO_AMOUNT : m_pParent->GetAmmoCount(m_nAmmoId);
	int nShotsPerClip = m_pWeaponData->nShotsPerClip;

	AmmoType eAmmoType = m_pAmmoData->eType;

	WeaponState eRet = W_IDLE;

	g_nRandomWeaponSeed = info.nSeed;

	if (nAmmo > 0 || bForceDetonate)
	{
		if (m_nAmmoInClip > 0)
		{
			LTRotation rRot;
			LTVector vU, vR, vF;

			g_pLTServer->GetObjectRotation(info.hFiredFrom, &rRot);
			vU = rRot.Up();
			vR = rRot.Right();
			vF = rRot.Forward();

			// Create a projectile for every vector...

			int nVectorsPerShot = m_pWeaponData->nVectorsPerRound;

			//uint8 nNewRandomSeed = GetRandom(2, 255);

			WeaponPath wp;
			LTVector vOriginalPath = info.vPath;

			wp.nWeaponId = m_nWeaponId;
			wp.vU		 = vU;
			wp.vR		 = vR;
			wp.fPerturbR = info.fPerturbR;
			wp.fPerturbU = info.fPerturbU;

			for (int i=0; i < nVectorsPerShot; i++)
			{
				srand(g_nRandomWeaponSeed);
				g_nRandomWeaponSeed = GetRandom(2, 255);

				wp.vPath = vOriginalPath;
				g_pWeaponMgr->CalculateWeaponPath(wp);
				info.vPath = wp.vPath;

				// Shoot the weapon...

				if (eAmmoType == PROJECTILE || bForceDetonate)
				{
					if (!CreateProjectile(rRot, info))
					{
						return W_IDLE;
					}
				}
				else if (eAmmoType == VECTOR)
				{
					// Don't actually create an object, just use our
					// parent's projectile object to do vector calculations...
					info.hClientObjImpact = info.hClientObjImpacts[i];
					info.eClientHitNode	  = info.eClientHitNodes[i];

					CProjectile* pProj = m_pParent->GetVecProjectile();
					if (pProj)
					{
						pProj->Setup(this, info, i);
					}
				}
			}

			//srand(nNewRandomSeed);

			if (nShotsPerClip > 0 && !bInfiniteAmmo)
			{
				m_nAmmoInClip--;
			}

			if (!bInfiniteAmmo)
			{
				m_pParent->DecrementAmmo(m_nAmmoId);
			}
		}

		eRet = W_FIRED;

		// Check to see if we need to reload...

		if (nShotsPerClip > 0)
		{
			if (m_nAmmoInClip <= 0)
			{
				// Only automatically reload if we're the player... and don't play the animation.
				// 광식 : Attach 시 Reload Ani 틀려지는 부분 수정 하려고...
				/*
				if ( IsPlayer(info.hFiredFrom) )
				{
					ReloadClip(LTFALSE, nAmmo);
				}
				*/
				m_nAmmoInClip = 0;
			}
		}
	}

	return eRet;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::HitPlayer
//
//	PURPOSE:	Fire the weapon
//
// ----------------------------------------------------------------------- //

WeaponState CWeapon::HitPlayer(WeaponFireInfo & info, uint8 nHitCount)
{
	if (!info.hFiredFrom || !g_pWeaponMgr || !m_pParent) return W_IDLE;

	LTBOOL bInfiniteAmmo = (g_bInfiniteAmmo || m_pWeaponData->bInfiniteAmmo);
	int nAmmo = bInfiniteAmmo ? INFINITE_AMMO_AMOUNT : m_pParent->GetAmmoCount(m_nAmmoId);
	int nShotsPerClip = m_pWeaponData->nShotsPerClip;

	WeaponState eRet = W_IDLE;

	if (nAmmo > 0)
	{
		if( m_nAmmoInClip <= 0 ) ReloadClip( LTFALSE, -1, WMGR_INVALID_ID );
		
		for( int i=0 ; i<nHitCount ; ++i )
		{
			info.hClientObjImpact = info.hClientObjImpacts[i];
			info.eClientHitNode	  = info.eClientHitNodes[i];

			CProjectile* pProj = m_pParent->GetVecProjectile();
			if (pProj)
			{
				pProj->SetupHitPlayer( this, info );
			}				
		}

		if (nShotsPerClip > 0 && !bInfiniteAmmo)
		{
			m_nAmmoInClip--;
		}

		if (!bInfiniteAmmo)
		{
			m_pParent->DecrementAmmo(m_nAmmoId);
		}		
		
		eRet = W_FIRED;

		// Check to see if we need to reload...

		if (nShotsPerClip > 0)
		{
			if (m_nAmmoInClip <= 0)
			{				
				m_nAmmoInClip = 0;
			}
		}
	}

	return eRet;
}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::CreateProjectile
//
//	PURPOSE:	Create the approprite projectile to fire.
//
// ----------------------------------------------------------------------- //

LTBOOL CWeapon::CreateProjectile(LTRotation & rRot, WeaponFireInfo & info)
{
    if (!m_pAmmoData->pProjectileFX || !(m_pAmmoData->pProjectileFX->szClass[0])) return LTFALSE;

	ObjectCreateStruct theStruct;
	INIT_OBJECTCREATESTRUCT(theStruct);

	// set the starting rotation
    theStruct.m_Rotation = rRot;

	// move the start position along the line of fire a little
	if ( MATH_EPSILON < m_pAmmoData->pProjectileFX->fFireOffset )
	{
		// determine the new point
		LTVector vNewFirePos;
		vNewFirePos = info.vPath;
		vNewFirePos *= m_pAmmoData->pProjectileFX->fFireOffset;
		vNewFirePos += info.vFirePos;

		// see if there is any geometry in the way
		IntersectInfo iInfo;
		IntersectQuery qInfo;
		qInfo.m_From = info.vFirePos;
		qInfo.m_To = vNewFirePos;
		qInfo.m_Direction = info.vPath;
		qInfo.m_Flags = INTERSECT_OBJECTS | IGNORE_NONSOLID | INTERSECT_HPOLY;
		qInfo.m_FilterFn  = SpecificObjectFilterFn;
		qInfo.m_PolyFilterFn = LTNULL;
		qInfo.m_pUserData = info.hFiredFrom;

		if ( LTTRUE == g_pLTServer->IntersectSegment( &qInfo, &iInfo ) )
		{
			// hit something, just put the fire position at the original
			theStruct.m_Pos = info.vFirePos;
		}
		else
		{
			// hit nothing, use the new fire position
			theStruct.m_Pos = vNewFirePos;
		}
	}
	else
	{
		theStruct.m_Pos = info.vFirePos;
	}

    HCLASS hClass = g_pLTServer->GetClass(m_pAmmoData->pProjectileFX->szClass);

	UBER_ASSERT1( hClass, "Unable to retreive class: %s", m_pAmmoData->pProjectileFX->szClass );

	if (hClass)
	{
        CProjectile* pProj = (CProjectile*)g_pLTServer->CreateObject(hClass, &theStruct);
		if (pProj)
		{
			if( !pProj->Setup(this, info) )
			{
				g_pLTServer->RemoveObject( pProj->m_hObject );
				return LTFALSE;
			}

			// Create a stimulus for AIs to notice projectile.
			
			pProj->CreateDangerStimulus( info.hFiredFrom );

            return LTTRUE;
		}
	}

    return LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::GetModelObject()
//
//	PURPOSE:	Get the object associated with the weapon model
//
// ----------------------------------------------------------------------- //

HOBJECT CWeapon::GetModelObject()
{
	return m_hModelObject;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::SetModelObject()
//
//	PURPOSE:	Set the object associated with the weapon model
//
// ----------------------------------------------------------------------- //

void CWeapon::SetModelObject(HOBJECT hObj)
{
	m_hModelObject = hObj;

	if (m_hModelObject)
	{
		// Init the model animations

		g_pModelLT->GetAnimIndex( m_hModelObject, "Hand",		m_nHandAni );
		g_pModelLT->GetAnimIndex( m_hModelObject, "PreFire",	m_nPreFireAni );
		g_pModelLT->GetAnimIndex( m_hModelObject, "Fire",		m_nFireAni );
		g_pModelLT->GetAnimIndex( m_hModelObject, "PostFire",	m_nPostFireAni );

        g_pModelLT->SetLooping( m_hModelObject, MAIN_TRACKER, false );
		
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::Select()
//
//	PURPOSE:	Select the weapon
//
// ----------------------------------------------------------------------- //

void CWeapon::Select()
{
	// We must have ammo in the clip the very first time the weapon is selected...

	if( m_bFirstSelection )
	{
		m_bFirstSelection = LTFALSE;

		// Set the initial ammo count...

		int nAmmo = m_pParent->GetAmmoCount( m_nAmmoId );
		int nShotsPerClip = m_pWeaponData->nShotsPerClip;

		if (nAmmo > 0 && nShotsPerClip > 0)
		{
			m_nAmmoInClip = nAmmo < nShotsPerClip ? nAmmo : nShotsPerClip;
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::Deselect()
//
//	PURPOSE:	Deselect the weapon
//
// ----------------------------------------------------------------------- //

void CWeapon::Deselect()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::Save
//
//	PURPOSE:	Save the object
//
// ----------------------------------------------------------------------- //

void CWeapon::Save(ILTMessage_Write *pMsg, uint8 nType)
{
	if (!pMsg) return;

	SAVE_HOBJECT( m_hObject );
    SAVE_HOBJECT( m_hModelObject );

    SAVE_FLOAT( m_fLifeTime );
    SAVE_INT( m_nCurTracer );
    SAVE_BOOL( m_bHave );
    SAVE_INT( m_nAmmoInClip );
    SAVE_BYTE( m_eState );
    SAVE_FLOAT( m_fDamageFactor );
    SAVE_FLOAT( m_fRangeAdjust );
    SAVE_BYTE( m_nWeaponId );
    SAVE_BYTE( m_nAmmoId );
	SAVE_BOOL( m_bFirstSelection );
	SAVE_DWORD( m_nHandAni );
	SAVE_DWORD( m_nPreFireAni );
	SAVE_DWORD( m_nFireAni );
	SAVE_DWORD( m_nPostFireAni );

	SAVE_TIME( m_fLastFireTime );
	
	for (int i=0; i < W_MAX_MODS; i++)
	{
        SAVE_DWORD( m_Mods[i] );
	}

	SAVE_bool( m_bHidden );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::Load
//
//	PURPOSE:	Load the object
//
// ----------------------------------------------------------------------- //

void CWeapon::Load(ILTMessage_Read *pMsg, uint8 nType)
{
	if (!pMsg) return;

    LOAD_HOBJECT( m_hObject );
    LOAD_HOBJECT( m_hModelObject );

    LOAD_FLOAT( m_fLifeTime );
    LOAD_INT( m_nCurTracer );
    LOAD_BOOL( m_bHave );
    LOAD_INT( m_nAmmoInClip );
    LOAD_BYTE_CAST( m_eState, WeaponState );
    LOAD_FLOAT( m_fDamageFactor );
    LOAD_FLOAT( m_fRangeAdjust );
    LOAD_BYTE( m_nWeaponId );
    LOAD_BYTE( m_nAmmoId );
	LOAD_BOOL( m_bFirstSelection );
	LOAD_DWORD_CAST( m_nHandAni, HMODELANIM );
	LOAD_DWORD_CAST( m_nPreFireAni, HMODELANIM );
	LOAD_DWORD_CAST( m_nFireAni, HMODELANIM );
	LOAD_DWORD_CAST( m_nPostFireAni, HMODELANIM );
	
	LOAD_TIME( m_fLastFireTime );
	
	for (int i=0; i < W_MAX_MODS; i++)
	{
        LOAD_DWORD( m_Mods[i] );
	}

	LOAD_bool( m_bHidden );

	m_pWeaponData	= g_pWeaponMgr->GetWeapon(m_nWeaponId);
	m_pAmmoData		= g_pWeaponMgr->GetAmmo(m_nAmmoId);
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::GetInstDamage()
//
//	PURPOSE:	Get the instant damage done by this weapon (value can vary)
//
// ----------------------------------------------------------------------- //

LTFLOAT CWeapon::GetInstDamage() const
{
	if (!m_pAmmoData) return 0.0f;


    LTFLOAT fDamage = m_bSpecialModeOn ? (LTFLOAT)m_pAmmoData->nSpecialInstDamage : (LTFLOAT)m_pAmmoData->nInstDamage;

	fDamage *= GetRandom(0.8f, 1.2f) * m_fDamageFactor;
	
	return fDamage;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::GetProgDamage()
//
//	PURPOSE:	Get the progressive damage done by this weapon
//
// ----------------------------------------------------------------------- //

LTFLOAT CWeapon::GetProgDamage() const
{
	if (!m_pAmmoData) return 0.0f;

    LTFLOAT fDamage = m_pAmmoData->fProgDamage;

	fDamage *= m_fDamageFactor;

	return fDamage;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CWeapon::AddMod
//
//	PURPOSE:	Add the specified mod to the weapon
//
// ----------------------------------------------------------------------- //

LTBOOL CWeapon::AddMod( int nMod )
{
	MOD const *pM = g_pWeaponMgr->GetMod( nMod );
    if (!pM) return LTFALSE;

    LTBOOL bRet = LTFALSE;

	for (int i=0; i < W_MAX_MODS; i++)
	{
		if (m_Mods[i] == WMGR_INVALID_ID)
		{
			m_Mods[i] = nMod;

            return LTTRUE;
		}
	}

    return LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CWeapon::PlayAnimation
//
//  PURPOSE:	Plays the given animation of the model.
//
// ----------------------------------------------------------------------- //

bool CWeapon::PlayAnimation( HMODELANIM hAni, bool bForce, bool bLoop, bool bReset )
{
	if( hAni == INVALID_ANI	|| !m_hModelObject )
		return false;

	// See if we should brute force the animation or wait until the current ani is done...

	if( !bForce )
	{
		uint32 dwFlags;
		g_pModelLT->GetPlaybackState( m_hModelObject, MAIN_TRACKER, dwFlags );
		
		if( !(dwFlags & MS_PLAYDONE) )
			return false;
	}

	g_pModelLT->SetCurAnim( m_hModelObject, MAIN_TRACKER, hAni );
	g_pModelLT->SetLooping( m_hModelObject, MAIN_TRACKER, bLoop );
	
	if( bReset )
		g_pModelLT->ResetAnim( m_hModelObject, MAIN_TRACKER );

	return true;
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CWeapon::KillLoopSound
//
//  PURPOSE:	Call to the hand held weapon to kill any looping sound it may have.
//
// ----------------------------------------------------------------------- //

void CWeapon::KillLoopSound()
{
	if( !m_hModelObject )
		return;

	CHHWeaponModel *pHHWeapon = dynamic_cast< CHHWeaponModel* >( g_pLTServer->HandleToObject( m_hModelObject ));
	if( pHHWeapon )
	{
		pHHWeapon->KillLoopSound();
	}
}

LTFLOAT CWeapon::GetLifeTime() const
{
	if (!g_pWeaponMgr) return 0.0f;

	AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(m_nAmmoId);
	if (!pAmmo || !pAmmo->pProjectileFX) return 0.0f;

	return m_fLifeTime < 0.0f ? pAmmo->pProjectileFX->fLifeTime : m_fLifeTime;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CWeapon::HideWeapon
//
//  PURPOSE:	Hide or show the weapon...
//
// ----------------------------------------------------------------------- //

void CWeapon::HideWeapon( bool bHidden )
{
	m_bHidden = bHidden;
	g_pCommonLT->SetObjectFlags( m_hModelObject, OFT_Flags, (m_bHidden ? FLAG_FORCECLIENTUPDATE : FLAG_VISIBLE), FLAG_VISIBLE | FLAG_FORCECLIENTUPDATE );
}

void CWeapon::SetAmmoInClip( int nAmmoInClip )
{
	m_nAmmoInClip = nAmmoInClip;
}