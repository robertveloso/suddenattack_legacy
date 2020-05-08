// ----------------------------------------------------------------------- //
//
// MODULE  : TeamDeathMatchMissionMgr.cpp
//
// PURPOSE : Implementation of class to handle team deathmatch missions
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

//
// Includes...
//

	#include "stdafx.h"
	#include "TeamDeathMatchMissionMgr.h"
	#include "TeamMgr.h"
	#include "MissionButeMgr.h"

// Added By Bestline ==>
// Time : 2004-06-11 오후 4:40:55
#include "PlayerObj.h"
// Added By Bestline <==

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTeamDeathMatchMissionMgr::CTeamDeathMatchMissionMgr
//
//	PURPOSE:	Constructor...
//
// ----------------------------------------------------------------------- //

CTeamDeathMatchMissionMgr::CTeamDeathMatchMissionMgr()
:	CDeathMatchMissionMgr	()
{

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTeamDeathMatchMissionMgr::~CTeamDeathMatchMissionMgr
//
//	PURPOSE:	Destructor...
//
// ----------------------------------------------------------------------- //

CTeamDeathMatchMissionMgr::~CTeamDeathMatchMissionMgr()
{
	Term( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTeamDeathMatchMissionMgr::Init
//
//	PURPOSE:	Init the mgr
//
// ----------------------------------------------------------------------- //

bool CTeamDeathMatchMissionMgr::Init()
{
	if( !CDeathMatchMissionMgr::Init() )
		return false;

	// Add the teams to the team mgr...

	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );

	uint8 nNumTeams = sgo.GetTeamDeathmatch().m_nNumTeams;

	// 광식 : Spectator Mode 제거 [ 원위치~]
// Edited By IN'T ==>
// Time : 2004-04-29 오전 9:45:46
 	for( uint8 i = 0; i < nNumTeams; i++ )
	{
		//CTeamMgr::Instance().AddTeam( sgo.GetTeamDeathmatch().m_sTeamName[i].c_str(),
		//							  sgo.GetTeamDeathmatch().m_nTeamModel[i] );
		CTeamMgr::Instance().AddTeam( sgo.GetTeamDeathmatch().m_sTeamName[i].c_str(),
									  i );
	}
 	//for( uint8 i = 0; i < nNumTeams+1 ; ++i )	// For Spectator
	//{
	//	if (i==0)	// Spectator
	//	{
	//		CTeamMgr::Instance().AddTeam("Spectator", i);
	//	}
	//	else
	//	{
	//		CTeamMgr::Instance().AddTeam( sgo.GetTeamDeathmatch().m_sTeamName[i-1].c_str(), i);
	//	}
	//}
// Edited By IN'T <==

	return !!CTeamMgr::Instance().GetNumTeams();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTeamDeathMatchMissionMgr::Update
//
//	PURPOSE:	Frame update.  Check for end level condition...
//
// ----------------------------------------------------------------------- //

void CTeamDeathMatchMissionMgr::Update()
{
	// Check the teams to see if any of them have reached the score limit...
	
	//[MURSUM]==============================================================
	//스코어 계산은 메인서버에서 한다.
	/*/
	if( m_ServerSettings.m_nScoreLimit > 0 && m_fStartTime >= 0.0f )
	{
		for( uint8 i = 0; i < CTeamMgr::Instance().GetNumTeams(); ++i )
		{
			CTeam *pTeam = CTeamMgr::Instance().GetTeam( i );
			if( pTeam && (pTeam->GetScore() >= m_ServerSettings.m_nScoreLimit) )
			{
				CTeamMgr::Instance( ).WonRound( i );
				g_pLTServer->CPrint( "ScoreLimit reached." );
				NextRound();
			}
		}
	}
	/*/
	//=======================================================================


	// Skip the CDeathMatchMissionMgr::Update() but be sure to call up...

	CServerMissionMgr::Update( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTeamDeathMatchMissionMgr::FinishExitLevel
//
//	PURPOSE:	Finish the exitlevel we started....
//
// ----------------------------------------------------------------------- //

bool CTeamDeathMatchMissionMgr::FinishExitLevel()
{
	if( !CDeathMatchMissionMgr::FinishExitLevel() )
		return false;

	return true;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CTeamDeathMatchMissionMgr::StartGame
//
//	PURPOSE:	Start the game.
//
// --------------------------------------------------------------------------- //

bool CTeamDeathMatchMissionMgr::StartGame()
{
	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );
	ServerMissionSettings sms = GetServerSettings();

	sms.m_bUseSkills = false;
	sms.m_bFriendlyFire = sgo.GetTeamDeathmatch().m_bFriendlyFire;
	sms.m_nScoreLimit = sgo.GetTeamDeathmatch().m_nScoreLimit;
	sms.m_nTimeLimit = sgo.GetTeamDeathmatch().m_nTimeLimit;
	sms.m_nRounds = sgo.GetTeamDeathmatch().m_nRounds;
	sms.m_nFragScore = sgo.GetTeamDeathmatch().m_nFragScore;
	sms.m_nTagScore = sgo.GetTeamDeathmatch().m_nTagScore;;

	SetServerSettings(sms);

	// Edited By IN'T ==>
	// 	if( !g_pMissionButeMgr->Init( MISSION_DM_FILE ))
	//		return false;
	// Edited By IN'T <==
	if( !g_pMissionButeMgr->Init( MISSION_DEFAULT_FILE ))
		return false;

	if( !CServerMissionMgr::StartGame())
		return false;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTeamDeathMatchMissionMgr::LevelStarted()
//
//	PURPOSE:	Called when the clients have finished loading and have started
//				playing a level
//
// ----------------------------------------------------------------------- //

void CTeamDeathMatchMissionMgr::LevelStarted()
{
	CServerMissionMgr::LevelStarted( );

	// Check if this is a new level, or just another round.
	if( m_bNewMission )
	{
		CTeamMgr::Instance( ).NewLevel( );
	}
	else
	{
		CTeamMgr::Instance( ).NewRound( );
	}
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::HandleMultiplayerOptions
//
//	PURPOSE:	Handle updating the host options
//
// --------------------------------------------------------------------------- //

bool CTeamDeathMatchMissionMgr::HandleMultiplayerOptions( HCLIENT hSender, ILTMessage_Read& msg )
{

	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );
	sgo.GetTeamDeathmatch().Read(&msg);

	ServerMissionSettings sms = GetServerSettings();

	sms.m_bUseSkills = false;
	sms.m_bFriendlyFire = sgo.GetTeamDeathmatch().m_bFriendlyFire;
	sms.m_nScoreLimit = sgo.GetTeamDeathmatch().m_nScoreLimit;
	sms.m_nTimeLimit = sgo.GetTeamDeathmatch().m_nTimeLimit;
	sms.m_nRounds = sgo.GetTeamDeathmatch().m_nRounds;
	sms.m_nFragScore = sgo.GetTeamDeathmatch().m_nFragScore;
	sms.m_nTagScore = sgo.GetTeamDeathmatch().m_nTagScore;;

	SetServerSettings(sms);

	return true;

}

bool CTeamDeathMatchMissionMgr::EndGame( uint8 nWhy )
{
	if( !CServerMissionMgr::EndGame( nWhy ))	return false;

	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_END_GAME );
	cMsg.Writeuint8( nWhy );
	g_pLTServer->SendToClient( cMsg.Read(), g_pGameServerShell->GetHostPlayerHandle(), MESSAGE_GUARANTEED );
	return true;
}


