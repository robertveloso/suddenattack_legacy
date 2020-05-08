// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerMgr.cpp
//
// PURPOSE : Implementation of class to handle managment of missions and worlds.
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#include "stdafx.h"
#include "SAPlayerMgr.h"
#include "SAHUDMgr.h"
#include "MsgIDs.h"
#include "GameClientShell.h"
#include "PlayerCamera.h"
#include "TargetMgr.h"
#include "WeaponMgr.h"





// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerMgr::CSAPlayerMgr
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CSAPlayerMgr::CSAPlayerMgr() : CPlayerMgr()
{
	m_nFlashlightID = WMGR_INVALID_ID;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAPlayerMgr::~CSAPlayerMgr
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CSAPlayerMgr::~CSAPlayerMgr()
{
}


LTBOOL CSAPlayerMgr::Init()
{
	const WEAPON* pWeapon = g_pWeaponMgr->GetWeapon("SA_Knife"); //Li_Knife"); //Li_RKnife
//	const WEAPON* pWeapon = g_pWeaponMgr->GetWeapon("Keychain");
	m_nFlashlightID = pWeapon->nId;
	return CPlayerMgr::Init();
}


LTBOOL CSAPlayerMgr::OnCommandOn(int command)
{
	// Make sure we're in the world...

	if (!IsPlayerInWorld()) return LTFALSE;

	// Take appropriate action

	switch (command)
	{
		case COMMAND_ID_FLASHLIGHT:
		{
			/*
			// ±¤½Ä
			//-------------------------------------------------------------
			if( g_pPlayerMgr->IsPlayerDead() && IsRevivePlayerGameType() )
				return LTTRUE;
			//-------------------------------------------------------------

			if (g_pPlayerStats->GetCurrentWeapon() == m_nFlashlightID)
				LastWeapon();
			else
				ChangeWeapon(m_nFlashlightID);
			*/
		} break;
	};


	return CPlayerMgr::OnCommandOn(command);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::OnEnterWorld()
//
//	PURPOSE:	Handle entering world
//
// ----------------------------------------------------------------------- //

void CSAPlayerMgr::OnEnterWorld()
{
	CPlayerMgr::OnEnterWorld( );

	// Check if they want radar on.
	if( g_pGameClientShell->ShouldUseRadar( ) )
	{
		g_pCompass->SetDraw( true );
		g_pHUDTimer->SetDraw( true );
		g_pRadar->SetDraw( true );		
	}
	
}
