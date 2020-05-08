// ----------------------------------------------------------------------- //
//
// MODULE  : GameServerShell.cpp
//
// PURPOSE : The game's server shell - Implementation
//
// CREATED : 9/17/97
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#include "stdafx.h"
#include "GameServerShell.h"
#include "PlayerObj.h"
#include "iltserver.h"
#include "MsgIDs.h"
#include "CommandIDs.h"
#include "ServerUtilities.h"
#include "Trigger.h"
#include "Sparam.h"
#include "ObjectMsgs.h"
#include <stdio.h>
#include "AssertMgr.h"
#include "WorldProperties.h"
#include "GameBase.h"
#include "MusicMgr.h"
#include "Camera.h"
#include "ServerSpecialFX.h"
#include "AI.h"
#include "AIGoalAbstract.h"
#include "GadgetTarget.h"
#include "DecisionObject.h"
#include "DebugLineSystem.h"
#include "AIVolumeMgr.h"
#include "AIInformationVolumeMgr.h"
#include "AINodeMgr.h"
#include "GameStartPoint.h"
#include "AIStimulusMgr.h"
#include "AICentralKnowledgeMgr.h"
#include "ServerTrackedNodeMgr.h"
#include "GlobalServerMgr.h"
#include "iserverdir.h"
#include "ObjectTemplateMgr.h"
#include "LiteObjectMgr.h"
#include "GameBaseLite.h"

#include "ServerMissionMgr.h"
#include "SinglePlayerMissionMgr.h"
#include "CoopMissionMgr.h"
#include "DeathMatchMissionMgr.h"
#include "TeamDeathMatchMissionMgr.h"
#include "DoomsDayMissionMgr.h"
#include "SurvivalMissionMgr.h"
#include "ServerSaveLoadMgr.h"
#include "CharacterMgr.h"
#include "ServerAssetMgr.h"
#include "VersionMgr.h"
#include "CRC32.h"
#include "PlayerButes.h"
#include "CVarTrack.h"
#include "ParsedMsg.h"
#include "ScmdServer.h"
//[MURSUM] #include "BanIPMgr.h"
#include <time.h>
#include <algorithm>


// Added By IN'T ==>
#include "ButeFileDefines.h"
// Added By IN'T <==
#include "ProjectileTypes.h"
// 광식
#include "Attachments.h"

//#define WOONGBO_ATTACH	
#define WOONGBO_ATTACH	/##/

#define MAX_CLIENT_NAME_LENGTH		100
#define CLIENT_PING_UPDATE_RATE		3.0f
#define UPDATENAME_INTERVAL			10.0f

#define GAME_VERSION				1


// simple macro to clean up code and reduce typing overhead
#define GET_PLAYER(hSender) (CPlayerObj*)g_pLTServer->GetClientUserData(hSender)

//#define _DEGUG

// debug output messages to sever console
#ifdef _DEGUG
	enum EDebugOutput {eAlways = 0, eSomeTimes = 1, eRarely = 2};

	// if true all server messages will be output to console
	LTBOOL			g_debugOutputMessages(LTFALSE);
	EDebugOutput	g_debugOutputLevel(eRarely);

	#define DEBUG_CONSOLE_OUTPUT(output, level)						\
		if (g_debugOutputMessages && level >= g_debugOutputLevel)	\
			g_pLTServer->CPrint(output)
#else
	#define DEBUG_CONSOLE_OUTPUT(output, level)
#endif // _DEBUG

#ifdef USE_AI
extern CAIInformationVolumeMgr* g_pAIInformationVolumeMgr;
#endif

// Stuff to create a GameServerShell.

//SETUP_SERVERSHELL()

#define GS_CAN_SHOWDIMS_TRACK	0.0f
#define GS_SHOW_VOLUME_TRACK	0.0f
#define GS_SHOW_ALL_INFO_VOLUME	0.0f
#define GS_SHOW_AI_NODE			0.0f
#define GS_NET_AUDIO_TAUNT		1.0f

char*	g_pTempBuffer = LTNULL;

CGameServerShell*   g_pGameServerShell = LTNULL;

// ----------------------------------------------------------------------- //
// Helpers for world time conversion.
// ----------------------------------------------------------------------- //

inline float TODSecondsToHours(float time)
{
	return (float)(time / 3600.0);
}

inline float TODHoursToSeconds(float time)
{
	return (float)(time * 3600.0);
}


ClientData::ClientData( )
{
	m_hClient = NULL;
	m_nLastClientUpdateTime = GetTickCount( );
}



