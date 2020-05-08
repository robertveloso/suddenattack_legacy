// ----------------------------------------------------------------------- //
//
// MODULE  : ServerMissionMgr.cpp
//
// PURPOSE : Implementation of class to handle managment of missions and worlds.
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#include "stdafx.h"
#include "ServerMissionMgr.h"
#include "MsgIds.h"
#include "WinUtil.h"
#include "MissionButeMgr.h"
#include "ServerSaveLoadMgr.h"
#include "PlayerObj.h"
#include "DisplayTimer.h"
#include "Spawner.h"
#include "WorldProperties.h"

CServerMissionMgr* g_pServerMissionMgr = LTNULL;

#ifdef USE_AI
extern CVarTrack g_vtDifficultyFactorPlayerIncrease;
#endif

// --------------------------------------------------------------------------- //
// Constructor & Destructor
// --------------------------------------------------------------------------- //
CServerMissionMgr::CServerMissionMgr()
{
	m_bCustomLevel = false;
    m_nCurrentMission = -1;
    m_nCurrentLevel = -1;
	m_nCurrentRound	= 0;
	m_nCurCampaignIndex = -1;

	m_bTransitionLevels = false;
	m_bNewMission = false;
	m_bExitingLevel = false;
	m_bPlayerTrackerAborted = false;


	m_bMissionFailed = false;
	m_bLoopCampaign = false;

	m_fStartTime		= -1.0f;
	m_bNetFriendlyFire	= false;
}

