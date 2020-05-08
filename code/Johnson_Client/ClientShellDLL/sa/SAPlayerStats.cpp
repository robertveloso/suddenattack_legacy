// ----------------------------------------------------------------------- //
//
// MODULE  : SAPlayerStats.cpp
//
// PURPOSE : Implementation of SAPlayerStats class
//
// CREATED : 10/9/97
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include <stdio.h>
#include "SAPlayerStats.h"
#include "GameClientShell.h"
#include "MsgIDs.h"
#include "SAHUDMgr.h"
#include "ClientWeaponBase.h"
#include "ClientWeaponMgr.h"
#include "MissionMgr.h"
#include "KeyMgr.h"
#include "clientres.h"

#ifdef _FINAL
	#define USE_CRYPT_KEY
#endif


// 광식
#include "..\\clientshellshared\\PlayerCamera.h"

void ArmorFn(int argc, char **argv)
{
	if (!g_pPlayerStats || argc < 1) return;
	g_pPlayerStats->UpdateArmor(atoi(argv[0]));
}
void MaxArmorFn(int argc, char **argv)
{
	if (!g_pPlayerStats || argc < 1) return;
	g_pPlayerStats->UpdateMaxArmor(atoi(argv[0]));
}
void HealthFn(int argc, char **argv)
{
	if (!g_pPlayerStats || argc < 1) return;
	g_pPlayerStats->UpdateHealth(atoi(argv[0]));
}
void MaxHealthFn(int argc, char **argv)
{
	if (!g_pPlayerStats || argc < 1) return;
	g_pPlayerStats->UpdateMaxHealth(atoi(argv[0]));
}
void AirFn(int argc, char **argv)
{
	if (!g_pPlayerStats || argc < 1) return;
	g_pPlayerStats->UpdateAir((uint8)atof(argv[0]));
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::CSAPlayerStats()
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CSAPlayerStats::CSAPlayerStats()
{
    LTRect nullrect(0,0,0,0);
    LTIntPt nullPt(0,0);

    m_pnAmmo            = LTNULL;
    m_pbHaveAmmo        = LTNULL;
    m_pbHaveWeapon      = LTNULL;
    m_pbCanUseWeapon    = LTNULL;
    m_pbCanUseAmmo      = LTNULL;
    m_pbHaveMod         = LTNULL;
    m_pbCanUseMod       = LTNULL;
    m_pbHaveGear        = LTNULL;
    m_pbCanUseGear      = LTNULL;
	m_nHealth			= 0;
	m_nDamage			= 0;
	m_nArmor			= 0;
	m_nMaxHealth		= 100;
	m_nMaxArmor			= 100;
	m_nCurrentWeapon	= WMGR_INVALID_ID;
	m_nCurrentAmmo		= WMGR_INVALID_ID;

	m_bHiding			= LTFALSE;
	m_bHidden			= LTFALSE;

	m_nAir				= 100;

	m_bObjAdded = LTFALSE;
	m_bObjRemoved = LTFALSE;
	m_bObjCompleted = LTFALSE;

	m_nTotalSkillPoints = 0;
	m_nAvailSkillPoints = 0;
	
	m_dwProgress		= 0;
	m_dwMaxProgress		= 0;

	memset(m_nSkills,0,sizeof(m_nSkills));

	// Added By Bestline ==>
	// Time : 2004-06-18 오후 6:22:09
	m_pbSelectAmmo		= LTNULL;
	// Added By Bestline <==
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::~CSAPlayerStats()
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CSAPlayerStats::~CSAPlayerStats()
{
	Term();
	g_pPlayerStats = LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::Init()
//
//	PURPOSE:	Initialize
//
// ----------------------------------------------------------------------- //

LTBOOL CSAPlayerStats::Init()
{
    if (!g_pLTClient) return LTFALSE;

	g_pLTClient->RegisterConsoleProgram("Armor", ArmorFn);
	g_pLTClient->RegisterConsoleProgram("MaxArmor", MaxArmorFn);
	g_pLTClient->RegisterConsoleProgram("Health", HealthFn);
	g_pLTClient->RegisterConsoleProgram("MaxHealth", MaxHealthFn);
	g_pLTClient->RegisterConsoleProgram("Air", AirFn);

	if (m_pnAmmo)
	{
		debug_deletea(m_pnAmmo);
        m_pnAmmo = LTNULL;
	}

	if (m_pbHaveAmmo)
	{
		debug_deletea(m_pbHaveAmmo);
        m_pbHaveAmmo = LTNULL;
	}

	if (m_pbCanUseAmmo)
	{
		debug_deletea(m_pbCanUseAmmo);
        m_pbCanUseAmmo = LTNULL;
	}

	int nNumAmmoIds = g_pWeaponMgr->GetNumAmmoIds();
	if (nNumAmmoIds > 0)
	{
        m_pnAmmo = debug_newa(uint32, nNumAmmoIds);
        m_pbCanUseAmmo = debug_newa(LTBOOL, nNumAmmoIds);
        m_pbHaveAmmo = debug_newa(LTBOOL, nNumAmmoIds);
        memset(m_pnAmmo, 0, sizeof(uint32) * nNumAmmoIds);
        memset(m_pbHaveAmmo, 0, sizeof(LTBOOL) * nNumAmmoIds);
        memset(m_pbCanUseAmmo, 0, sizeof(LTBOOL) * nNumAmmoIds);
	}

	if (m_pbHaveWeapon)
	{
		debug_deletea(m_pbHaveWeapon);
        m_pbHaveWeapon = LTNULL;
	}

	if (m_pbCanUseWeapon)
	{
		debug_deletea(m_pbCanUseWeapon);
        m_pbCanUseWeapon = LTNULL;
	}

	int nNumWeapons = g_pWeaponMgr->GetNumWeapons();
	if (nNumWeapons > 0)
	{
        m_pbHaveWeapon = debug_newa(LTBOOL, nNumWeapons);
        memset(m_pbHaveWeapon, 0, sizeof(LTBOOL) * nNumWeapons);

        m_pbCanUseWeapon = debug_newa(LTBOOL, nNumWeapons);
        memset(m_pbCanUseWeapon, 0, sizeof(LTBOOL) * nNumWeapons);
	}

	if (m_pbHaveMod)
	{
		debug_deletea(m_pbHaveMod);
        m_pbHaveMod = LTNULL;
	}

	if (m_pbCanUseMod)
	{
		debug_deletea(m_pbCanUseMod);
        m_pbCanUseMod = LTNULL;
	}

	if (m_pbHaveGear)
	{
		debug_deletea(m_pbHaveGear);
        m_pbHaveGear = LTNULL;
	}

	if (m_pbCanUseGear)
	{
		debug_deletea(m_pbCanUseGear);
        m_pbCanUseGear = LTNULL;
	}

	int nNumMods = g_pWeaponMgr->GetNumModIds();
	if (nNumMods > 0)
	{
        m_pbHaveMod = debug_newa(LTBOOL, nNumMods);
        memset(m_pbHaveMod, 0, sizeof(LTBOOL) * nNumMods);

        m_pbCanUseMod = debug_newa(LTBOOL, nNumMods);
        memset(m_pbCanUseMod, 0, sizeof(LTBOOL) * nNumMods);

	}

	int nNumGear = g_pWeaponMgr->GetNumGearIds();
	if (nNumGear > 0)
	{
        m_pbHaveGear = debug_newa(LTBOOL, nNumGear);
        memset(m_pbHaveGear, 0, sizeof(LTBOOL) * nNumGear);

        m_pbCanUseGear = debug_newa(LTBOOL, nNumGear);
        memset(m_pbCanUseGear, 0, sizeof(LTBOOL) * nNumGear);

	}

	ResetSkills();
	ResetObjectives();
	ResetMissionStats();

	g_pPlayerStats = this;


	// Added By Bestline ==>
	// Time : 2004-06-18 오후 6:22:46
	if (m_pbSelectAmmo)
	{
		debug_deletea(m_pbSelectAmmo);
        m_pbSelectAmmo = LTNULL;
	}

	if( nNumWeapons > 0 )
	{	
		m_pbSelectAmmo = debug_newa(LTBOOL, nNumWeapons);
		memset(m_pbSelectAmmo, 0, sizeof(LTBOOL) * nNumWeapons);
		memset(m_nOrderedWeaponId, 0xffffffff, sizeof(int)*MAX_WEAPON_CLASS);

		memset(m_nTempOrderWeaponId, 0xffffffff, sizeof(int)*MAX_WEAPON_CLASS);		
	}

	// Added By Bestline <==
	

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::Term()
//
//	PURPOSE:	Terminate the player stats
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::Term()
{
    if (!g_pLTClient) return;

	m_nHealth		= 0;
	m_nDamage		= 0;
	m_nArmor		= 0;
	m_nMaxHealth	= 0;
	m_nMaxArmor		= 0;

	if (m_pnAmmo)
	{
		debug_deletea(m_pnAmmo);
        m_pnAmmo = LTNULL;
	}

	if (m_pbCanUseAmmo)
	{
		debug_deletea(m_pbCanUseAmmo);
        m_pbCanUseAmmo = LTNULL;
	}

	if (m_pbHaveAmmo)
	{
		debug_deletea(m_pbHaveAmmo);
        m_pbHaveAmmo = LTNULL;
	}

	if (m_pbHaveWeapon)
	{
		debug_deletea(m_pbHaveWeapon);
        m_pbHaveWeapon = LTNULL;
	}

	if (m_pbCanUseWeapon)
	{
		debug_deletea(m_pbCanUseWeapon);
        m_pbCanUseWeapon = LTNULL;
	}

	if (m_pbHaveMod)
	{
		debug_deletea(m_pbHaveMod);
        m_pbHaveMod = LTNULL;
	}

	if (m_pbCanUseMod)
	{
		debug_deletea(m_pbCanUseMod);
        m_pbCanUseMod = LTNULL;
	}

	if (m_pbHaveGear)
	{
		debug_deletea(m_pbHaveGear);
        m_pbHaveGear = LTNULL;
	}

	if (m_pbCanUseGear)
	{
		debug_deletea(m_pbCanUseGear);
        m_pbCanUseGear = LTNULL;
	}

	// Added By Bestline ==>
	// Time : 2004-06-18 오후 6:22:46
	if (m_pbSelectAmmo)
	{
		debug_deletea(m_pbSelectAmmo);
        m_pbSelectAmmo = LTNULL;
	}
	// Added By Bestline <==
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::Update()
//
//	PURPOSE:	Handle regular updates
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::Update()
{
	if (m_bObjAdded)
	{
		g_pClientSoundMgr->PlaySoundLocal("interface\\snd\\objadd.wav", SOUNDPRIORITY_MISC_MEDIUM);
	}
	else if (m_bObjCompleted)
	{
		g_pClientSoundMgr->PlaySoundLocal("interface\\snd\\objcomplete.wav", SOUNDPRIORITY_MISC_MEDIUM);
	}
	else if (m_bObjRemoved)
	{
		g_pClientSoundMgr->PlaySoundLocal("interface\\snd\\objremove.wav", SOUNDPRIORITY_MISC_MEDIUM);
	}

	m_bObjAdded = LTFALSE;
	m_bObjRemoved = LTFALSE;
	m_bObjCompleted = LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::OnEnterWorld()
//
//	PURPOSE:	Handle entering the world
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::OnEnterWorld(LTBOOL bRestoringGame)
{
    if (!g_pLTClient || !g_pGameClientShell) return;

	// find out what mode we are in and make sure that mode is set

	ResetStats();

	if (!bRestoringGame)
	{
		// clear the values

		Clear();
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::OnExitWorld()
//
//	PURPOSE:	Handle exiting the world
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::OnExitWorld()
{
    if (!g_pLTClient) return;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::Clear()
//
//	PURPOSE:	Handle clearing the stats
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::Clear()
{
	UpdateHealth(0);
	m_nDamage = 0;
	UpdateArmor(0);

	// Get rid of any weapons, ammo, gear or mods..
	
	ResetInventory();
	ResetSkills();
	ResetObjectives();
	ResetMissionStats();
	m_IntelList.Clear();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::ResetStats()
//
//	PURPOSE:	Reset the stats
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::ResetStats()
{
	m_MissionStats.Init();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdatePlayerWeapon()
//
//	PURPOSE:	Update the weapon related stats
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdatePlayerWeapon(uint8 nWeaponId, uint8 nAmmoId, LTBOOL bForce)
{
	if (!g_pGameClientShell || !g_pWeaponMgr->IsValidWeaponId(nWeaponId)) return;

	if (!bForce)
	{
		if (m_nCurrentWeapon == nWeaponId && m_nCurrentAmmo == nAmmoId) return;
	}

	m_nCurrentWeapon = nWeaponId;

	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(m_nCurrentWeapon);
	if (!pWeapon) return;
	
	if( pWeapon->aAmmoIds ) m_nCurrentAmmo = pWeapon->aAmmoIds[0];
	g_pHUDMgr->QueueUpdate(kHUDAmmo);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateHealth()
//
//	PURPOSE:	Update the health stat
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateHealth(uint32 nHealth)
{
	if (nHealth > m_nMaxHealth)
		nHealth = m_nMaxHealth;

	if (m_nHealth == nHealth) return;

	if (nHealth < m_nHealth)
	{
		m_nDamage += (m_nHealth - nHealth);
	}


	// update the member variable
	m_nHealth = nHealth;
    g_pHUDMgr->QueueUpdate(kHUDHealth);



}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateArmor()
//
//	PURPOSE:	Update the armor stat
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateArmor(uint32 nArmor)
{
	if (nArmor > m_nMaxArmor)
		nArmor = m_nMaxArmor;
	if (m_nArmor == nArmor) return;

	// update the member variable
	m_nArmor = nArmor;
    g_pHUDMgr->QueueUpdate(kHUDArmor);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateMaxHealth()
//
//	PURPOSE:	Update the health stat maximum
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateMaxHealth(uint32 nHealth)
{
	if (m_nMaxHealth == nHealth) return;

	// update the member variable
	m_nMaxHealth = nHealth;

	//if we have more than our max... reduce
	if (m_nHealth > m_nMaxHealth)
		m_nHealth = m_nMaxHealth;

    g_pHUDMgr->QueueUpdate(kHUDHealth);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateMaxArmor()
//
//	PURPOSE:	Update the armor stat maximum
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateMaxArmor(uint32 nArmor)
{
	if (m_nMaxArmor == nArmor) return;

	// update the member variables
	m_nMaxArmor = nArmor;

	//if we have more than our max... reduce
	if (m_nArmor > m_nMaxArmor)
		m_nArmor = m_nMaxArmor;
    g_pHUDMgr->QueueUpdate(kHUDArmor);
}

//void CSAPlayerStats::SetCanUseWeapon( int nWeapon )
void CSAPlayerStats::SetCanUseWeapon( int nWeapon, int nClass )
{
	ASSERT( nClass >= 0 && nClass < MAX_WEAPON_CLASS );

	// Johnson Li 2007-07-26 
	if (nWeapon > g_pWeaponMgr->GetNumWeapons())
	{
		return ;
	}
	
	m_pbSelectAmmo[nWeapon] = LTTRUE;
	m_nOrderedWeaponId[nClass] = nWeapon;
	m_nTempOrderWeaponId[nClass] = nWeapon;
}

void CSAPlayerStats::SetResetWeapon( )
{
	int nNumWeapons = g_pWeaponMgr->GetNumWeapons();
	if (nNumWeapons > 0)
	{
		memset(m_pbSelectAmmo, 0, sizeof(LTBOOL) * nNumWeapons);
		memset(m_nOrderedWeaponId, 0xffffffff, sizeof(int)*MAX_WEAPON_CLASS);

		memset(m_nTempOrderWeaponId, 0xffffffff, sizeof(int)*MAX_WEAPON_CLASS);
	}
}

void CSAPlayerStats::RestartWeapon()
{
	int nNumWeapons = g_pWeaponMgr->GetNumWeapons();
	if (nNumWeapons > 0)
	{
		memset(m_pbSelectAmmo, 0, sizeof(LTBOOL) * nNumWeapons);
		memset(m_pbHaveWeapon, 0, sizeof(LTBOOL) * nNumWeapons);
        memset(m_pbCanUseWeapon, 0, sizeof(LTBOOL) * nNumWeapons);

		g_pWpnChooser->RemoveAllWeapon();
	}

	/*
	for( int i = 0; i < MAX_WEAPON_CLASS; i++ )
	{
		if( g_pWeaponMgr->IsValidWeaponId( m_nOrderedWeaponId[i] ) )
		{
			m_pbSelectAmmo[m_nOrderedWeaponId[i]]	= LTTRUE;
			m_pbHaveWeapon[m_nOrderedWeaponId[i]]	= LTTRUE;			
			m_pbCanUseWeapon[m_nOrderedWeaponId[i]]	= LTTRUE;

			m_nTempOrderWeaponId[i] = m_nOrderedWeaponId[i];
			
			g_pWpnChooser->AddWeapon( i, m_nOrderedWeaponId[i], (i==1), LTFALSE );
		}
	}
	*/
	for(int i = 0; i < MAX_WEAPON_CLASS-1; i++ )
	{
		if( g_pWeaponMgr->IsValidWeaponId( m_nOrderedWeaponId[i] ) )
		{
			m_pbSelectAmmo[m_nOrderedWeaponId[i]]	= LTTRUE;
			m_pbHaveWeapon[m_nOrderedWeaponId[i]]	= LTTRUE;			
			m_pbCanUseWeapon[m_nOrderedWeaponId[i]]	= LTTRUE;

			m_nTempOrderWeaponId[i] = m_nOrderedWeaponId[i];
			
			g_pWpnChooser->AddWeapon( i, m_nOrderedWeaponId[i], (i==1), LTFALSE );
		}
	}

	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon("TimeBomb");
	if( g_GameDoc.GetTeam() == TEAM_A && IsRevivePlayerGameType() )
	{	
		if( pWeapon )
		{
			if( g_pWeaponMgr->IsValidWeaponId(pWeapon->nId) )
			{
				m_pbSelectAmmo[pWeapon->nId]	= LTTRUE;
				m_pbHaveWeapon[pWeapon->nId]	= LTTRUE;	
				m_pbCanUseWeapon[pWeapon->nId]	= LTTRUE;
				m_nTempOrderWeaponId[4] = m_nOrderedWeaponId[4] = pWeapon->nId;

				g_pWpnChooser->AddWeapon( 4, m_nOrderedWeaponId[4], LTFALSE, LTFALSE );
			}
			
		}
	}
	else
	{
		m_pbSelectAmmo[pWeapon->nId]	= LTFALSE;
		m_pbHaveWeapon[pWeapon->nId]	= LTFALSE;	
		m_pbCanUseWeapon[pWeapon->nId]	= LTFALSE;
		m_nTempOrderWeaponId[4] = m_nOrderedWeaponId[4] = 0xffffffff;
	}

	g_pWpnChooser->SetIconPos();

	DropInventory();
}

void CSAPlayerStats::RestartWpnChooser()
{
	g_pWpnChooser->RemoveAllWeapon();

	for(int i = 0; i < MAX_WEAPON_CLASS-1; i++ )
	{
		if( g_pWeaponMgr->IsValidWeaponId( m_nOrderedWeaponId[i] ) )
		{
			m_pbSelectAmmo[m_nOrderedWeaponId[i]]	= LTTRUE;
			m_pbHaveWeapon[m_nOrderedWeaponId[i]]	= LTTRUE;			
			m_pbCanUseWeapon[m_nOrderedWeaponId[i]]	= LTTRUE;

			m_nTempOrderWeaponId[i] = m_nOrderedWeaponId[i];
			
			g_pWpnChooser->AddWeapon( i, m_nOrderedWeaponId[i], LTFALSE, LTFALSE );
		}
	}

	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon("TimeBomb");
	if( g_GameDoc.GetTeam() == TEAM_A && IsRevivePlayerGameType() )
	{		
		if( pWeapon )
		{
			if( g_pWeaponMgr->IsValidWeaponId(pWeapon->nId) )
			{
				g_pWpnChooser->AddWeapon( 4, pWeapon->nId, LTFALSE, LTFALSE );
			}			
		}
	}
	
	g_pWpnChooser->SetIconPos();
}

LTBOOL CSAPlayerStats::ThrowWeapon( int nWeaponId, int nClass )
{
	ASSERT( (nClass - 1) >= 0 );

	if( g_pWeaponMgr->IsValidWeaponId( nWeaponId ) &&
		m_nTempOrderWeaponId[nClass-1] == nWeaponId )
	{
		m_pbSelectAmmo[nWeaponId]	= LTFALSE;
		m_pbHaveWeapon[nWeaponId]	= LTFALSE;
		m_pbCanUseWeapon[nWeaponId]	= LTFALSE;

		m_nTempOrderWeaponId[nClass-1] = WMGR_INVALID_ID;

		return LTTRUE;
	}

	return LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateAmmo()
//
//	PURPOSE:	Update the ammo stat
//
// ----------------------------------------------------------------------- //
 
void CSAPlayerStats::UpdateAmmo(uint8 nWeaponId, uint8 nAmmoId,
                              uint32 nAmmo, LTBOOL bPickedup,
                              LTBOOL bDisplayMsg)
{
	if (!g_pLTClient || !g_pWeaponMgr) return;

	// 광식
	//--------------------------------------------------------------------------------
	if( bDisplayMsg && bPickedup )
	{
		// 무기 바꾸기		
		uint8 nWeaponClass = g_pWeaponMgr->GetWeaponClass(nWeaponId);
		if( (nWeaponClass - 1) >= 0 )
		{
			/*
			m_pbSelectAmmo[m_nTempOrderWeaponId[nWeaponClass - 1]] = LTFALSE;
			m_pbHaveWeapon[m_nTempOrderWeaponId[nWeaponClass - 1]] = LTFALSE;
			m_pbCanUseWeapon[m_nTempOrderWeaponId[nWeaponClass - 1]] = LTFALSE;

			// 가지고 있던 무기를 던지자..
			//------------------------------------------
			LTRotation rRot = g_pPlayerMgr->GetPlayerCamera()->GetRotation();
			LTVector vForward = rRot.Forward();
			CAutoMessage cMsg;
			cMsg.Writeuint8( MID_PROJECTILE );
			cMsg.Writeuint8( MPROJ_WEAPON_THROW );
			cMsg.Writeuint8( m_nTempOrderWeaponId[nWeaponClass - 1] );			
			cMsg.WriteLTVector( vForward );
			g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
			//------------------------------------------
			*/

			m_nTempOrderWeaponId[nWeaponClass - 1] = nWeaponId;
		}
		m_pbSelectAmmo[nWeaponId] = LTTRUE;
		m_pbHaveWeapon[nWeaponId] = LTTRUE;
		m_pbCanUseWeapon[nWeaponId] = LTTRUE;
	}
	//--------------------------------------------------------------------------------

	if( m_pbSelectAmmo )
	{
		if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
		{
		}
		else
		{
			if(m_pbSelectAmmo[nWeaponId] == LTFALSE )
				return;
		}
	}
	
	if (bPickedup && g_pWeaponMgr->IsValidWeaponId(nWeaponId))
	{
		if (m_pbHaveWeapon)
		{
			m_pbHaveWeapon[nWeaponId] = LTTRUE;
			
			if (m_pbCanUseWeapon)
			{
				m_pbCanUseWeapon[nWeaponId] = LTTRUE;
			}
		}	
	}

	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(nWeaponId);

	if (bPickedup && pWeapon && m_pbHaveWeapon[nWeaponId])
	{
		for (int m = 0; m < pWeapon->nNumModIds; m++)
		{
			int nModId = pWeapon->aModIds[m];
			MOD const *pMod = g_pWeaponMgr->GetMod(nModId);

			if (pMod && pMod->bIntegrated)
			{
				if (m_pbHaveMod)
				{
					m_pbHaveMod[nModId] = LTTRUE;
					if (m_pbCanUseMod)
						m_pbCanUseMod[nModId] = LTTRUE;
				}
			}
		}
	}

	AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(nAmmoId);

    LTBOOL bInfiniteAmmo	= pWeapon ? pWeapon->bInfiniteAmmo : LTFALSE;
	LTBOOL bDefaultAmmo		= pWeapon ? pWeapon->nDefaultAmmoId == nAmmoId : LTFALSE;
	int nEquipWeapon = WMGR_INVALID_ID;

	if (pAmmo )//&& (!bInfiniteAmmo || !bDefaultAmmo) )
	{
		if (m_pnAmmo)
		{
			if (bPickedup) /*** BL 12/08/2000 Added to equip weapon when you are out of ammo and pick up ammo/weapon ******/
			{
				// Check to see if we are out of ammo (minus gadgets, infinite ammo weapons like fisticuffs, barette, etc)

				LTBOOL bOutOfAmmo = LTTRUE;
				int nWeaponBest = WMGR_INVALID_ID;

				for ( int32 iWeapon = g_pWeaponMgr->GetNumWeapons()-1 ; iWeapon >= 0 ; iWeapon-- )
				{
					// There's an assumption that the ammo id
					// is the same as the index, so I've fixed
					// original code to do what it meant to do.
					int32 nWeapon = iWeapon;
					ASSERT ( nWeapon == g_pWeaponMgr->GetWeapon(nWeapon)->nId );

					// Here's the original incorrect line.  (Notice
					// that GetWeaponId wants a CommandID.)
					//int32 nWeapon = g_pWeaponMgr->GetWeaponId(iWeapon);

					if ( nWeapon != WMGR_INVALID_ID && m_pbHaveWeapon[nWeapon] )
					{
						WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(nWeapon);

						if ( !pWeapon->bInfiniteAmmo )
						{
							for ( int32 iAmmo = 0 ; iAmmo < pWeapon->nNumAmmoIds ; iAmmo++ )
							{
								if (pWeapon->aAmmoIds[iAmmo] == WMGR_INVALID_ID) 
									continue;
								if ( (nWeaponBest == WMGR_INVALID_ID) && (pWeapon->aAmmoIds[iAmmo] == nAmmoId) )
								{
									nWeaponBest = nWeapon;
								}

								AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(pWeapon->aAmmoIds[iAmmo]);

								if ( pAmmo->eType == GADGET )
								{
								}
								else if ( GetAmmoCount(pWeapon->aAmmoIds[iAmmo]) > 0 )
								{
									bOutOfAmmo = LTFALSE;
									break;
								}
								else
								{
									//g_pLTClient->CPrint("%s is out of %s", pWeapon->szName, pAmmo->szName);
								}
							}
						}
					}
				}

				if ( bOutOfAmmo )
				{
					//g_pLTClient->CPrint("Out of ammo!");

					if ( nWeaponId == WMGR_INVALID_ID )
					{
						// Picked up ammo -- equip the best gun for that can use the ammo

						nEquipWeapon = nWeaponBest;
					}
					else
					{
						// Picked up a gun -- equip that gun

						nEquipWeapon = nWeaponId;
					}
				}
				else
				{
					//g_pLTClient->CPrint("Not out of ammo!");
				}
			}  /*** BL 12/08/2000 End changes ******/

			uint32 maxAmmo = pAmmo->GetMaxAmount(LTNULL);
			if ( GetAmmoCount(nAmmoId) > maxAmmo)
			{
				SetAmmoCount(nAmmoId, maxAmmo);				
			}

			if (nAmmo > maxAmmo)
			{
				nAmmo = maxAmmo;
			}

			int taken = nAmmo - GetAmmoCount(nAmmoId);

			SetAmmoCount( nAmmoId, nAmmo );

			// [KLS 7/14/02] Display a message even if we didn't pick up anything...
			if (bPickedup && bDisplayMsg)
			{
				// [KLS 7/12/02] If this ammo is associated with a weapon
				// only display a message if the weapon doesn't use infinite ammo
				if (!pWeapon || !(pWeapon->bInfiniteAmmo))
				{
					bool bTookAmmo = bool(taken != 0);
					//g_pGameClientShell->HandleAmmoPickup(nAmmoId, taken, bTookAmmo, nWeaponId);
					// 광식 : 총을 줍는 동시에 총알도 함께 습득하는 것으로 처리					
					g_pGameClientShell->HandleAmmoPickup(nAmmoId, nAmmo, true, nWeaponId);
				}
			}

			if (taken != 0)
			{
				if (m_pbHaveAmmo)
				{
					m_pbHaveAmmo[nAmmoId] = LTTRUE;
					if (m_pbCanUseAmmo)
						m_pbCanUseAmmo[nAmmoId] = LTTRUE;
				}
			}
		}
	}

	//------------------------------------------------------
	// 광식
	//------------------------------------------------------
	if( bDisplayMsg && bPickedup )
	{
		IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
		if( !pClientWeapon ) return;

		int nCurWeaponClass = g_pWeaponMgr->GetWeaponClass(pClientWeapon->GetWeaponId());
		int nWeaponClass = g_pWeaponMgr->GetWeaponClass(nWeaponId);
		if( nCurWeaponClass > nWeaponClass )
		{
			g_pPlayerMgr->ChangeWeapon( nWeaponId, nAmmoId );
		}
	}
	//------------------------------------------------------


//[SPIKE] 임시 주석 ///////////////////////////
//	if ( nEquipWeapon != WMGR_INVALID_ID )  /*** BL 12/08/2000 Added to change equip weapon when you are out of ammo and pick up a weapon ******/
//	{
//		g_pPlayerMgr->ChangeWeapon( nEquipWeapon, nAmmoId );
//	}  /*** BL 12/08/2000 End changes ******/
///////////////////////////////////////////////
	g_pHUDMgr->QueueUpdate(kHUDAmmo);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateGear()
//
//	PURPOSE:	Update the gear stats
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateGear(uint8 nGearId)
{
    if (!g_pLTClient || !g_pWeaponMgr) return;

	if (g_pWeaponMgr->IsValidGearId(nGearId))
	{
        LTBOOL bHadAirSupply = HaveAirSupply();
		if (m_pbHaveGear)
		{
            m_pbHaveGear[nGearId] = LTTRUE;
			if (m_pbCanUseGear)
			{
				GEAR const *pGear = g_pWeaponMgr->GetGear(nGearId);
				if (pGear && pGear->bExclusive)
					m_pbCanUseGear[nGearId] = LTTRUE;
			}

		}

	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateMod()
//
//	PURPOSE:	Update the mod stats
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateMod(uint8 nModId)
{
	if (g_pWeaponMgr->IsValidModId(nModId))
	{
		if (m_pbHaveMod)
		{
			if (!m_pbHaveMod[nModId])
			{
				m_pbHaveMod[nModId] = LTTRUE;
				m_pbCanUseMod[nModId] = LTTRUE;

				// Make sure the player-view weapon is updated if
				// this mod is used with the weapon...

				MOD const *pMod = g_pWeaponMgr->GetMod(nModId);
				if (!pMod) return;

				IClientWeaponBase* pClientWeapon = g_pPlayerMgr->GetClientWeaponMgr()->GetCurrentClientWeapon();
				
				if (pClientWeapon && pClientWeapon->GetWeaponId() == pMod->GetWeaponId())
				{
					pClientWeapon->CreateMods();
				}
			}
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateAir()
//
//	PURPOSE:	Update the air stat
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateAir(uint8 nAir)
{
    if (!g_pLTClient) return;

	if (nAir > 100)
		nAir = 100;
	if (nAir < 0)
		nAir = 0;

	if (m_nAir == nAir) return;

	m_nAir = nAir;
	g_pHUDMgr->QueueUpdate(kHUDAir);

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateObjectives()
//
//	PURPOSE:	Update the objectives
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateObjectives(uint8 nThing, uint8 nType, uint32 dwId)
{
	switch (nType)
	{
		case ITEM_ADD_ID:
		{
			uint8 nIndex;
			if (m_CompletedObjectives.Have(dwId, nIndex))
			{
				g_pLTClient->CPrint("objective %d already completed", dwId);
			}
			else
			{
				if (nThing == IC_OBJECTIVE_ID && m_Objectives.Add(dwId))
				{
					g_pLTClient->CPrint("objective %d added", dwId);
					m_bObjAdded = LTTRUE;
				}
				else if (nThing == IC_OPTION_ID && m_OptionalObjectives.Add(dwId))
				{
					g_pLTClient->CPrint("optional objective %d added", dwId);
					m_bObjAdded = LTTRUE;
				}
				else if (nThing == IC_PARAMETER_ID && m_Parameters.Add(dwId))
				{
					g_pLTClient->CPrint("mission parameter %d added", dwId);
					m_bObjAdded = LTTRUE;
				}
			}
		}
		break;

		case ITEM_REMOVE_ID:
		{
			m_bObjRemoved = m_Objectives.Remove(dwId);
			m_bObjRemoved |= m_OptionalObjectives.Remove(dwId);
			m_bObjRemoved |= m_Parameters.Remove(dwId);
			m_bObjRemoved |= m_CompletedObjectives.Remove(dwId);

			if (m_bObjRemoved)
				g_pLTClient->CPrint("objective %d removed", dwId);


		}
		break;

		case ITEM_COMPLETE_ID:
		{
			m_Objectives.Remove(dwId);
			m_OptionalObjectives.Remove(dwId);
			m_bObjCompleted = m_CompletedObjectives.Add(dwId);
			g_pLTClient->CPrint("objective %d completed", dwId);

		}
		break;

		case ITEM_CLEAR_ID:
		{
			if (m_Objectives.m_IDArray.size() || m_OptionalObjectives.m_IDArray.size() ||
				m_CompletedObjectives.m_IDArray.size() || m_Parameters.m_IDArray.size())
                m_bObjRemoved = LTTRUE;

			m_Objectives.Clear();
			m_OptionalObjectives.Clear();
			m_CompletedObjectives.Clear();
			m_Parameters.Clear();

			g_pLTClient->CPrint("objectives cleared", dwId);

		}
		break;

		default :
		break;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateKeys()
//
//	PURPOSE:	Update the key item list
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateKeys(uint8 nType, uint16 nId)
{
	switch (nType)
	{
		case ITEM_ADD_ID:
		{
			if (g_pKeyMgr->IsValidKey(nId))
			{
				m_Keys.Add(nId);
				KEY* pKey = g_pKeyMgr->GetKey(nId);
				//g_pPickupMsgs->AddMessage(LoadTempString(pKey->nNameId),pKey->szImage);
			}
		}
		break;

		case ITEM_REMOVE_ID:
		{
			if (g_pKeyMgr->IsValidKey(nId))
			{
				m_Keys.Remove(nId);
			}
		}
		break;

		case ITEM_CLEAR_ID:
		{
			m_Keys.Clear();
		}
		break;

		default :
		break;
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateIntel()
//
//	PURPOSE:	Update the intel item list
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateIntel(uint32 nTextId, uint8 nPopupId, LTBOOL bIsIntel)
{
	uint16 nIndex = m_IntelList.GetIndex(nTextId);
	if (!m_IntelList.IsValid(nIndex))
	{
		INTEL_ITEM* pItem = debug_new(INTEL_ITEM);
		pItem->nTextId = nTextId;
		pItem->nPopupId = nPopupId;
		pItem->bIsIntel = bIsIntel;
		pItem->nMissionNum = (uint8)g_pMissionMgr->GetCurrentMission();
		m_IntelList.Add(pItem);
	}

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::UpdateHiding()
//
//	PURPOSE:	Update the player hiding state
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::UpdateHiding(LTBOOL bIsHiding, LTBOOL bIsHidden, LTBOOL bCanthide, float fHideDuration)
{
	if( bIsHidden && !m_bHidden )
	{
		g_pClientSoundMgr->PlaySoundLocal("interface\\snd\\playerhidden.wav", SOUNDPRIORITY_MISC_MEDIUM);
	}

	m_bHiding = bIsHiding;
	m_bHidden = bIsHidden;
	m_bCantHide = bCanthide;
	m_fHideDuration = fHideDuration;
	g_pHUDMgr->QueueUpdate(kHUDHiding);
}


void CSAPlayerStats::UpdateSkills(ILTMessage_Read *pMsg)
{
	m_nTotalSkillPoints = pMsg->Readuint32();
	m_nAvailSkillPoints = pMsg->Readuint32();

	for (uint8 i = 0; i < kNumSkills; i++)
	{
		m_nSkills[i] = pMsg->Readuint8();
	}

}

void CSAPlayerStats::UpdateMissionStats(ILTMessage_Read *pMsg)
{
	m_MissionStats.ReadData(pMsg);

}


void CSAPlayerStats::GainSkills(uint8 nRewardId, uint8 nBonusId, uint16 nAmount)
{
	/*
	if (IsMultiplayerGame()) return;


	//deal with bonus points here
	if (nBonusId && nAmount)
	{
		m_nTotalSkillPoints += nAmount;
		m_nAvailSkillPoints += nAmount;

		// Inform the user that they gained a reward...
		g_pClientSoundMgr->PlayInterfaceSound("Interface\\Snd\\reward.wav");
		return;
	}

	int nCurrentMission = g_pMissionMgr->GetCurrentMission();

	if (nCurrentMission < 0 || nCurrentMission >= g_pMissionButeMgr->GetNumMissions()) 
	{
		if (!nAmount)
			g_pLTClient->CPrint("CSAPlayerStats::GainSkills Invalid current mission number (%d)", nCurrentMission);
		else
		{
			m_nTotalSkillPoints += nAmount;
			m_nAvailSkillPoints += nAmount;
			g_pClientSoundMgr->PlayInterfaceSound("Interface\\Snd\\reward.wav");
		}
		return;
	}

	MISSION *pMission = g_pMissionButeMgr->GetMission(nCurrentMission);
	if (!pMission)
	{
		g_pLTClient->CPrint("CSAPlayerStats::GainSkills Invalid mission (%d)", nCurrentMission);
		return;
	}

	if (nRewardId >= pMission->nNumRewards)
	{
		if (!nAmount)
			g_pLTClient->CPrint("CSAPlayerStats::GainSkills Invalid reward id (%d)", nRewardId);
		else
		{
			m_nTotalSkillPoints += nAmount;
			m_nAvailSkillPoints += nAmount;
			g_pClientSoundMgr->PlayInterfaceSound("Interface\\Snd\\reward.wav");
		}
		return;
	}

	uint32 nPoints = pMission->aRewards[nRewardId].nVal;
	m_nTotalSkillPoints += nPoints;
	m_nAvailSkillPoints += nPoints;

	// Inform the user that they gained a reward...
	g_pClientSoundMgr->PlayInterfaceSound("Interface\\Snd\\reward.wav");
	*/
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::OnObjectivesDataMessage()
//
//	PURPOSE:	Override current objectives with new list from server
//
// ----------------------------------------------------------------------- //

void CSAPlayerStats::OnObjectivesDataMessage(ILTMessage_Read *pMsg)
{
	if (!pMsg) return;

	if (m_Objectives.m_IDArray.size() || m_CompletedObjectives.m_IDArray.size())
        m_bObjRemoved = LTTRUE;
	m_Objectives.Clear();
	m_CompletedObjectives.Clear();

	//read number of objectives
    uint8 nNumObj = pMsg->Readuint8();

	//read list of objectives
    for (uint8 i = 0; i < nNumObj; i++)
	{
        uint32 dwId = pMsg->Readuint32();
		m_Objectives.Add(dwId);
	}

	//read number of optional objectives
    uint8 nNumOptObj = pMsg->Readuint8();

	//read list of optional objectives
	for (i = 0; i < nNumOptObj; i++)
	{
        uint32 dwId = pMsg->Readuint32();
		m_OptionalObjectives.Add(dwId);
	}

	//read number of completed objectives
    uint8 nNumCompObj = pMsg->Readuint8();

	//read list of completed objectives
	for (i = 0; i < nNumCompObj; i++)
	{
        uint32 dwId = pMsg->Readuint32();
		m_CompletedObjectives.Add(dwId);
	}

	//read number of parameters
    uint8 nNumParam = pMsg->Readuint8();

	//read list of parameters
    for (i = 0; i < nNumParam; i++)
	{
        uint32 dwId = pMsg->Readuint32();
		m_Parameters.Add(dwId);
	}

	m_bObjAdded = (nNumObj + nNumOptObj + nNumCompObj + nNumParam) > 0;
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::Save
//
//	PURPOSE:	Save the player stats info
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::Save(ILTMessage_Write *pMsg)
{
    if (!pMsg || !g_pWeaponMgr) return;

    m_Objectives.Save(pMsg);
    m_OptionalObjectives.Save(pMsg);
    m_CompletedObjectives.Save(pMsg);
    m_Parameters.Save(pMsg);
    m_Keys.Save(pMsg);
    m_IntelList.Save(pMsg);

    pMsg->Writeuint32(m_nHealth);
    pMsg->Writeuint32(m_nDamage);
    pMsg->Writeuint32(m_nArmor);
    pMsg->Writeuint32(m_nMaxHealth);
    pMsg->Writeuint32(m_nMaxArmor);

	int nNumAmmoTypes = g_pWeaponMgr->GetNumAmmoIds();
    uint8 i;
    for (i = 0; i < nNumAmmoTypes; i++)
	{
        pMsg->Writebool(m_pbCanUseAmmo[i] != LTFALSE);
        pMsg->Writebool(m_pbHaveAmmo[i] != LTFALSE);
        pMsg->Writeuint32( GetAmmoCount(i) );
	}

	int nNumWeapons = g_pWeaponMgr->GetNumWeapons();
	for (i = 0; i < nNumWeapons; i++)
	{
        pMsg->Writebool(m_pbCanUseWeapon[i] != LTFALSE);
        pMsg->Writebool(m_pbHaveWeapon[i] != LTFALSE);
	}

	int nNumMods = g_pWeaponMgr->GetNumModIds();
	for (i = 0; i < nNumMods; i++)
	{
        pMsg->Writebool(m_pbCanUseMod[i] != LTFALSE);
        pMsg->Writebool(m_pbHaveMod[i] != LTFALSE);
	}

	int nNumGear = g_pWeaponMgr->GetNumGearIds();
	for (i = 0; i < nNumGear; i++)
	{
        pMsg->Writebool(m_pbCanUseGear[i] != LTFALSE);
        pMsg->Writebool(m_pbHaveGear[i] != LTFALSE);
	}

    pMsg->Writeuint8(m_nCurrentWeapon);
    pMsg->Writeuint8(m_nCurrentAmmo);
    pMsg->Writefloat((LTFLOAT)m_nAir);

    pMsg->Writebool(m_bHiding != LTFALSE);
    pMsg->Writebool(m_bHidden != LTFALSE);

	pMsg->Writeuint32(m_nTotalSkillPoints);
	pMsg->Writeuint32(m_nAvailSkillPoints);
	for (uint32 nCurSkill = 0; nCurSkill < kNumSkills; ++nCurSkill)
		pMsg->Writeuint8(m_nSkills[nCurSkill]);

}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::Load
//
//	PURPOSE:	Load the player stats info
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::Load(ILTMessage_Read *pMsg)
{
    if (!pMsg || !g_pWeaponMgr) return;

    m_Objectives.Load(pMsg);
    m_OptionalObjectives.Load(pMsg);
    m_CompletedObjectives.Load(pMsg);
    m_Parameters.Load(pMsg);
    m_Keys.Load(pMsg);
    m_IntelList.Load(pMsg);

    m_nHealth           = pMsg->Readuint32();
    m_nDamage           = pMsg->Readuint32();
    m_nArmor            = pMsg->Readuint32();
    m_nMaxHealth        = pMsg->Readuint32();
    m_nMaxArmor         = pMsg->Readuint32();


    uint8 i;
	int nNumAmmoTypes = g_pWeaponMgr->GetNumAmmoIds();
    for (i = 0; i < nNumAmmoTypes; i++)
	{
        m_pbCanUseAmmo[i]   = pMsg->Readbool() ? LTTRUE : LTFALSE;
        m_pbHaveAmmo[i] = pMsg->Readbool() ? LTTRUE : LTFALSE;
        SetAmmoCount( i, pMsg->Readuint32() );
	}

	int nNumWeapons = g_pWeaponMgr->GetNumWeapons();
	for ( i = 0; i < nNumWeapons; i++)
	{
        m_pbCanUseWeapon[i] = pMsg->Readbool() ? LTTRUE : LTFALSE;
        m_pbHaveWeapon[i] = pMsg->Readbool() ? LTTRUE : LTFALSE;
	}

	int nNumMods = g_pWeaponMgr->GetNumModIds();
	for ( i = 0; i < nNumMods; i++)
	{
        m_pbCanUseMod[i] = pMsg->Readbool() ? LTTRUE : LTFALSE;
        m_pbHaveMod[i] = pMsg->Readbool() ? LTTRUE : LTFALSE;
	}

	int nNumGear = g_pWeaponMgr->GetNumGearIds();
	for ( i = 0; i < nNumGear; i++)
	{
        m_pbCanUseGear[i] = pMsg->Readbool() ? LTTRUE : LTFALSE;
        m_pbHaveGear[i] = pMsg->Readbool() ? LTTRUE : LTFALSE;
	}

    m_nCurrentWeapon    = pMsg->Readuint8();
    m_nCurrentAmmo      = pMsg->Readuint8();
    m_nAir				= (uint8)pMsg->Readfloat();

    m_bHiding			= pMsg->Readbool() ? LTTRUE : LTFALSE;
    m_bHidden			= pMsg->Readbool() ? LTTRUE : LTFALSE;

	m_nTotalSkillPoints = pMsg->Readuint32();
	m_nAvailSkillPoints = pMsg->Readuint32();
	for (uint32 nCurSkill = 0; nCurSkill < kNumSkills; ++nCurSkill)
		m_nSkills[nCurSkill] = pMsg->Readuint8();

    UpdatePlayerWeapon(m_nCurrentWeapon, m_nCurrentAmmo, LTTRUE);
}
// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::GetMod
//
//	PURPOSE:	Get the id of thefirst mod for the current weapon
//				of the given type
//
// --------------------------------------------------------------------------- //

uint8 CSAPlayerStats::GetMod(ModType eType, const WEAPON* pW/*=LTNULL*/)
{
	uint8 nModId	= WMGR_INVALID_ID;
	int nPriority	= -1;

	// [KLS - 02/13/02] Updated to support checking for mods on weapons other than
	// the current weapon...
	WEAPON const *pWpn = (pW ? pW : g_pWeaponMgr->GetWeapon(m_nCurrentWeapon));

	if (pWpn)
	{
		for (int i=0; i < pWpn->nNumModIds; i++)
		{
			if (HaveMod(pWpn->aModIds[i]))
			{
				MOD const *pMod = g_pWeaponMgr->GetMod(pWpn->aModIds[i]);

				if (pMod && pMod->eType == eType)
				{
					// Get the mod with the hightest priority...

					if (pMod->nPriority > nPriority)
					{
						nPriority = pMod->nPriority;
						nModId = pWpn->aModIds[i];
					}
				}
			}
		}
	}


	return nModId;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::GetAmmoCount
//
//	PURPOSE:	Get the ammo count for the passed in ammo id
//
// --------------------------------------------------------------------------- //

uint32 CSAPlayerStats::GetAmmoCount(uint8 nAmmoId) const
{
	 if (!m_pnAmmo || !g_pWeaponMgr->IsValidAmmoId(nAmmoId))  return 0;

#ifdef USE_CRYPT_KEY
	 return (m_pnAmmo[nAmmoId]^m_nCryptKey);
#else
	 return m_pnAmmo[nAmmoId];	 
#endif
}

uint32 CSAPlayerStats::GetCurrentAmmoCount()
{
	if (!m_pnAmmo || m_nCurrentAmmo < 0 ||	m_nCurrentAmmo >= g_pWeaponMgr->GetNumAmmoIds())
		return 0;
#ifdef USE_CRYPT_KEY
	return (m_pnAmmo[m_nCurrentAmmo]^m_nCryptKey);
#else
	return m_pnAmmo[m_nCurrentAmmo];
#endif
}

void CSAPlayerStats::SetAmmoCount( uint8 nAmmoId, uint32 nAmmoCount )
{
#ifdef USE_CRYPT_KEY
	m_pnAmmo[nAmmoId] = (nAmmoCount^m_nCryptKey);
#else
	m_pnAmmo[nAmmoId] = nAmmoCount;
#endif
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::HaveWeapon
//
//	PURPOSE:	Do we have the weapon associated with the passed in id
//
// --------------------------------------------------------------------------- //

LTBOOL CSAPlayerStats::HaveWeapon(uint8 nWeaponId) const
{
     if (!m_pbHaveWeapon || !g_pWeaponMgr->IsValidWeaponId(nWeaponId)) return LTFALSE;

	 //-----------------------------------------------------------
	 // 광식
	 //-----------------------------------------------------------
	 if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
	 {
		 return LTTRUE;
	 }
	 //-----------------------------------------------------------

	 return m_pbHaveWeapon[nWeaponId];
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::CanUseWeapon
//
//	PURPOSE:	Can we use the weapon associated with the passed in id
//
// --------------------------------------------------------------------------- //

LTBOOL CSAPlayerStats::CanUseWeapon(uint8 nWeaponId) const
{
     if (!m_pbCanUseWeapon || !g_pWeaponMgr->IsValidWeaponId(nWeaponId) || !g_pWeaponMgr->IsPlayerWeapon(nWeaponId)) return LTFALSE;

	 //-----------------------------------------------------------
	 // 광식
	 //-----------------------------------------------------------
	 if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
	 {
		 return LTTRUE;
	 }
	 //-----------------------------------------------------------

	 return m_pbCanUseWeapon[nWeaponId];
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::HaveMod
//
//	PURPOSE:	Do we have the mod associated with the passed in id
//
// --------------------------------------------------------------------------- //

LTBOOL CSAPlayerStats::HaveMod(uint8 nModId) const
{
     if (!m_pbHaveMod || !g_pWeaponMgr->IsValidModId(nModId)) return LTFALSE;

	 return m_pbHaveMod[nModId];
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::CanUseMod
//
//	PURPOSE:	Can we use the mod associated with the passed in id
//
// --------------------------------------------------------------------------- //

LTBOOL CSAPlayerStats::CanUseMod(uint8 nModId) const
{
     if (!m_pbCanUseMod || !g_pWeaponMgr->IsValidModId(nModId)) return LTFALSE;

	 return m_pbCanUseMod[nModId];
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::HaveGear
//
//	PURPOSE:	Do we have the Gear associated with the passed in id
//
// --------------------------------------------------------------------------- //

LTBOOL CSAPlayerStats::HaveGear(uint8 nGearId) const
{
     if (!m_pbHaveGear || !g_pWeaponMgr->IsValidGearId(nGearId)) return LTFALSE;

	 return m_pbHaveGear[nGearId];
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::CanUseGear
//
//	PURPOSE:	Can we use the Gear associated with the passed in id
//
// --------------------------------------------------------------------------- //

LTBOOL CSAPlayerStats::CanUseGear(uint8 nGearId) const
{
     if (!m_pbCanUseGear || !g_pWeaponMgr->IsValidGearId(nGearId)) return LTFALSE;

	 return m_pbCanUseGear[nGearId];
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::CanUseAmmo
//
//	PURPOSE:	Can we use the ammo associated with the passed in id
//
// --------------------------------------------------------------------------- //

LTBOOL CSAPlayerStats::CanUseAmmo(uint8 nAmmoId) const
{
     if (!m_pbCanUseAmmo || !g_pWeaponMgr->IsValidAmmoId(nAmmoId)) return LTFALSE;

	 return m_pbCanUseAmmo[nAmmoId];
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::AddCanUseWeapon
//
//	PURPOSE:	Add a weapon to our can use list
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::AddCanUseWeapon(uint8 nWeaponId)
{
	 if (!m_pbCanUseWeapon || !g_pWeaponMgr->IsValidWeaponId(nWeaponId) || !g_pWeaponMgr->IsPlayerWeapon(nWeaponId)) return;

     m_pbCanUseWeapon[nWeaponId] = LTTRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::AddCanUseMod
//
//	PURPOSE:	Add a mod to our can use list
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::AddCanUseMod(uint8 nModId)
{
	 if (!m_pbCanUseMod || !g_pWeaponMgr->IsValidModId(nModId)) return;

     m_pbCanUseMod[nModId] = LTTRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::AddCanUseGear
//
//	PURPOSE:	Add a Gear to our can use list
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::AddCanUseGear(uint8 nGearId)
{
	 if (!m_pbCanUseGear || !g_pWeaponMgr->IsValidGearId(nGearId)) return;

     m_pbCanUseGear[nGearId] = LTTRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::AddCanUseAmmo
//
//	PURPOSE:	Add the ammo to our can use list
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::AddCanUseAmmo(uint8 nAmmoId)
{
	 if (!m_pbCanUseAmmo || !g_pWeaponMgr->IsValidAmmoId(nAmmoId)) return;

     m_pbCanUseAmmo[nAmmoId] = LTTRUE;
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::Setup
//
//	PURPOSE:	Setup the stats
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::Setup( )
{
	if (!m_pnAmmo || !m_pbHaveWeapon || !m_pbCanUseAmmo || !m_pbCanUseWeapon) return;

}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::ResetInventory
//
//	PURPOSE:	Reset all inventory items
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::ResetInventory()
{
	// Clear our data...
	
	g_pPlayerMgr->GetClientWeaponMgr()->ResetWeapons();

	int nNumAmmoTypes = g_pWeaponMgr->GetNumAmmoIds();
	if (nNumAmmoTypes > 0)
	{
        memset(m_pnAmmo, 0, sizeof(uint32) * nNumAmmoTypes);
        memset(m_pbHaveAmmo, 0, sizeof(LTBOOL) * nNumAmmoTypes);
        memset(m_pbCanUseAmmo, 0, sizeof(LTBOOL) * nNumAmmoTypes);
	}

	int nNumWeapons = g_pWeaponMgr->GetNumWeapons();
	if (nNumWeapons > 0)
	{
        memset(m_pbHaveWeapon, 0, sizeof(LTBOOL) * nNumWeapons);
        memset(m_pbCanUseWeapon, 0, sizeof(LTBOOL) * nNumWeapons);
	}

	int nNumMods = g_pWeaponMgr->GetNumModIds();
	if (nNumMods > 0)
	{
		memset(m_pbHaveMod, 0, sizeof(LTBOOL) * nNumMods);
		memset(m_pbCanUseMod, 0, sizeof(LTBOOL) * nNumMods);
	}

	int nNumGear = g_pWeaponMgr->GetNumGearIds();
	if (nNumGear > 0)
	{
		memset(m_pbHaveGear, 0, sizeof(LTBOOL) * nNumGear);
		memset(m_pbCanUseGear, 0, sizeof(LTBOOL) * nNumGear);
	}

	m_Keys.Clear();
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::DropInventory
//
//	PURPOSE:	Removes all currently carried weapons and ammo, optionally
//				removes gear and mods
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::DropInventory(LTBOOL bRemoveGear)
{
	// Clear our data...

#ifdef USE_CRYPT_KEY
	//상위 2바이트는 현재 틱의 하위 2바이트
	//하위 2바이트는 랜덤한 숫자로 키를 만든다.
	
	uint32	nKey1 = GetTickCount();
	uint32	nKey2 = GetRandom( 0, 65535 );
	nKey1 = nKey1 << 16;
	m_nCryptKey = nKey1 | nKey2;
#endif

	g_pPlayerMgr->GetClientWeaponMgr()->ResetWeapons();

	int nNumAmmoTypes = g_pWeaponMgr->GetNumAmmoIds();
	if (nNumAmmoTypes > 0)
	{
        memset(m_pnAmmo, 0, sizeof(uint32) * nNumAmmoTypes);
        memset(m_pbHaveAmmo, 0, sizeof(LTBOOL) * nNumAmmoTypes);
	}

	int nNumWeapons = g_pWeaponMgr->GetNumWeapons();
	if (nNumWeapons > 0)
	{
        memset(m_pbHaveWeapon, 0, sizeof(LTBOOL) * nNumWeapons);
	}

	if (bRemoveGear)
	{
		int nNumMods = g_pWeaponMgr->GetNumModIds();
		if (nNumMods > 0)
		{
			memset(m_pbHaveMod, 0, sizeof(LTBOOL) * nNumMods);
		}

		int nNumGear = g_pWeaponMgr->GetNumGearIds();
		if (nNumGear > 0)
		{
			memset(m_pbHaveGear, 0, sizeof(LTBOOL) * nNumGear);
		}

		m_Keys.Clear();

	}
	
	//[MURSUM]
	// Load Default Ammo
	// IC_WEAPON_OBTAIN 을 여기서 대신함.

	MISSION* pMission = g_pMissionButeMgr->GetMission( g_pMissionMgr->GetCurrentMission() );
	if( !pMission ) return;
	
	int i;
	int nWeaponId;

	for( i=0 ; i< pMission->nNumDefaultWeapons ; ++i )
	{
		nWeaponId = pMission->aDefaultWeapons[i];
		WEAPON const *pWeaponData = g_pWeaponMgr->GetWeapon(nWeaponId);
		if( !pWeaponData ) continue;
		AMMO const *pAmmoData = g_pWeaponMgr->GetAmmo(pWeaponData->nDefaultAmmoId);
		if( !pAmmoData ) continue;

		UpdateAmmo( nWeaponId, pWeaponData->nDefaultAmmoId, pAmmoData->nSelectionAmount, LTTRUE, LTFALSE );
	}
	
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::Skills
//
//	PURPOSE:	Reset all skill data
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::ResetSkills()
{
	// Clear our data...
	m_nTotalSkillPoints = 0;
	m_nAvailSkillPoints = 0;
	memset(m_nSkills, 0, sizeof(m_nSkills));


}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::Objectives
//
//	PURPOSE:	Reset all Objective data
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::ResetObjectives()
{
	// Clear our data...
	m_Objectives.Clear();
	m_OptionalObjectives.Clear();
	m_CompletedObjectives.Clear();
	m_Parameters.Clear();

	if (g_pHUDMgr)
	{
		g_pHUDMgr->QueueUpdate(kHUDObjectives);
	}


}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::RestMissionStats
//
//	PURPOSE:	Reset all mission data
//
// --------------------------------------------------------------------------- //

void CSAPlayerStats::ResetMissionStats()
{
	// Clear our data...
	m_MissionStats.Init();
}

void CSAPlayerStats::ClearMissionInfo()
{
	ResetMissionStats();
	ResetObjectives();
	m_nDamage = 0;
	m_Keys.Clear();
	m_IntelList.Clear();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerStats::HaveAirSupply
//
//	PURPOSE:	Returns true if current gear provides air supply
//
// ----------------------------------------------------------------------- //

LTBOOL CSAPlayerStats::HaveAirSupply()
{
    LTBOOL bAir = LTFALSE;
    GEAR const *pGear = LTNULL;

	int numGear = g_pWeaponMgr->GetNumGearIds();
	for (int nGearId=0; nGearId < numGear && !bAir; nGearId++)
	{
		if (m_pbHaveGear[nGearId])
		{
			pGear = g_pWeaponMgr->GetGear(nGearId);
			if (pGear)
			{
				bAir = ( (pGear->eProtectionType == DT_CHOKE) && (pGear->fProtection >= 1.0f) );
			}
		}
	}
	return bAir;
}


//Skills and experience functions
const RANK* CSAPlayerStats::GetRank()
{
	return g_pSkillsButeMgr->GetRank(m_nTotalSkillPoints);
}

uint32  CSAPlayerStats::GetCostToUpgrade(eSkill skill)
{
	uint8 nTgtLevel = m_nSkills[skill] + 1;
	if (nTgtLevel < kNumSkills)
		return g_pSkillsButeMgr->GetCostToUpgrade(skill,(eSkillLevel)nTgtLevel);
	else
		return -1;
}


void CSAPlayerStats::SetObjectivesSeen()
{
}

float CSAPlayerStats::GetSkillModifier(eSkill skl, uint8 nMod)
{
	return g_pSkillsButeMgr->GetModifier(skl, (eSkillLevel)m_nSkills[skl], nMod);
}

