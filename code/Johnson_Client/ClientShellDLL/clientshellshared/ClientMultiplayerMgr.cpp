// ----------------------------------------------------------------------- //
//
// MODULE  : CClientMultiplayerMgr.cpp
//
// PURPOSE : Clientside multiplayer mgr - Definition
//
// CREATED : 02/05/02
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "ClientMultiplayerMgr.h"
#include "GameClientShell.h"
#include "MsgIds.h"
#include "CharacterFx.h"
#include "CMoveMgr.h"
#include "iserverdir.h"
#include "MissionMgr.h"
#include "MissionButeMgr.h"
#include "InterfaceMgr.h"
#include "ProfileMgr.h"
#include "WeaponMgr.h"
#include "CRC32.h"
#include "../SA/SAHUDMgr.h"

#define LT_MAX_CON	64

#define PRE_LOAD_FILE_LIST
#ifndef _FINAL
	//#undef PRE_LOAD_FILE_LIST
#endif

#pragma message( "FIXFIX:  Should really be called ClientConnectionMgr." )


CClientMultiplayerMgr* g_pClientMultiplayerMgr = NULL;


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::CClientMultiplayerMgr
//
//	PURPOSE:	Constructor
//
// --------------------------------------------------------------------------- //

CClientMultiplayerMgr::CClientMultiplayerMgr( )
{
	m_nServerPort = -1;
	m_nServerKey = 0;
	m_nDisconnectCode = 0;
	m_bForceDisconnect = false;
	m_pServerDir = 0;
	m_aCurMessageSourceAddr[0] = 0;
	m_aCurMessageSourceAddr[1] = 0;
	m_aCurMessageSourceAddr[2] = 0;
	m_aCurMessageSourceAddr[3] = 0;
	m_nCurMessageSourcePort = 0;

	memset( &m_StartGameRequest, 0, sizeof( m_StartGameRequest ));
	m_StartGameRequest.m_Type = GAMEMODE_NONE;
	memset( &m_NetClientData, 0, sizeof( m_NetClientData ));

	m_nLastConnectionResult = LT_OK;

	g_pClientMultiplayerMgr = this;
	
	g_GameDoc.SetMandateHost( LTFALSE );
	g_GameDoc.SetRejoinHost( LTFALSE );

#ifdef BIG_FAT_MODE_TEST
	m_ePlayerModelType	= eModelNormal;
#endif 
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::~CClientMultiplayerMgr
//
//	PURPOSE:	Destructor
//
// --------------------------------------------------------------------------- //

CClientMultiplayerMgr::~CClientMultiplayerMgr( )
{
	delete GetServerDir();

	g_pClientMultiplayerMgr = NULL;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::InitSinglePlayer
//
//	PURPOSE:	Send the server the initial single player info
//
// --------------------------------------------------------------------------- //

bool CClientMultiplayerMgr::InitSinglePlayer()
{

	g_pWeaponMgr->LoadOverrideButes( WEAPON_DEFAULT_FILE );

	//force server to update difficulty
	GameDifficulty eDiff = g_pGameClientShell->GetDifficulty();
	g_pGameClientShell->SetDifficulty(eDiff);

	//force server to update performance settings
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();
	pProfile->SendPerformanceMsg();

	return true;
}



//[MURSUM] Cone
bool CClientMultiplayerMgr::InitNetworking( bool bTermShell )
{
	// Initialize the networking.  Always start a new server with hosted games.
	m_nLastConnectionResult = g_pLTClient->InitNetworking(NULL, 0,bTermShell);
	if (m_nLastConnectionResult != LT_OK) return false;

	// Initialize our protocol.
	if (!SetService()) return false;

	return true;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::InitMultiPlayer
//
//	PURPOSE:	Send the server the initial multiplayer info
//
// --------------------------------------------------------------------------- //

bool CClientMultiplayerMgr::InitMultiPlayer()
{
	CAutoMessage cMsg;

	cMsg.Writeuint8( MID_PLAYER_MULTIPLAYER_INIT );	
	cMsg.Writeuint8( g_GameDoc.GetTeam() );
	
	if( LTFALSE == g_GameDoc.IsMandateHost() && LTFALSE == g_GameDoc.IsRejoinHost() )
	{
		cMsg.Writebool(false);
		g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);
		return true;
	}

	//방장 위임 후 재 접속일 경우 
	cMsg.Writebool(true);

	if( g_GameDoc.IsMandateHost() )	//미션정보는 Host가 될 놈만 쓴다.
	{
		SetMissionInfo( cMsg );
		cMsg.Writebool(true);
		cMsg.Writefloat( g_GameDoc.GetBackupWorldTime() );
	}
	else
	{
		cMsg.Writebool(false);	//미션정보가 없음
		cMsg.Writebool(false);	//시간 정보가 없음 
	}

	cMsg.WriteLTVector( g_GameDoc.GetBackupPlayerPos() );
	cMsg.WriteLTVector( g_GameDoc.GetBackupPlayerPYR() );
	cMsg.Writeuint8( g_GameDoc.GetBackupPlayerHP() );
	cMsg.Writeuint8( g_GameDoc.GetBackupPlayerAP() );

	//[Weapon Data]
	T_BACKUP_WEAPONptr pBackupWeapon;
	for( int i=0 ; i<5 ; ++i ) //MAX_WEAPON_CLASS
	{
		pBackupWeapon = g_GameDoc.GetBackupPlayerWeapon( i );
		if( pBackupWeapon )
		{
			cMsg.Writebool(true);
			cMsg.Writeuint8( (uint8)pBackupWeapon->nWeaponID );
			cMsg.Writeuint16( (uint16)pBackupWeapon->nAmmoCount );
			cMsg.Writeuint8( (uint8)pBackupWeapon->nAmmoInClip );
		}
	}	
	cMsg.Writebool(false);	//더 이상 무기정보가 없음

    g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);
	return true;
}


void CClientMultiplayerMgr::SetMissionInfo( ILTMessage_Write *pMsg )
{
	switch( g_pGameClientShell->GetGameType() )
	{
	case eGameTypeSurvival:
		if( LTTRUE == g_pPlayerMgr->GetPlantedTimeBomb() )
		{
			pMsg->Writebool(true);
			pMsg->WriteLTVector( g_GameDoc.GetTimeBombPos() );
			pMsg->WriteLTRotation( g_GameDoc.GetTimeBombRot() );
			pMsg->Writefloat( g_GameDoc.GetTimeBombTime() );
		}
		else
		{
			pMsg->Writebool(false);
		}
		break;

	case eGameTypeTeamDeathmatch:
	default:
		pMsg->Writebool(false);
		break;
	}
}

// returns true if the passed in address matches the current server address
bool CClientMultiplayerMgr::CheckServerAddress(char const*pszTestAddress, int nPort)
{
	if (!pszTestAddress) return false;
	if (nPort != m_nServerPort) return false;
	return (stricmp(pszTestAddress,m_sServerAddress) == 0);
}

bool CClientMultiplayerMgr::SetupClient( char const* pszIpAddress, char const* pszHostName, char const* pszPassword, bool bSpectator )
{
	// Check inputs.
	if( !pszIpAddress || !pszIpAddress[0] )
	{
		ASSERT( !"CClientMultiplayerMgr::SetupClient: Invalid inputs." );
		return false;
	}

	// Start the game...

	memset( &m_StartGameRequest, 0, sizeof( m_StartGameRequest ));
	memset( &m_NetClientData, 0, sizeof( m_NetClientData ));
	if( !UpdateNetClientData( )) return false;	
	m_NetClientData.m_bHost			= false;
	m_NetClientData.m_bSpectator	= bSpectator;
	m_NetClientData.m_nRound		= g_pMissionMgr->GetCurrentRound();

	m_StartGameRequest.m_Type = STARTGAME_CLIENTTCP;
	strncpy( m_StartGameRequest.m_TCPAddress, pszIpAddress, MAX_SGR_STRINGLEN);
	if( pszHostName && pszHostName[0] )
	{
		SAFE_STRCPY(m_StartGameRequest.m_HostInfo.m_sName ,pszHostName);
	}

	if (pszPassword)
	{
		m_nClientPass = str_Hash(pszPassword);
	}
	else
	{
		m_nClientPass = 0;
	}

	g_GameDoc.SetMandateHost( LTFALSE );
	g_GameDoc.SetRejoinHost( LTFALSE );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::SetupServerSinglePlayer
//
//	PURPOSE:	Setup server for singleplayer game.
//
// ----------------------------------------------------------------------- //
bool CClientMultiplayerMgr::SetupServerSinglePlayer( )
{
	memset(&m_StartGameRequest, 0, sizeof(StartGameRequest));
	m_ServerGameOptions.Clear( );
	memset( &m_NetClientData, 0, sizeof( m_NetClientData ));

	// Set the game type to normal (single player).
	m_StartGameRequest.m_Type = STARTGAME_NORMAL;

	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();
	m_ServerGameOptions = pProfile->m_ServerGameOptions;
	m_ServerGameOptions.m_bPreCacheAssets = (pProfile->m_sPerformance.nSettings[kPerform_PreCacheAssets] > 0);
	m_ServerGameOptions.m_bPerformanceTest = false;

	return true;
}
// --------------------------------------------------------------------------- //
//
//  ROUTINE:    CClientMultiplayerMgr::SetupServerHost()
//
//  PURPOSE:    Host a game.
//
//	PARAMETERS:	int nPort - Port to use, 0 to use default
//
// --------------------------------------------------------------------------- //

bool CClientMultiplayerMgr::SetupServerHost()
{
	memset( &m_StartGameRequest, 0, sizeof( m_StartGameRequest ));
	memset( &m_NetClientData, 0, sizeof( m_NetClientData ));	
	if( !UpdateNetClientData( )) return false;
	m_NetClientData.m_bSpectator	= false;
	m_NetClientData.m_bHost			= true;
	m_NetClientData.m_nRound		= 0;

	m_StartGameRequest.m_Type							= STARTGAME_HOST;
	m_StartGameRequest.m_HostInfo.m_Port				= m_ServerGameOptions.m_nPort;
	m_StartGameRequest.m_HostInfo.m_bHasPassword		= m_ServerGameOptions.m_bUsePassword;
	m_StartGameRequest.m_HostInfo.m_nGameType			= m_ServerGameOptions.m_eGameType;
	m_StartGameRequest.m_HostInfo.m_dwMaxConnections	= LT_MAX_CON;
	strcpy( m_StartGameRequest.m_HostInfo.m_sName, g_GameDoc.GetTitle() );
	m_StartGameRequest.m_HostInfo.m_dwFlags				= 0;
	
	// Set the name of the mod we are using...
	
	m_ServerGameOptions.m_sModName = GetModName();
	
	// Make sure that the multiplayer mgr doesn't have a server directory in use
	// This must be done because there can only be one IServerDirectory object
	// created at a time for proper shutdown.  (Internal Titan implementation BS...)
	// NYI - Note : This shouldn't be necessary, and if it is, it will cause problems
	// elsewhere.  (Host/Join sequence = 2 objects) Figure out a way to get around this 
	// restriction.
	DeleteServerDir( );
	
	g_GameDoc.SetMandateHost( LTFALSE );
	g_GameDoc.SetRejoinHost( LTFALSE );

	return true;
}

bool CClientMultiplayerMgr::SetupMandateHost()
{
	memset( &m_StartGameRequest, 0, sizeof( m_StartGameRequest ));
	memset( &m_NetClientData, 0, sizeof( m_NetClientData ));	
	if( !UpdateNetClientData() ) return false;
	m_NetClientData.m_bHost			= true;
	m_NetClientData.m_bSpectator	= false;
	m_NetClientData.m_nRound		= g_pMissionMgr->GetCurrentRound();	
	
	m_StartGameRequest.m_Type							= MANDATE_HOST;
	m_StartGameRequest.m_HostInfo.m_Port				= m_ServerGameOptions.m_nPort;
	m_StartGameRequest.m_HostInfo.m_bHasPassword		= m_ServerGameOptions.m_bUsePassword;
	m_StartGameRequest.m_HostInfo.m_nGameType			= m_ServerGameOptions.m_eGameType;
	m_StartGameRequest.m_HostInfo.m_dwMaxConnections	= LT_MAX_CON;
	strcpy( m_StartGameRequest.m_HostInfo.m_sName, g_GameDoc.GetTitle() );	
	m_StartGameRequest.m_HostInfo.m_dwFlags				= 0;
	
	// Set the name of the mod we are using...
	
	m_ServerGameOptions.m_sModName = GetModName();
	
	DeleteServerDir();

	g_GameDoc.SetMandateHost( LTTRUE );

	return true;
}

bool CClientMultiplayerMgr::SetupRejoin( char const* pszIpAddress, char const* pszHostName, char const* pszPassword, bool bSpectator )
{
	// Check inputs.
	if( !pszIpAddress || !pszIpAddress[0] )
	{
		ASSERT( !"CClientMultiplayerMgr::SetupClient: Invalid inputs." );
		return false;
	}

	memset( &m_StartGameRequest, 0, sizeof( m_StartGameRequest ));
	memset( &m_NetClientData, 0, sizeof( m_NetClientData ));	
	if( !UpdateNetClientData( )) return false;
	m_NetClientData.m_bHost			= false;
	m_NetClientData.m_bSpectator	= bSpectator;
	m_NetClientData.m_nRound		= g_pMissionMgr->GetCurrentRound();	

	m_StartGameRequest.m_Type = REJOIN_HOST;
	strncpy( m_StartGameRequest.m_TCPAddress, pszIpAddress, MAX_SGR_STRINGLEN);
	if( pszHostName && pszHostName[0] )
	{
		SAFE_STRCPY(m_StartGameRequest.m_HostInfo.m_sName ,pszHostName);
	}

	if (pszPassword)
	{
		m_nClientPass = str_Hash(pszPassword);
	}
	else
	{
		m_nClientPass = 0;
	}

	g_GameDoc.SetRejoinHost( LTTRUE );

	return true;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::SetDisconnectCode
//
//	PURPOSE:	Sets the disconnection code and message
//
// --------------------------------------------------------------------------- //

void CClientMultiplayerMgr::SetDisconnectCode(uint32 nCode, const char *pMsg)
{
	// Don't override what someone already told us
	if (m_nDisconnectCode)
		return;

	m_nDisconnectCode = nCode;

	if( pMsg )
		m_sDisconnectMsg = pMsg;
	else
		m_sDisconnectMsg.Empty( );
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::ClearDisconnectCode
//
//	PURPOSE:	Clears the disconnection code and message
//
// --------------------------------------------------------------------------- //

void CClientMultiplayerMgr::ClearDisconnectCode()
{
	m_nDisconnectCode = 0;
	m_sDisconnectMsg.Empty( );
	m_StartGameRequest.m_Type = GAMEMODE_NONE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::CreateServerDir
//
//	PURPOSE:	Creates the client's serverdir for joining a remote game.
//
// --------------------------------------------------------------------------- //

IServerDirectory* CClientMultiplayerMgr::CreateServerDir( )
{
	// Make sure we don't already have one.
	DeleteServerDir( );

	// Get the resource module so we can give it to the serverdir for
	// error messages.
	void* pModule = NULL;
	g_pLTClient->GetEngineHook("cres_hinstance",&pModule);
	HMODULE hModule = (HINSTANCE)pModule;

	m_pServerDir = Factory_Create_IServerDirectory_Titan( true, *g_pLTClient, hModule );
	if( !m_pServerDir )
		return NULL;

	// Set the game's name
	m_pServerDir->SetGameName(g_pVersionMgr->GetNetGameName());
	// Set the version
	m_pServerDir->SetVersion(g_pVersionMgr->GetNetVersion());
	m_pServerDir->SetRegion(g_pVersionMgr->GetNetRegion());
	// Set up the packet header
	CAutoMessage cMsg;
	cMsg.Writeuint8(11); // CMSG_MESSAGE
	cMsg.Writeuint8(MID_MULTIPLAYER_SERVERDIR);
	m_pServerDir->SetNetHeader(*cMsg.Read());

	return m_pServerDir;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::DeleteServerDir
//
//	PURPOSE:	Remove the server dir.
//
// --------------------------------------------------------------------------- //

void CClientMultiplayerMgr::DeleteServerDir( )
{ 
	if( m_pServerDir )
	{
		// No leaking, please...
		delete m_pServerDir; 
		m_pServerDir = NULL; 
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::SetCurMessageSource
//
//	PURPOSE:	Set the source address of the message which is currently being processed
//
// --------------------------------------------------------------------------- //

void CClientMultiplayerMgr::SetCurMessageSource(const uint8 aAddr[4], uint16 nPort)
{
	m_aCurMessageSourceAddr[0] = aAddr[0];
	m_aCurMessageSourceAddr[1] = aAddr[1];
	m_aCurMessageSourceAddr[2] = aAddr[2];
	m_aCurMessageSourceAddr[3] = aAddr[3];
	m_nCurMessageSourcePort = nPort;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::GetCurMessageSource
//
//	PURPOSE:	Get the source address of the message which is currently being processed
//
// --------------------------------------------------------------------------- //

void CClientMultiplayerMgr::GetCurMessageSource(uint8 aAddr[4], uint16 *pPort)
{
	aAddr[0] = m_aCurMessageSourceAddr[0];
	aAddr[1] = m_aCurMessageSourceAddr[1];
	aAddr[2] = m_aCurMessageSourceAddr[2];
	aAddr[3] = m_aCurMessageSourceAddr[3];
	*pPort = m_nCurMessageSourcePort;
}


#ifndef SOURCE_RELEASE

#pragma message( "FIXFIX: Remember to remove sections defined without SOURCE_RELEASE defines" )

// Global key used for cross-encryption  NOTE: MUST BE SAME AS ON SERVER!!
#define KEY_MESSER_UPPER 0xDEADFACE

inline uint32 MessUp32BitValue(uint32 nValue, uint32 nRot)
{
	// "Encrypt" the value (Yes, it's a very simple encryption...)
	uint32 nXORShift = nRot & 0xF;
	return ((KEY_MESSER_UPPER << nXORShift) + (KEY_MESSER_UPPER >> (32 - nXORShift))) ^ nValue;
}
#endif // SOURCE_RELEASE

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::OnMessage()
//
//	PURPOSE:	Handle client messages
//
// ----------------------------------------------------------------------- //

bool CClientMultiplayerMgr::OnMessage(uint8 messageID, ILTMessage_Read *pMsg)
{
	// Check inputs.
	if( !pMsg )
	{
		ASSERT( !"CClientMultiplayerMgr::OnMessage: Invalid msg." );
		return false;
	}

	switch(messageID)
	{
		case MID_HANDSHAKE:
			return HandleMsgHandshake( *pMsg );
			break;
		case MID_PLAYER_MULTIPLAYER_INIT:
			return HandleMsgPlayerMultiplayerInit( *pMsg );
			break;
		case MID_MULTIPLAYER_DATA:
			return HandleMsgMultiplayerData( *pMsg );
			break;
		case MID_MULTIPLAYER_SERVERDIR:
			if (GetServerDir())
			{
				char aAddrBuffer[16];
				sprintf(aAddrBuffer, "%d.%d.%d.%d", 
					(uint32)m_aCurMessageSourceAddr[0], 
					(uint32)m_aCurMessageSourceAddr[1], 
					(uint32)m_aCurMessageSourceAddr[2], 
					(uint32)m_aCurMessageSourceAddr[3]);
				return GetServerDir()->HandleNetMessage(*CLTMsgRef_Read(pMsg->SubMsg(pMsg->Tell())), aAddrBuffer, m_nCurMessageSourcePort);
			}
			else
				return false;
			break;
		default:
			break;
		
	}

	return false;
}


bool CClientMultiplayerMgr::HandleMsgHandshake( ILTMessage_Read & msg )
{
	int nHandshakeSub = (int)msg.Readuint8();
	switch (nHandshakeSub)
	{
		case MID_HANDSHAKE_HELLO :
		{
			int nHandshakeVer = (int)msg.Readuint16();
			if (nHandshakeVer != GAME_HANDSHAKE_VER)
			{
				// Disconnect
				m_bForceDisconnect = true;
				SetDisconnectCode(eDisconnect_NotSameGUID,NULL);

				return true;
			}

			// Send back a hello response
			CAutoMessage cResponse;
			cResponse.Writeuint8(MID_HANDSHAKE);
			cResponse.Writeuint8(MID_HANDSHAKE_HELLO);
		    cResponse.Writeuint16(GAME_HANDSHAKE_VER);
			// Send them our secret key
#ifdef SOURCE_RELEASE
			cResponse.Writeuint32( GAME_HANDSHAKE_PASSWORD );
#else // SOURCE_RELEASE
			cResponse.Writeuint32((uint32)this);
#endif // SOURCE_RELEASE
			g_pLTClient->SendToServer(cResponse.Read(), MESSAGE_GUARANTEED);
		}
		break;
		case MID_HANDSHAKE_PASSWORD:
		{
			// Read in their key
			m_nServerKey = msg.Readuint32();
			bool   bNeedPassword = msg.Readbool();

#ifdef SOURCE_RELEASE
			uint32 nPassword = GAME_HANDSHAKE_PASSWORD;
			uint32 nXORMask = GAME_HANDSHAKE_MASK;
#else // SOURCE_RELEASE
			// Get the player name
			char sName[255];
			// (Make sure to clear out the first 4 characters, since that's all we care about)
			sName[0] = sName[1] = sName[2] = sName[3] = 0;
			CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();

			SAFE_STRCPY(sName,pProfile->m_sPlayerGuid.c_str( ));

			// Decrypt it
			m_nServerKey ^= MessUp32BitValue((uint32)this, (uint32)(sName[0]));

			// Encrypt "the string" with it
			uint32 nPassword = *((uint32*)sName);
			uint32 nXORMask = MessUp32BitValue(m_nServerKey, (uint32)(sName[0]));
#endif // SOURCE_RELEASE

			nPassword ^= nXORMask;

			// Game password.
			uint32 nHashedPassword = 0;
			if (bNeedPassword)
			{
				nHashedPassword = m_nClientPass;
			}

			// Get the weapons file CRC
			uint32 nWeaponCRC = g_pWeaponMgr->GetFileCRC();
			// Mask that up too
			nWeaponCRC ^= nXORMask;

			// CRC the modelbutes.txt
			uint32 nModelButesMaskedCRC = m_nModelButesCRC ^ nXORMask;

			// CRC the surface.txt
			uint32 nSurfaceMaskedCRC = m_nSurfaceCRC ^ nXORMask;

			// CRC the damagefx.txt
			uint32 nDamageFxMaskedCRC = m_nDamageFxCRC ^ nXORMask;

			// Send it back their direction
			CAutoMessage cResponse;
			cResponse.Writeuint8(MID_HANDSHAKE);
			cResponse.Writeuint8(MID_HANDSHAKE_LETMEIN);
			cResponse.Writeuint32(nPassword);
			cResponse.Writeuint32(nWeaponCRC);
			cResponse.Writeuint32((m_nClientCRC^nXORMask));
			cResponse.Writeuint32(nModelButesMaskedCRC);
			cResponse.Writeuint32(nSurfaceMaskedCRC);
			cResponse.Writeuint32(nDamageFxMaskedCRC);
			cResponse.Writeuint32(nHashedPassword);
			g_pLTClient->SendToServer(cResponse.Read(), MESSAGE_GUARANTEED);

		}
		break;
		case MID_HANDSHAKE_DONE:
		{

			// This just means the server validated us...
		}
		break;
		case MID_HANDSHAKE_WRONGPASS:
		{
			// Oops... wrong password, disconnect
			m_bForceDisconnect = true;
			SetDisconnectCode(eDisconnect_WrongPassword, NULL);
		}
		break;
		default :
		{
			// Disconnect
			m_bForceDisconnect = true;
			SetDisconnectCode(eDisconnect_NotSameGUID, NULL);
		}
		break;
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::HandleMsgPlayerSingleplayerInit()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

bool CClientMultiplayerMgr::HandleMsgPlayerSingleplayerInit (ILTMessage_Read& msg)
{
	return InitSinglePlayer();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::HandleMsgPlayerMultiplayerInit()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

bool CClientMultiplayerMgr::HandleMsgPlayerMultiplayerInit(ILTMessage_Read& msg)
{
	return InitMultiPlayer();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::HandleMsgMultiplayerData()
//
//	PURPOSE:	Read multiplayer data sent from server.
//
// ----------------------------------------------------------------------- //

bool CClientMultiplayerMgr::HandleMsgMultiplayerData( ILTMessage_Read& msg )
{
    GameType eGameType = ( GameType )msg.Readuint8();
	g_pGameClientShell->SetGameType( eGameType );

	// Check if some joker set us to single player.
	if( eGameType == eGameTypeSingle )
	{
		ASSERT( !"CClientMultiplayerMgr::HandleMsgMultiplayerData: Invalid game type." );
		return false;
	}

	msg.ReadString( m_sServerAddress.GetBuffer( 256 ), 256 );
	m_sServerAddress.ReleaseBuffer( );
	uint32 tmp = msg.Readuint32();
	m_nServerPort = (int)tmp;

	return true;
}

bool CClientMultiplayerMgr::SetMultiplayerData( GameType eGameType, const char* lpszHostIP, int nHostPort )
{
	if( !lpszHostIP ) return false;
	
	g_pGameClientShell->SetGameType( eGameType );
	m_sServerAddress.Format( "%s", lpszHostIP );
	m_nServerPort = (int)nHostPort;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::OnEvent()
//
//	PURPOSE:	Called for asynchronous errors that cause the server
//				to shut down
//
// ----------------------------------------------------------------------- //

void CClientMultiplayerMgr::OnEvent(uint32 dwEventID, uint32 dwParam)
{
	if( !IsMultiplayerGame( ))
		return;

	switch(dwEventID)
	{
		// Client disconnected from server.  dwParam will
		// be a error flag found in de_codes.h.
		case LTEVENT_DISCONNECT :
		{
			m_sServerAddress.Empty( );
			m_nServerPort = -1;
			m_sServerName.Empty( );
			m_nServerKey = 0;
		} 
		break;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::Update()
//
//	PURPOSE:	Frame update.
//
// ----------------------------------------------------------------------- //

void CClientMultiplayerMgr::Update( )
{
	// This will happen when something wanted to disconnect, but wasn't
	// in a valid location to do so.  (e.g. when processing packets..)
	if (m_bForceDisconnect)
	{
		g_pLTClient->Disconnect();
		m_bForceDisconnect = false;
		return;
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::SetService
//
//	PURPOSE:	Selects the connection service for hosting/joining internet games.
//
// --------------------------------------------------------------------------- //

bool CClientMultiplayerMgr::SetService( )
{
	NetService *pCur, *pListHead;
	HNETSERVICE hNetService;

	pCur      = NULL;
	pListHead = NULL;
	hNetService = NULL;

    if( g_pLTClient->GetServiceList( pListHead ) != LT_OK || !pListHead )
        return false;

	// Find the service specified.
	pCur = pListHead;
	while( pCur )
	{
		if( pCur->m_dwFlags & NETSERVICE_TCPIP )
		{
			hNetService = pCur->m_handle;
			break;
		}

		pCur = pCur->m_pNext;
	}

	// Free the service list.
    g_pLTClient->FreeServiceList( pListHead );

	// Check if tcp not found.
	if( !hNetService )
        return false;

	// Select it.
    if( g_pLTClient->SelectService( hNetService ) != LT_OK )
        return false;

    return true;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::StartClient
//
//	PURPOSE:	Start a client of a remote server.
//
// --------------------------------------------------------------------------- //

bool CClientMultiplayerMgr::StartClient( )
{
    if( !InitNetworking() ) return false;

	uint8 nModelId = g_GameDoc.GetUserModelId(g_GameDoc.GetTeam());

	m_NetClientData.m_ePlayerModelId = ModelId(nModelId);
	m_NetClientData.m_eModelType	 = eModelNormal;
	
#ifdef BIG_FAT_MODE_TEST
	m_NetClientData.m_eModelType	 = m_ePlayerModelType;
#endif 

	ServerGameOptions* pServerGameOptions = &m_ServerGameOptions;
	m_StartGameRequest.m_pGameInfo = &pServerGameOptions;
	m_StartGameRequest.m_GameInfoLen = sizeof( pServerGameOptions );
	m_StartGameRequest.m_pClientData = &m_NetClientData;
	m_StartGameRequest.m_ClientDataLen = sizeof( m_NetClientData );	
#ifdef _DEBUG
	m_StartGameRequest.m_HostInfo.m_Port = m_nServerPort;
#endif
	//strcpy( m_StartGameRequest.m_WorldName, g_GameDoc.GetWorldName() );
	
	int nRetries = 0;
	while (nRetries >= 0)
	{
		// If successful, then we're done.
		m_nLastConnectionResult = g_pLTClient->StartGame( const_cast< StartGameRequest * >( &m_StartGameRequest ));
		if( m_nLastConnectionResult == LT_OK )
		{
#ifdef PRE_LOAD_FILE_LIST
			PreLoadFileList();
#endif
			g_pLTClient->StartDummyHost( g_GameDoc.GetWorldName() );
			return true;
		}

		// If we didn't timeout, then there's no reason to try again.
		if( m_nLastConnectionResult != LT_TIMEOUT )
			break;

		// Wait a sec and try again.
		Sleep(1000);
		nRetries--;
	}

	
	return false;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::StartServerHost
//
//	PURPOSE:	Start a hosted game.
//
// --------------------------------------------------------------------------- //

bool CClientMultiplayerMgr::StartServerAsHost( )
{
	// If they want a dedicated server, then launch the serverapp.
	/*
	if( m_ServerGameOptions.m_bDedicated )
	{
		if( !g_pGameClientShell->LauncherServerApp( m_ServerGameOptions.m_sProfileName.c_str( )))
			return false;

		return true;
	}
	*/

	// Check if we're already connected to a server.
  	if( g_pLTClient->IsConnected( ))
	{
  		// Check if we are already hosting mp.
  		if( IsMultiplayerGame( ) && m_StartGameRequest.m_Type == STARTGAME_HOST )
  		{
  			// Don't need to restart a server.
  			return true;
		}
	}

	if( !InitNetworking() ) return false;
	
	uint8 nModelId = g_GameDoc.GetUserModelId(g_GameDoc.GetTeam());
	
	m_NetClientData.m_ePlayerModelId = ModelId(nModelId);
	m_NetClientData.m_eModelType	 = eModelNormal;

#ifdef BIG_FAT_MODE_TEST
	m_NetClientData.m_eModelType	 = m_ePlayerModelType;
#endif 

#ifdef GAME_SHOW
	m_NetClientData.m_bSpectator	 = true;
#endif
	
	// Hook up the netgame and clientinfo.
	ServerGameOptions* pServerGameOptions = &m_ServerGameOptions;
	m_StartGameRequest.m_pGameInfo = &pServerGameOptions;
	m_StartGameRequest.m_GameInfoLen = sizeof( pServerGameOptions );
	m_StartGameRequest.m_pClientData = &m_NetClientData;
	m_StartGameRequest.m_ClientDataLen = sizeof( m_NetClientData );
	m_StartGameRequest.m_WorldName[0] = LTNULL;

	// Start the server.
	m_nLastConnectionResult = g_pLTClient->StartGame( const_cast< StartGameRequest * >( &m_StartGameRequest ));
#ifdef PRE_LOAD_FILE_LIST
	if( m_nLastConnectionResult == LT_OK ) PreLoadFileList();
#endif

	return ( m_nLastConnectionResult == LT_OK );
}


bool CClientMultiplayerMgr::MandateHost()
{
	//if( !InitNetworking(false) ) return false;
	
	uint8 nModelId = g_GameDoc.GetUserModelId(g_GameDoc.GetTeam());
	
	m_NetClientData.m_ePlayerModelId = ModelId(nModelId);
	m_NetClientData.m_eModelType	 = eModelNormal;

#ifdef BIG_FAT_MODE_TEST
	m_NetClientData.m_eModelType	 = m_ePlayerModelType;
#endif 
	
	// Hook up the netgame and clientinfo.
	ServerGameOptions* pServerGameOptions = &m_ServerGameOptions;
	m_StartGameRequest.m_pGameInfo = &pServerGameOptions;
	m_StartGameRequest.m_GameInfoLen = sizeof( pServerGameOptions );
	m_StartGameRequest.m_pClientData = &m_NetClientData;
	m_StartGameRequest.m_ClientDataLen = sizeof( m_NetClientData );

	// Start the server.
	m_nLastConnectionResult = g_pLTClient->StartGame( const_cast< StartGameRequest * >( &m_StartGameRequest ));
#ifdef PRE_LOAD_FILE_LIST
	if( m_nLastConnectionResult == LT_OK ) PreLoadFileList();
#endif

	return ( m_nLastConnectionResult == LT_OK );
}

bool CClientMultiplayerMgr::RejoinHost()
{
	//if( !InitNetworking(false) ) return false;

	uint8 nModelId = g_GameDoc.GetUserModelId(g_GameDoc.GetTeam());

	m_NetClientData.m_ePlayerModelId = ModelId(nModelId);
	m_NetClientData.m_eModelType	 = eModelNormal;

#ifdef BIG_FAT_MODE_TEST
	m_NetClientData.m_eModelType	 = m_ePlayerModelType;
#endif 
	
	ServerGameOptions* pServerGameOptions = &m_ServerGameOptions;
	m_StartGameRequest.m_pGameInfo = &pServerGameOptions;
	m_StartGameRequest.m_GameInfoLen = sizeof( pServerGameOptions );
	m_StartGameRequest.m_pClientData = &m_NetClientData;
	m_StartGameRequest.m_ClientDataLen = sizeof( m_NetClientData );	

	//strcpy( m_StartGameRequest.m_WorldName, g_GameDoc.GetWorldName() );

	int nRetries = 0;
	while (nRetries >= 0)
	{
		// If successful, then we're done.
		m_nLastConnectionResult = g_pLTClient->StartGame( const_cast< StartGameRequest * >( &m_StartGameRequest ));
		if( m_nLastConnectionResult == LT_OK )
		{
#ifdef PRE_LOAD_FILE_LIST
			PreLoadFileList();
#endif
			return true;
		}

		// If we didn't timeout, then there's no reason to try again.
		if( m_nLastConnectionResult != LT_TIMEOUT )
			break;

		// Wait a sec and try again.
		Sleep(1000);
		nRetries--;
	}
	
	return false;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::StartServerSinglePlayer
//
//	PURPOSE:	Starts the single player server.
//
// --------------------------------------------------------------------------- //

bool CClientMultiplayerMgr::StartServerAsSinglePlayer( )
{
	// Check if we're already connected to a server.
  	if( g_pLTClient->IsConnected( ))
	{
  		// Check if we are already running sp server.
  		if( !IsMultiplayerGame( ) && m_StartGameRequest.m_Type == STARTGAME_NORMAL )
  		{
  			// Don't need to restart a server.
  			return true;
		}
	}

	// Make sure the profile is set.
	m_ServerGameOptions.m_eGameType = g_pGameClientShell->GetGameType( );
	ServerGameOptions* pServerGameOptions = &m_ServerGameOptions;
	m_StartGameRequest.m_pGameInfo = &pServerGameOptions;
	m_StartGameRequest.m_GameInfoLen = sizeof( pServerGameOptions );


	// Start with clean slate
	m_StartGameRequest.m_Type = STARTGAME_NORMAL;

    m_nLastConnectionResult = g_pLTClient->StartGame(&m_StartGameRequest);
	return ( m_nLastConnectionResult == LT_OK );
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CClientMultiplayerMgr::StartClientServer
//
//	PURPOSE:	Starts a client/server based on previously set startgamerequest.
//
// --------------------------------------------------------------------------- //

bool CClientMultiplayerMgr::StartClientServer( )
{
	switch( m_StartGameRequest.m_Type )
	{
		case STARTGAME_NORMAL:
			return StartServerAsSinglePlayer( );
			break;
		case STARTGAME_HOST:
			return StartServerAsHost( );
			break;
		case STARTGAME_CLIENTTCP:
			return StartClient( );
			break;	
		case MANDATE_HOST:
			return MandateHost();
			break;
		case REJOIN_HOST:
			return RejoinHost();
			break;
		default:
			ASSERT( !"CClientMultiplayerMgr::StartClientServer: Invalid gamerequest type." );
			return false;
			break;
	}
}


// --------------------------------------------------------------------------- //
//
//  ROUTINE:    CClientMultiplayerMgr::UpdateNetClientData
//
//  PURPOSE:    Updates the NetClientData to reflect current settings.
//
// --------------------------------------------------------------------------- //

bool CClientMultiplayerMgr::UpdateNetClientData( )
{
	m_NetClientData.m_nServerIndex	= g_GameDoc.GetUniqueIndex();
	return true;	
}

void CClientMultiplayerMgr::SetupDefaultSetting( GameType eGameType, int nMaxPlayer, bool bFriendlyFire, uint8 nTimeLimit )
{
	m_ServerGameOptions.Clear();

	m_ServerGameOptions.m_eGameType					= eGameType;
	m_ServerGameOptions.m_bUsePassword				= false;
	m_ServerGameOptions.m_sPassword					= "password";
	m_ServerGameOptions.m_sScmdPassword				= "password";
	m_ServerGameOptions.m_bAllowScmdCommands		= false;
	m_ServerGameOptions.m_nPort						= g_pLTClient->GetHostPort();
	m_ServerGameOptions.m_nBandwidthServer			= eBandwidth_LAN;
	m_ServerGameOptions.m_nBandwidthServerCustom	= g_BandwidthServer[eBandwidth_LAN];
	m_ServerGameOptions.m_bLANOnly					= false;
	m_ServerGameOptions.m_bDedicated				= false;
	m_ServerGameOptions.m_bPreCacheAssets			= true;
	m_ServerGameOptions.m_bPerformanceTest			= false;

	
	switch( m_ServerGameOptions.m_eGameType )
	{
	case eGameTypeTeamDeathmatch:
		//Game
		m_ServerGameOptions.GetTeamDeathmatch().m_sCampaignName		= "";
		m_ServerGameOptions.GetTeamDeathmatch().m_sSessionName		= g_GameDoc.GetTitle();
		m_ServerGameOptions.GetTeamDeathmatch().m_nMaxPlayers		= nMaxPlayer;
		m_ServerGameOptions.GetTeamDeathmatch().m_bFriendlyFire		= bFriendlyFire;
				
		//Limit
		m_ServerGameOptions.GetTeamDeathmatch().m_nTimeLimit		= nTimeLimit * 60;
		m_ServerGameOptions.GetTeamDeathmatch().m_nScoreLimit		= 0;
		m_ServerGameOptions.GetTeamDeathmatch().m_nRounds			= 0;
		
		//Score
		m_ServerGameOptions.GetTeamDeathmatch().m_nFragScore		= 1;
		m_ServerGameOptions.GetTeamDeathmatch().m_nTagScore			= 1;
		m_ServerGameOptions.GetTeamDeathmatch().m_nRevivingScore	= 0;

		//Team
		m_ServerGameOptions.GetTeamDeathmatch().m_nNumTeams			= MAX_TEAMS;
		m_ServerGameOptions.GetTeamDeathmatch().m_sTeamName[0]		= "RED";
		m_ServerGameOptions.GetTeamDeathmatch().m_sTeamName[1]		= "BLUE";
		m_ServerGameOptions.GetTeamDeathmatch().m_nTeamModel[0]		= 0;
		m_ServerGameOptions.GetTeamDeathmatch().m_nTeamModel[1]		= 0;		
		break;

	case eGameTypeSurvival:
		//Game
		m_ServerGameOptions.GetSurvival().m_sCampaignName			= "";
		m_ServerGameOptions.GetSurvival().m_sSessionName			= g_GameDoc.GetTitle();
		m_ServerGameOptions.GetSurvival().m_nMaxPlayers				= nMaxPlayer;
		m_ServerGameOptions.GetSurvival().m_bFriendlyFire			= bFriendlyFire;
				
		//Limit
		m_ServerGameOptions.GetSurvival().m_nTimeLimit				= nTimeLimit * 60;
		m_ServerGameOptions.GetSurvival().m_nScoreLimit				= 0;
		m_ServerGameOptions.GetSurvival().m_nRounds					= 0;
		
		//Score
		m_ServerGameOptions.GetSurvival().m_nFragScore				= 1;
		m_ServerGameOptions.GetSurvival().m_nTagScore				= 1;
		m_ServerGameOptions.GetSurvival().m_nRevivingScore			= 0;

		//Team
		m_ServerGameOptions.GetSurvival().m_nNumTeams				= MAX_TEAMS;
		m_ServerGameOptions.GetSurvival().m_sTeamName[0]			= "RED";
		m_ServerGameOptions.GetSurvival().m_sTeamName[1]			= "BLUE";
		m_ServerGameOptions.GetSurvival().m_nTeamModel[0]			= 0;
		m_ServerGameOptions.GetSurvival().m_nTeamModel[1]			= 0;
		break;
	}
}

bool CClientMultiplayerMgr::PreLoadFileList()
{
	bool	bResult = true;
	int		nCount1, nCount2;
	int		i,j;
	const char*	pszName;
	
	//World
	if( false == g_pLTClient->PreLoadFileList( g_GameDoc.GetWorldName(), false ) )
	{
		ASSERT( !"PreLoadFileList World" );
		bResult = false;
	}
	/*
	Ambient
	if( false == g_pLTClient->PreLoadFileList( "SA_WORLDS\\SND", true ) )
	{
		ASSERT( !"PreLoadFileList World" );
		bResult = false;
	}
	*/
	
	//Character
	nCount1 = g_pModelButeMgr->GetNumModels();
	for( i=0 ; i<nCount1 ; ++i )
	{
		ModelId eModelId = (ModelId)i;

		pszName = g_pModelButeMgr->GetMultiModelFilename( eModelId );
		g_pLTClient->PreLoadFileList( pszName, false );

		nCount2 = g_pModelButeMgr->GetNumModelHeadSkins( eModelId );
		for( j=0 ; j<nCount2 ; ++j )
		{
			g_pModelButeMgr->GetModelHeadSkinFilename( eModelId, j, pszName );
			g_pLTClient->PreLoadFileList( pszName, false );
		}

		nCount2 = g_pModelButeMgr->GetNumModelBodySkins( eModelId );
		for( j=0 ; j<nCount2 ; ++j )
		{
			g_pModelButeMgr->GetModelBodySkinFilename( eModelId, j, pszName );
			g_pLTClient->PreLoadFileList( pszName, false );
		}
	}

	//Weapon
	char	szSkin[MAX_CS_FILENAME_LEN];
	nCount1 = g_pWeaponMgr->GetNumWeapons();
	for( i=0 ; i<nCount1 ; ++i )
	{
		WEAPON const* pWeapon = g_pWeaponMgr->GetWeapon(i);

		g_pLTClient->PreLoadFileList( pWeapon->szHHModel, false );

		pWeapon->blrHHSkins.CopyList( 0, szSkin, MAX_CS_FILENAME_LEN +1 );	
		g_pLTClient->PreLoadFileList( szSkin, false );
	}

	//Surface
	SurfaceList* pSurfaceList = g_pSurfaceMgr->GetSurfaceList();
	SURFACE** pSurface;
	if( pSurfaceList )
	{
		pSurface = pSurfaceList->GetItem(TLIT_FIRST);

		while( pSurface )
		{
			g_pLTClient->PreLoadFileList( (*pSurface)->szGrenadeImpactSnd, false );

			if( strlen( (*pSurface)->szProjectileImpactSnds[0] ) > 0 )
			{
				g_pLTClient->PreLoadFileList( (*pSurface)->szProjectileImpactSnds[0], false );
			}
			if( strlen( (*pSurface)->szProjectileImpactSnds[1] ) > 0 )
			{
				g_pLTClient->PreLoadFileList( (*pSurface)->szProjectileImpactSnds[1], false );
			}

			pSurface = pSurfaceList->GetItem(TLIT_NEXT);
		}
	}

	//FX
	ProjectileFXList* pProjectileFXList = g_pFXButeMgr->GetProjectileFXList();
	PROJECTILEFX** pProjectileFX;
	if( pProjectileFXList )
	{
		pProjectileFX = pProjectileFXList->GetItem(TLIT_FIRST);

		while( pProjectileFX )
		{
			g_pLTClient->PreLoadFileList( (*pProjectileFX)->szModel, false );
			g_pLTClient->PreLoadFileList( (*pProjectileFX)->szSkin, false );

			pProjectileFX = pProjectileFXList->GetItem(TLIT_NEXT);
		}
	}
		
	//Etc
	if( false == g_pLTClient->PreLoadFileList( "GLOBAL\\MODELS\\DEFAULT.LTB", false ) )
	{
		ASSERT( !"PreLoadFileList Etc" );
		bResult = false;
	}

	return bResult;
}

void CClientMultiplayerMgr::CalcCRC()
{
	m_nModelButesCRC = CRC32::CalcRezFileCRC( g_pModelButeMgr->GetAttributeFile( ));
	m_nSurfaceCRC = CRC32::CalcRezFileCRC( g_pSurfaceMgr->GetAttributeFile( ));
	m_nDamageFxCRC = CRC32::CalcRezFileCRC( DMGFXMGR_DEFAULT_FILE );

	char aClientShellName[MAX_PATH + 1];
	aClientShellName[0] = 0; 
	HMODULE hClientShell;
	g_pLTClient->GetEngineHook("cshell_hinstance", (void**)&hClientShell);
	DWORD nResult = GetModuleFileName(hClientShell, aClientShellName, sizeof(aClientShellName));
	m_nClientCRC = CRC32::CalcFileCRC(aClientShellName);
}