CServerMissionMgr::~CServerMissionMgr()
{
	Term( );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::Init
//
//	PURPOSE:	Init the mgr
//
// ----------------------------------------------------------------------- //
bool CServerMissionMgr::Init()
{
	g_pServerMissionMgr = this;
	m_bTransitionLevels = false;
	m_bCustomLevel  = false;
	m_nCurrentMission = -1;
	m_nCurCampaignIndex = -1;
	m_nCurrentLevel = -1;
	m_nCurrentRound = 0;
	m_sCurrentWorldName.Empty( );
	m_sCampaignFile.Empty( );
	m_bNewMission = false;
	m_bExitingLevel = false;
	m_bPlayerTrackerAborted = false;
	m_bMissionFailed = false;
	// 광식
	//for( int i = 0; i < MAX_MANAGED_WEAPON; i++ )
	//{
	//	m_hManaged_Weapon[i] = LTNULL;
	//}
	memset(m_hManaged_Weapon, LTNULL, sizeof(HOBJECT)*MAX_MANAGED_WEAPON);
	return true;	
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::Term
//
//	PURPOSE:	Term the mgr
//
// ----------------------------------------------------------------------- //
void CServerMissionMgr::Term()
{
	g_pServerMissionMgr = LTNULL;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::GetLevelFromMission
//
//	PURPOSE:	Get the level name from mission info.
//
// --------------------------------------------------------------------------- //

char const* CServerMissionMgr::GetLevelFromMission( int nMission, int nLevel )
{

	MISSION* pMission = g_pMissionButeMgr->GetMission( nMission );
	if( !pMission )
	{
		ASSERT( !"CServerMissionMgr::GetLevelFromMission: Invalid mission." );
		return NULL;
	}

	return pMission->szMap;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::SetMissionBasedOnLevel
//
//	PURPOSE:	Sets mission info based on level.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::SetMissionBasedOnLevel( char const* pszFilename )
{
	// Check inputs.
	if( !pszFilename || !pszFilename[0] )
	{
		ASSERT( !"CServerMissionMgr::SetMissionBasedOnLevel: Invalid filename." );
		return false;
	}

	// See if the loaded world is a custom level or not...
	int nMissionId, nLevel;
	if( g_pMissionButeMgr->IsMissionLevel(( char* )pszFilename, nMissionId, nLevel ))
	{
		m_nCurrentMission	= nMissionId;
		m_nCurrentLevel		= nLevel;
		m_bCustomLevel		= false;

		MISSION* pMission = g_pMissionButeMgr->GetMission( m_nCurrentMission );
		if( pMission )
		{
			m_nWalkSpeed		= pMission->nWalkSpeed;
			m_nRunSpeed			= pMission->nRunSpeed;
			m_nJumpSpeed		= pMission->nJumpSpeed;
			m_nSuperJumpSpeed	= pMission->nSuperJumpSpeed;
			m_nSwimSpeed		= pMission->nSwimSpeed;
			m_nLadderSpeed		= pMission->nLadderSpeed;
		}
		else
		{
			m_nWalkSpeed		= -1;
			m_nRunSpeed			= -1;
			m_nJumpSpeed		= -1;
			m_nSuperJumpSpeed	= -1;
			m_nSwimSpeed		= -1;
			m_nLadderSpeed		= -1;
		}
	}
	else
	{
		m_nCurrentMission	= -1;
		m_nCurrentLevel		= -1;
        m_bCustomLevel		= true;
	}
	
	// Save our world name.
	m_sCurrentWorldName = pszFilename;

	return true;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::FindNextCampaignIndex
//
//	PURPOSE:	Finds the next occurrance of the mission in the campaign.
//
// --------------------------------------------------------------------------- //

int CServerMissionMgr::FindNextCampaignIndex( int nStartingCampaignIndex, int nMissionIndex )
{
	int nCampaignIndex = -1;

	// Check inputs.
	if( nMissionIndex < 0 || nMissionIndex >= g_pMissionButeMgr->GetNumMissions( ))
		return -1;

	// Starting from a campaign index, find the next occurance of the mission.
	for( int nIndex = nStartingCampaignIndex + 1; nIndex < ( int )m_Campaign.size( ); nIndex++ )
	{
		if( m_Campaign[nIndex] == nMissionIndex )
		{
			return nIndex;
		}
	}

	return -1;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::Save()
//
//	PURPOSE:	Saves state of misionmgr.
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::Save( ILTMessage_Write& msg, uint32 dwSaveFlags )
{
	msg.WriteString( m_sCampaignFile );

	msg.Writeuint8( m_Campaign.size( ));
	for( Campaign::iterator iter = m_Campaign.begin( ); iter != m_Campaign.end( ); iter++ )
	{
		msg.Writeuint8( *iter );
	}
	msg.Writeuint8( m_nCurCampaignIndex );

	msg.Writebool(m_ServerSettings.m_bUseSkills);
	msg.Writebool(m_ServerSettings.m_bFriendlyFire);
	msg.Writeuint8(m_ServerSettings.m_nMPDifficulty);
	msg.Writefloat(m_ServerSettings.m_fPlayerDiffFactor);


	return true;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::Load()
//
//	PURPOSE:	Loads state of misionmgr.
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::Load( ILTMessage_Read& msg, uint32 dwSaveFlags )
{
	msg.ReadString( m_sCampaignFile.GetBuffer( MAX_PATH ), MAX_PATH );
	m_sCampaignFile.ReleaseBuffer( );

	m_Campaign.clear( );
	int nNumCampaignEntries = msg.Readuint8( );
	for( int nCampaignIndex = 0; nCampaignIndex < nNumCampaignEntries; ++nCampaignIndex )
	{
		m_Campaign.push_back( msg.Readuint8( ));
	}
	m_nCurCampaignIndex = msg.Readuint8( );

	ServerMissionSettings ss = m_ServerSettings;
	ss.m_bUseSkills = msg.Readbool();
	ss.m_bFriendlyFire = msg.Readbool();
	ss.m_nMPDifficulty = msg.Readuint8();
	ss.m_fPlayerDiffFactor = msg.Readfloat();
	SetServerSettings(ss);

	

	return true;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::OnMessage
//
//	PURPOSE:	Handle missionmgr messages.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::OnMessage( HCLIENT hSender, ILTMessage_Read& msg )
{
	msg.SeekTo(0);
	uint8 messageID = msg.Readuint8();

	switch (messageID)
	{
		case MID_START_GAME:			HandleStartGame			( hSender, msg );	return true;
		case MID_START_LEVEL:			HandleStartLevel		( hSender, msg );	return true;
		case MID_EXIT_LEVEL:			HandleExitLevel			( hSender, msg );	return true;
		case MID_MULTIPLAYER_OPTIONS:	HandleMultiplayerOptions( hSender, msg );	return true;
		case MID_RESTART_LEVEL:			HandleRestartLevel		( hSender, msg );	return true;
		case MID_END_LEVEL:				HandleEndLevel			( hSender, msg );	return true;

		default:
			break;
	}

	return false;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::HandleStartGame
//
//	PURPOSE:	Handle start game message.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::HandleStartGame( HCLIENT hSender, ILTMessage_Read& msg )
{
	// Initialize the bute file the client wants.
	ServerGameOptions const& serverGameOptions = g_pGameServerShell->GetServerGameOptions( );
	m_sCampaignFile = serverGameOptions.GetCampaignName();

	ServerMissionSettings ss = m_ServerSettings;
	ss.m_nMPDifficulty = (uint8)g_pGameServerShell->GetDifficulty();
	SetServerSettings(ss);

	// Start the game.
	if( !StartGame())
		return false;
	
	return true;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::HandleStartLevel
//
//	PURPOSE:	Start a level from scratch.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::HandleStartLevel( HCLIENT hSender, ILTMessage_Read& msg )
{
	char szStartLevel[MAX_PATH];

	msg.ReadString( szStartLevel, ARRAY_LEN( szStartLevel ));

	// Starting level from scratch, so this is a new mission for us.
	m_bNewMission = true;

	// Setup mission based on level.
	if( !SetMissionBasedOnLevel( szStartLevel ))
		return false;

	// Setup the campaign index.
	if( !m_bCustomLevel )
		m_nCurCampaignIndex = FindNextCampaignIndex( -1, m_nCurrentMission );

	// Load the level.
	if( !g_pServerSaveLoadMgr->LoadNewLevel( szStartLevel ))
		return false;
	
	return true;
}

bool CServerMissionMgr::ClientStart( const char* lpszWorldName )
{
	// Initialize the bute file the client wants.
	char szStartLevel[MAX_PATH];
	strcpy( szStartLevel, lpszWorldName );
	szStartLevel[strlen(lpszWorldName)-4] = LTNULL;
	
	ServerGameOptions const& serverGameOptions = g_pGameServerShell->GetServerGameOptions( );
	m_sCampaignFile = serverGameOptions.GetCampaignName();

	ServerMissionSettings ss = m_ServerSettings;
	ss.m_nMPDifficulty = (uint8)g_pGameServerShell->GetDifficulty();
	SetServerSettings(ss);

	// Start the game.
	if( !StartGame()) return false;

	// Starting level from scratch, so this is a new mission for us.
	m_bNewMission = true;

	// Setup mission based on level.
	if( !SetMissionBasedOnLevel( szStartLevel ))
		return false;

	// Setup the campaign index.
	if( !m_bCustomLevel )
		m_nCurCampaignIndex = FindNextCampaignIndex( -1, m_nCurrentMission );

	// Load the level.
	if( !g_pServerSaveLoadMgr->LoadNewLevel( szStartLevel ))
		return false;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::LevelStarted()
//
//	PURPOSE:	Called when the clients have finished loading and have started
//				playing a level
//
// ----------------------------------------------------------------------- //

void CServerMissionMgr::LevelStarted()
{
	//[MURSUM] m_fStartTime = g_pLTServer->GetTime();
	g_pLTServer->CPrint( "CServerMissionMgr::LevelStarted" );
	m_fStartTime = 0.0f;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::ExitLevelSwitch()
//
//	PURPOSE:	Called when it's time to go to the next level in the mission.
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::ExitLevelSwitch( )
{
	TRACE( "CServerMissionMgr::ExitLevelSwitch\n" );

	// Check if we're already exiting a level.
	if( m_bExitingLevel )
		return true;

	// If this was a custom level, then the game is over.
	if( m_bCustomLevel )
	{
		// Let the endgame code figure out what we should do.
		if( !EndGame( EG_NORMAL ))
			return false;

		return true;
	}

	// Get the mission.
	MISSION* pMission = g_pMissionButeMgr->GetMission(m_nCurrentMission);
	if( !pMission )
	{
        ASSERT( !"CServerMissionMgr::ExitLevelSwitch:  Invalid mission." );
		return false;
	}

	// Go to the next level.
	m_nCurrentLevel++;
	m_nCurrentRound = 0;

	// Default to not a new mission.
	m_bNewMission = false;

	// See if we finished a mission.
	//if( m_nCurrentLevel >= pMission->nNumLevels )
	{
		// Start level index over and advance the mission index.
		m_nCurrentLevel = 0;
		m_nCurCampaignIndex++;

		// See if we reached the end of our campaign.
		if( m_nCurCampaignIndex >= ( int )m_Campaign.size( ))
		{
			// Let the endgame code figure out what we should do.
			if( !EndGame( EG_NORMAL ))
				return false;

			return true;
		}

		// Get the missionid from the campaign.
		m_nCurrentMission = m_Campaign[m_nCurCampaignIndex];

		// Flag that this is a new mission so we can tell everyone else.
		m_bNewMission = true;
	}

	// Get the worldname from the mission/level indices.
	char const* pszFilename = GetLevelFromMission( m_nCurrentMission, m_nCurrentLevel );
	if( !pszFilename )
		return false;

	// Do the exit level.
	if( !ExitLevel( pszFilename, false ))
		return false;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::NextMission()
//
//	PURPOSE:	Called to force transition to the next mission.
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::NextMission( )
{
	TRACE( "CServerMissionMgr::NextMission\n" );

	// Check if we're already exiting a level.
	if( m_bExitingLevel )
		return true;

	// If this was a custom level, then the game is over.
	if( m_bCustomLevel )
	{
		// Let the endgame code figure out what we should do.
		if( !EndGame( EG_NORMAL ))
			return false;

		return true;
	}

	// Get the mission.
	MISSION* pMission = g_pMissionButeMgr->GetMission(m_nCurrentMission);
	if( !pMission )
	{
        ASSERT( !"CServerMissionMgr::NextMission:  Invalid mission." );
		return false;
	}

	// Start level index over and advance the mission index.
	m_nCurrentLevel = 0;
	m_nCurrentRound = 0;
	m_nCurCampaignIndex++;

	// See if we reached the end of our campaign.
	if( m_nCurCampaignIndex >= ( int )m_Campaign.size( ))
	{
		// Let the endgame code figure out what we should do.
		if( !EndGame( EG_NORMAL ))
			return false;

		return true;
	}

	// Get the missionid from the campaign.
	m_nCurrentMission = m_Campaign[m_nCurCampaignIndex];

	// Flag that this is a new mission so we can tell everyone else.
	m_bNewMission = true;

	// Get the worldname from the mission/level indices.
	char const* pszFilename = GetLevelFromMission( m_nCurrentMission, m_nCurrentLevel );
	if( !pszFilename )
		return false;

	// Do the exit level.
	if( !ExitLevel( pszFilename, false ))
		return false;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::NextRound()
//
//	PURPOSE:	Called to force switch to next round, if on last round will call NextMission()...
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::NextRound( )
{
	TRACE( "CServerMissionMgr::NextRound\n" );	

	// Check if we're already exiting a level.
	
	if( m_bExitingLevel )
		return true;

	// Get the mission...

	MISSION* pMission = g_pMissionButeMgr->GetMission(m_nCurrentMission);
	if( !pMission )
	{
        ASSERT( !"CServerMissionMgr::NextMission:  Invalid mission." );
		return false;
	}

	// Advance to the next round...
	
	++m_nCurrentRound;

	// Default to not a new mission.
	m_bNewMission = false;

	// See if we finished the rounds for this level...
	
 	if( m_nCurrentRound >= m_ServerSettings.m_nRounds )
	{
		// Just go to the next mission...

		if( !NextMission() )
			return false;

		return true;
	}

	if( !ExitLevel( m_sCurrentWorldName, false ))
		return false;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::SwitchToCampaignIndex
//
//	PURPOSE:	Called to force transition to the specific mission within campaign.
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::SwitchToCampaignIndex( int nCampaignIndex )
{
	TRACE( "CServerMissionMgr::SwitchToCampaignIndex\n" );

	m_nCurCampaignIndex = nCampaignIndex - 1;
	return NextMission( );
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::ExitLevelToLevel()
//
//	PURPOSE:	Called when you want to go to any arbitrary level.
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::ExitLevelToLevel( char const* pszNewLevel )
{
	TRACE( "CServerMissionMgr::ExitLevelToLevel\n" );

	// Check if we're already exiting a level.
	if( m_bExitingLevel )
		return true;

	// Verify inputs.
	if( !pszNewLevel )
	{
		ASSERT( !"CServerMissionMgr::ExitLevelToLevel: Invalid inputs." );
		return false;
	}

	// Default to this not being a mission change.
	m_bNewMission = false;

	// Check if the new level is a new mission.
	int nCurMission = m_nCurrentMission;
	if( !SetMissionBasedOnLevel( pszNewLevel ))
		return false;

	// If we switch to a custom level, then consider it a new mission.
	if( m_bCustomLevel )
	{
		m_bNewMission = true;
		m_nCurCampaignIndex = -1;
	}
	// If it's not a custom world, check if our mission changed.
	else
	{
		m_bNewMission = ( nCurMission != m_nCurrentMission );

		// If we had a mission switch, advance our campaign index.
		if( m_bNewMission )
		{
			// See if we can find the campaign index from our previous position.
			m_nCurCampaignIndex = FindNextCampaignIndex( m_nCurCampaignIndex, m_nCurrentMission );
		}
	}

	// Do the exit level.
	if( !ExitLevel( pszNewLevel, false ))
		return false;

	return true;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::ExitLevelToSavedGame()
//
//	PURPOSE:	Called before you load a game to notify clients
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::ExitLevelToSavedGame( char const* pszNewLevel )
{
	TRACE( "CServerMissionMgr::ExitLevelToSavedGame\n" );

	// Check if we're already exiting a level.
	if( m_bExitingLevel )
		return true;

	// Verify inputs.
	if( !pszNewLevel )
	{
		ASSERT( !"CServerMissionMgr::ExitLevelToSavedGame: Invalid inputs." );
		return false;
	}

	// Default to this not being a mission change.
	m_bNewMission = false;

	// Check if the new level is a new mission.
	int nCurMission = m_nCurrentMission;
	if( !SetMissionBasedOnLevel( pszNewLevel ))
		return false;

	// If we switch to a custom level, then consider it a new mission.
	if( m_bCustomLevel )
	{
		m_bNewMission = true;
		m_nCurCampaignIndex = -1;
	}
	// If it's not a custom world, check if our mission changed.
	else
	{
		m_bNewMission = ( nCurMission != m_nCurrentMission );

		// If we had a mission switch, advance our campaign index.
		if( m_bNewMission )
		{
			// See if we can find the campaign index from our previous position.
			m_nCurCampaignIndex = FindNextCampaignIndex( m_nCurCampaignIndex, m_nCurrentMission );
		}
	}

	// Tell clients we're exiting. Don't wait for their response.
	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_EXIT_LEVEL );
	cMsg.WriteString( m_sCurrentWorldName );
	cMsg.Writebool( m_bNewMission );
	cMsg.Writebool( false );
	cMsg.Writebool( false );
	g_pLTServer->SendToClient(cMsg.Read(), LTNULL, MESSAGE_GUARANTEED);

	return true;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::ExitLevelTransition()
//
//	PURPOSE:	Called when level transition is needed.
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::ExitLevelTransition( int nNewLevel )
{
	TRACE( "CServerMissionMgr::ExitLevelTransition\n" );

	// Check if we're already exiting a level.
	if( m_bExitingLevel )
		return true;
	
	// Default to this not being a mission change.
	m_bNewMission = false;

	// Get the next level name.
	char const* pszFilename = GetLevelFromMission( m_nCurrentMission, nNewLevel );
	if( !pszFilename )
		return false;

	// Do the exit level.
	if( !ExitLevel( pszFilename, true ))
		return false;

	return true;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::ExitLevel()
//
//	PURPOSE:	Called by exitlevelXXX routines to go to a new level.
//
// ----------------------------------------------------------------------- //

bool CServerMissionMgr::ExitLevel( char const* pszNewLevel, bool bTransitionToLevel )
{
	TRACE( "CServerMissionMgr::ExitLevel\n" );

	// Verify inputs.
	if( !pszNewLevel )
	{
		ASSERT( !"CServerMissionMgr::ExitLevel: Invalid inputs." );
		return false;
	}

	// Check if we're already exiting a level.
	if( m_bExitingLevel )
		return true;

	// Make sure there is a worldproperties object.
	if (!g_pWorldProperties)
	{
		ASSERT( !"CServerMissionMgr::ExitLevel:  Could not find worldproperties." );
		return false;
	}
	
	// Tell worldproperties to send its level end command.
	g_pWorldProperties->SendLevelEndCmd( );

	// Record that we are transitioning.
	m_bTransitionLevels = bTransitionToLevel;

	// Make sure mission information is setup for the new level.  The
	// campaign index should have already been updated.
	if( !SetMissionBasedOnLevel( pszNewLevel ))
		return false;

	// Initialize the PlayerTracker to the current set of players.
	m_bPlayerTrackerAborted = false;
	if( !m_PlayerTracker.Init( *this ))
	{
		// No players, just go to the next level.
		return FinishExitLevel( );
	}

	// Tell the client.
	if( !SendExitLevelMessage( ))
		return false;

	// Pause the server while we switch levels.
	g_pGameServerShell->PauseGame( LTTRUE );

	// We're now waiting for exitlevel messages back from client.
	m_bExitingLevel = true;

	//clear out the start time
	m_fStartTime = -1.0f;

	return true;
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::HandleExitLevel
//
//	PURPOSE:	Handle the client's response to exit level.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::HandleExitLevel( HCLIENT hSender, ILTMessage_Read& msg )
{
	TRACE( "CServerMissionMgr::HandleExitLevel: hSender(0x%X)\n", hSender );

	// Get the playerobj for this client.
    void *pData = g_pLTServer->GetClientUserData(hSender);
	CPlayerObj* pPlayerObjSender = (CPlayerObj*)pData;
	if( !pPlayerObjSender )
	{
		ASSERT( !"CServerMissionMgr::HandleExitLevel: Invalid playerobj sender." );
		return false;
	}

	// Remove this player from the tracker.
	m_PlayerTracker.RemoveClient( hSender );

	// Check if we are still waiting for some clients to finish their exiting procedures.
	if( !m_PlayerTracker.IsEmpty( ))
		return true;

	return FinishExitLevel( );
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::OnPlayerTrackerAbort
//
//	PURPOSE:	Handle when we have to abort a playertracker.
//
// --------------------------------------------------------------------------- //

void CServerMissionMgr::OnPlayerTrackerAbort( )
{
	// Tell update to handle the playertrackerabort.  We can't continue
	// with the savedata right now because the player is still in the
	// process of getting deleted.
	m_bPlayerTrackerAborted = true;
}

float CServerMissionMgr::GetTimeLeft()
{
	return m_ServerSettings.m_nTimeLimit-(g_pLTServer->GetTime()-m_fStartTime);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::Update
//
//	PURPOSE:	Frame update.
//
// --------------------------------------------------------------------------- //

void CServerMissionMgr::Update( )
{
	// Update the playertracker.
	m_PlayerTracker.Update( );

	// Since all the players dropped out, we can just finish what we
	// were doing.
	if( m_bPlayerTrackerAborted )
	{
		// Clear the abort signal.
		m_bPlayerTrackerAborted = false;
		FinishExitLevel( );
	}

	// Check to see if the time limit was reached...
	if( m_ServerSettings.m_nTimeLimit > 0 && m_fStartTime >= 0.0f )
	{
		float fElapsedTime = g_pLTServer->GetTime()-m_fStartTime;
		
		// Convert timelimit to seconds and see how much we have left.
		float fTimeLeft = ((float)m_ServerSettings.m_nTimeLimit) - fElapsedTime;

		// If we ran out of time, then go to the next round.
		if( fTimeLeft <= 0 )
		{
			EndGame( EG_TIME_OUT );
		}
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::FinishExitLevel
//
//	PURPOSE:	Finish the exitlevel we started.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::FinishExitLevel( )
{
	TRACE( "CServerMissionMgr::FinishExitLevel:\n" );

	// No longer waiting for exitlevel messages back from client.
	m_bExitingLevel = false;

	// Clear out the playertracker.
	m_PlayerTracker.Term( );

	CHECK_PLAYEROBJ_BEGIN();
	// Tell the players to handle an exit level.
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayerObj = *iter;
		pPlayerObj->HandleExit( m_bNewMission );
		iter++;
	}
	CHECK_PLAYEROBJ_END();

	// Save the keepalives between levels.
	bool bSaveKeepAlives = true;
	
	if( m_bNewMission )
	{
		// Clear out our objectives...
		g_pGameServerShell->ResetObjectives();

		// Clear working save dir since we're starting fresh.
		g_pServerSaveLoadMgr->ClearWorkingDir( );

		// If this is mp, then consider this to be a new game.  Saves between
		// missions is only available in sp which allows skills to be saved.
		// Tell the gameservershell it's a new game.
		g_pGameServerShell->SetLGFlags( LOAD_NEW_GAME );
		bSaveKeepAlives = false;
	}

	// Default to failure.
	bool bRet = false;

	// Do transition.
	if( m_bTransitionLevels )
	{
		m_bTransitionLevels = false;
		bRet = g_pServerSaveLoadMgr->TransitionLevels( m_sCurrentWorldName );
	}
	// Do switch.
	else
	{
		bRet = g_pServerSaveLoadMgr->SwitchLevels( m_sCurrentWorldName, bSaveKeepAlives );
	}

	// Unpause the server now that we're done switching levels.
	g_pGameServerShell->PauseGame( LTFALSE );	

	return bRet;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::SendExitLevelMessage
//
//	PURPOSE:	Tell client to exit level.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::SendExitLevelMessage( )
{
	CTeamMgr::Instance().UpdateClient();

	// See if we have already visited the level we are going to.
	CString sLoadGameFile = g_pServerSaveLoadMgr->GetWorldSaveFile( m_sCurrentWorldName );
	bool bRestoringLevel = !!CWinUtil::FileExist( sLoadGameFile );

	// Tell client's we're exiting.  Wait for their response.
	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_EXIT_LEVEL );
	cMsg.WriteString( m_sCurrentWorldName );
	cMsg.Writebool( m_bNewMission );
	cMsg.Writebool( bRestoringLevel );
	cMsg.Writebool( true );
	g_pLTServer->SendToClient(cMsg.Read(), LTNULL, MESSAGE_GUARANTEED);

	return true;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::EndGame
//
//	PURPOSE:	The game has ended.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::EndGame( uint8 nWhy )
{
	if( false == g_pGameServerShell->IsResultState() )
	{
		// Make sure there is a worldproperties object.
		if (!g_pWorldProperties)
		{
			ASSERT( !"CServerMissionMgr::EndGame:  Could not find worldproperties." );
			return false;
		}

		// Tell worldproperties to send its level end command.
		g_pWorldProperties->SendLevelEndCmd( );
		g_pGameServerShell->SetResultState(true);
	}
	return true;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::StartGame
//
//	PURPOSE:	Default start game.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::StartGame()
{
	// Setup the campaign from the campaign file.
	SetupCampaign( m_sCampaignFile );
	g_pGameServerShell->SetResultState(false);

	return true;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::SetupCampaign
//
//	PURPOSE:	Setup the campaign list of missions.  The campaign
//				defines which missions we will play.  Having a blank
//				campaign file means that all missions will be used.
//
// --------------------------------------------------------------------------- //

bool CServerMissionMgr::SetupCampaign( char const* pszCampaignName )
{
	// Clear all missions from our campaign.
	m_Campaign.clear( );

	ServerGameOptions const& serverGameOptions = g_pGameServerShell->GetServerGameOptions( );
	char const* pszCampaignFile = GetCampaignFile( serverGameOptions );

	// If the campaignfile is blank, then just use all missions.
	if( !pszCampaignFile || !pszCampaignFile[0] )
	{
		for( int nMissionId = 0; nMissionId < g_pMissionButeMgr->GetNumMissions( ); nMissionId++ )
		{
			m_Campaign.push_back( nMissionId );
		}

		// Default to not looping.
		m_bLoopCampaign = false;

		return true;
	}

	// Read the missionid's for this campaign.
	char szMissionId[4];
	char szCampaignIndexKeyName[32];
	int nCampaignIndex = 0;
	while( 1 )
	{
		// Create the key name for this entry.
		sprintf( szCampaignIndexKeyName, "Mission%d", nCampaignIndex );

		// Get the missionid for this entry.
		CWinUtil::WinGetPrivateProfileString( "MissionList", szCampaignIndexKeyName, "", szMissionId, 
			ARRAY_LEN( szMissionId ), pszCampaignFile );

		// If there wasn't a missionid, then we reached the end.
		if( !szMissionId[0] )
			break;

		// Get the mission id.
		int nMissionId = atoi( szMissionId );

		// Make sure it's a valid mission.
		if( nMissionId < g_pMissionButeMgr->GetNumMissions( ))
			m_Campaign.push_back( nMissionId );

		nCampaignIndex++;
	}

	// Get the looping flag.
	char szLoop[4];
	CWinUtil::WinGetPrivateProfileString( "MissionList", "LoopMissions", "0", szLoop, 
		ARRAY_LEN( szLoop ), pszCampaignFile );
	m_bLoopCampaign = !!atoi( szLoop );

//// Added By IN'T ==>
//	m_nCurrentMission = 0;
//	m_nCurrentLevel = 0;
//	m_nCurrentRound = 0;
//// Added By IN'T <==

	return true;
}




// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::SetMissionFailed()
//
//	PURPOSE:	Set or clear mission failed state
//
// --------------------------------------------------------------------------- //

void CServerMissionMgr::SetMissionFailed(bool bFail)
{ 
	if (bFail != m_bMissionFailed)
	{
		g_pGameServerShell->PauseGame(bFail);
		
		if( bFail )
		{
			// Send a message to the serverapp letting it know the mission failed.
			// If a ServerApp doesn't exist then nothing happens, otherwise GameServerShell::ServerAppMessageFn()
			// will recieve a message and handle it.

			CAutoMessage cMsg;
			cMsg.Writeuint8( SERVERAPP_MISSIONFAILED );

			CLTMsgRef_Read cMsgRef = cMsg.Read(); 
			g_pLTServer->SendToServerApp( *cMsgRef );
		}
	}
	m_bMissionFailed = bFail; 
};


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::SetServerSettings
//
//	PURPOSE:	Sets friendly fire info.
//
// --------------------------------------------------------------------------- //

void CServerMissionMgr::SetServerSettings(ServerMissionSettings& ServerSettings)
{

#ifdef USE_AI
	if (!g_vtDifficultyFactorPlayerIncrease.IsInitted())
	{
		g_vtDifficultyFactorPlayerIncrease.Init( g_pLTServer, "DifficultyFactorPlayerIncrease", LTNULL, 0.1f );
	}
#endif
	
	m_ServerSettings = ServerSettings;

	m_bNetFriendlyFire	= ServerSettings.m_bFriendlyFire;

	if (IsDifficultyGameType())
	{
		g_pGameServerShell->SetDifficulty((GameDifficulty)ServerSettings.m_nMPDifficulty);
		g_pLTServer->CPrint("CServerMissionMgr::StartGame() setting difficulty to %d",ServerSettings.m_nMPDifficulty);
	}
	else
		g_pGameServerShell->SetDifficulty(GD_NORMAL);
	
#ifdef USE_AI
	g_vtDifficultyFactorPlayerIncrease.SetFloat(ServerSettings.m_fPlayerDiffFactor);
#endif
}


//[MURSUM]==============================================================
bool CServerMissionMgr::RestartLevel()
{
	//Reset World///////////////////////////////////////////
	g_pGameServerShell->SetResultState(false);
	//g_pWorldProperties->SendLevelEndCmd();	
	////////////////////////////////////////////////////////////
	
	++m_nCurrentRound;

	RemoveAllManagedWeapon();		// 광식 : world 상에 떨어져 있는 모든 무기 삭제

	ResetTimer();

	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayerObj = *iter;
		/*
#ifdef GAME_SHOW
		if( pPlayerObj->IsHostPlayer() )
		{
			pPlayerObj->SetSpectatorMode( LTFALSE );
			pPlayerObj->ChangeState( PS_GHOST );
			CAutoMessage cMsg;
			cMsg.Writeuint8( MID_ATTACH_FIRST );
			g_pGameServerShell->HandlePlayerAttachMessage( pPlayerObj->GetClient(), cMsg.Read() );
			g_pCommonLT->SetObjectFlags(pPlayerObj->m_hObject, OFT_Flags, 0, FLAG_SOLID|FLAG_RAYHIT);
			

			//리스폰은 안해도 시간은 받아야지..
			CAutoMessage cMsg2;
			cMsg2.Writeuint8( MID_SERVER_TIME );
			cMsg2.Writefloat( GetTimeLeft() );
			g_pLTServer->SendToClient( cMsg2.Read(), pPlayerObj->GetClient(), MESSAGE_GUARANTEED );

			iter++;
			continue;
		}
#endif
		*/

		pPlayerObj->StopGodMode();
		// 광식
		pPlayerObj->DetachAllPlayer();		// 자신에게 붙어있는 Client 모두 제거
		pPlayerObj->RestartRemoveBody();	// 시체 지우기
		pPlayerObj->RestartWeaponClass();
		pPlayerObj->ResetAfterDeath();		//에너지 풀
		pPlayerObj->Respawn(false);

		iter++;
	}
	CHECK_PLAYEROBJ_END();

	g_pGameServerShell->PauseGame(LTFALSE);

	return true;
}
//=====================================================================
// 광식
void CServerMissionMgr::AddManagedWeapon(HOBJECT hObj)
{
	for( int i = 0; i < MAX_MANAGED_WEAPON; i++ )
	{
		if( m_hManaged_Weapon[i] == LTNULL )
		{
			m_hManaged_Weapon[i] = hObj;
			return;
		}
	}
}

LTBOOL CServerMissionMgr::RemoveManagedWeapon(HOBJECT hObj)
{
	for( int i = 0; i < MAX_MANAGED_WEAPON; i++ )
	{
		if( m_hManaged_Weapon[i] == hObj )
		{
			m_hManaged_Weapon[i] = LTNULL;
			return LTTRUE;
		}
	}

	return LTFALSE;
}

void CServerMissionMgr::RemoveAllManagedWeapon()
{
	for( int i = 0; i < MAX_MANAGED_WEAPON; i++ )
	{
		if( m_hManaged_Weapon[i] != LTNULL )
		{
			g_pLTServer->RemoveObject( m_hManaged_Weapon[i] );
		}
		
		m_hManaged_Weapon[i] = LTNULL;
	}
}

bool CServerMissionMgr::ResetTimer()
{
	m_fStartTime		= g_pLTServer->GetTime();
	return true;
}

void CServerMissionMgr::SetCurrentRound( int nCurrentRound )
{
	m_nCurrentRound = nCurrentRound;
}