LTRESULT CGameServerShell::OnServerInitialized()
{
	g_pLTServer->CPrint( "OnServerInitialized %f", g_pLTServer->GetTime() );

	g_pGameServerShell = this;
	m_bResultState		= false;

	//////////////////////////////////////////////////////////////////////////////
	// Set up all the stuff that everything relies on

	// Redirect our asserts as specified by the assert convar

	CAssertMgr::Enable();

	// Set up the LT subsystem pointers

    g_pModelLT = g_pLTServer->GetModelLT();
    g_pTransLT = g_pLTServer->GetTransformLT();
    g_pPhysicsLT = g_pLTServer->Physics();
	g_pLTBase = static_cast<ILTCSBase*>(g_pLTServer);

	// Make sure we are using autodeactivation...

    g_pLTServer->RunGameConString("autodeactivate 1.0");

	g_pPhysicsLT->SetStairHeight( DEFAULT_STAIRSTEP_HEIGHT );


	//////////////////////////////////////////////////////////////////////////////
	// Do all the stuff that used to be in the ctor for the server

	ClearClientList();
	SetUpdateGameServ();

	m_SunVec[0] = m_SunVec[1] = m_SunVec[2] = 0;

	if( !InitGameType())
		return LT_ERROR;

#ifdef USE_AI
	// Create the AI class factory.
	if ( 0 == m_pAIClassFactory )
	{
		m_pAIClassFactory = debug_new( CAIClassFactory );
		ASSERT( 0 != m_pAIClassFactory );
	}
#endif

	SetLGFlags( LOAD_NEW_GAME );
    m_hSwitchWorldVar = LTNULL;
    m_bFirstUpdate = LTTRUE;

	m_TODSeconds = TODHoursToSeconds(12.0f);
    m_TODCounter = 0.0f;
	m_ClientPingSendCounter = 0.0f;
	m_iPrevRamp = 0;

	m_sWorld.Empty( );

	// Initialize all the globals...
	if ( m_pGlobalMgr == 0 )
	{
		m_pGlobalMgr = debug_new( CGlobalServerMgr );
		ASSERT( 0 != m_pGlobalMgr );
	}
	m_pGlobalMgr->Init();

#ifdef USE_AI
	if ( m_pAIStimulusMgr == 0 )
	{
		m_pAIStimulusMgr = debug_new( CAIStimulusMgr );
		ASSERT( 0 != m_pAIStimulusMgr );
	}

	if ( m_pAICentralKnowledgeMgr == 0 )
	{
		m_pAICentralKnowledgeMgr = debug_new( CAICentralKnowledgeMgr );
		ASSERT( 0 != m_pAICentralKnowledgeMgr );
	}

	if ( m_pCharacterMgr == 0 )
	{
		m_pCharacterMgr = debug_new( CCharacterMgr );
		ASSERT( 0 != m_pCharacterMgr );
	}
#endif


	if( m_pServerTrackedNodeMgr == 0 )
	{
		m_pServerTrackedNodeMgr = debug_new( CServerTrackedNodeMgr );
		ASSERT( 0 != m_pServerTrackedNodeMgr );
	}

	m_bShowMultiplayerSummary = LTFALSE;
	m_bStartNextMultiplayerLevel = LTFALSE;
	m_fSummaryEndTime = 0.0f;

	m_bPreCacheAssets = m_ServerGameOptions.m_bPreCacheAssets;

	m_nPauseCount = 0;
	m_bClientPaused = false;

	m_eSwitchingWorldsState = eSwitchingWorldsStateNone;

	m_nCShellCRC = CRC32::CalcRezFileCRC("cshell.dll");
	m_nModelButesCRC = CRC32::CalcRezFileCRC( g_pModelButeMgr->GetAttributeFile( ));
	m_nSurfaceCRC = CRC32::CalcRezFileCRC( g_pSurfaceMgr->GetAttributeFile( ));
	m_nDamageFxCRC = CRC32::CalcRezFileCRC( DMGFXMGR_DEFAULT_FILE );

	m_eMinMusicMood = CMusicMgr::eMoodRoutine;

	// Use max on LAN games
	float fMinBandwidth = ( m_ServerGameOptions.m_bLANOnly ) ? k_fMaxBandwidth : 0.0f;

	if( m_ServerGameOptions.m_nBandwidthServer < eBandwidth_Custom)
	{
		WriteConsoleFloat("BandwidthTargetServer",
			LTCLAMP(((float)g_BandwidthServer[m_ServerGameOptions.m_nBandwidthServer] * 1024.0f), fMinBandwidth, k_fMaxBandwidth) );
	}
	else
	{
		WriteConsoleFloat("BandwidthTargetServer",
			LTCLAMP(((float)m_ServerGameOptions.m_nBandwidthServerCustom * 1024.0f), fMinBandwidth, k_fMaxBandwidth) );
	}

	// Initialize the scmd handler.
	if( !ScmdServer::Instance( ).Init( ))
		return LT_ERROR;

	//[MURSUM]
	/*
	// Initialize the BanIPMgr.
	if( !BanIPMgr::Instance( ).Init( ))
		return LT_ERROR;
		*/

	// Do any game specific initialization.
	switch( m_ServerGameOptions.m_eGameType )
	{
		// Single player and coop use the same weapon files...

		case eGameTypeSingle:
		case eGameTypeCooperative:
		{
			g_pWeaponMgr->LoadOverrideButes( WEAPON_DEFAULT_FILE );
		}
		break;

		case eGameTypeDeathmatch:
		case eGameTypeTeamDeathmatch:
		case eGameTypeDoomsDay:
		case eGameTypeSurvival :
		{
			//g_pWeaponMgr->LoadOverrideButes( WEAPON_DEFAULT_MULTI_FILE );
			g_pWeaponMgr->LoadOverrideButes( WEAPON_DEFAULT_FILE );
			ApplyWeaponRestrictions( );
		}
		break;
	}

	// Seed the random number generator so GetRandom() isn't the same each game.
	// IMPORTANT: reseeding the random number generator should not happen all the time
	// as it can lead to GetRandom() not acting as random as you might think.
	
	srand(time(NULL));

	return LT_OK;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::OnServerTerm()
//
//	PURPOSE:	Server is shutting down...
//
// ----------------------------------------------------------------------- //

void CGameServerShell::OnServerTerm()
{
	CAssertMgr::Disable();
}


LTBOOL g_bInfiniteAmmo = LTFALSE;


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CGameServerShell()
//
//	PURPOSE:	Initialize
//
// ----------------------------------------------------------------------- //

CGameServerShell::CGameServerShell() :
	m_pServerDir( 0 )
#ifdef USE_AI
	, m_pAIStimulusMgr( 0 )
	, m_pAICentralKnowledgeMgr( 0 )
	, m_pAIClassFactory( 0 )
	, m_pCharacterMgr( 0 )
#endif
	, m_pServerTrackedNodeMgr( 0 )
	, m_pGlobalMgr( 0 )	
	, m_bCachedLevelFiles( false )
{
	uint32 nTempBufferSize = GetTickCount()%10000;
	g_pTempBuffer = new char[nTempBufferSize];
	
	m_aCurMessageSourceAddr[0] = 0;
	m_aCurMessageSourceAddr[1] = 0;
	m_aCurMessageSourceAddr[2] = 0;
	m_aCurMessageSourceAddr[3] = 0;
	m_nCurMessageSourcePort = 0;

	m_bPreCacheAssets = false;

	// Create the object template mgr
	m_pObjectTemplates = debug_new( CObjectTemplateMgr );

	// Create the lite object mgr
	m_pLiteObjectMgr = debug_new( CLiteObjectMgr );

	// Note : ctor stuff is now all handled by ::OnServerInitialized

	m_pServerMissionMgr = NULL;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::~CGameServerShell()
//
//	PURPOSE:	Deallocate
//
// ----------------------------------------------------------------------- //

CGameServerShell::~CGameServerShell()
{
	if ( m_pServerTrackedNodeMgr != 0 )
	{
		debug_delete( m_pServerTrackedNodeMgr );
		m_pServerTrackedNodeMgr = 0;
	}

#ifdef USE_AI
	if ( m_pAIStimulusMgr != 0 )
	{
		debug_delete( m_pAIStimulusMgr );
		m_pAIStimulusMgr = 0;
	}

	if ( m_pAICentralKnowledgeMgr != 0 )
	{
		debug_delete( m_pAICentralKnowledgeMgr );
		m_pAICentralKnowledgeMgr = 0;
	}

	if ( m_pCharacterMgr != 0 )
	{
		debug_delete( m_pCharacterMgr );
		m_pCharacterMgr = 0;
	}

	// Delete the AI class factory LAST.
	if ( m_pAIClassFactory != 0 )
	{
		debug_delete( m_pAIClassFactory );
		m_pAIClassFactory = 0;
	}
#endif


	// Clean up the Pointer to the GlobalMemmoryMgr as we want to clean up the
	// 
	if ( m_pGlobalMgr != 0 )
	{
		debug_delete( m_pGlobalMgr );
		m_pGlobalMgr = 0;
	}

	// Get rid of the serverdir, if we have one
	if( m_pServerDir )
	{
		delete m_pServerDir;
	  	m_pServerDir = 0;
	}

	// Get rid of the object template mgr
	debug_delete( m_pObjectTemplates );
	m_pObjectTemplates = 0;

	// Get rid of the lite object mgr
	debug_delete( m_pLiteObjectMgr );
	m_pLiteObjectMgr = 0;

	// Toss the server missionmgr.
	if( m_pServerMissionMgr )
	{
		debug_delete( m_pServerMissionMgr );
		m_pServerMissionMgr = NULL;
	}

	// Remove all remaining clients from our list of clients.
	ClientDataList::iterator iter = m_ClientDataList.begin( );
	for( ; iter != m_ClientDataList.end( ); iter++ )
	{
		ClientData* pClientData = *iter;
		debug_delete( pClientData );
	}
	m_ClientDataList.clear( );

	if( g_pTempBuffer )
	{
		delete [] g_pTempBuffer;
		g_pTempBuffer = LTNULL;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::OnAddClient()
//
//	PURPOSE:	Called when a client connects to a server
//
// ----------------------------------------------------------------------- //

void CGameServerShell::OnAddClient(HCLIENT hClient)
{
	DEBUG_CONSOLE_OUTPUT("Add Client", eRarely);

	// Add this client to our list of clients.
	ClientData* pClientData = debug_new( ClientData );
	pClientData->m_hClient = hClient;
	pClientData->m_nLastClientUpdateTime = GetTickCount( );
	m_ClientDataList.push_back( pClientData );

	//[MURSUM]
	/*
	// Check if the client is banned.
	if( !BanIPMgr::Instance( ).OnAddClient( hClient ))
	{
		g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
		g_pLTServer->KickClient( hClient );
	}
	*/

	// Tell serverapp about it.
	ServerAppAddClient( hClient );

	// Tell the client about the pause state.
	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_SWITCHINGWORLDSSTATE );
	cMsg.Writeuint8( m_eSwitchingWorldsState );
	g_pLTServer->SendToClient( cMsg.Read(), hClient, MESSAGE_GUARANTEED);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::OnRemoveClient()
//
//	PURPOSE:	Called when a client disconnects from a server
//
// ----------------------------------------------------------------------- //

void CGameServerShell::OnRemoveClient(HCLIENT hClient)
{
	uint32 nClientID = g_pLTServer->GetClientID(hClient);

	if (IsTeamGameType())
	{
		CTeamMgr::Instance().RemovePlayer( nClientID );
	}

	// Clear our client data...
    CPlayerObj* pPlayer = GET_PLAYER(hClient);
	if (pPlayer)
	{
        pPlayer->SetClient(LTNULL,false);
	}

	// Tell server app about it.
	ServerAppRemoveClient( hClient );

	// Tell the scmd about it.
	ScmdServer::Instance( ).OnRemoveClient( hClient );

	// Remove this client from our list of clients.
	ClientDataList::iterator iter = m_ClientDataList.begin( );
	for( ; iter != m_ClientDataList.end( ); iter++ )
	{
		ClientData* pClientData = *iter;
		if( pClientData->m_hClient == hClient )
		{
			m_ClientDataList.erase( iter );
			debug_delete( pClientData );
			break;
		}
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CreateClientName
//	ROUTINE:	GetIpStringFromClientName
//	ROUTINE:	GetClientGivenPlayerNameFromClientName
//
//	PURPOSE:	Creates a clientname based on the ip and client given player name.
//
// --------------------------------------------------------------------------- //

static void CreateClientName( HCLIENT hClient, char const* pszClientGivenPlayerName, char* pszClientName, 
						   int nSize )
{
	uint8 aTcpIp[4];
	uint16 nPort;
	g_pLTServer->GetClientAddr( hClient, aTcpIp, &nPort );

	_snprintf( pszClientName, nSize, "%d.%d.%d.%d:%d#%s", aTcpIp[0], aTcpIp[1], aTcpIp[2], aTcpIp[3], nPort,
		pszClientGivenPlayerName );
	pszClientName[nSize-1] = 0;
}

static void GetIpStringFromClientName( char const* pszClientName, char* pszIpString, int nSize )
{
	CString sCopy = pszClientName;
	int nSeperator = sCopy.Find( "#" );
	if( nSeperator == -1 )
	{
		pszIpString[0] = 0;
		return;
	}

	strncpy( pszIpString, sCopy.Left( nSeperator ), nSize );
	pszIpString[nSize-1] = 0;
}

static void GetClientGivenPlayerNameFromClientName( char const* pszClientName, 
												   char* pszClientGivenPlayerName, int nSize )
{
	CString sCopy = pszClientName;
	int nSeperator = sCopy.Find( "#" );

	strncpy( pszClientGivenPlayerName, sCopy.Mid( nSeperator + 1 ), nSize );
	pszClientGivenPlayerName[nSize-1] = 0;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::OnClientEnterWorld()
//
//	PURPOSE:	Add a client
//
// ----------------------------------------------------------------------- //

LPBASECLASS	CGameServerShell::OnClientEnterWorld(HCLIENT hClient)
{
    if (!hClient) return LTNULL;
	
	DEBUG_CONSOLE_OUTPUT("Client Enter World", eRarely);

    CPlayerObj* pPlayer			= LTNULL;
	ModelId		ePlayerModelId	= eModelIdInvalid;
	ModelLoadType eModelLoadType = eModelNormal;

	g_pLTServer->CPrint( "CGameServerShell::OnClientEnterWorld hClient: 0x%08x", hClient );
	g_pPhysicsLT->SetStairHeight( DEFAULT_STAIRSTEP_HEIGHT );

	// This will be filled in with clientdata for mp games.
	NetClientData ncd;
	memset( &ncd, 0, sizeof( ncd ));

	char szClientName[32];

	// Must have client data for mp game.
	
	// Make sure the NetClientData is valid.
	int nNcdSize = sizeof( ncd );
	if( !g_pLTServer->GetClientData( hClient, ( uint8* )&ncd, nNcdSize ) || nNcdSize != sizeof( NetClientData ))
	{
		return NULL;
	}
	
	//[MURSUM]
	//만약 존재하는 사용자이면 짤라버림..
	KickBanIfExist( ncd.m_nServerIndex );
	
	//[MURSUM]
	sprintf( szClientName, "%d", ncd.m_nServerIndex );


	ePlayerModelId = (ModelId)ncd.m_ePlayerModelId;
	eModelLoadType = ncd.m_eModelType;
	if( ePlayerModelId == eModelIdInvalid || 
		ePlayerModelId >= g_pModelButeMgr->GetNumModels( ))
	{
		g_pLTServer->CPrint( "[ERROR] Player Model ID : %d  %d", ncd.m_nServerIndex, ncd.m_ePlayerModelId );
		ePlayerModelId = (ModelId)0;
		//return NULL;
	}	

	// Set our clientname from the netclientdata.
	g_pLTServer->SetClientName( hClient, szClientName, strlen( szClientName ) + 1 );

	// See if we can use a player from one of the clientrefs.
	pPlayer = PickClientRefPlayer( hClient );

	// If we found a player, then respawn, otherwise create a new player.
	if( !pPlayer )
	{
		// 광식
		//pPlayer = CreatePlayer( hClient, ePlayerModelId );
		pPlayer = CreatePlayerExt(hClient, ePlayerModelId, eModelLoadType);
		if( !pPlayer )
		{
			ASSERT( !"CGameServerShell::OnClientEnterWorld: Could not create player." );
			return NULL;
		}
	}
	pPlayer->SetHostPlayer( ncd.m_bHost );
	if( ncd.m_bHost )
	{
		g_pServerMissionMgr->SetCurrentRound( ncd.m_nRound );
	}

	RespawnPlayer( pPlayer, hClient, true );

	// Add this client to our local list...
	AddClientToList(hClient);
	SetUpdateGameServ();

	//[MURSUM] SendMultiPlayerDataToClient(hClient);
	
	//[MURSUM] SendObjectivesDataToClient( hClient );

	// Tell the player about our ncd.
	pPlayer->SetNetClientData( ncd );

	uint32  nClientID   = g_pLTServer->GetClientID( hClient );
	char const* pszNetUniqueName = pPlayer->GetNetUniqueName( );

	/*
	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_PLAYER_CONNECTED );
	cMsg.Writeuint32( nClientID );		
	cMsg.WriteObject(pPlayer->m_hObject);	
	cMsg.WriteObject(pPlayer->GetWeaponModel());
	cMsg.Writeuint8((uint8)pPlayer->GetState());		
	g_pLTServer->SendToClient( cMsg.Read(), hClient, MESSAGE_GUARANTEED );
	*/

	// All done...
	if( IsRevivePlayerGameType() && ncd.m_bSpectator )
	{
		pPlayer->SetSpectatorMode( LTTRUE );
	}
	
	return pPlayer;
}


// ----------------------------------------------------------------------- //
// Sends a game data message to the specified client.
// ----------------------------------------------------------------------- //
void CGameServerShell::SendMultiPlayerDataToClient(HCLIENT hClient)
{
	if( !hClient )
		return;
	
	CAutoMessage cMsg;

	cMsg.Writeuint8(MID_MULTIPLAYER_DATA);
	cMsg.Writeuint8((uint8)GetGameType());

	// Get our ip address and port...
	char sBuf[32];
	int  nBufSize = 30;
	WORD wPort = 0;
	g_pLTServer->GetTcpIpAddress(sBuf, nBufSize, wPort);
	cMsg.WriteString(sBuf);
	cMsg.Writeuint32((uint32) wPort);

	g_pLTServer->SendToClient(cMsg.Read(), hClient, MESSAGE_GUARANTEED);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::SendObjectivesDataToClient()
//
//	PURPOSE:	Sends all current objective data to the client...
//
// ----------------------------------------------------------------------- //

void CGameServerShell::SendObjectivesDataToClient( HCLIENT hClient )
{
	if( !hClient )
		return;

	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_OBJECTIVES_DATA );

	int nNumObj = m_Objectives.m_IDArray.size();

	//send total number of objectives
    cMsg.Writeuint8((uint8)nNumObj);

	//send list of general objectives
	for (uint8 i = 0; i < m_Objectives.m_IDArray.size(); i++)
	{
        cMsg.Writeuint32((uint32)m_Objectives.m_IDArray[i]);
	}

	//send total number of optional objectives
	nNumObj = m_OptionalObjectives.m_IDArray.size();
    cMsg.Writeuint8((uint8)nNumObj);

	//send list of optional objectives
	for (i = 0; i < m_OptionalObjectives.m_IDArray.size(); i++)
	{
        cMsg.Writeuint32((uint32)m_OptionalObjectives.m_IDArray[i]);
	}

	//send total number of completed objectives
	nNumObj = m_CompletedObjectives.m_IDArray.size();
    cMsg.Writeuint8((uint8)nNumObj);

	//send list of general completed objectives
	for (i = 0; i < m_CompletedObjectives.m_IDArray.size(); i++)
	{
        cMsg.Writeuint32((uint32)m_CompletedObjectives.m_IDArray[i]);
	}

	//send total number of mission parameters
	nNumObj = m_Parameters.m_IDArray.size();
    cMsg.Writeuint8((uint8)nNumObj);

	//send list of mission parameters
	for (i = 0; i < m_Parameters.m_IDArray.size(); i++)
	{
        cMsg.Writeuint32((uint32)m_Parameters.m_IDArray[i]);
	}
	
	g_pLTServer->SendToClient(cMsg.Read(), hClient, MESSAGE_GUARANTEED);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::OnClientExitWorld()
//
//	PURPOSE:	remove a client
//
// ----------------------------------------------------------------------- //

void CGameServerShell::OnClientExitWorld(HCLIENT hClient)
{

	// Remove this client from our local list...

	g_pLTServer->CPrint( "CGameServerShell::OnClientExitWorld hClient: 0x%08x", hClient );
	RemoveClientFromList(hClient);
	SetUpdateGameServ();


	// Remove the player object...

	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj* pPlayer = LTNULL;
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList().begin();
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		if( (*iter)->GetClient() == hClient )
		{
			pPlayer = (CPlayerObj*)(*iter);
			break;
		}
		iter++;
	}
	CHECK_PLAYEROBJ_END();

    //CPlayerObj* pPlayer = (CPlayerObj*)g_pLTServer->GetClientUserData(hClient);
	if (pPlayer)
	{
		//-------------------------------------
		// 광식
		//------------------------------------------------------------------
		// 내가 다른 사람에게 Attach 된 경우
		CPlayerObj* pUsedPlayer = LTNULL;

		if( pPlayer->m_hAttachedPlayer )
		{
			CHECK_PLAYEROBJ_BEGIN();
			iter = CPlayerObj::GetPlayerObjList( ).begin( );
			while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
			{
				CPlayerObj* pUsedPlayer = (*iter);
				if( pPlayer->m_hAttachedPlayer == pUsedPlayer->GetClient() )
				{
					if( pUsedPlayer->IsAttachPlayer(hClient) )
					{
						pUsedPlayer->DetachPlayer( pPlayer->GetClient() );
						pPlayer->m_hAttachedPlayer = LTNULL;
						break;
					}
				}

				iter++;
			}
			CHECK_PLAYEROBJ_END();
			/*
			CPlayerObj* pUsedPlayer = (CPlayerObj*)g_pLTServer->GetClientUserData(pPlayer->m_hAttachedPlayer);
			if( pUsedPlayer )
			{
				if( pUsedPlayer->IsAttachPlayer( hClient ) )
				{
					pUsedPlayer->DetachPlayer( pPlayer->GetClient() );
					pPlayer->m_hAttachedPlayer = LTNULL;
				}
			}
			*/
		}
		// 나에게 다른 사람이 Attach 된 경우
		pPlayer->AutoMoveAttachProcess();
		//------------------------------------------------------------------
		
		// Clear our client pointer.
		pPlayer->SetClient(LTNULL,false);
        g_pLTServer->RemoveObject(pPlayer->m_hObject);
	    g_pLTServer->SetClientUserData(hClient, LTNULL);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::PreStartWorld()
//
//	PURPOSE:	Handle pre start world
//
// ----------------------------------------------------------------------- //

void CGameServerShell::PreStartWorld(bool bSwitchingWorlds)
{
	// Make sure the client doesn't hold a pause on us.
	ClientPauseGame( false );

	// No commands yet...
	m_CmdMgr.Clear();

#ifdef USE_AI
	m_pCharacterMgr->PreStartWorld(m_nLastLGFlags);

	m_pAIStimulusMgr->Term();
	m_pAIStimulusMgr->Init();

	m_pAICentralKnowledgeMgr->Term();
	m_pAICentralKnowledgeMgr->Init();
#endif

	// Tell the server app.
	ServerAppPreStartWorld( );


	// Make sure we reset any necessary globals...

	Camera::ClearActiveCameras();

	// Clear the object template mgr
	m_pObjectTemplates->Clear();

	// Clear the lite object mgr
	m_pLiteObjectMgr->PreStartWorld(bSwitchingWorlds);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::PostStartWorld()
//
//	PURPOSE:	Handle post switch world
//
// ----------------------------------------------------------------------- //

void CGameServerShell::PostStartWorld()
{
#ifdef USE_AI
	m_pCharacterMgr->PostStartWorld(m_nLastLGFlags);
#endif

	// Load any commands we might have for this level...

	if( g_pCommandButeMgr )
		g_pCommandButeMgr->PostStartWorld( m_nLastLGFlags );

	// Tell the server app.
	ServerAppPostStartWorld( );

	// Start the switching worlds state machine.
	StartSwitchingWorlds( );

	// Give the lite objects a chance to do post-start world stuff
	GetLiteObjectMgr()->PostStartWorld();


	//Clear the mission failed flag (and unpause if it had paused us)
	g_pServerMissionMgr->SetMissionFailed(false);


	// Set the min music mood for the current level.

	m_eMinMusicMood = CMusicMgr::eMoodRoutine;
	HCONVAR hMinMusicMood = g_pLTServer->GetGameConVar("MinMusicMood");
	if( hMinMusicMood )
	{
		const char* pVal = g_pLTServer->GetVarValueString(hMinMusicMood);
		if (pVal && pVal[0] != 0)
		{
			for( int iMood=0; iMood < CMusicMgr::kNumMoods; ++iMood )
			{
				if( stricmp( s_aszMusicMoods[iMood], pVal ) == 0 )
				{
					m_eMinMusicMood = (CMusicMgr::Mood)iMood;
					break;
				}
			}
		}
	}
}


void CGameServerShell::SendPlayerInfoMsgToClients(HCLIENT hClients, CPlayerObj *pPlayer, uint8 nInfoType)
{
	HCLIENT hClient	= pPlayer->GetClient();
	if (!hClient) return;

    uint32  nClientID   = g_pLTServer->GetClientID(hClient);

	bool bIsAdmin = false;
	if( ScmdServer::Instance( ).GetAdminControl( ) == ScmdServer::kAdminControlClient && 
		hClient == ScmdServer::Instance( ).GetAdminClient( ))
	{
		bIsAdmin = true;
	}
		
	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_PLAYER_INFO);
	cMsg.Writeuint8((uint8)nClientID);
	cMsg.WriteObject(pPlayer->m_hObject);
	cMsg.WriteObject(pPlayer->GetWeaponModel());
	cMsg.Writeint32(pPlayer->GetServerIndex());
	cMsg.Writeuint8((uint8)pPlayer->GetState());
    g_pLTServer->SendToClient(cMsg.Read(), hClients, MESSAGE_GUARANTEED);

	

	//pPlayer->GetPlayerScore()->UpdateClients(hClients);
}

#ifndef SOURCE_RELEASE
// Global key used for cross-encryption  NOTE: MUST BE SAME AS ON CLIENT!!
#define KEY_MESSER_UPPER 0xDEADFACE

inline uint32 MessUp32BitValue(uint32 nValue, uint32 nRot)
{
	// "Encrypt" the value (Yes, it's a very simple encryption...)
	uint32 nXORShift = nRot & 0xF;
	return ((KEY_MESSER_UPPER << nXORShift) + (KEY_MESSER_UPPER >> (32 - nXORShift))) ^ nValue;
}
#endif // SOURCE_RELEASE

void CGameServerShell::ProcessHandshake(HCLIENT hClient, ILTMessage_Read *pMsg)
{
	// Get the player
	CPlayerObj *pPlayer = GetPlayerFromClientList(hClient);
	if( !pPlayer ) pPlayer = PickClientRefPlayer(hClient);

	if (!pPlayer)
	{
		// Uh...  Who are you again?
		g_pLTServer->CPrint( "KickClient hClient: 0x%08x  %s %d", hClient, __FILE__, __LINE__ );
		g_pLTServer->KickClient(hClient);
		return;
	}
	
	//host doesn't need to provide user password 
	bool bIsHost = g_pLTServer->GetClientInfoFlags(hClient) & CIF_LOCAL;

	// Get the player's GUID.
	char szClientName[MAX_CLIENT_NAME_LENGTH] = "";
	g_pLTServer->GetClientName( hClient, szClientName, ARRAY_LEN( szClientName ));
	if( !szClientName[0] )
	{
		g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
		g_pLTServer->KickClient( hClient );
		return;
	}

	int nHandshakeSub = (int)pMsg->Readuint8();
	switch (nHandshakeSub)
	{
		case MID_HANDSHAKE_HELLO :
		{


			// Check the client's version
			int nHandshakeVer = (int)pMsg->Readuint16();
			if (nHandshakeVer != GAME_HANDSHAKE_VER)
			{
				// If they got here, they ignored our different version number.  Boot 'em!
				g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
				g_pLTServer->KickClient(hClient);

				// Jump out of the handshake
				return;
			}

			// Read in the client's sooper-secret key
			uint32 nClientKey = pMsg->Readuint32();

			// Write out a password message, encrypted with their key
			// Note : A key from them that null encrypts our key doesn't really effect
			// anything..  I mean, they DID send the key, and they're the ones that need
			// to use our key anyway..
			CAutoMessage cResponse;
			cResponse.Writeuint8(MID_HANDSHAKE);
			cResponse.Writeuint8(MID_HANDSHAKE_PASSWORD);
#ifdef SOURCE_RELEASE
			cResponse.Writeuint32( GAME_HANDSHAKE_PASSWORD );
#else // SOURCE_RELEASE
			cResponse.Writeuint32((uint32)this ^ MessUp32BitValue(nClientKey, (uint32)(szClientName[0])));
#endif // SOURCE_RELEASE
			cResponse.Writebool((!bIsHost && m_ServerGameOptions.m_bUsePassword));
			g_pLTServer->SendToClient(cResponse.Read(), hClient, MESSAGE_GUARANTEED);
		}
		break;
		case MID_HANDSHAKE_LETMEIN :
		{
			// Get the client's password
			uint32 nClientPassword = pMsg->Readuint32();

#ifdef SOURCE_RELEASE

			uint32 nXORMask = GAME_HANDSHAKE_MASK;
			nClientPassword ^= nXORMask;
			uint32 nClientName = GAME_HANDSHAKE_PASSWORD;

#else // SOURCE_RELEASE

			// Encrypt the first 4 bytes of the client name
			uint32 nClientName = *((uint32*)( &szClientName[0] ));
			// Handle short client names...
			if ((nClientName && 0xFF) == 0)
				nClientName = 0;
			else if ((nClientName && 0xFF00) == 0)
				nClientName &= 0xFF;
			else if ((nClientName && 0xFF0000) == 0)
				nClientName &= 0xFFFF;
			uint32 nXORMask = MessUp32BitValue((uint32)this, (uint32)(szClientName[0]));
			nClientName ^= nXORMask;

#endif // SOURCE_RELEASE

#ifdef CHECK_FILE_CRC
			// Validate
			if (nClientName != nClientPassword)
			{
				// They're a fake!!
				g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
				g_pLTServer->KickClient(hClient);
				return;
			}
#endif

			// Read in their weapons file CRC
			uint32 nWeaponCRC = g_pWeaponMgr->GetFileCRC();
			nWeaponCRC ^= nXORMask;
			uint32 nClientCRC = pMsg->Readuint32();
			
#ifdef CHECK_FILE_CRC
			if (nWeaponCRC != nClientCRC)
			{
				// They're cheating!!
				g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
				g_pLTServer->KickClient(hClient);

				return;
			}
#endif

			// Read in their client file CRC
			uint32 nCShellCRC = m_nCShellCRC;
			nCShellCRC ^= nXORMask;
			nClientCRC = pMsg->Readuint32();

#ifdef CHECK_FILE_CRC
			if (nCShellCRC != nClientCRC)
			{
				// They're cheating!!
				g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
				g_pLTServer->KickClient(hClient);

				return;
			}
#endif

			// CRC the modelbutes.txt
			uint32 nModelButesMaskedCRC = m_nModelButesCRC ^ nXORMask;
			uint32 nClientModelButesCRC = pMsg->Readuint32();
#ifdef CHECK_FILE_CRC
			if( nClientModelButesCRC !=  nModelButesMaskedCRC )
			{
				// They're cheating!!
				g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
				g_pLTServer->KickClient(hClient);
				return;
			}
#endif

			// CRC the surface.txt
			uint32 nSurfaceMaskedCRC = m_nSurfaceCRC ^ nXORMask;
			uint32 nClientSurfaceCRC = pMsg->Readuint32();
#ifdef CHECK_FILE_CRC
			if( nClientSurfaceCRC !=  nSurfaceMaskedCRC )
			{
				// They're cheating!!
				g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
				g_pLTServer->KickClient(hClient);
				return;
			}
#endif

			// CRC the damagefx.txt
			uint32 nDamageFxMaskedCRC = m_nDamageFxCRC ^ nXORMask;
			uint32 nClientDamageFxCRC = pMsg->Readuint32();
#ifdef CHECK_FILE_CRC
			if( nClientDamageFxCRC !=  nDamageFxMaskedCRC )
			{
				// They're cheating!!
				g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
				g_pLTServer->KickClient(hClient);
				return;
			}
#endif
	
			uint32 nHashedPassword = pMsg->Readuint32();
			uint32 nServerHashedPassword = str_Hash(m_ServerGameOptions.m_sPassword.c_str( ));
			// Validate
			if (!bIsHost && m_ServerGameOptions.m_bUsePassword && nHashedPassword != nServerHashedPassword)
			{
				// They don't know the password
				// Tell the client...
				CAutoMessage cMsg;
				cMsg.Writeuint8(MID_HANDSHAKE);
				cMsg.Writeuint8((uint8)MID_HANDSHAKE_WRONGPASS);
				g_pLTServer->SendToClient(cMsg.Read(), hClient, MESSAGE_GUARANTEED);

				//and boot 'em
				//jrg - 9/4/02 in this particular case we let the client disconnect itself so that it can handle
				//		an incorrect password a little more smoothly
				g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
				g_pLTServer->KickClient(hClient);
				return;
			}


			// Tell the client they passed the test...
			CAutoMessage cMsg;
			cMsg.Writeuint8(MID_HANDSHAKE);
			cMsg.Writeuint8((uint8)MID_HANDSHAKE_DONE);
			g_pLTServer->SendToClient(cMsg.Read(), hClient, MESSAGE_GUARANTEED);


			// Unlock the player
			pPlayer->FinishHandshake();

			// Spawn them in..
			RespawnPlayer(pPlayer, hClient,false);
		}
		break;
		default :
		{
			// Hmm..  They sent over an invalid handshake message.  They're naughty.
			g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
			g_pLTServer->KickClient(hClient);
			return;
		}
	}

}


/***************************************************************************/

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::OnMessage()
//
//	PURPOSE:	Routes messages to a specific handler in an effort to reduce 
//				ungodly sized switch cases.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::OnMessage(HCLIENT hSender, ILTMessage_Read *pMsg)
{
	if( !pMsg )
		return;

	// Let the server missionmgr handle its messages.
	if( g_pServerMissionMgr )
	{
		if( g_pServerMissionMgr->OnMessage( hSender, *pMsg ))
			return;
	}

	// Let the server saveloadmgr handle its messages.
	if( g_pServerSaveLoadMgr )
	{
		if( g_pServerSaveLoadMgr->OnMessage( hSender, *pMsg ))
			return;
	}

	// Let the scmd check if it needs this message.
	if( ScmdServer::Instance( ).OnMessage( hSender, *pMsg ))
	{
		return;
	}

	// Update our clientupdate time if mp.
	if( IsMultiplayerGame( ) && !( g_pLTServer->GetClientInfoFlags( hSender ) & CIF_LOCAL ))
	{
		ClientData* pClientData = GetClientData( hSender );
		if( pClientData )
		{
			pClientData->m_nLastClientUpdateTime = GetTickCount( );
		}
	}

	pMsg->SeekTo(0);
	uint8 messageID = pMsg->Readuint8();
	
	switch (messageID)
	{
		case MID_PLAYER_UPDATE:				HandlePlayerUpdate				(hSender, pMsg);	break;
		case MID_WEAPON_FIRE:				HandleWeaponFire				(hSender, pMsg);	break;
		case MID_HIT_PLAYER:				HandleHitPlayer					(hSender, pMsg);	break;
		case MID_WEAPON_RELOAD:				HandleWeaponReload				(hSender, pMsg);	break;
		case MID_WEAPON_SOUND:				HandleWeaponSound				(hSender, pMsg);	break;
		case MID_WEAPON_SOUND_LOOP:			HandleWeaponSoundLoop			(hSender, pMsg);	break;
		case MID_WEAPON_CHANGE:				HandleWeaponChange				(hSender, pMsg);	break;
		case MID_PLAYER_ACTIVATE:			HandlePlayerActivate			(hSender, pMsg);	break;
		case MID_PLAYER_CLIENTMSG:			HandlePlayerClientMsg			(hSender, pMsg);	break;
		case MID_FRAG_SELF:					HandleFragSelf					(hSender, pMsg);	break;
		case MID_PLAYER_RESPAWN:			HandlePlayerRespawn				(hSender, pMsg);	break;
		case MID_CLIENT_LOADED:				HandleClientLoaded				(hSender, pMsg);	break;
		case MID_PLAYER_CHEAT:				HandlePlayerCheat				(hSender, pMsg);	break;
		case MID_PLAYER_TELEPORT:			HandlePlayerTeleport			(hSender, pMsg);	break;
		case MID_GAME_PAUSE:				HandleGamePause					(hSender, pMsg);	break;
		case MID_GAME_UNPAUSE:				HandleGameUnpause				(hSender, pMsg);	break;
		case MID_CONSOLE_TRIGGER:			HandleConsoleTrigger			(hSender, pMsg);	break;
		case MID_CONSOLE_COMMAND:			HandleConsoleCommand			(hSender, pMsg);	break;
		case MID_DIFFICULTY:				HandleDifficulty				(hSender, pMsg);	break;
#ifdef USE_AI
		case MID_STIMULUS:					HandleStimulus					(hSender, pMsg);	break;
		case MID_RENDER_STIMULUS:			HandleRenderStimulus			(hSender, pMsg);	break;
		case MID_ADD_GOAL:					HandleAddGoal					(hSender, pMsg);	break;
		case MID_REMOVE_GOAL:				HandleRemoveGoal				(hSender, pMsg);	break;
#endif
		case MID_GADGETTARGET:				HandleGadgetTarget				(hSender, pMsg);	break;
		case MID_SEARCH:					HandleSearch					(hSender, pMsg);	break;
		case MID_DECISION:					HandleDecision					(hSender, pMsg);	break;
		case MID_MULTIPLAYER_UPDATE:		HandleMultiplayerUpdate			(hSender, pMsg);	break;
		case MID_PLAYER_MULTIPLAYER_INIT:	HandleMultiplayerInit			(hSender, pMsg);	break;
		case MID_PLAYER_INFOCHANGE:			HandleMultiplayerPlayerUpdate	(hSender, pMsg);	break;
		case MID_HANDSHAKE:					HandleHandshake					(hSender, pMsg);	break;
		case MID_CLEAR_PROGRESSIVE_DAMAGE:	HandleClearProgressiveDamage	(hSender, pMsg);	break;
		case MID_PROJECTILE:				HandleProjectileMessage         (hSender, pMsg);	break;
		case MID_SAVE_GAME:					HandleSaveGame			        (hSender, pMsg);	break;
		case MID_OBJECT_MESSAGE:			HandleObjectMessage				(hSender, pMsg);	break;
		case MID_PERFORMANCE:				HandlePerformanceMessage		(hSender, pMsg);	break;
		case MID_PLAYER_ATTACH:				HandlePlayerAttachMessage		(hSender, pMsg);	break;
		case MID_PLAYER_KICKBAN:			HandlePlayerKickBan				(hSender, pMsg);	break;
		case MID_CHANGE_TEAM:				HandleChangeTeam				(hSender, pMsg);	break;
		default:							HandleDefault					(hSender, pMsg);	break;

	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Handle ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandlePlayerUpdate (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	// Update the player...

	CPlayerObj* pPlayer = GET_PLAYER(hSender);	
	
	if (pPlayer)
	{
		if (pPlayer->ClientUpdate(pMsg))
		{
			// Merged player position and update messages.
			pPlayer->HandlePlayerPositionMessage(pMsg); 
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Handle ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleWeaponFire (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	CPlayerObj* pPlayer = GET_PLAYER(hSender);
		
	if (pPlayer)
	{
		pPlayer->HandleWeaponFireMessage (pMsg);
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Handle ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleHitPlayer (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	CPlayerObj* pPlayer = GET_PLAYER(hSender);
		
	if (pPlayer)
	{
		pPlayer->HandleHitMessage (pMsg);
	}
}



// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CGameServerShell::HandleWeaponReload
//
//  PURPOSE:
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleWeaponReload(HCLIENT hSender, ILTMessage_Read *pMsg)
{
	CPlayerObj *pPlayer	= GET_PLAYER( hSender );
	if( pPlayer )
	{
		uint8 nAmmoId = pMsg->Readuint8();
		pPlayer->DoWeaponReload( nAmmoId );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Handle ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleWeaponSound (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	CPlayerObj* pPlayer = GET_PLAYER(hSender);

	if (pPlayer)
	{
		pPlayer->HandleWeaponSoundMessage (pMsg);
	}
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CGameServerShell::HandleWeaponSoundLoop
//
//  PURPOSE:	Relays message to specific player to handle
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleWeaponSoundLoop( HCLIENT hSender, ILTMessage_Read *pMsg )
{
	CPlayerObj *pPlayer = GET_PLAYER( hSender );

	if( pPlayer )
	{
		pPlayer->HandleWeaponSoundLoopMessage( pMsg );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleWeaponChange ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleWeaponChange (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	CPlayerObj* pPlayer = GET_PLAYER(hSender);

	if (pPlayer)
	{
		uint8 nWeaponId		= pMsg->Readuint8();
        uint8 nAmmoId		= pMsg->Readuint8();
		
		// 광식 : ClientShell 이 알고 있는 총알 개수로 맞추기
		//-----------------------------------------------------
		uint8 nAmmoCount	= pMsg->Readuint8();
		uint8 nClipInAmmo	= pMsg->Readuint8();
		//-----------------------------------------------------
		
		//pPlayer->DoWeaponChange(nWeaponId,nAmmoId);
		pPlayer->DoWeaponChange(nWeaponId,nAmmoId, nAmmoCount, nClipInAmmo);
		pPlayer->m_nCurWeaponClass	= g_pWeaponMgr->GetWeaponClass( nWeaponId );
		if( pPlayer->m_nCurWeaponClass > 0 ) --pPlayer->m_nCurWeaponClass;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandlePlayerActivate ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandlePlayerActivate (HCLIENT hSender, ILTMessage_Read *pMsg)
{
    CPlayerObj* pPlayer = GET_PLAYER(hSender);

	if (pPlayer)
	{
		pPlayer->HandleActivateMessage(pMsg);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandlePlayerClientMsg ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandlePlayerClientMsg (HCLIENT hSender, ILTMessage_Read *pMsg)
{
    CPlayerObj* pPlayer = GET_PLAYER(hSender);
	
	if (pPlayer)
	{
		pPlayer->HandleClientMsg(pMsg);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandlePlayerTeleport ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandlePlayerTeleport (HCLIENT hSender, ILTMessage_Read *pMsg)
{
    CPlayerObj* pPlayer = GET_PLAYER(hSender);
	
	if (pPlayer)
	{
		pPlayer->HandleTeleportMsg(pMsg);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleFragSelf ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleFragSelf (HCLIENT hSender, ILTMessage_Read *pMsg)
{
    CPlayerObj* pPlayer = GET_PLAYER(hSender);
	
	if (pPlayer)
	{
        LPBASECLASS pClass = g_pLTServer->HandleToObject(pPlayer->m_hObject);
		if (pClass)
		{
			DamageStruct damage;

			damage.eType	= DT_EXPLODE;
			damage.fDamage	= damage.kInfiniteDamage;
			damage.hDamager = pPlayer->m_hObject;
			damage.vDir.Init(0, 1, 0);

			damage.DoDamage(pClass, pPlayer->m_hObject);
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandlePlayerRespawn ()
//
//	PURPOSE:	when the player dies he is responed back into the world in
//				one of two states. the first state is PS_ALIVE where the 
//				player can participate in the match. the second state is
//				PS_GHOST where the player has no interaction with the 
//				in-game world and basically acts as a spectator.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandlePlayerRespawn (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	CPlayerObj* pPlayer = GET_PLAYER(hSender);
	
	// don't respawn player if he is alive (may cause problems for end of match spawns) 
	if (pPlayer && pPlayer->CanRespawn( ))
	{
		pPlayer->Respawn( false );
		pPlayer->ResetAfterDeath( );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleClientLoaded()
//
//	PURPOSE:	Sent from the client in response to a server message of
//				MID_CLIENT_LOADED.  Client sends this when it is fully
//				loaded and in the play state.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleClientLoaded( HCLIENT hSender, ILTMessage_Read *pMsg )
{
    CPlayerObj* pPlayer = GET_PLAYER(hSender);

	ASSERT( pPlayer && "No player associated with client!" );

	if( pPlayer )
	{
		pPlayer->SetClientLoaded( true );
		
	// Added By Bestline ==>
	// Time : 2004-06-22 오후 4:03:33
	//	RespawnPlayer( pPlayer, hSender );
		
	// Added By Bestline <==

	}	
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleMultiplayerUpdate ()
//
//	PURPOSE:	send all of the player states to everyone on the server. this
//				is basically just a state update.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleMultiplayerUpdate (HCLIENT hSender, ILTMessage_Read *pMsg)
{
    CPlayerObj* pPlayer = GET_PLAYER(hSender);

	if (pPlayer)
	{
		// Tell the new Client about all the clients already
		// on the server...

		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			CPlayerObj* pCurPlayer = GetPlayerFromClientList(m_aClients[i]);
			if (pCurPlayer)
			{
				SendPlayerInfoMsgToClients(pPlayer->GetClient(), pCurPlayer, MID_PI_UPDATE);
			}
		}
	}
} 

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleMultiplayerServerDir ()
//
//	PURPOSE:	Handle messages sent through the server directory interface
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleMultiplayerServerDir (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	// Only handle this message if we actually have a server directory
	if (!m_pServerDir)
		return;

	uint8 aNumericAddr[4];
	char aSourceAddr[16];
	uint16 nSourcePort;

	// Get the address and port of where this came from
	if (!hSender)
	{
		GetCurMessageSource(aNumericAddr, &nSourcePort);
	}
	else
	{
		g_pLTServer->GetClientAddr(hSender, aNumericAddr, &nSourcePort);
	}
	// Convert the address portion
	sprintf(aSourceAddr, "%d.%d.%d.%d", 
		(uint32)aNumericAddr[0],
		(uint32)aNumericAddr[1],
		(uint32)aNumericAddr[2],
		(uint32)aNumericAddr[3]);

	// Let the server directory handle it
	m_pServerDir->HandleNetMessage(*CLTMsgRef_Read(pMsg->SubMsg(pMsg->Tell())), aSourceAddr, nSourcePort);
} 

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleMultiplayerInit ()
//
//	PURPOSE:	initializes a clients player object when they join a server
//				and informs all other clients that someone has joined.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleMultiplayerInit (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	CPlayerObj* pNewPlayer = GET_PLAYER(hSender);

	if( !pNewPlayer )
		return;

		// try and init player from message
	if( !pNewPlayer->MultiplayerInit( pMsg ))
		return;

	SendPlayerInfoMsgToClients( hSender, pNewPlayer, MID_PI_UPDATE_FIRST );

	// Don't just broadcast the list.  That would send messages to clients that aren't fully connected yet.
	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pOldPlayer = *iter;
		if( pOldPlayer )
		{
			// Make sure the player has a client.
			if( pOldPlayer->GetClient( ) && pOldPlayer->RespawnCalled() )
			{

				// Send a message to each existing clients, letting them know a  new client has joined the game...

				SendPlayerInfoMsgToClients(pOldPlayer->GetClient( ), pNewPlayer, MID_PI_JOIN);

				// Tell the new Client about each client already on the server...
				if (pNewPlayer != pOldPlayer) // avoid telling the client about himself (shouldn't be necessary)
				{
					SendPlayerInfoMsgToClients(pNewPlayer->GetClient(), pOldPlayer, MID_PI_EXIST);
				}
				else
				{
					ASSERT( !"CGameServerShell::HandleMultiplayerInit() :  new player already respawned?" );
				}
			}
		}
		else
		{
			ASSERT( !"CGameServerShell::HandleMultiplayerInit() :  Invalid playerobj entry." );
		}

		iter++;
	}
	CHECK_PLAYEROBJ_END();

	// Resend all the line systems.
	LineSystem::ResendAll( );

	// now get the new player into the world
	pNewPlayer->Respawn( !!pNewPlayer->IsSpectatorMode() );

	if( pNewPlayer->IsSpectatorMode() )
	{
		pNewPlayer->SetSpectatorMode( LTFALSE );
		pNewPlayer->ChangeState( PS_GHOST );
		CAutoMessage cMsg;
#ifdef GAME_SHOW
		if( pNewPlayer->IsHostPlayer() )
		{
			cMsg.Writeuint8( MID_DETACH_OBSERVER );
		}
		else
		{
			cMsg.Writeuint8( MID_ATTACH_FIRST );
		}
#else
		cMsg.Writeuint8( MID_ATTACH_FIRST );
#endif
		HandlePlayerAttachMessage( hSender, cMsg.Read() );
		g_pCommonLT->SetObjectFlags(pNewPlayer->m_hObject, OFT_Flags, 0, FLAG_SOLID|FLAG_RAYHIT);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleMultiplayerPlayerUpdate ()
//
//	PURPOSE:	updates a clients player info and informs all other clients of the changes.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleMultiplayerPlayerUpdate (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	CPlayerObj* pPlayer = GET_PLAYER(hSender);

	if( !pPlayer )
		return;

	// try and update player from message
	if( !pPlayer->MultiplayerUpdate( pMsg ))
		return;

	// Send a message to all clients, letting them know a
	// new client has joined the game...

    SendPlayerInfoMsgToClients(LTNULL, pPlayer, MID_PI_UPDATE);

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleHandshake ()
//
//	PURPOSE:	the handshack process is a way to get the client and server
//				agree upon a communiction protocol. in cases like "Denile
//				Of Service" (DOS) attacks the client will be droped if they
//				dont respond with in a fixed amount of time.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleHandshake (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	ProcessHandshake(hSender, pMsg);
}


void CGameServerShell::HandlePlayerKickBan (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	int32 nServerIndex = pMsg->Readint32();
	KickBanIfExist( nServerIndex );
}

void CGameServerShell::HandleChangeTeam (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	uint8 nTeamID	= pMsg->Readuint8();
	uint8 nModelID	= pMsg->Readuint8();

	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayer = *iter;
		if( pPlayer && pPlayer->GetClient() == hSender )
		{
			pPlayer->ChangeTeam(nTeamID, nModelID );
			CHECK_PLAYEROBJ_END();
			return;
		}

		iter++;
	}
	CHECK_PLAYEROBJ_END();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandlePlayerCheat ()
//
//	PURPOSE:	this handler is called when the player enters a cheat code
//				into the console. it checks some validity issues and then 
//				routes the cheat to the appropriat handler
//
// ----------------------------------------------------------------------- //



void CGameServerShell::HandlePlayerCheat (HCLIENT hSender, ILTMessage_Read *pMsg)
{

#ifndef _FINAL

	// get a pointer to the sender's player object

    CPlayerObj* pPlayer = GET_PLAYER(hSender);

	if (!pPlayer) return;

	// retrieve message data

    CheatCode nCheatCode = (CheatCode) pMsg->Readuint8();
    uint32 nData = pMsg->Readuint32();

	// now deal with the specific cheat code

	if (nCheatCode <= CHEAT_NONE || nCheatCode >= CHEAT_MAX) return; // wont the default case catch these?

	switch (nCheatCode)				// why always send the data? it was easier to cut & paste
	{	
		case CHEAT_GOD:				CheatGod		(pPlayer, nData);	break;
		case CHEAT_AMMO:			CheatAmmo		(pPlayer, nData);	break;
		case CHEAT_ARMOR:			CheatArmor		(pPlayer, nData);	break;
		case CHEAT_HEALTH:			CheatHealth		(pPlayer, nData);	break;
		case CHEAT_CLIP:			CheatClip		(pPlayer, nData);	break;
		case CHEAT_INVISIBLE:		CheatInvisible	(pPlayer, nData);	break;
		case CHEAT_TELEPORT:		CheatTeleport	(pPlayer, nData);	break;
		case CHEAT_FULL_WEAPONS:	CheatWeapons	(pPlayer, nData);	break;
		case CHEAT_TEARS:			CheatTears		(pPlayer, nData);	break;
		case CHEAT_REMOVEAI:		CheatRemoveAI	(pPlayer, nData);	break;
		case CHEAT_SNOWMOBILE:		CheatSnowmobile	(pPlayer, nData);	break;
		case CHEAT_MODSQUAD:		CheatModSquad	(pPlayer, nData);	break;
		case CHEAT_FULL_GEAR:		CheatFullGear	(pPlayer, nData);	break;
		case CHEAT_EXITLEVEL:		CheatExitLevel	(pPlayer, nData);	break;
		case CHEAT_NEXTMISSION:		CheatNextMission(pPlayer, nData);	break;
		case CHEAT_BOOT:			CheatBootPlayer	(pPlayer, nData);	break;
		case CHEAT_GIMMEGUN:		CheatGimmeGun	(pPlayer, nData);	break;
		case CHEAT_GIMMEMOD:		CheatGimmeMod	(pPlayer, nData);	break;
		case CHEAT_GIMMEGEAR:		CheatGimmeGear	(pPlayer, nData);	break;
		case CHEAT_GIMMEAMMO:		CheatGimmeAmmo	(pPlayer, nData);	break;
		case CHEAT_SKILLZ:			CheatSkillz		(pPlayer, nData);	break;
		case CHEAT_BODYGOLFING:		CheatBodyGolfing(pPlayer, nData);	break;
		default:					CheatDefault	(pPlayer, nData);	break; // good way to catch bugs, we hate those default cheaters
	}

#endif	//#ifndef _FINAL

}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleGamePause ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleGamePause (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	ClientPauseGame( true );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleGameUnpause ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleGameUnpause (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	ClientPauseGame( false );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleConsoleTrigger ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleConsoleTrigger (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	CPlayerObj* pPlayer = GET_PLAYER(hSender);

	if (!pPlayer)
	{
		return;
	}

	// Read the message data...

    HSTRING hstrObjName = pMsg->ReadHString();
    HSTRING hstrCommand = pMsg->ReadHString();

    const char* pName = LTNULL;
    const char* pCommand  = LTNULL;

	if (hstrObjName)
	{
        pName = g_pLTServer->GetStringData(hstrObjName);
	}

	if (hstrCommand)
	{
        pCommand = g_pLTServer->GetStringData(hstrCommand);
	}

	// Special case if we're supposed to list objects of a certain type...

	if (_strnicmp(pCommand, "LIST", 4) == 0)
	{
		ConParse parse;
		parse.Init(pCommand);

        LTBOOL bNoObjects = LTTRUE;

		if (g_pCommonLT->Parse(&parse) == LT_OK)
		{
			if (parse.m_nArgs > 1)
			{
                g_pLTServer->CPrint("Listing objects of type '%s'", parse.m_Args[1]);

				LTVector vPos;
                HCLASS  hClass = g_pLTServer->GetClass(parse.m_Args[1]);

				// Get the names of all the objects of the specified class...

                HOBJECT hObj = g_pLTServer->GetNextObject(LTNULL);
				while (hObj)
				{
                    if (g_pLTServer->IsKindOf(g_pLTServer->GetObjectClass(hObj), hClass))
					{
						g_pLTServer->GetObjectPos( hObj, &vPos );
                        g_pLTServer->CPrint("%s (active) Pos<%.3f,%.3f,%.3f>", GetObjectName(hObj), 
							VEC_EXPAND( vPos ));
                        bNoObjects = LTFALSE;
					}

                    hObj = g_pLTServer->GetNextObject(hObj);
				}

                hObj = g_pLTServer->GetNextInactiveObject(LTNULL);
				while (hObj)
				{
                    if (g_pLTServer->IsKindOf(g_pLTServer->GetObjectClass(hObj), hClass))
					{
						g_pLTServer->GetObjectPos( hObj, &vPos );
                        g_pLTServer->CPrint("%s (inactive) Pos<%.3f,%.3f,%.3f>", GetObjectName(hObj), 
							VEC_EXPAND( vPos ));
                        bNoObjects = LTFALSE;
					}

                    hObj = g_pLTServer->GetNextInactiveObject(hObj);
				}

				CLiteObjectMgr::TObjectList aLiteObjects;
				g_pGameServerShell->GetLiteObjectMgr()->GetObjectsOfClass(hClass, &aLiteObjects);
				CLiteObjectMgr::TObjectList::iterator iCurObj = aLiteObjects.begin();
				for (; iCurObj != aLiteObjects.end(); ++iCurObj)
				{
                    g_pLTServer->CPrint("%s %s", (*iCurObj)->GetName(), (*iCurObj)->IsActive() ? "(active)" : "(inactive)");
				}
				bNoObjects |= !aLiteObjects.empty();

				if (bNoObjects)
				{
                    g_pLTServer->CPrint("No objects of type '%s' exist (NOTE: object type IS case-sensitive)", parse.m_Args[1]);
				}
			}
		}
	}
	// Send the message to all appropriate objects...
	else if (SendTriggerMsgToObjects(pPlayer, hstrObjName, hstrCommand))
	{
        g_pLTServer->CPrint("Sent '%s' Msg '%s'", pName ? pName : "Invalid Object!", pCommand ? pCommand : "Empty Message!!!");
	}
	else
	{
        g_pLTServer->CPrint("Failed to Send '%s' Msg '%s'!", pName ? pName : "Invalid Object!", pCommand ? pCommand : "Empty Message!!!");
	}

	if (hstrObjName)
	{
        g_pLTServer->FreeString(hstrObjName);
	}

	if (hstrCommand)
	{
        g_pLTServer->FreeString(hstrCommand);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleConsoleCommand ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleConsoleCommand (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	HSTRING hCommand = pMsg->ReadHString();

	if (!hCommand) return;

    if (m_CmdMgr.Process(g_pLTServer->GetStringData(hCommand), (ILTBaseClass*)NULL, (ILTBaseClass*)NULL))
	{
        g_pLTServer->CPrint("Sent Command '%s'", g_pLTServer->GetStringData(hCommand));
	}

	if (hCommand)
	{
        g_pLTServer->FreeString(hCommand);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleDifficulty ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleDifficulty (HCLIENT hSender, ILTMessage_Read *pMsg)
{

	if (!IsDifficultyGameType()) return;

	bool bIsHost = g_pLTServer->GetClientInfoFlags(hSender) & CIF_LOCAL;
	if (!bIsHost) return;


	m_eDifficulty = (GameDifficulty)pMsg->Readuint8();
	g_pLTServer->CPrint("Difficulty:%d",(int)m_eDifficulty);
}

#ifdef USE_AI
// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleStimulus()
//
//	PURPOSE:	Handle a stimulus message by placing one in the world.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleStimulus(HCLIENT hSender, ILTMessage_Read *pMsg)
{
	uint8 cArgs = pMsg->Readuint8();

	char szStimType[64];
	pMsg->ReadString(szStimType, 64);
	EnumAIStimulusType eStimType = CAIStimulusMgr::StimulusFromString(szStimType);

	float fMultiplier = (cArgs == 2) ? pMsg->Readfloat() : 1.f;

	// Place stimulus at the location of the player.
	LTVector vPos;
	CPlayerObj *pPlayer = g_pCharacterMgr->FindPlayer();
	g_pLTServer->GetObjectPos(pPlayer->m_hObject, &vPos);

	g_pAIStimulusMgr->RegisterStimulus(eStimType, pPlayer->m_hObject, vPos, fMultiplier);

	g_pLTServer->CPrint("Stimulus: %s at (%.2f, %.2f, %.2f) x%.2f", szStimType, vPos.x, vPos.y, vPos.z, fMultiplier);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleRenderStimulus()
//
//	PURPOSE:	Handle a render stimulus message by toggling stimulus rendering.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleRenderStimulus(HCLIENT hSender, ILTMessage_Read *pMsg)
{
	LTBOOL bRender = (LTBOOL)pMsg->Readuint8();

	g_pAIStimulusMgr->RenderStimulus(bRender);

	g_pLTServer->CPrint("Stimulus Rendering: %d", bRender);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleAddGoal()
//
//	PURPOSE:	Handle an add goal message by adding a goal to an AI.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleAddGoal(HCLIENT hSender, ILTMessage_Read *pMsg)
{
	HOBJECT hAI;

	char szAIName[32];
	pMsg->ReadString(szAIName, 32);
	if ( LT_OK == FindNamedObject(szAIName, hAI) )
	{
		char szGoalType[64];
		pMsg->ReadString(szGoalType, 64);

		char szMsg[128];
		sprintf(szMsg, "AddGoal %s", szGoalType);

		char szNameValuePair[64];
		uint32 cNameValuePairs = pMsg->Readuint32();
		for(uint32 iNameValuePair=0; iNameValuePair < cNameValuePairs; ++iNameValuePair)
		{
			pMsg->ReadString(szNameValuePair, 64);
			strcat(szMsg, " ");
			strcat(szMsg, szNameValuePair);
		}

		CAI* pAI = (CAI*)g_pLTServer->HandleToObject(hAI);
		SendTriggerMsgToObject(pAI, hAI, LTFALSE, szMsg);
	}
	else {
		g_pLTServer->CPrint("Could not find AI named: %s", szAIName);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleRemoveGoal()
//
//	PURPOSE:	Handle a remove goal message by removing a goal from an AI.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleRemoveGoal(HCLIENT hSender, ILTMessage_Read *pMsg)
{
	HOBJECT hAI;

	char szAIName[32];
	pMsg->ReadString(szAIName, 32);
	if ( LT_OK == FindNamedObject(szAIName, hAI) )
	{
		char szGoalType[64];
		pMsg->ReadString(szGoalType, 64);

		char szMsg[128];
		sprintf(szMsg, "RemoveGoal %s", szGoalType);

		CAI* pAI = (CAI*)g_pLTServer->HandleToObject(hAI);
		SendTriggerMsgToObject(pAI, hAI, LTFALSE, szMsg);
	}
	else {
		g_pLTServer->CPrint("Could not find AI named: %s", szAIName);
	}
}

#endif

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CGameServerShell::HandleGadgetTarget
//
//  PURPOSE:	Send a stopped message to the target.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleGadgetTarget( HCLIENT hSender, ILTMessage_Read *pMsg )
{
	HOBJECT hTarget = pMsg->ReadObject( );
	LTFLOAT	fTime	= pMsg->Readfloat( );

	if( hTarget )
	{
		char szMsg[32] = {0};
		sprintf( szMsg, "Stopped %f", fTime );

		GadgetTarget *pGadgetTarget = (GadgetTarget*)g_pLTServer->HandleToObject( hTarget );
		if( pGadgetTarget )
		{
			CPlayerObj* pPlayerObj = GET_PLAYER(hSender);
			pGadgetTarget->KillDisablingSound();
			SendTriggerMsgToObject( pPlayerObj, hTarget, LTFALSE, szMsg );
		}
	}
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CGameServerShell::HandleSearch
//
//  PURPOSE:	Send an update message to the target.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleSearch( HCLIENT hSender, ILTMessage_Read *pMsg )
{
	HOBJECT hTarget = pMsg->ReadObject( );
	LTFLOAT	fTime	= pMsg->Readfloat( );

	if( hTarget )
	{
		char szMsg[32] = {0};
		sprintf( szMsg, "update %f", fTime );

		CPlayerObj* pPlayer = GET_PLAYER(hSender);

		if( pPlayer )
		{
			SendTriggerMsgToObject( pPlayer, hTarget, LTFALSE, szMsg );
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleDecision()
//
//	PURPOSE:	Handle Decision object choices
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleDecision(HCLIENT hSender, ILTMessage_Read *pMsg)
{
	HOBJECT hObj = pMsg->ReadObject();
	uint8 nChoice = pMsg->Readuint8();

	DecisionObject *pDecisionObject = (DecisionObject*)g_pLTServer->HandleToObject( hObj );
	if( pDecisionObject )
	{
		pDecisionObject->HandleChoose(nChoice);
	}

}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CGameServerShell::HandleClearDamage
//
//  PURPOSE:	Clear the specified damage for the client
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleClearProgressiveDamage( HCLIENT hSender, ILTMessage_Read *pMsg )
{
	CPlayerObj* pPlayer = GET_PLAYER(hSender);

	if( pPlayer )
	{
		CDestructible	*pDamage = 	pPlayer->GetDestructible();
		DamageFlags		nDmgFlags = pMsg->Readuint64();
	
		pDamage->ClearProgressiveDamage( nDmgFlags );
	}
}


// ----------------------------------------------------------------------- //
//
//	FUNCTION:	CGameServerShell::HandleProjectileMessage
//
//	PURPOSE:	Handles messages from the client to projectiles
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleProjectileMessage( HCLIENT hSender, ILTMessage_Read *pMsg )
{
	LTRESULT ltResult;

	CPlayerObj* pPlayer = GET_PLAYER( hSender );

	// CLTMsgRef_Read cSubMsg(pMsg->SubMsg(pMsg->Tell()));
	if ( pPlayer )
	{
		ltResult = g_pLTServer->SendToObject(
			pMsg,
			LTNULL,
				g_pLTServer->ObjectToHandle( pPlayer ),
			MESSAGE_GUARANTEED
		);
		ASSERT( LT_OK == ltResult );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleDefault ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleDefault (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	// unhandled message?
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatGod (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return; // yeah, this is redundant but hell, cheats aren't called every frame

	pPlayer->ToggleGodMode ();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatAmmo (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	pPlayer->FullAmmoCheat ();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatArmor (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	pPlayer->RepairArmorCheat ();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatHealth (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	pPlayer->HealCheat ();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatClip()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatClip (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	pPlayer->SetSpectatorMode(nData);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatInvisible()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatInvisible(CPlayerObj* pPlayer, uint32 nData)
{
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatTeleport (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	pPlayer->Respawn (false);	
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatWeapons (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	pPlayer->FullWeaponCheat ();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatTears (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	if ((g_bInfiniteAmmo = !g_bInfiniteAmmo)) pPlayer->FullWeaponCheat();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatSkillz (CPlayerObj* pPlayer, uint32 nData)
{
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatRemoveAI (CPlayerObj* pPlayer, uint32 nData)
{
	HandleCheatRemoveAI(nData);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatSnowmobile (CPlayerObj* pPlayer, uint32 nData)
{
	if (!g_pLTServer) return; // if this is true you are really screwed

	g_pLTServer->RunGameConString("spawnobject PlayerVehicle (VehicleType Snowmobile)");
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatModSquad (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	pPlayer->FullModsCheat();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatFullGear (CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	pPlayer->FullGearCheat();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatExitLevel()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatExitLevel(CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	bool bIsHost = g_pLTServer->GetClientInfoFlags(pPlayer->GetClient()) & CIF_LOCAL;
	if (!bIsHost) return;

	g_pServerMissionMgr->ExitLevelSwitch( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatNextMission()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatNextMission(CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer) return;

	bool bIsHost = g_pLTServer->GetClientInfoFlags(pPlayer->GetClient()) & CIF_LOCAL;
	if (!bIsHost) return;


	g_pServerMissionMgr->NextMission( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatBootPlayer()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatBootPlayer(CPlayerObj* pPlayer, uint32 nData)
{
	if (!pPlayer || !IsMultiplayerGame() ) return;

	bool bIsHost = g_pLTServer->GetClientInfoFlags(pPlayer->GetClient()) & CIF_LOCAL;
	if (!bIsHost) return;

	//don't boot yourself
	uint32 clientID = g_pLTServer->GetClientID(pPlayer->GetClient());
	if (clientID == nData) return;

	HCLIENT hClient = GetClientFromID(nData);
	if (hClient)
	{
		g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
		g_pLTServer->KickClient(hClient);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatGimmeGun()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- 

void CGameServerShell::CheatGimmeGun( CPlayerObj *pPlayer, uint32 nData )
{
	if( !pPlayer )
		return;

	pPlayer->GimmeGunCheat( nData );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatGimmeMod()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- 

void CGameServerShell::CheatGimmeMod( CPlayerObj *pPlayer, uint32 nData )
{
	if( !pPlayer )
		return;

	pPlayer->GimmeModCheat( nData );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatGimmeGear()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- 

void CGameServerShell::CheatGimmeGear( CPlayerObj *pPlayer, uint32 nData )
{
	if( !pPlayer )
		return;

	pPlayer->GimmeGearCheat( nData );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatGimmeAmmo()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- 

void CGameServerShell::CheatGimmeAmmo( CPlayerObj *pPlayer, uint32 nData )
{
	if( !pPlayer )
		return;

	pPlayer->GimmeAmmoCheat( nData );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CheatBodyGolfing ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatBodyGolfing (CPlayerObj* pPlayer, uint32 nData)
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Cheat ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CheatDefault (CPlayerObj* pPlayer, uint32 nData)
{
	// some how i don't think we are in kansas any more toto
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::FindStartPoint()
//
//	PURPOSE:	Find a good start point.
//
// ----------------------------------------------------------------------- //

GameStartPoint* CGameServerShell::FindStartPoint(CPlayerObj* pPlayer)
{
	// We need at least one start point...

	if( GameStartPoint::GetStartPointList().size() == 0 )
    {
        return LTNULL;
    }

    // Build a list of unlocked start points...

	GameStartPoint* pStartPt = LTNULL;
	GameStartPoint::StartPointList lstValidStartPts;

	bool bStartPointsOnly = pPlayer ? !pPlayer->UsedStartPoint() : true;

	GameStartPoint::StartPointList::const_iterator iter = GameStartPoint::GetStartPointList().begin();
	while( iter != GameStartPoint::GetStartPointList().end() )
	{
		pStartPt = *iter;
		if( !pStartPt->IsLocked() )
		{
				if( (bStartPointsOnly && pStartPt->IsStartPoint()) || (!bStartPointsOnly && pStartPt->IsSpawnPoint()) )
				{
					lstValidStartPts.push_back( pStartPt );
				}
		}

		++iter;
	}

	// Sort the list based on last time used...

	std::sort(lstValidStartPts.begin(),lstValidStartPts.end(),GameStartPointLesser() );

    pStartPt = LTNULL;
	int nCount = lstValidStartPts.size();

	// Get the index into the valid list based on game type...

	if( nCount > 1 )
	{
		int nIndex = 0;
		switch( GetGameType() )
		{
			case eGameTypeSingle:
			{
				nIndex = 0;
			}
			break;

			case eGameTypeCooperative:
            {
				nIndex = nCount+1;
				int nRetries = nCount;
				LTBOOL bSafe = LTFALSE;
				while (nRetries && !bSafe)
				{
					if (nIndex > nCount)
					{
						nIndex = GetRandom(0, nCount-1);
					}
					else
					{
						nIndex = (nIndex+1)%nCount;
					}
					
					pStartPt = lstValidStartPts[ nIndex ];

					LTVector testPos;
					g_pLTServer->GetObjectPos(pStartPt->m_hObject, &testPos);
					bSafe = !IsPositionOccupied(testPos,pPlayer);
					nRetries--;
				}
            }
			break;

			case eGameTypeDeathmatch:
            {

				nIndex = GetRandom(0,1);
				bool bSafe = false;
				while (nIndex < nCount && !bSafe)
				{

					pStartPt = lstValidStartPts[ nIndex ];
					LTVector testPos;
					g_pLTServer->GetObjectPos(pStartPt->m_hObject, &testPos);
					bSafe = !IsPositionOccupied(testPos,pPlayer);
					if (!bSafe)
					{
						g_pLTServer->CPrint("CGameServerShell::FindStartPoint(): option %d occupied",nIndex);
						nIndex++;
					}

				}
				
				if (!bSafe)
					nIndex = 0;

            }
			break;

			case eGameTypeTeamDeathmatch:
			case eGameTypeDoomsDay:
			case eGameTypeSurvival :
			{
				// Try and get an available team start point...
				
				nIndex = 0;
				bool bSafe = false;

				if( pPlayer )
				{

					while( nIndex < nCount && !bSafe )
					{
						pStartPt = lstValidStartPts[ nIndex ];

						LTVector vPos;
						g_pLTServer->GetObjectPos( pStartPt->m_hObject, &vPos );

						bSafe = !IsPositionOccupied( vPos, pPlayer ) && (pStartPt->GetTeamID() == pPlayer->GetTeamID());
						if( !bSafe )
						{
							++nIndex;
						}
					}
					
					if( !bSafe )
					{
						// No available team start point so just get the first team start point...

						nIndex = 0;
						while( nIndex < nCount && !bSafe )
						{
							pStartPt = lstValidStartPts[ nIndex ];

 							bSafe = (pStartPt->GetTeamID() == pPlayer->GetTeamID());
							TRACE("%s(%d) : Find Safe Start Point With StartPointTeamID(%d) == PlayerTeamID(%d)\n",__FILE__,__LINE__,pStartPt->GetTeamID(),pPlayer->GetTeamID());
							if( !bSafe )
							{
								++nIndex;
							}
						}
					}
				}

				if( !bSafe )
				{
					//g_pLTServer->CPrint( "No Team start point for team (%i)!", pPlayer ? pPlayer->GetTeamID() : INVALID_TEAM );
					
					nIndex = GetRandom(0,1);
					bool bSafe = false;
					while (nIndex < nCount && !bSafe)
					{

						pStartPt = lstValidStartPts[ nIndex ];
						LTVector testPos;
						g_pLTServer->GetObjectPos(pStartPt->m_hObject, &testPos);
						bSafe = !IsPositionOccupied(testPos,pPlayer);
						if (!bSafe)
						{
							g_pLTServer->CPrint("CGameServerShell::FindStartPoint(): option %d occupied",nIndex);
							nIndex++;
						}
					}
				
					/*
					if (!bSafe)
						nIndex = 0;
						*/
					// 광식 : 마지막에는 잘 못 되면 다른 팀 Respawn 지역에서 살아나겠군..
					//-------------------------------------------------------
					int testCount = 0;
					while( testCount < 10 && !bSafe )
					{
						nIndex = GetRandom(0, nCount-1);
						pStartPt = lstValidStartPts[ nIndex ];
						bSafe = (pStartPt->GetTeamID() == pPlayer->GetTeamID());
						++testCount;
					}					
					//-------------------------------------------------------
				}
            }
			break;

			default : break;
		}

		pStartPt = lstValidStartPts[ nIndex ];
	}
	else
	{
		// We know there is at least one valid start point.
		
		pStartPt = GameStartPoint::GetStartPointList()[ 0 ];
	}

	if (pStartPt)
	{
		pStartPt->SetLastUse(g_pLTServer->GetTime());

		if( pPlayer )
			pPlayer->SetUsedStartPoint( true );
	}

	return pStartPt;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::RespawnPlayer()
//
//	PURPOSE:	Respawn the player object
//
// ----------------------------------------------------------------------- //

void CGameServerShell::RespawnPlayer(CPlayerObj* pPlayer, HCLIENT hClient,bool bCreateFX)
{
	if (!pPlayer || !hClient) return;


	// Player object meet client, client meet player object...

	pPlayer->SetClient(hClient,bCreateFX);
    g_pLTServer->SetClientUserData(hClient, (void *)pPlayer);

	// If this is a multiplayer game wait until MULTIPLAYER_INIT message
	// to respawn the player...

	if (!pPlayer->HasDoneHandshake())
	{
		// Start a handshake with the client
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_HANDSHAKE);
		cMsg.Writeuint8(MID_HANDSHAKE_HELLO);
		cMsg.Writeuint16(GAME_HANDSHAKE_VER);
		g_pLTServer->SendToClient(cMsg.Read(), hClient, MESSAGE_GUARANTEED);
	}
	else
	{
		// Tell the client to init us!
		SendEmptyClientMsg(MID_PLAYER_MULTIPLAYER_INIT, hClient, MESSAGE_GUARANTEED);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleCheatRemoveAI()
//
//	PURPOSE:	Handle the remove ai cheat
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleCheatRemoveAI(uint32 nData)
{
    HOBJECT hObj   = g_pLTServer->GetNextObject(LTNULL);
    HCLASS  hClass = g_pLTServer->GetClass("CAI");

	// Remove all the ai objects...

    LTBOOL bRemove = LTFALSE;

    HOBJECT hRemoveObj = LTNULL;
	while (hObj)
	{
        if (g_pLTServer->IsKindOf(g_pLTServer->GetObjectClass(hObj), hClass))
		{
			hRemoveObj = hObj;
		}

        hObj = g_pLTServer->GetNextObject(hObj);

		if (hRemoveObj)
		{
            LPBASECLASS pClass = g_pLTServer->HandleToObject(hRemoveObj);
			if (pClass)
			{
				DamageStruct damage;

				damage.eType	= DT_EXPLODE;
				damage.fDamage	= damage.kInfiniteDamage;
				damage.hDamager = hRemoveObj;

				damage.DoDamage(pClass, hRemoveObj);
			}

            hRemoveObj = LTNULL;
		}
	}


    hObj = g_pLTServer->GetNextInactiveObject(LTNULL);
    hRemoveObj = LTNULL;
	while (hObj)
	{
        if (g_pLTServer->IsKindOf(g_pLTServer->GetObjectClass(hObj), hClass))
		{
			hRemoveObj = hObj;
		}

        hObj = g_pLTServer->GetNextInactiveObject(hObj);

		if (hRemoveObj)
		{
            LPBASECLASS pClass = g_pLTServer->HandleToObject(hRemoveObj);
			if (pClass)
			{
				DamageStruct damage;

				damage.eType	= DT_EXPLODE;
				damage.fDamage	= damage.kInfiniteDamage;
				damage.hDamager = hRemoveObj;

				damage.DoDamage(pClass, hRemoveObj);
			}

            hRemoveObj = LTNULL;
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ReportError()
//
//	PURPOSE:	Tell the client about a server error
//
// ----------------------------------------------------------------------- //

void CGameServerShell::ReportError(HCLIENT hSender, uint8 nErrorType)
{
	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_SERVER_ERROR);
	cMsg.Writeuint8((uint8)nErrorType);
	g_pLTServer->SendToClient(cMsg.Read(), hSender, MESSAGE_GUARANTEED);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CacheFiles()
//
//	PURPOSE:	Cache files that are used often
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CacheFiles()
{
	if (m_bPreCacheAssets)
	{

#ifndef _FINAL
		g_pLTServer->CPrint("Caching files...");
#endif
		CacheLevelSpecificFiles();

#ifndef _FINAL
		g_pLTServer->CPrint("Done caching files...");
#endif

	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::CacheLevelSpecificFiles()
//
//	PURPOSE:	Cache files specific to the current level...
//
// ----------------------------------------------------------------------- //

void CGameServerShell::CacheLevelSpecificFiles()
{
	const char* pCurLevelName = GetCurLevel();

	if (m_bCachedLevelFiles || !pCurLevelName || !(*pCurLevelName)) return;

	char pTempCurLevel[MAX_PATH + 1];
	LTStrCpy(pTempCurLevel, pCurLevelName, sizeof(pTempCurLevel));

	// Chop off any suffix...

    char *pDot = strchr(pTempCurLevel, '.');
    if (pDot)
	{
        *pDot = '\0';
	}

	// The asset file should be in with the world files...

    char fname[MAX_PATH + 1];
    LTSNPrintF(fname, sizeof(fname), "%s.txt", pTempCurLevel);

	
    // Have the asset mgr handle the loading of the assets...

 	CServerAssetMgr assetMgr;
    if (!assetMgr.Init(fname))
	{ 
#ifndef _FINAL
		g_pLTServer->CPrint("CServerAssetMgr::Init() falied for file: %s", fname);
#endif
	}

	// Reset so we don't try and cache again until the next level is loaded...

	m_bCachedLevelFiles = true;
}

void CGameServerShell::UpdateClientPingTimes()
{
	/*
	float ping;
    uint32 clientID;
	HCLIENT hClient;

    m_ClientPingSendCounter += g_pLTServer->GetFrameTime();
	if(m_ClientPingSendCounter > CLIENT_PING_UPDATE_RATE)
	{
		CAutoMessage cMsg;

		cMsg.Writeuint8(MID_PINGTIMES);

        hClient = LTNULL;
        while(hClient = g_pLTServer->GetNextClient(hClient))
		{
            clientID = g_pLTServer->GetClientID(hClient);
            g_pLTServer->GetClientPing(hClient, ping);

			cMsg.Writeuint16((uint16)clientID);
			cMsg.Writeuint16((uint16)(ping + 0.5f));
		}

		cMsg.Writeuint16(0xFFFF);
		g_pLTServer->SendToClient(cMsg.Read(), LTNULL, 0);

		m_ClientPingSendCounter = 0.0f;
	}
	*/
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::SetSwitchingWorldsState
//
//	PURPOSE:	Sets the switching worlds state and tells it to the clients.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::SetSwitchingWorldsState( SwitchingWorldsState eSwitchingWorldsState )
{
	if( m_eSwitchingWorldsState == eSwitchingWorldsState )
		return;

	m_eSwitchingWorldsState = eSwitchingWorldsState;

	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_SWITCHINGWORLDSSTATE );
	cMsg.Writeuint8( eSwitchingWorldsState );
	g_pLTServer->SendToClient( cMsg.Read(), NULL, MESSAGE_GUARANTEED);
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::StartSwitchingWorlds
//
//	PURPOSE:	Finish our switch world process.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::StartSwitchingWorlds( )
{
	// Pause the game until all the clients are in.  There is an unpause
	// in FinishSwitchingWorlds.
	PauseGame( true );
	SetSwitchingWorldsState( eSwitchingWorldsStatePlayerHookup );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::FinishSwitchingWorlds
//
//	PURPOSE:	Finish our switch world process.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::FinishSwitchingWorlds( )
{
	// Remove our pause we put on in StartSwitchingWorlds.
	PauseGame( false );
	
	// Done switching worlds.
	SetSwitchingWorldsState( eSwitchingWorldsStateFinished );

	g_pServerMissionMgr->LevelStarted();

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::UpdateSwitchingWorlds
//
//	PURPOSE:	Update our switch world state machine.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::UpdateSwitchingWorlds( )
{
	switch( m_eSwitchingWorldsState )
	{
		case eSwitchingWorldsStateNone:
		{
			return;
		}
		break;

		// Waiting for clients from the previous level to 
		// get hooked up to players in this level.
		case eSwitchingWorldsStatePlayerHookup:
		{
			SetSwitchingWorldsState( eSwitchingWorldsStateWaitForClient );
			/*
			// Loop through the clients that don't have players bound to them.  If they
			// match any of the clientrefs, we have to wait until they get their
			// OnClientEnterWorld called.  If none of the clients match a clientref, then
			// we're done with the switchworld.
			HCLIENT hNextClient = g_pLTServer->GetNextClient( LTNULL );
			bool bWaitLonger = false;
			while( hNextClient )
			{
				// Copy the current client and advance to the next.  Makes body of loop easier.
				HCLIENT hClient = hNextClient;
				hNextClient = g_pLTServer->GetNextClient( hClient );

				// Check the client's connection.
				if( ClientConnectionInTrouble( hClient ))
				{
					// Kick the client if they are too slow to play.
					g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
					g_pLTServer->KickClient( hClient );
					continue;
				}

				// Check if this client has already been hooked up to a player.  If so, skip it.
				CPlayerObj* pUsedPlayer = ( CPlayerObj* )g_pLTServer->GetClientUserData( hClient );
				if( pUsedPlayer )
				{
					// If the player hasn't respawned we need to wait until they do...
					if( pUsedPlayer->RespawnCalled() )
					{
						continue;
					}

					bWaitLonger = true;
					break;
				}

				// If there are no clientrefs, then we didn't come from a previous
				// level with players. This guy is just joining and 
				// we need to wait longer for him to finish handshaking.
				HCLIENTREF hNextClientRef = g_pLTServer->GetNextClientRef(LTNULL);
				if (!hNextClientRef)
				{
					bWaitLonger = true;
					break;
				}

				// See if this client will match any clientrefs.  If so, then we have to wait
				// for the OnClientEnterWorld before we can finish the switch world.
				pUsedPlayer = PickClientRefPlayer( hClient );
				if( pUsedPlayer )
				{
					bWaitLonger = true;
					break;
				}
			}

			// See if we found didn't find a player.
			if( bWaitLonger )
				return;

			// Do an autosave if we aren't loading from a save game.
			if( m_nLastLGFlags != LOAD_RESTORE_GAME )
			{
				// No more unclaimed clientrefs.  Now do the autosave.
				if( !g_pServerSaveLoadMgr->CanSaveGame( ) )
				{
					// Can't save, so skip the autosave state.
					SetSwitchingWorldsState( eSwitchingWorldsStateWaitForClient );
					return;
				}

				// Do an autosave.
				g_pServerSaveLoadMgr->AutoSave( g_pServerMissionMgr->GetCurrentMission( ), 
					g_pServerMissionMgr->GetCurrentLevel( ));

				SetSwitchingWorldsState( eSwitchingWorldsStateAutoSave );
			}
			else
			{
				// Wait for player loaded.
				SetSwitchingWorldsState( eSwitchingWorldsStateWaitForClient );
			}

			return;
			*/
		}
		break;

		// Wait for the autosave to finish.
		case eSwitchingWorldsStateAutoSave:
		{
			// See if we're done with the autosave.
			if( g_pServerSaveLoadMgr->GetSaveDataState( ) == eSaveDataStateNone )
			{
				// Wait for player loaded.
				SetSwitchingWorldsState( eSwitchingWorldsStateWaitForClient );
				return;
			}
		}
		break;

		// Wait for at least one player to be loaded.
		case eSwitchingWorldsStateWaitForClient:
		{
			FinishSwitchingWorlds( );
			/*
			// Iterate over all the players and see if any of them have finished loading.
			CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
			while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
			{
				CPlayerObj* pPlayerObj = *iter;
				if( pPlayerObj->IsClientLoaded( ))
				{
					FinishSwitchingWorlds( );
					return;
				}
				iter++;
			}
			*/
		}
		break;

		case eSwitchingWorldsStateFinished:
		{
			return;
		}
		break;

		default:
		{
			ASSERT( !"CGameServerShell::UpdateSwitchingWorlds:  Invalid switching worlds state." );
			return;
		}
		break;
	}

	return;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Update
//
//	PURPOSE:	Update servier stuff periodically
//
// ----------------------------------------------------------------------- //

void CGameServerShell::Update(LTFLOAT timeElapsed)
{
	// Check for first update...

	if (m_bFirstUpdate)
	{
        m_bFirstUpdate = LTFALSE;
		FirstUpdate();
	}

	// Update the switching worlds state machine.
	UpdateSwitchingWorlds();

	// Update the lite objects
	if ((g_pLTServer->GetServerFlags() & SS_PAUSED) == 0)
		GetLiteObjectMgr()->Update();


	g_pMusicMgr->Update();


	// Update the command mgr...

	m_CmdMgr.Update();

	

	/*
	//[MURSUM]
	g_pAIStimulusMgr->Update();

	if (GS_CAN_SHOWDIMS_TRACK)
	{
		RemoveBoundingBoxes();
	}	
	*/

#ifndef _FINAL

#ifdef USE_AI
	if( g_pAIVolumeMgr )
	{
		g_pAIVolumeMgr->UpdateDebugRendering( GS_SHOW_VOLUME_TRACK );
	}

	if( g_pAIInformationVolumeMgr )
	{
		g_pAIInformationVolumeMgr->UpdateDebugRendering( GS_SHOW_ALL_INFO_VOLUME );
	}

	if( g_pAINodeMgr )
	{
		g_pAINodeMgr->UpdateDebugRendering( GS_SHOW_AI_NODE );
	}

#endif //USE_AI
	
#endif

	//LineSystem::RemoveAll();
	
	g_pServerMissionMgr->Update( );

	//[MURSUM] g_pServerSaveLoadMgr->Update( );

	UpdateMultiplayer();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::FirstUpdate
//
//	PURPOSE:	Do the first update
//
// ----------------------------------------------------------------------- //

void CGameServerShell::FirstUpdate()
{
    // Init some console vars...

#ifdef USE_AI
	extern CVarTrack g_vtAIDamageAdjustEasy;
	extern CVarTrack g_vtAIDamageAdjustNormal;
	extern CVarTrack g_vtAIDamageAdjustHard;
	extern CVarTrack g_vtAIDamageAdjustVeryHard;
	extern CVarTrack g_vtAIDamageIncreasePerPlayer;
	extern CVarTrack g_vtPlayerDamageDecreasePerPlayer;

	float fVal = 0.0f;
	if( !g_vtAIDamageAdjustEasy.IsInitted() )
	{
		fVal = g_pServerButeMgr->GetPlayerAttributeFloat(PLAYER_BUTE_AIDAMAGEADJUSTEASY);
		g_vtAIDamageAdjustEasy.Init( g_pLTServer, "AIDamageAdjustEasy", LTNULL, fVal );
	}
	if( !g_vtAIDamageAdjustNormal.IsInitted() )
	{
		fVal = g_pServerButeMgr->GetPlayerAttributeFloat(PLAYER_BUTE_AIDAMAGEADJUSTNORM);
		g_vtAIDamageAdjustNormal.Init( g_pLTServer, "AIDamageAdjustNormal", LTNULL, fVal );
	}
	if( !g_vtAIDamageAdjustHard.IsInitted() )
	{
		fVal = g_pServerButeMgr->GetPlayerAttributeFloat(PLAYER_BUTE_AIDAMAGEADJUSTHARD);
		g_vtAIDamageAdjustHard.Init( g_pLTServer, "AIDamageAdjustHard", LTNULL, fVal );
	}
	if( !g_vtAIDamageAdjustVeryHard.IsInitted() )
	{
		fVal = g_pServerButeMgr->GetPlayerAttributeFloat(PLAYER_BUTE_AIDAMAGEADJUSTVERYHARD);
		g_vtAIDamageAdjustVeryHard.Init( g_pLTServer, "AIDamageAdjustVeryHard", LTNULL, fVal );
	}
	if( !g_vtAIDamageIncreasePerPlayer.IsInitted() )
	{	
		fVal = g_pServerButeMgr->GetPlayerAttributeFloat(PLAYER_BUTE_AIDAMAGEADJUSTPERPLAYER);
		g_vtAIDamageIncreasePerPlayer.Init( g_pLTServer, "AIDamageIncreasePerPlayer", LTNULL, fVal );
	}
	if( !g_vtPlayerDamageDecreasePerPlayer.IsInitted() )
	{
		fVal = g_pServerButeMgr->GetPlayerAttributeFloat(PLAYER_BUTE_PLAYERDAMAGEDECREASE);
		g_vtPlayerDamageDecreasePerPlayer.Init( g_pLTServer, "PlayerDamageDecreasePerPlayer", LTNULL, fVal );
	}
#endif
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::UpdateGameServer
//
//	PURPOSE:	Updates a stand-alone server with game info if necessary
//
// ----------------------------------------------------------------------- //

LTBOOL CGameServerShell::UpdateGameServer()
{
    // Check if we need to update...

	if (!m_bUpdateGameServ)
	{
        return(LTFALSE);
	}

    m_bUpdateGameServ = LTFALSE;


	// Make sure we are actually being hosted via GameServ...

	if( !m_ServerGameOptions.m_bDedicated )
	{
        return(LTFALSE);
	}

	ServerAppShellUpdate( );

	// All done...
    return(LTTRUE);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::AddClientToList
//
//	PURPOSE:	Adds the given client handle to our local list
//
// ----------------------------------------------------------------------- //

LTBOOL CGameServerShell::AddClientToList(HCLIENT hClient)
{
	// Sanity checks...

    if (!hClient) return(LTFALSE);


	// Make sure this client isn't already in our list...

	if (IsClientInList(hClient))
	{
        return(LTTRUE);
	}


	// Add this client handle to our array...

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
        if (m_aClients[i] == LTNULL)
		{
			m_aClients[i] = hClient;
            return(LTTRUE);
		}
	}


	// If we get here, there wasn't any space left in the array...

    return(LTFALSE);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::RemoveClientFromList
//
//	PURPOSE:	Adds the given client handle to our local list
//
// ----------------------------------------------------------------------- //

LTBOOL CGameServerShell::RemoveClientFromList(HCLIENT hClient)
{
	// Sanity checks...

    if (!hClient) return(LTFALSE);


	// Remove this client handle from our array...

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (m_aClients[i] == hClient)
		{
            m_aClients[i] = LTNULL;
            return(LTTRUE);
		}
	}


	// If we get here, we didn't find the given client handle in the array...

    return(LTFALSE);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::IsClientInList
//
//	PURPOSE:	Determines if the given client handle is in our list
//
// ----------------------------------------------------------------------- //

LTBOOL CGameServerShell::IsClientInList(HCLIENT hClient)
{
	// Sanity checks...

    if (!hClient) return(LTFALSE);


	// Look for this client handle in our array...

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (m_aClients[i] == hClient)
		{
            return(LTTRUE);
		}
	}


	// If we get here, we didn't find the given client handle in the array...

    return(LTFALSE);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetPlayerFromClientList
//
//	PURPOSE:	Adds the given client handle to our local list
//
// ----------------------------------------------------------------------- //

CPlayerObj*	CGameServerShell::GetPlayerFromClientList(HCLIENT hClient)
{
	// Sanity checks...

    if (!hClient) return(LTNULL);


	// Remove this client handle from our array...

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (m_aClients[i] == hClient)
		{
			// 광식
			CHECK_PLAYEROBJ_BEGIN();
			CPlayerObj* pPlayer = LTNULL;
			CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
			while( iter != CPlayerObj::GetPlayerObjList().end() )
			{
				if( (*iter)->GetClient() == hClient )
				{
					pPlayer = (*iter);
					break;
				}
				iter++;
			}
			CHECK_PLAYEROBJ_END();
            //CPlayerObj* pPlayer = (CPlayerObj*)g_pLTServer->GetClientUserData(hClient);
			return(pPlayer);
		}
	}


	// If we get here, we didn't find the given client handle in the array...

    return(LTNULL);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetClientFromID
//
//	PURPOSE:	Get the handle to a client based on an id
//
// ----------------------------------------------------------------------- //

HCLIENT CGameServerShell::GetClientFromID(uint32 nID)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		uint32 clientID = g_pLTServer->GetClientID(m_aClients[i]);
		if (clientID == nID)
		{
			return (m_aClients[i]);
		}
	}


	// If we get here, we didn't find the given client handle in the array...

    return(LTNULL);

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::InitGameType
//
//	PURPOSE:	Setup game type.
//
// ----------------------------------------------------------------------- //

bool CGameServerShell::InitGameType()
{
	ServerGameOptions** ppServerGameOptions;
    uint32 dwLen = sizeof( ServerGameOptions* );
    g_pLTServer->GetGameInfo((void**)&ppServerGameOptions, &dwLen);

	if( ppServerGameOptions && *ppServerGameOptions )
	{
		m_ServerGameOptions = **ppServerGameOptions;
	}
	
	// Toss any old missionmgr.
	if( m_pServerMissionMgr )
	{
		debug_delete( m_pServerMissionMgr );
		m_pServerMissionMgr = NULL;
	}

	// Reseed random number generator if necessary...
	if (m_ServerGameOptions.m_bPerformanceTest)
	{
		srand(123); // Same as client, doesn't really matter...
	}

	switch( m_ServerGameOptions.m_eGameType )
	{
		case eGameTypeSingle:
			m_pServerMissionMgr = debug_new( CSinglePlayerMissionMgr );
			break;
		case eGameTypeCooperative:
			m_pServerMissionMgr = debug_new( CCoopMissionMgr );
			break;
		case eGameTypeDeathmatch:
			m_pServerMissionMgr = debug_new( CDeathMatchMissionMgr );
			break;
		case eGameTypeTeamDeathmatch:
			m_pServerMissionMgr = debug_new( CTeamDeathMatchMissionMgr );
			break;
		case eGameTypeDoomsDay:
			m_pServerMissionMgr = debug_new( CDoomsDayMissionMgr );
			break;
		case eGameTypeSurvival :
			m_pServerMissionMgr = debug_new( CSurvivalMissionMgr );
			break;
		default:
			ASSERT( !"CGameServerShell::InitGameType: Invalid game type." );
			break;
	}

	if( !m_pServerMissionMgr )
		return false;

	if( !m_pServerMissionMgr->Init( ))
		return false;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::UpdateBoundingBoxes()
//
//	PURPOSE:	Update object bounding boxes
//
// ----------------------------------------------------------------------- //

void CGameServerShell::UpdateBoundingBoxes()
{
    HOBJECT hObj   = g_pLTServer->GetNextObject(LTNULL);
    HCLASS  hClass = g_pLTServer->GetClass("GameBase");

	// Active objects...

	while (hObj)
	{
        if (g_pLTServer->IsKindOf(g_pLTServer->GetObjectClass(hObj), hClass))
		{
            GameBase* pObj = (GameBase*)g_pLTServer->HandleToObject(hObj);
			if (pObj)
			{
				pObj->UpdateBoundingBox();
			}
		}

        hObj = g_pLTServer->GetNextObject(hObj);
	}

	// Inactive objects...

    hObj = g_pLTServer->GetNextInactiveObject(LTNULL);
	while (hObj)
	{
        if (g_pLTServer->IsKindOf(g_pLTServer->GetObjectClass(hObj), hClass))
		{
            GameBase* pObj = (GameBase*)g_pLTServer->HandleToObject(hObj);
			if (pObj)
			{
				pObj->UpdateBoundingBox();
			}
		}

        hObj = g_pLTServer->GetNextInactiveObject(hObj);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::RemoveBoundingBoxes()
//
//	PURPOSE:	Remove object bounding boxes
//
// ----------------------------------------------------------------------- //

void CGameServerShell::RemoveBoundingBoxes()
{
    HOBJECT hObj   = g_pLTServer->GetNextObject(LTNULL);
    HCLASS  hClass = g_pLTServer->GetClass("GameBase");

	// Active objects...

	while (hObj)
	{
        if (g_pLTServer->IsKindOf(g_pLTServer->GetObjectClass(hObj), hClass))
		{
            GameBase* pObj = (GameBase*)g_pLTServer->HandleToObject(hObj);
			if (pObj)
			{
				pObj->RemoveBoundingBox();
			}
		}

        hObj = g_pLTServer->GetNextObject(hObj);
	}

	// Inactive objects...

    hObj = g_pLTServer->GetNextInactiveObject(LTNULL);
	while (hObj)
	{
        if (g_pLTServer->IsKindOf(g_pLTServer->GetObjectClass(hObj), hClass))
		{
            GameBase* pObj = (GameBase*)g_pLTServer->HandleToObject(hObj);
			if (pObj)
			{
				pObj->RemoveBoundingBox();
			}
		}

        hObj = g_pLTServer->GetNextInactiveObject(hObj);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::UpdateMultiplayer
//
//	PURPOSE:	Determine if it is time to change levels
//
// ----------------------------------------------------------------------- //
void CGameServerShell::UpdateMultiplayer()
{
	// Update client ping times
	
	//[MURSUM]
	//핑 사용안함
	//UpdateClientPingTimes();

	// Update game server info...
	UpdateGameServer();

	if (m_bShowMultiplayerSummary)
	{
		LTBOOL bReady = LTTRUE;
		if (m_fSummaryEndTime > g_pLTServer->GetTime())
		{
			for (int i = 0; i < MAX_CLIENTS && bReady; i++)
			{
				CPlayerObj* pPlayer = GetPlayerFromClientList(m_aClients[i]);
				if (pPlayer)
				{
					if (!pPlayer->IsReadyToExit())
					{
						bReady = LTFALSE;
						break;
					}
				}
			}
		}
		if (bReady)
		{
			m_bStartNextMultiplayerLevel = LTTRUE;
		}
		return;
	}

	/*
	// Check if any client has not talked to us in a very long time.  If so, it may
	// mean there is a bug in the networking code that refuses to kick clients not responding.
	// Because this bug is rare, it's difficult to track in the networking.  This is just a bit
	// of fullproofing.  The autoboottime is in seconds, so we need to convert it to ms.
	uint32 nCurrentTime = GetTickCount( );
	uint32 nAutoBootTime = ( uint32 )( GetConsoleFloat( "AutoBootTime", 5.0f * 60.0f ) * 1000.0f + 0.5f );
	ClientDataList::const_iterator iter = m_ClientDataList.begin( );
	for( ; iter != m_ClientDataList.end( ); iter++ )
	{
		ClientData* pClientData = *iter;

		// Skip the host.
		if( g_pLTServer->GetClientInfoFlags( pClientData->m_hClient ) & CIF_LOCAL )
			continue;

		// Get the time since the ping.  Account for wrapping.
		uint32 nDeltaTime = ( nCurrentTime > pClientData->m_nLastClientUpdateTime ) ? 
			( nCurrentTime - pClientData->m_nLastClientUpdateTime ) : 
			( nCurrentTime + ~( pClientData->m_nLastClientUpdateTime - 1 ));

		// Check if they haven't talked in a while.
		if( nDeltaTime >= nAutoBootTime )
		{
			g_pLTServer->CPrint( "Booting innactive client." );
			g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
			g_pLTServer->KickClient( pClientData->m_hClient );
		}
	}
	*/
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::StartNextMultiplayerLevel
//
//	PURPOSE:	Start the next multiplayer level
//
// ----------------------------------------------------------------------- //

bool CGameServerShell::StartNextMultiplayerLevel()
{
	m_bShowMultiplayerSummary = LTFALSE;

	return g_pServerMissionMgr->NextMission( );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ServerAppMessageFn
//
//	PURPOSE:	Server app message function
//
// ----------------------------------------------------------------------- //

LTRESULT CGameServerShell::ServerAppMessageFn( ILTMessage_Read& msg )
{
	// Let the scmd check if it needs this message.
	if( ScmdServer::Instance( ).OnMessage( NULL, msg ))
	{
		return LT_OK;
	}

	msg.SeekTo(0);
	switch( msg.Readuint8( ))
	{
		case SERVERSHELL_INIT:
		{
			if( !OnServerShellInit( ))
				return LT_ERROR;
		}
		break;

		case SERVERSHELL_NEXTWORLD:
		{	
			if( !StartNextMultiplayerLevel( ))
				return LT_ERROR;
		}
		break;

		case SERVERSHELL_SETWORLD:
		{	
			// Read in the world index.  Go back one, since StartNextMultiplayerLevel
			// will preincrement the curlevel.
			int nNewMission;
			nNewMission = msg.Readuint32( );
			if( !g_pServerMissionMgr->SwitchToCampaignIndex( nNewMission ))
				return LT_ERROR;
		}
		break;

		case SERVERSHELL_MESSAGE:
		{
			char szMsg[256] = {0};
			msg.ReadString( szMsg, ARRAY_LEN( szMsg ));

			g_pLTServer->RunGameConString( szMsg );

		}
		break;
		
		case SERVERSHELL_MISSIONFAILED:
		{
			// We have a failed mission from the server app...
			// Wait for client responses to begin the auto load.
			
			g_pServerSaveLoadMgr->WaitForAutoLoadResponses();
		}
		break;

		// Invalid message.
		default:
		{
			ASSERT( FALSE );
			return LT_ERROR;
		}
		break;
	}

	return LT_OK;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::OnServerShellInit
//
//	PURPOSE:	Initializes the server servershell from serverapp.
//
// ----------------------------------------------------------------------- //

bool CGameServerShell::OnServerShellInit( )
{
	// Initialize the server game.  Need to do this first to init servermissionmgr.
	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_START_GAME );
	CLTMsgRef_Read msgRead = cMsg.Read( );
	if( !g_pServerMissionMgr->OnMessage( NULL, *msgRead ))
		return false;

	// First we'll tell the serverapp about the missions in the campaign.
	CAutoMessage cLevelsMsg;
	cLevelsMsg.Writeuint8( SERVERAPP_INIT );

	char const* pszFirstMission = NULL;

	Campaign& campaign = g_pServerMissionMgr->GetCampaign( );
	cLevelsMsg.Writeuint8( campaign.size( ));
	char fname[_MAX_FNAME] = "";
	for( Campaign::iterator iter = campaign.begin( ); iter != campaign.end( ); iter++ )
	{
		// Get the mission in the campaign.
		int nMission = *iter;
		MISSION* pMission = g_pMissionButeMgr->GetMission( nMission );
		if( !pMission )
			continue;

		// Use the first level of the mission as the name of the mission.
		if( !pszFirstMission )
		{
			pszFirstMission = pMission->szMap;
		}

		_splitpath( pMission->szMap, NULL, NULL, fname, NULL );
 		cLevelsMsg.WriteString( fname );
	}

	// Send the message to the server app.
	CLTMsgRef_Read msgRefReadLevels = cLevelsMsg.Read( );
	g_pLTServer->SendToServerApp( *msgRefReadLevels );

	// Start the first level.
	CAutoMessage cMsgStartLevel;
	cMsgStartLevel.Writeuint8( MID_START_LEVEL );
	cMsgStartLevel.WriteString( pszFirstMission );
	CLTMsgRef_Read msgReadStartLevel = cMsgStartLevel.Read( );
	if( !g_pServerMissionMgr->OnMessage( NULL, *msgReadStartLevel ))
		return false;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::PauseGame
//
//	PURPOSE:	Pause/unpause the game.  bForce overrides the counted pauses.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::PauseGame( bool bPause, bool bForce )
{
	// Reset the pause count if forced.
	if( bForce )
	{
		m_nPauseCount = 0;
	}

	PauseGame( bPause );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::PauseGame
//
//	PURPOSE:	Pause/unpause the game using reference counts.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::PauseGame( bool bPause )
{
   	if( bPause )
		m_nPauseCount++;
	else
		m_nPauseCount--;

	// Make sure we don't go negative.
	m_nPauseCount = Max( 0, m_nPauseCount );

	//determine what state we are in
	bool bInPauseState = (m_nPauseCount > 0);

	//if we are not paused, forget whether the client had paused us
	if (!bInPauseState)
		m_bClientPaused = false;


	//alright, we have changed pause state, so let us notify the server and its objects
	uint32 nFlags = g_pLTServer->GetServerFlags();
	if( bInPauseState )
		nFlags |= SS_PAUSED;
	else
		nFlags &= ~SS_PAUSED;
    g_pLTServer->SetServerFlags (nFlags);

	//handle setting of the paused flag
	HOBJECT hObj = g_pLTServer->GetNextObject(LTNULL);
	while (hObj)
	{
		g_pLTServer->Common()->SetObjectFlags(hObj, OFT_Flags, bInPauseState ? FLAG_PAUSED : 0, FLAG_PAUSED);
        hObj = g_pLTServer->GetNextObject(hObj);
	}

    hObj = g_pLTServer->GetNextInactiveObject(LTNULL);
	while (hObj)
	{
		g_pLTServer->Common()->SetObjectFlags(hObj, OFT_Flags, bInPauseState ? FLAG_PAUSED : 0, FLAG_PAUSED);
        hObj = g_pLTServer->GetNextInactiveObject(hObj);
	}

	//[MURSUM]====================================================================
	//포즈 메시지는 메인서버에서 받는 EndGame으로 대치한다.
	/*/
	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_GAME_PAUSE );
	cMsg.Writebool( bInPauseState );
	g_pLTServer->SendToClient( cMsg.Read(), NULL, MESSAGE_GUARANTEED );
	/*/
	//============================================================================
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::IsPaused( )
//
//	PURPOSE:	Check if server is paused.
//
// ----------------------------------------------------------------------- //

bool CGameServerShell::IsPaused( )
{
	uint32 nFlags = g_pLTServer->GetServerFlags();
	bool bInPauseState = (( nFlags & SS_PAUSED ) != 0 );
	return bInPauseState;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ClientPauseGame( )
//
//	PURPOSE:	Modifies the client's pause on the game.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::ClientPauseGame( bool bPause )
{
	if( bPause )
	{
		if( m_bClientPaused )
			return;

		m_bClientPaused = true;
		PauseGame( true );
	}
	else
	{
		if( m_bClientPaused )
		{
			m_bClientPaused = false;
			PauseGame( false );
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleConsoleCmdMsg()
//
//	PURPOSE:	Handle console cmd messages
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandleConsoleCmdMsg(HCLIENT hSender, ILTMessage_Read *pMsg)
{
    HSTRING hCommand = pMsg->ReadHString();
	if (!hCommand) return;

    if (m_CmdMgr.Process(g_pLTServer->GetStringData(hCommand), (ILTBaseClass*)NULL, (ILTBaseClass*)NULL))
	{
        g_pLTServer->CPrint("Sent Command '%s'", g_pLTServer->GetStringData(hCommand));
	}

	if (hCommand)
	{
        g_pLTServer->FreeString(hCommand);
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleSaveData
//
//	PURPOSE:	Handle savegame command from client.
//
// --------------------------------------------------------------------------- //

void CGameServerShell::HandleSaveGame( HCLIENT hSender, ILTMessage_Read* pMsg )
{
	if( !pMsg )
		return;

	// Ignore save requests from non-local clients.
	if( !( g_pLTServer->GetClientInfoFlags( hSender ) & CIF_LOCAL ))
		return;

	int nSlot = pMsg->Readuint8( );

	// Check if this is a quicksave.
	if( nSlot == 0 )
	{
		g_pServerSaveLoadMgr->QuickSave( g_pServerMissionMgr->GetCurrentMission( ), 
			g_pServerMissionMgr->GetCurrentLevel( ));
	}
	else
	{
		// Get the savegame name.
		char szSaveName[MAX_PATH];
		pMsg->ReadString( szSaveName, ARRAY_LEN( szSaveName ));
		g_pServerSaveLoadMgr->SaveGameSlot( nSlot, szSaveName );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandleObjectMessage
//
//	PURPOSE:	Handles generic message from the client to a specific object
//
// ----------------------------------------------------------------------- //
void CGameServerShell::HandleObjectMessage(HCLIENT hSender, ILTMessage_Read* pMsg)
{
	// Sanity check
	if(!pMsg)
		return;

	// Grab the object and make sure it exists
	HOBJECT hObj = pMsg->ReadObject();
	if(!hObj)
		return;

	// Cast it
	GameBase *pObj = (GameBase *)g_pLTServer->HandleToObject(hObj);
	if(!pObj)
		return;

	// Check for the player object
	CPlayerObj* pPlayer = GET_PLAYER(hSender);
	if(!pPlayer)
	{
        return;
	}

	// Call the object's objectmessagefn and forward the message
	pObj->ObjectMessageFn(pPlayer->m_hObject, CLTMsgRef_Read(pMsg->SubMsg( pMsg->Tell( ))));
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::HandlePerformanceMessage ()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::HandlePerformanceMessage (HCLIENT hSender, ILTMessage_Read *pMsg)
{
	//Ignore for multiplayer if not host?
	
	
	uint8 nEnvironmentalDetail = pMsg->Readuint8();

	m_bPreCacheAssets = pMsg->Readbool();
}

//광식
LTBOOL CGameServerShell::FirstAttachProcess( CPlayerObj* pPlayer )
{
	BOOL bResult = LTFALSE;

	switch( pPlayer->m_nAttachState )
	{
	case MID_ATTACH_FIRST :
		{
			bResult = AttachAfterDetach(pPlayer);
		}break;
	case MID_ATTACH_THIRD :
	case MID_DETACH_OBSERVER :	// 추가
		{
#ifdef GAME_SHOW
			if( pPlayer->m_hAttachedPlayer == pPlayer->GetClient() )
			{
				bResult = AttachAfterDetach(pPlayer);
			}
			else
			{
				bResult = LTTRUE;
			}
			break;
#endif // GAME_SHOW
			bResult = LTTRUE;
		}break;
	case MID_DETACH_DISPLAY :
	//case MID_DETACH_OBSERVER :	// 추가
		{
			bResult = AttachProcess(pPlayer);
		}break;
	}

	if( bResult )
		pPlayer->m_nAttachState = MID_ATTACH_FIRST;

	return bResult;
}

LTBOOL CGameServerShell::ThirdAttachProcess(CPlayerObj* pPlayer )
{
	BOOL bResult = LTFALSE;

	switch( pPlayer->m_nAttachState )
	{
	case MID_ATTACH_FIRST :
	case MID_DETACH_OBSERVER :
		{
#ifdef GAME_SHOW
			if( pPlayer->m_hAttachedPlayer == pPlayer->GetClient() )
			{
				bResult = AttachAfterDetach(pPlayer);
			}
			else
			{
				bResult = LTTRUE;
			}
			break;
#endif // GAME_SHOW
			bResult = LTTRUE;
		}break;

	case MID_ATTACH_THIRD :
		{
			bResult = AttachAfterDetach(pPlayer);
		}break;

	case MID_DETACH_DISPLAY :
	//case MID_DETACH_OBSERVER :
		{
			bResult = AttachProcess(pPlayer);
		}break;
	}

	if( bResult )
		pPlayer->m_nAttachState = MID_ATTACH_THIRD;

	return bResult;
}

LTBOOL CGameServerShell::AttachAfterDetach( CPlayerObj* pPlayer )
{
	//if( !pPlayer->m_hAttachedPlayer ) return LTFALSE;

	// 기존에 붙어 있던 위치에서 나를 제거하고 client의 다음 녀석에게 나를 Attach 시킨다
	//--------------------------------------------------------------
	LTBOOL	bDetach = LTFALSE;
	LTBOOL	bAttach	= LTFALSE;
	//HCLIENT hNextClient = g_pLTServer->GetNextClient( LTNULL );
	//while ( hNextClient )
	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList().end() )
	{
		//HCLIENT hOtherClient = hNextClient;
		//hNextClient = g_pLTServer->GetNextClient( hOtherClient );					
		//CPlayerObj* pUsedPlayer = ( CPlayerObj* )g_pLTServer->GetClientUserData( hOtherClient );
		CPlayerObj* pUsedPlayer = *iter;

		//--------------------------------------------------------------------------
		if( !pUsedPlayer )
		{
			WOONGBO_ATTACH	char DebugString[256] = {0, };
			WOONGBO_ATTACH	sprintf(DebugString, "  AttachAfterDetach pUsedPlayer NULL \n");
			WOONGBO_ATTACH	OutputDebugString(DebugString);
			continue;
		}
		//--------------------------------------------------------------------------

		// PS_GHOST로 Attach 되었을 경우 Team 에 상관 없이 Attach 시킨다.
		if( pPlayer->GetState() == PS_GHOST )
		{
			if( bDetach == LTTRUE && 
			pUsedPlayer->IsDead() == LTFALSE &&
			pUsedPlayer->GetPlayerAttachments()->GetWeapon() != LTNULL ) //CurWeapon 이 -1 인 경우는 스킵
			{
				pPlayer->m_hAttachedPlayer = pUsedPlayer->AttachPlayer( pPlayer->GetClient() );
				if( pPlayer->m_hAttachedPlayer )
				{
					bAttach = LTTRUE;
					CHECK_PLAYEROBJ_END();
					return LTTRUE;
				}
			}

			if( bDetach == LTFALSE )
			{
				if( pUsedPlayer->IsAttachPlayer(pPlayer->GetClient()) )
				{
					pUsedPlayer->DetachPlayer( pPlayer->GetClient() );
					bDetach = LTTRUE;
				}
			}
		}
		else
		{
			if( bDetach == LTTRUE && 
			pUsedPlayer->GetTeamID() == pPlayer->GetTeamID() &&
			pUsedPlayer->IsDead() == LTFALSE &&
			pUsedPlayer->GetPlayerAttachments()->GetWeapon() != LTNULL ) //CurWeapon 이 -1 인 경우는 스킵
			{
				pPlayer->m_hAttachedPlayer = pUsedPlayer->AttachPlayer( pPlayer->GetClient() );
				if( pPlayer->m_hAttachedPlayer )
				{
					CHECK_PLAYEROBJ_END();
					bAttach = LTTRUE;
					return LTTRUE;
				}

				//-----------------------------------------------------------------
				WOONGBO_ATTACH	char DebugString[256] = {0, };
				WOONGBO_ATTACH	sprintf(DebugString, "  pPlayer->m_hAttachedPlayer NULL Attach Fail after Detach \n");
				WOONGBO_ATTACH	OutputDebugString(DebugString);
				//-----------------------------------------------------------------
			}

			if( pUsedPlayer->GetTeamID() == pPlayer->GetTeamID() && bDetach == LTFALSE )
			{
				if( pUsedPlayer->IsAttachPlayer(pPlayer->GetClient()) )
				{
					pUsedPlayer->DetachPlayer( pPlayer->GetClient() );
					bDetach = LTTRUE;
				}
			}
		}

		iter++;
	}
	CHECK_PLAYEROBJ_END();
	//--------------------------------------------------------------
	if( !bDetach )
	{
		//-----------------------------------------------------------
		WOONGBO_ATTACH	char DebugString[256] = {0, };
		WOONGBO_ATTACH	sprintf(DebugString, "  Error : MID_ATTACH_FIRST -> Detach Fail \n");
		WOONGBO_ATTACH	OutputDebugString(DebugString);
		//-----------------------------------------------------------
		return LTFALSE;
	}
	

	// client 끝까지 돌았는데 붙일 녀석이 없었다면 다시 처음부터 search해서 붙을 녀석을 찾는다
	if( !bAttach )
	{
		CHECK_PLAYEROBJ_BEGIN();
		CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
		//HCLIENT hNextClient = g_pLTServer->GetNextClient( LTNULL );
		//while ( hNextClient )
		while( iter != CPlayerObj::GetPlayerObjList().end() )
		{
			//HCLIENT hOtherClient = hNextClient;
			//hNextClient = g_pLTServer->GetNextClient( hOtherClient );					
			//CPlayerObj* pUsedPlayer = ( CPlayerObj* )g_pLTServer->GetClientUserData( hOtherClient );
			CPlayerObj* pUsedPlayer = *iter;
			//--------------------------------------------------------------------------------------
			if( !pUsedPlayer )
			{
				WOONGBO_ATTACH	OutputDebugString("AttachAfterDetach Function [ pUsedPlayer ] NULL 2222222222");
				continue;
			}
			else
			{
				if( pPlayer->GetState() == PS_GHOST )
				{
					if( pUsedPlayer->GetClient() != pPlayer->m_hAttachedPlayer &&
						pUsedPlayer->IsDead() == LTFALSE &&
						pUsedPlayer->GetPlayerAttachments()->GetWeapon() != LTNULL )
						{
							pPlayer->m_hAttachedPlayer = pUsedPlayer->AttachPlayer( pPlayer->GetClient() );
							if( pPlayer->m_hAttachedPlayer )
							{
								CHECK_PLAYEROBJ_END();
								return LTTRUE;
							}							
						}
				}
				else
				{
					if( pUsedPlayer->GetTeamID() == pPlayer->GetTeamID() &&
						pUsedPlayer->GetClient() != pPlayer->m_hAttachedPlayer &&
						pUsedPlayer->IsDead() == LTFALSE &&
						pUsedPlayer->GetPlayerAttachments()->GetWeapon() != LTNULL )
						{
							pPlayer->m_hAttachedPlayer = pUsedPlayer->AttachPlayer( pPlayer->GetClient() );
							if( pPlayer->m_hAttachedPlayer )
							{
								CHECK_PLAYEROBJ_END();
								return LTTRUE;
							}							
						}
				}
			}
			//--------------------------------------------------------------------------------------
			iter++;
		}
		CHECK_PLAYEROBJ_END();
		// 같은 팀원이 모두 죽었거나 실패...
		//pPlayer->m_hAttachedPlayer = LTNULL;
		//g_pLTServer->CPrint(" Error : MID_ATTACH_FIRST -> Attach Fail ");
		//--------------------------------------------------------------------------------------
		if( pPlayer->m_hAttachedPlayer )
		{
			CHECK_PLAYEROBJ_BEGIN();
			CPlayerObj* pUsedPlayer = LTNULL;
			iter = CPlayerObj::GetPlayerObjList( ).begin( );
			while(iter != CPlayerObj::GetPlayerObjList().end())
			{
				pUsedPlayer = (*iter);
				if( pPlayer->m_hAttachedPlayer == pUsedPlayer->GetClient() )
				{
					pPlayer->m_hAttachedPlayer = pUsedPlayer->AttachPlayer( pPlayer->GetClient() );
					break;
				}
				iter++;
			}
			CHECK_PLAYEROBJ_END();
		}
		
		/*
		if( pPlayer->m_hAttachedPlayer )
		{
			CPlayerObj* pUsedPlayer = ( CPlayerObj* )g_pLTServer->GetClientUserData( pPlayer->m_hAttachedPlayer );
			if( pUsedPlayer )
			{
				pPlayer->m_hAttachedPlayer = pUsedPlayer->AttachPlayer(pPlayer->GetClient());
			}
		}
		*/

		WOONGBO_ATTACH	OutputDebugString("AttachFail~~~~~ All Killed \n");
		return LTFALSE;
		//--------------------------------------------------------------------------------------
	}

	if( !bAttach )
	{
		//HCLIENT hNextClient = g_pLTServer->GetNextClient( LTNULL );
		//while ( hNextClient )
		CHECK_PLAYEROBJ_BEGIN();
		CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
		while( iter != CPlayerObj::GetPlayerObjList().end() )
		{
			//HCLIENT hOtherClient = hNextClient;
			//hNextClient = g_pLTServer->GetNextClient( hOtherClient );					
			//CPlayerObj* pUsedPlayer = ( CPlayerObj* )g_pLTServer->GetClientUserData( hOtherClient );
			CPlayerObj* pUsedPlayer = *iter;
			if( !pUsedPlayer )
			{
				WOONGBO_ATTACH	OutputDebugString("AttachAfterDetach Function [ pUsedPlayer ] NULL 333333333 \n");
				continue;
			}
			else
			{
				if( pUsedPlayer->GetTeamID() == pPlayer->GetTeamID() &&
				pUsedPlayer->GetClient() != pPlayer->m_hAttachedPlayer &&
				pUsedPlayer->IsDead() == LTFALSE &&
				pUsedPlayer->GetPlayerAttachments()->GetWeapon() != LTNULL )
				{
					pPlayer->m_hAttachedPlayer = pUsedPlayer->AttachPlayer( pPlayer->GetClient() );
					if( pPlayer->m_hAttachedPlayer )
					{
						CHECK_PLAYEROBJ_END();
						return LTTRUE;
					}							
				}
			}

			iter++;
		}
		CHECK_PLAYEROBJ_END();

		g_pLTServer->CPrint(" Error : MID_ATTACH_FIRST -> Attach Fail ");
	}

	return LTFALSE;
}

LTBOOL CGameServerShell::AttachProcess( CPlayerObj* pPlayer )
{
	//HCLIENT hNextClient = g_pLTServer->GetNextClient( LTNULL );
	//while ( hNextClient )
	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList().end() )
	{
		//HCLIENT hOtherClient = hNextClient;
		//hNextClient = g_pLTServer->GetNextClient( hOtherClient );					
		//CPlayerObj* pUsedPlayer = ( CPlayerObj* )g_pLTServer->GetClientUserData( hOtherClient );
		CPlayerObj* pUsedPlayer = *iter;

		if( !pUsedPlayer )
		{
			WOONGBO_ATTACH	OutputDebugString("AttachProcess Function [pUsedPlayer] NULL 11111111 \n");
			continue;
		}
		else
		{
			// PS_GHOST로 Attach 되었을 경우 Team 에 상관 없이 Attach 시킨다.
			if( pPlayer->GetState() == PS_GHOST )
			{
				if( pUsedPlayer->IsDead() == LTFALSE && 
					pUsedPlayer->m_hObject != pPlayer->m_hObject &&
					pUsedPlayer->GetPlayerAttachments()->GetWeapon() != LTNULL )
				{
					pPlayer->m_hAttachedPlayer = pUsedPlayer->AttachPlayer( pPlayer->GetClient() );
					if( pPlayer->m_hAttachedPlayer )
					{
						CHECK_PLAYEROBJ_END();
						return LTTRUE;
					}
				}
			}
			else
			{
				if( pUsedPlayer->GetTeamID() == pPlayer->GetTeamID() &&
				pUsedPlayer->IsDead() == LTFALSE &&
				pUsedPlayer->GetPlayerAttachments()->GetWeapon() != LTNULL )
				{
					pPlayer->m_hAttachedPlayer = pUsedPlayer->AttachPlayer( pPlayer->GetClient() );
					if( pPlayer->m_hAttachedPlayer )
					{
						CHECK_PLAYEROBJ_END();
						return LTTRUE;
					}
				}
			}
		}

		iter++;
	}
	CHECK_PLAYEROBJ_END();


	return LTFALSE;
}

void CGameServerShell::HandlePlayerAttachMessage(HCLIENT hSender, ILTMessage_Read* pMsg)
{
	if( !hSender ) return;

	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj* pSenderPlayer = LTNULL;
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ) )
	{
		if( hSender == (*iter)->GetClient() )
		{
			pSenderPlayer = (*iter);
			break;
		}
		iter++;
	}
	CHECK_PLAYEROBJ_END();
	//CPlayerObj* pSenderPlayer = (CPlayerObj*)g_pLTServer->GetClientUserData(hSender);
	if (!pSenderPlayer) return;
	if( pSenderPlayer->GetState() == PS_ALIVE ) return;

	LTBOOL bResult = LTFALSE;

	uint8 MsgAttach1or3 = pMsg->Readuint8();

	switch( MsgAttach1or3 )
	{
		case MID_ATTACH_FIRST :
			{
				bResult = FirstAttachProcess( pSenderPlayer );
			}break;

		case MID_ATTACH_THIRD :
			{
				bResult = ThirdAttachProcess( pSenderPlayer );
			}break;

		case MID_DETACH_DISPLAY :		
			{
				bResult	= DetachProcess( hSender, MsgAttach1or3 );
			}break;

		case MID_DETACH_OBSERVER :
			{
				bResult = ObserverProcess( hSender, MsgAttach1or3 );
			}break;
			
		default : break;
	}

	if( bResult )
	{
		HOBJECT hObject = LTNULL;

		CHECK_PLAYEROBJ_BEGIN();
		CPlayerObj* pAttachPlayer = LTNULL;
		iter = CPlayerObj::GetPlayerObjList( ).begin( );
		while( iter != CPlayerObj::GetPlayerObjList( ).end( ) )
		{
			if( (*iter)->GetClient() == pSenderPlayer->m_hAttachedPlayer )
			{
				pAttachPlayer = (*iter);
				break;
			}
			iter++;
		}	
		CHECK_PLAYEROBJ_END();
		//CPlayerObj* pAttachPlayer = (CPlayerObj*)g_pLTServer->GetClientUserData(pSenderPlayer->m_hAttachedPlayer);
		if( !pAttachPlayer )
		{
			// Observer Mode
			if( MsgAttach1or3 == MID_DETACH_OBSERVER )
			{
			}
			else
			{
				return;
			}
			//return;
		}

		if( pAttachPlayer )
		{
			hObject = pAttachPlayer->m_hObject;
		}
		else
		{
			hObject = pSenderPlayer->m_hObject;
		}
		CAutoMessage cMsg;
		cMsg.Writeuint8( MID_PLAYER_ATTACH );
		cMsg.Writeuint8( MsgAttach1or3 );
		//cMsg.WriteObject( pAttachPlayer->m_hObject );
		cMsg.WriteObject( hObject );
		if( MsgAttach1or3 == MID_ATTACH_FIRST )
		{
			int nAmmoId = pAttachPlayer->GetCurrentAttachedAmmoId();
			// Weapon관련
			cMsg.Writeuint32( pAttachPlayer->GetCurrentAttachedWeaponId() );
			cMsg.Writeuint32( nAmmoId );
			cMsg.Writeuint32( pAttachPlayer->GetCurrentAttachedAmmoCount(nAmmoId) );

			cMsg.Writeuint32( pAttachPlayer->GetCurrentAttachedAmmoInClip() );
		}
		else if( MsgAttach1or3 == MID_DETACH_OBSERVER )
		{
			cMsg.Writebool( true );
		}		
		g_pLTServer->SendToClient(cMsg.Read(), hSender, MESSAGE_GUARANTEED);
	}
}

LTBOOL CGameServerShell::DetachProcess( HCLIENT hPlayer, uint8 Msg )
{
	CPlayerObj* pPlayer = GET_PLAYER(hPlayer);
	if( !pPlayer )
	{
		WOONGBO_ATTACH	OutputDebugString("  DetachProcess pPlayerError11111111111111 \n");
		return LTFALSE;
	}
	

	//HCLIENT hNextClient = g_pLTServer->GetNextClient( LTNULL );
	//while ( hNextClient )
	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList().end() )
	{
		//HCLIENT hOtherClient = hNextClient;
		//hNextClient = g_pLTServer->GetNextClient( hOtherClient );					
		//CPlayerObj* pUsedPlayer = ( CPlayerObj* )g_pLTServer->GetClientUserData( hOtherClient );
		CPlayerObj* pUsedPlayer = *iter;
		if( !pUsedPlayer )
		{
			WOONGBO_ATTACH	OutputDebugString("    pUsedPlayerError1111111111 \n");
			continue;
		}
		else
		{
			//if( pUsedPlayer->GetTeamID() == pPlayer->GetTeamID() &&
			//pUsedPlayer->IsDead() == LTFALSE )
			{
				if( pUsedPlayer->IsAttachPlayer( hPlayer ) )
				{
					pUsedPlayer->DetachPlayer( hPlayer );				
					pPlayer->m_nAttachState		= MID_DETACH_DISPLAY;
					//pPlayer->m_nAttachState		= Msg;
					pPlayer->m_hAttachedPlayer	= LTNULL;
					CHECK_PLAYEROBJ_END();
					return LTTRUE;
				}
			}
		}

		iter++;
	}
	CHECK_PLAYEROBJ_END();

	return LTFALSE;
}

LTBOOL CGameServerShell::ObserverProcess( HCLIENT hPlayer, uint8 Msg )
{
	CPlayerObj* pPlayer = GET_PLAYER(hPlayer);
	if( !pPlayer )
	{
		WOONGBO_ATTACH	OutputDebugString("  ObserverProcess pPlayerError [ pPlayer -> LTNULL ] \n");
		return LTFALSE;
	}

	//HCLIENT hNextClient = g_pLTServer->GetNextClient( LTNULL );
	//while ( hNextClient )
	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList().end() )	
	{
		//HCLIENT hOtherClient = hNextClient;
		//hNextClient = g_pLTServer->GetNextClient( hOtherClient );
		//CPlayerObj* pUsedPlayer = (CPlayerObj*)g_pLTServer->GetClientUserData( hOtherClient );
		CPlayerObj* pUsedPlayer = *iter;
		if( !pUsedPlayer )
		{
			WOONGBO_ATTACH	OutputDebugString("  ObserverProcess pPlayerError [ pUsedPlayer -> LTNULL ] \n");
			continue;
		}
		else
		{
			if( pPlayer->m_nAttachState != MID_DETACH_OBSERVER )
			{
				pPlayer->m_nAttachState		= MID_DETACH_OBSERVER;
#ifdef GAME_SHOW
				pPlayer->m_hAttachedPlayer = pPlayer->AttachPlayer( pPlayer->GetClient() );
#endif // GAME_SHOW
				CHECK_PLAYEROBJ_END();
				return LTTRUE;

			}
			else
			{
				LTBOOL bResult = AttachAfterDetach(pPlayer);
				if( bResult )
				{
					pPlayer->m_nAttachState		= MID_DETACH_OBSERVER;
					CHECK_PLAYEROBJ_END();
					return LTTRUE;
				}
			}
		}

		iter++;
	}
	CHECK_PLAYEROBJ_END();

	//if( hNextClient == LTNULL )
	if( iter == CPlayerObj::GetPlayerObjList().end() )
	{
		return LTTRUE;
	}

	return LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Save
//
//	PURPOSE:	Save the global world info
//
// ----------------------------------------------------------------------- //

void CGameServerShell::Save(ILTMessage_Write *pMsg, uint32 dwSaveFlags)
{
	_ASSERT(pMsg);
	if (!pMsg) return;

	SAVE_DWORD(g_pVersionMgr->GetSaveVersion());

	// Important: This has to be first, since it's taking the place of engine object behavior
	m_pLiteObjectMgr->Save(pMsg);

#ifdef USE_AI
	m_pCharacterMgr->Save(pMsg);
#endif
	m_CmdMgr.Save(pMsg);
#ifdef USE_AI
	m_pAIStimulusMgr->Save(pMsg);
	m_pAICentralKnowledgeMgr->Save(pMsg);
#endif
	m_pObjectTemplates->Save( pMsg );

	//save difficulty
	pMsg->Writeuint8(m_eDifficulty);

	//save min music mood.
	SAVE_DWORD(m_eMinMusicMood);

	g_pServerMissionMgr->Save( *pMsg, dwSaveFlags );
	g_pKeyMgr->Save( pMsg, dwSaveFlags );

	{ // BL 09/28/00 need to save/load these because they don't
	  // get set by worldproperties in a load situation

		SAVE_DWORD(m_nTimeRamps);
		SAVE_DWORD(m_iPrevRamp);
		for ( uint32 iTimeRamp = 0 ; iTimeRamp < MAX_TIME_RAMPS ; iTimeRamp++ )
		{
			SAVE_FLOAT(m_TimeRamps[iTimeRamp].m_Time);
			SAVE_VECTOR(m_TimeRamps[iTimeRamp].m_Color);
			SAVE_HSTRING(m_TimeRamps[iTimeRamp].m_hTarget);
			SAVE_HSTRING(m_TimeRamps[iTimeRamp].m_hMessage);
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::Load
//
//	PURPOSE:	Load the global world info
//
// ----------------------------------------------------------------------- //

void CGameServerShell::Load(ILTMessage_Read *pMsg, uint32 dwLoadFlags)
{
	_ASSERT(pMsg);
	if (!pMsg) return;

	// This old version didn't save the version number at the top.  We need
	// to use an engine value to determine the version.
	uint32 nSaveFileVersion = 0;
	g_pLTServer->GetSaveFileVersion( nSaveFileVersion );
	if( nSaveFileVersion > 2001 )
	{
		uint32 nSaveVersion;
		LOAD_DWORD(nSaveVersion);
		g_pVersionMgr->SetCurrentSaveVersion( nSaveVersion );
	}

	// Important: This has to be first, since it's taking the place of engine object behavior
	m_pLiteObjectMgr->Load(pMsg);

#ifdef USE_AI
	m_pCharacterMgr->Load(pMsg);
#endif
	m_CmdMgr.Load(pMsg);
#ifdef USE_AI
	m_pAIStimulusMgr->Load(pMsg);
	m_pAICentralKnowledgeMgr->Load(pMsg);
#endif
	m_pObjectTemplates->Load( pMsg );

	//load difficulty
	m_eDifficulty = (GameDifficulty)pMsg->Readuint8();
	g_pLTServer->CPrint("Difficulty:%d",(int)m_eDifficulty);

	//load min music mood.
	LOAD_DWORD_CAST(m_eMinMusicMood, CMusicMgr::Mood);

	g_pServerMissionMgr->Load( *pMsg, dwLoadFlags );
	g_pKeyMgr->Load( pMsg, dwLoadFlags );	

	{ // BL 09/28/00 need to save/load these because they don't
	  // get set by worldproperties in a load situation

		LOAD_DWORD(m_nTimeRamps);
		LOAD_DWORD(m_iPrevRamp);
		for ( uint32 iTimeRamp = 0 ; iTimeRamp < MAX_TIME_RAMPS ; iTimeRamp++ )
		{
			LOAD_FLOAT(m_TimeRamps[iTimeRamp].m_Time);
			LOAD_VECTOR(m_TimeRamps[iTimeRamp].m_Color);
			LOAD_HSTRING(m_TimeRamps[iTimeRamp].m_hTarget);
			LOAD_HSTRING(m_TimeRamps[iTimeRamp].m_hMessage);
		}
	}

	if( nSaveFileVersion == 2001 )
	{
		uint32 nSaveVersion;
		LOAD_DWORD(nSaveVersion);
		g_pVersionMgr->SetCurrentSaveVersion( nSaveVersion );
		HSTRING hDummy = NULL;
		LOAD_HSTRING(hDummy);
		g_pLTServer->FreeString( hDummy );
		hDummy = NULL;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetTimeRamp
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

TimeRamp* CGameServerShell::GetTimeRamp(uint32 i)
{
	if(i >= MAX_TIME_RAMPS)
	{
		ASSERT(FALSE);
		return NULL;
	}

	return &m_TimeRamps[i];
}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ShowMultiplayerSummary
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::ShowMultiplayerSummary()
{
	m_bShowMultiplayerSummary = LTTRUE;
	int endString = 0;
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		CPlayerObj* pPlayer = GetPlayerFromClientList(m_aClients[i]);
		if (pPlayer)
		{
			pPlayer->ReadyToExit(LTFALSE);
		}
	}
	m_fSummaryEndTime = g_pServerButeMgr->GetSummaryDelay() + g_pLTServer->GetTime();

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ProcessPacket
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

LTRESULT CGameServerShell::ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4], uint16 senderPort)
{
	if (pMsg->Size() >= 16)
	{
		// Skip the engine-side portion of the message header
		CLTMsgRef_Read cSubMsg(pMsg->SubMsg(8));

		// Remember what address this came from
		SetCurMessageSource(senderAddr, senderPort);

		cSubMsg->SeekTo(0);
		uint8 messageID = cSubMsg->Readuint8();
		
		switch (messageID)
		{
			case MID_MULTIPLAYER_SERVERDIR:		HandleMultiplayerServerDir		(NULL, cSubMsg);	break;
			default:
				break;
		}
	}

	return(LT_OK);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetFirstNetPlayer
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

CPlayerObj* CGameServerShell::GetFirstNetPlayer()
{
	m_nGetPlayerIndex = 0;
	CPlayerObj* pPlayer = GetPlayerFromClientList(m_aClients[m_nGetPlayerIndex]);
	while (!pPlayer && m_nGetPlayerIndex < MAX_CLIENTS-1)
	{
		m_nGetPlayerIndex++;
		pPlayer = GetPlayerFromClientList(m_aClients[m_nGetPlayerIndex]);
	}

	return(pPlayer);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetNextNetPlayer
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

CPlayerObj* CGameServerShell::GetNextNetPlayer()
{
	CPlayerObj* pPlayer = NULL;
	while (!pPlayer && m_nGetPlayerIndex < MAX_CLIENTS-1)
	{
		m_nGetPlayerIndex++;
		pPlayer = GetPlayerFromClientList(m_aClients[m_nGetPlayerIndex]);
	}

	return(pPlayer);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetPlayerPing
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

uint32 CGameServerShell::GetPlayerPing(CPlayerObj* pPlayer)
{
	if (!pPlayer) return(0);

	float ping = 0.0f;

	HCLIENT hClient = pPlayer->GetClient();
	if (!hClient) return(0);

	uint32 clientID = g_pLTServer->GetClientID(hClient);
	g_pLTServer->GetClientPing(hClient, ping);

	return((uint16)(ping + 0.5f));
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ClientConnectionInTrouble
//
//	PURPOSE:	Checks if client connection is in trouble.
//
// --------------------------------------------------------------------------- //

bool CGameServerShell::ClientConnectionInTrouble( HCLIENT hClient )
{
	// Check inputs.
	if( hClient == NULL )
	{
		ASSERT( !"CGameServerShell::ClientConnectionInTrouble:  Invalid inputs." );
		return false;
	}

	// If the player's ping is larger than the max allowed, drop him.
	float fClientPing = 0.0f;
	if( g_pLTServer->GetClientPing( hClient, fClientPing ) != LT_OK )
	{
		ASSERT( !"CGameServerShell::ClientConnectionInTrouble:  Bad client handle." );
		return false;
	}

	// Check if client is under limit.
	float fMaxClientPing = GetConsoleFloat( "MaxClientPing", 30000.0f );
	if( fClientPing  < fMaxClientPing )
		return false;

	// Client connection is in trouble.
	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetHostName
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

char const* CGameServerShell::GetHostName()
{
	return(m_ServerGameOptions.GetSessionName());
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetCurLevel
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

char const* CGameServerShell::GetCurLevel()
{
	return(m_sWorld);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::SetCurLevel
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::SetCurLevel( char const* pszWorldName )
{
	m_sWorld = pszWorldName;

	// [KLS 5/13/02] If we're changing the current level, then we need to allow for
	// caching of the level specific files for this new level...
	// (see the CacheLevelSpecificFiles() function)
	m_bCachedLevelFiles = false;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetNumPlayers
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

int	CGameServerShell::GetNumPlayers()
{
	CPlayerObj* pPlr   = GetFirstNetPlayer();
	int         nCount = 0;

	while (pPlr)
	{
		nCount++;
		pPlr = GetNextNetPlayer();
	}

	return(nCount);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::IsPositionOccupied
//
//	PURPOSE:	Check for any other player object's at this position
//
// ----------------------------------------------------------------------- //

LTBOOL CGameServerShell::IsPositionOccupied(LTVector & vPos, CPlayerObj* pPlayer)
{
	if( !pPlayer ) return LTFALSE;

	//CHECK_PLAYEROBJ_BEGIN();
	//CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjListBackup().begin();
	while( iter != CPlayerObj::GetPlayerObjListBackup().end() )
	{
		CPlayerObj* pUsedPlayer = *iter;

		if( pUsedPlayer && 
			pUsedPlayer->m_hObject != pPlayer->m_hObject &&
			pUsedPlayer->GetTeamID() == pPlayer->GetTeamID() )
		{
			LTVector vObjPos, vDims;
			g_pLTServer->GetObjectPos(pUsedPlayer->m_hObject, &vObjPos);
			g_pPhysicsLT->GetObjectDims(pUsedPlayer->m_hObject, &vDims);

			vDims *= 2.0f;
			if (vObjPos.x - vDims.x < vPos.x && vPos.x < vObjPos.x + vDims.x &&
				vObjPos.y - vDims.y < vPos.y && vPos.y < vObjPos.y + vDims.y &&
				vObjPos.z - vDims.z < vPos.z && vPos.z < vObjPos.z + vDims.z)
			{
				//CHECK_PLAYEROBJ_END();
				return LTTRUE;
			}			
		}

		iter++;
	}
	//CHECK_PLAYEROBJ_END();

	return LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ServerAppAddClient
//
//	PURPOSE:	The server app is the dedicated server application and is
//				usefull when running a dedicated server. on a regular server
//				SendToServerApp returns LT_NOTFOUND and nothing actually 
//				gets sent. inorder for SendToServerApp to return LT_OK we
//				need to call CreateServer to get a ServerInterface* and 
//				register a ServerAppHandler to it.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::ServerAppAddClient( HCLIENT hClient )
{
	// Write out the player information.
	CAutoMessage cMsg;
	cMsg.Writeuint8( SERVERAPP_ADDCLIENT );
	cMsg.Writeuint16( g_pLTServer->GetClientID( hClient ));

	// Send to the server app.
	CLTMsgRef_Read msgRefRead = cMsg.Read( );
	g_pLTServer->SendToServerApp( *msgRefRead );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ServerAppRemoveClient
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::ServerAppRemoveClient( HCLIENT hClient )
{
	// Write out the player information.
	CAutoMessage cMsg;
	cMsg.Writeuint8( SERVERAPP_REMOVECLIENT );
	cMsg.Writeuint16( g_pLTServer->GetClientID( hClient ));

	// Send to the server app.
	CLTMsgRef_Read msgRefRead = cMsg.Read( );
	g_pLTServer->SendToServerApp( *msgRefRead );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ServerAppShellUpdate
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::ServerAppShellUpdate( )
{
	int i;

	// Write the message id.
	CAutoMessage cMsg;
	cMsg.Writeuint8( SERVERAPP_SHELLUPDATE );

	// Add info for each player.
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		CPlayerObj* pPlayer = GetPlayerFromClientList(m_aClients[i]);
		if( !pPlayer )
			continue;

		HCLIENT hClient = pPlayer->GetClient();
		if( !hClient )
			continue;

		cMsg.Writeuint16( g_pLTServer->GetClientID( hClient ));
		cMsg.WriteString( pPlayer->GetNetUniqueName( ));
		cMsg.Writeint16( pPlayer->GetPlayerScore()->GetFrags( ));
		cMsg.Writeint16( pPlayer->GetPlayerScore()->GetTags( ));
		cMsg.Writeint16( pPlayer->GetPlayerScore()->GetScore( ));
	}

	// Signal end of player list.
	cMsg.Writeuint16(( uint16 )-1 );

	// Send to the server app.
	CLTMsgRef_Read msgRefRead = cMsg.Read( );
	g_pLTServer->SendToServerApp( *msgRefRead );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ServerAppPreStartWorld
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::ServerAppPreStartWorld( )
{
	CAutoMessage cMsg;
	cMsg.Writeuint8( SERVERAPP_PRELOADWORLD );

	// Send to the server app.
	CLTMsgRef_Read msgRefRead = cMsg.Read( );
	g_pLTServer->SendToServerApp( *msgRefRead );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ServerAppPostStartWorld
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CGameServerShell::ServerAppPostStartWorld( )
{
	CAutoMessage cMsg;
	cMsg.Writeuint8( SERVERAPP_POSTLOADWORLD );
	cMsg.Writeuint16(( int )g_pServerMissionMgr->GetCurrentCampaignIndex( ));

	char fname[_MAX_FNAME] = "";
	_splitpath( GetCurLevel( ), NULL, NULL, fname, NULL );
 	cMsg.WriteString( fname );

	// Send to the server app.
	CLTMsgRef_Read msgRefRead = cMsg.Read( );
	g_pLTServer->SendToServerApp( *msgRefRead );
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::SetServerDir
//
//	PURPOSE:	Change the server directory object
//
// --------------------------------------------------------------------------- //

void CGameServerShell::SetServerDir(IServerDirectory *pDir)
{ 
	if( m_pServerDir )
	{
		// No leaking, please...
		delete m_pServerDir; 
	}

	m_pServerDir = pDir; 
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::SetCurMessageSource
//
//	PURPOSE:	Set the source address of the message which is currently being processed
//
// --------------------------------------------------------------------------- //

void CGameServerShell::SetCurMessageSource(const uint8 aAddr[4], uint16 nPort)
{
	m_aCurMessageSourceAddr[0] = aAddr[0];
	m_aCurMessageSourceAddr[1] = aAddr[1];
	m_aCurMessageSourceAddr[2] = aAddr[2];
	m_aCurMessageSourceAddr[3] = aAddr[3];
	m_nCurMessageSourcePort = nPort;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetCurMessageSource
//
//	PURPOSE:	Get the source address of the message which is currently being processed
//
// --------------------------------------------------------------------------- //

void CGameServerShell::GetCurMessageSource(uint8 aAddr[4], uint16 *pPort)
{
	aAddr[0] = m_aCurMessageSourceAddr[0];
	aAddr[1] = m_aCurMessageSourceAddr[1];
	aAddr[2] = m_aCurMessageSourceAddr[2];
	aAddr[3] = m_aCurMessageSourceAddr[3];
	*pPort = m_nCurMessageSourcePort;
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::MatchClientNameToPlayer
//
//	PURPOSE:	Matches a client name to a clientref and returns the
//				playerobj of the clientref.  If it can't match the client
//				to the player, it returns null.
//
// --------------------------------------------------------------------------- //

CPlayerObj* CGameServerShell::MatchClientNameToPlayer( char const* pszClientName )
{
	// Check inputs.
	if( !pszClientName )
	{
		ASSERT( !"CGameServerShell::MatchClientToPlayer: Invalid inputs." );
		return NULL;
	}

	char szClientRefName[MAX_CLIENT_NAME_LENGTH] = "";

	// Search through all the clientrefs to find the one we were using when the player
	// got saved.  This is done by matching our IP addresses together.
    HCLIENTREF hNextClientRef = g_pLTServer->GetNextClientRef(LTNULL);
	while (hNextClientRef)
	{
		// Copy the current clientref and advance to the next.  Makes body of loop easier.
		HCLIENTREF hClientRef = hNextClientRef;
		hNextClientRef = g_pLTServer->GetNextClientRef(hClientRef);

		// Get the player associated with this clientref.
		HOBJECT hObject = g_pLTServer->GetClientRefObject( hClientRef );
		CPlayerObj* pPlayer = dynamic_cast< CPlayerObj* >( g_pLTServer->HandleToObject( hObject ));
		if( !pPlayer )
			continue;

		// Get the clientname from the clientref and compare it to the input clientname.
		g_pLTServer->GetClientRefName( hClientRef, szClientRefName, ARRAY_LEN( szClientRefName ));
		if( _stricmp( pszClientName, szClientRefName ) != 0 )
			continue;

		// Found a matching player.
		return pPlayer;
	}

	// No player found.
	return NULL;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::FindFreePlayerFromClientRefs
//
//	PURPOSE:	Look through all the clientrefs and see if there is any
//				that doesn't match a different client.  
//
// --------------------------------------------------------------------------- //

CPlayerObj* CGameServerShell::FindFreePlayerFromClientRefs( )
{
	char szClientName[MAX_CLIENT_NAME_LENGTH] = "";

	// Iterate through the clientrefs.  Check if the clientref's playerobj
	// doesn't match a client that doesn't have a playerobj associated with it yet.
	HCLIENTREF hNextClientRef = g_pLTServer->GetNextClientRef(LTNULL);
	while( hNextClientRef )
	{
		// Copy the current clientref and advance to the next.  Makes body of loop easier.
		HCLIENTREF hClientRef = hNextClientRef;
		hNextClientRef = g_pLTServer->GetNextClientRef( hClientRef );

		// Get the player associated with this clientref.
		HOBJECT hObject = g_pLTServer->GetClientRefObject( hClientRef );
		CPlayerObj* pPlayerRef = dynamic_cast< CPlayerObj* >( g_pLTServer->HandleToObject( hObject ));
		if( !pPlayerRef )
			continue;

		// Loop through the other clients and see if they match this clientref.
		HCLIENT hNextClient = g_pLTServer->GetNextClient( LTNULL );
		bool bMatched = false;
		while( hNextClient )
		{
			// Copy the current client and advance to the next.  Makes body of loop easier.
			HCLIENT hOtherClient = hNextClient;
			hNextClient = g_pLTServer->GetNextClient( hOtherClient );

			// Check if this client has already been hooked up to a player.  If so, skip it.
			CPlayerObj* pUsedPlayer = ( CPlayerObj* )g_pLTServer->GetClientUserData( hOtherClient );
			if( pUsedPlayer )
				continue;

			// Get the client name.
			g_pLTServer->GetClientName( hOtherClient, szClientName, ARRAY_LEN( szClientName ));

			// If this client doesn't match any clientrefs, then skip it.
			CPlayerObj* pClientMatch = MatchClientNameToPlayer( szClientName );
			if( !pClientMatch )
				continue;

			// Check if this client matches a different clientref.  If so, skip it.
			if( pClientMatch != pPlayerRef )
				continue;

			// This client matches this clientref.  Stop looking and go to the next clientref.
			bMatched = true;				
			break;
		}

		// If no client matched this clientref, then we can take it.
		if( !bMatched )
		{
			// No client matched this clientref, we can use it.
			if( pPlayerRef )
				return pPlayerRef;
		}
	}

	// Didn't find any free clientref players.
	return NULL;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::PickClientRefPlayer
//
//	PURPOSE:	Find the clientref for this client based on the set clientname.
//
// --------------------------------------------------------------------------- //

CPlayerObj* CGameServerShell::PickClientRefPlayer( HCLIENT hClient )
{
	// Check inputs.
	if( !hClient )
	{
		ASSERT( !"CGameServerShell::FindPlayerFromClientRefs: Invalid inputs." );
		return NULL;
	}
	
	// Get the clientname.  Must be set before entering this function.
	char szClientName[MAX_CLIENT_NAME_LENGTH] = "";
	g_pLTServer->GetClientName( hClient, szClientName, ARRAY_LEN( szClientName ));
	if( !szClientName[0] )
	{
		// If we've got a client without a name, they're not ready yet...
		return NULL;
	}

	// See if we have a direct match between our client and a player.
	CPlayerObj* pPlayer = MatchClientNameToPlayer( szClientName );
	if( !pPlayer )
		return NULL;

	return pPlayer;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ProcessObjectiveMessage
//
//	PURPOSE:	Process the objectives and fill out the info if supplied...
//
// --------------------------------------------------------------------------- //

bool CGameServerShell::ProcessObjectiveMessage( const CParsedMsg &cMsg, ObjectiveMsgInfo *pInfo )
{
	static CParsedMsg::CToken s_cTok_Objective("Objective");
	static CParsedMsg::CToken s_cTok_Option("Option");
	static CParsedMsg::CToken s_cTok_Parameter("Parameter");
	static CParsedMsg::CToken s_cTok_Add("Add");
	static CParsedMsg::CToken s_cTok_Remove("Remove");
	static CParsedMsg::CToken s_cTok_RemoveAll("RemoveAll");
	static CParsedMsg::CToken s_cTok_Completed("Completed");

	if (cMsg.GetArgCount() < 2)
		return false;

	uint8 nType = 0;
	if( cMsg.GetArg(0) == s_cTok_Objective )
	{
		nType = IC_OBJECTIVE_ID;
	}
	else if( cMsg.GetArg(0) == s_cTok_Option )
	{
		nType = IC_OPTION_ID;
	}
	else if( cMsg.GetArg(0) == s_cTok_Parameter )
	{
		nType = IC_PARAMETER_ID;
	}
	else
	{
		return false;
	}

	
    uint8 nRequest = ITEM_ADD_ID;
	if (cMsg.GetArg(1) == s_cTok_Add)
	{
		nRequest = ITEM_ADD_ID;
	}
	else if (cMsg.GetArg(1) == s_cTok_Remove)
	{
		nRequest = ITEM_REMOVE_ID;
	}
	else if (cMsg.GetArg(1) == s_cTok_RemoveAll)
	{
		nRequest = ITEM_CLEAR_ID;
	}
	else if ((cMsg.GetArg(1) == s_cTok_Completed) && (nType != IC_PARAMETER_ID))
	{
		nRequest = ITEM_COMPLETE_ID;
	}
	else
	{
        return false;
	}



    uint32 dwId = 0;
	if (nRequest != ITEM_CLEAR_ID)
	{
        if (cMsg.GetArgCount() < 3) return false;
        dwId = (uint32) atol(cMsg.GetArg(2));
	}


	switch (nRequest)
	{
		case ITEM_ADD_ID:
		{
			if (nType == IC_OPTION_ID)
			{
				m_OptionalObjectives.Add(dwId);
			}
			else if (nType == IC_PARAMETER_ID)
			{
				m_Parameters.Add(dwId);
			}
			else
			{
				m_Objectives.Add(dwId);
			}
			
		}
		break;

		case ITEM_REMOVE_ID:
		{
			m_Objectives.Remove(dwId);
			m_OptionalObjectives.Remove(dwId);
			m_CompletedObjectives.Remove(dwId);
			m_Parameters.Remove(dwId);
		}
		break;

		case ITEM_COMPLETE_ID:
		{
			m_Objectives.Remove(dwId);
			m_OptionalObjectives.Remove(dwId);
			m_CompletedObjectives.Add(dwId);

		}
		break;

		case ITEM_CLEAR_ID:
		{
			m_Objectives.Clear();
			m_OptionalObjectives.Clear();
			m_CompletedObjectives.Clear();
			m_Parameters.Clear();
		}
		break;
	}

	if( pInfo )
	{
		pInfo->m_nType		= nType;
		pInfo->m_nRequest	= nRequest;
		pInfo->m_dwId		= dwId;
	}

	return true;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ResetObjectives
//
//	PURPOSE:	Clear out all of our objectives...
//
// --------------------------------------------------------------------------- //

void CGameServerShell::ResetObjectives()
{
	m_Objectives.Clear();
	m_OptionalObjectives.Clear();
	m_CompletedObjectives.Clear();
	m_Parameters.Clear();
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::SaveObjectives
//
//	PURPOSE:	Save all of our objectives...
//
// --------------------------------------------------------------------------- //

void CGameServerShell::SaveObjectives( ILTMessage_Write *pMsg, uint32 dwSaveFlags )
{
	m_Objectives.Save( pMsg );
	m_OptionalObjectives.Save( pMsg );
	m_CompletedObjectives.Save( pMsg );
	m_Parameters.Save( pMsg );
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::LoadObjectives
//
//	PURPOSE:	Load all of our objectives...
//
// --------------------------------------------------------------------------- //

void CGameServerShell::LoadObjectives( ILTMessage_Read *pMsg, uint32 dwLoadFlags )
{
	m_Objectives.Load( pMsg );
	m_OptionalObjectives.Load( pMsg );
	m_CompletedObjectives.Load( pMsg );
	m_Parameters.Load( pMsg );
}
// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::ApplyWeaponRestrictions
//
//	PURPOSE:	Apply the server game option weapon restrictions.
//
// ----------------------------------------------------------------------- //

void CGameServerShell::ApplyWeaponRestrictions( )
{
	StringSet::iterator iter = m_ServerGameOptions.m_setRestrictedWeapons.begin( );
	while( iter != m_ServerGameOptions.m_setRestrictedWeapons.end( ))
	{
		std::string const& sValue = *iter;
		iter++;

		WEAPON* pWeapon = ( WEAPON* )g_pWeaponMgr->GetWeapon( sValue.c_str( ));
		if( !pWeapon )
			continue;

		if( pWeapon->bCanServerRestrict )
			pWeapon->bServerRestricted = true;
	}

	iter = m_ServerGameOptions.m_setRestrictedAmmo.begin( );
	while( iter != m_ServerGameOptions.m_setRestrictedAmmo.end( ))
	{
		std::string const& sValue = *iter;
		iter++;

		AMMO* pAmmo = ( AMMO* )g_pWeaponMgr->GetAmmo( sValue.c_str( ));
		if( !pAmmo )
			continue;

		if( pAmmo->bCanServerRestrict )
			pAmmo->bServerRestricted = true;
	}

	iter = m_ServerGameOptions.m_setRestrictedGear.begin( );
	while( iter != m_ServerGameOptions.m_setRestrictedGear.end( ))
	{
		std::string const& sValue = *iter;
		iter++;

		GEAR* pGear = ( GEAR* )g_pWeaponMgr->GetGear( sValue.c_str( ));
		if( !pGear )
			continue;

		if( pGear->bCanServerRestrict )
			pGear->bServerRestricted = true;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::SetLGFlags
//
//	PURPOSE:	Set the load game flags for...
//
// ----------------------------------------------------------------------- //

void CGameServerShell::SetLGFlags( uint8 nLGFlags )
{
	m_nLastLGFlags = nLGFlags;
	
	if( g_pVersionMgr )
		g_pVersionMgr->SetLGFlags( m_nLastLGFlags );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGameServerShell::GetClientData
//
//	PURPOSE:	Gets the clientdata given a HCLIENT.
//
// ----------------------------------------------------------------------- //

ClientData* CGameServerShell::GetClientData( HCLIENT hClient )
{
	// Remove this client from our list of clients.
	ClientDataList::iterator iter = m_ClientDataList.begin( );
	for( ; iter != m_ClientDataList.end( ); iter++ )
	{
		ClientData* pClientData = *iter;
		if( pClientData->m_hClient == hClient )
		{
			return pClientData;
		}
	}

	return NULL;
}

bool CGameServerShell::StartClientTCP( const char* lpszWorldName )
{
	return g_pServerMissionMgr->ClientStart( lpszWorldName );
}

CPlayerObj* CGameServerShell::GetHostPlayer()
{
	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayer = *iter;
		if( pPlayer && pPlayer->IsHostPlayer() )
		{
			CHECK_PLAYEROBJ_END();
			return pPlayer;
		}

		iter++;
	}
	CHECK_PLAYEROBJ_END();

	return LTNULL;
}

HCLIENT CGameServerShell::GetHostPlayerHandle()
{
	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayer = *iter;
		if( pPlayer && pPlayer->IsHostPlayer() )
		{
			CHECK_PLAYEROBJ_END();
			return pPlayer->GetClient();
		}

		iter++;
	}
	CHECK_PLAYEROBJ_END();

	return LTNULL;
}

bool CGameServerShell::KickBanIfExist( int32 nServerIndex )
{
	HCLIENT hRemoveClient = LTNULL;

	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayer = *iter;
		if( pPlayer && pPlayer->GetServerIndex() == nServerIndex )
		{
			hRemoveClient = pPlayer->GetClient();
			break;
		}

		iter++;
	}
	CHECK_PLAYEROBJ_END();

	if( hRemoveClient )
	{
		g_pLTServer->CPrint( "SilentKickClient: %d", nServerIndex );
		g_pLTServer->SilentKickClient( hRemoveClient );
		OnClientExitWorld( hRemoveClient );
		OnRemoveClient( hRemoveClient );	
		return true;
	}

	return false;
}

void CGameServerShell::SetMissionInfo(ILTMessage_Read *pMsg)
{
	switch( GetGameType() )
	{
	case eGameTypeSurvival:
		{
			
			LTVector	vPos	= pMsg->ReadLTVector();
			LTRotation	rRot	= pMsg->ReadLTRotation();
			float		fTime	= pMsg->Readfloat();		
			g_pLTServer->SetObjectPos( g_pGadgetTarget->GetHOBJECT(), &vPos );
			g_pLTServer->SetObjectRotation(g_pGadgetTarget->GetHOBJECT(), &rRot);
			g_pGadgetTarget->ForcePlantedTimeBomb( fTime );
		}
		break;
	}
}