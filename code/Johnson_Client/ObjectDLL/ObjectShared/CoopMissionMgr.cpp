// ----------------------------------------------------------------------- //
//
// MODULE  : CoopMissionMgr.cpp
//
// PURPOSE : Implementation of class to handle coop missions
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#include "stdafx.h"
#include "CoopMissionMgr.h"
#include "MusicMgr.h"
#include "PlayerObj.h"
#include "MsgIds.h"
#include "ServerSaveLoadMgr.h"
#include "MissionButeMgr.h"

#ifdef USE_AI
extern CVarTrack g_vtDifficultyFactorPlayerIncrease;
#endif

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CCoopMissionMgr::StartGame
//
//	PURPOSE:	Default start game.
//
// --------------------------------------------------------------------------- //

bool CCoopMissionMgr::StartGame(ILTMessage_Read& msg )
{
	if( !g_pMissionButeMgr->Init( MISSION_COOP_FILE ))
		return false;

	if( !CServerMissionMgr::StartGame())
		return false;

	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );
	ServerMissionSettings sms = GetServerSettings();

	sms.m_bUseSkills = sgo.GetCoop().m_bUseSkills;
	sms.m_bFriendlyFire = sgo.GetCoop().m_bFriendlyFire;
	sms.m_nMPDifficulty = sgo.GetCoop().m_nDifficulty;
	sms.m_fPlayerDiffFactor = sgo.GetCoop().m_fPlayerDiffFactor;
	sms.m_nRunSpeed = 100;
	sms.m_nScoreLimit = 0;
	sms.m_nTimeLimit = 0;

	SetServerSettings(sms);

	g_pMusicMgr->Enable( );
	return true;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CCoopMissionMgr::EndGame
//
//	PURPOSE:	End the game.
//
// --------------------------------------------------------------------------- //

bool CCoopMissionMgr::EndGame( uint8 nWhy )
{
	// Call base.
	if( !CServerMissionMgr::EndGame( nWhy ))
		return false;

	return true;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::HandleMultiplayerOptions
//
//	PURPOSE:	Handle updating the host options
//
// --------------------------------------------------------------------------- //

bool CCoopMissionMgr::HandleMultiplayerOptions( HCLIENT hSender, ILTMessage_Read& msg )
{
	ServerMissionSettings sms = GetServerSettings();

	sms.m_bFriendlyFire = msg.Readbool( );
	sms.m_nMPDifficulty = msg.Readuint8( );
	sms.m_fPlayerDiffFactor = msg.Readfloat();

	SetServerSettings(sms);
	
	return true;
}
