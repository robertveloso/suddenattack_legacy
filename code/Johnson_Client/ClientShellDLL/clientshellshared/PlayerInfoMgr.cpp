#include "stdafx.h"
#include "ClientInfoMgr.h"
#include "iltclient.h"
#include "GameClientShell.h"
#include "CharacterFX.h"
#include "GameText.h"
#include "GameMessage.h"

#include <stdio.h>

#include "PlayerCamera.h"

extern CGameClientShell* g_pGameClientShell;

struct MoreThenKill
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_PLAYER_INFOptr)p)->nKill > ((T_PLAYER_INFOptr)q)->nKill;
	}
};


CPlayerInfoMgr::CPlayerInfoMgr()
{
}

CPlayerInfoMgr::~CPlayerInfoMgr()
{
	if( false == m_PlayerInfoList.empty() )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			SAFE_DELETE(ptr);
		}
		m_PlayerInfoList.clear();
	}
}

void CPlayerInfoMgr::Init()
{
}

void CPlayerInfoMgr::AddPlayer( int32 nServerIndex, const char* lpszNick, uint8 nGrade, uint8 nTeamID, LTBOOL bCaptain )
{
	g_pLTClient->CPrint( "CPlayerInfoMgr::AddPlayer %d\t%s", nServerIndex, lpszNick);

	T_PLAYER_INFOptr pDup = GetPlayerByServerIndex( nServerIndex );

	if( pDup )
	{
		strcpy( pDup->szNickName, lpszNick );
		pDup->nGrade	= nGrade;
		pDup->nTeamID	= nTeamID;
		pDup->bCaptain	= bCaptain;

		pDup->bInGame		= LTFALSE;
		pDup->bRecvInfo		= LTFALSE;
		pDup->nPlayerID		= -1;
		pDup->nKill			= -1;
		pDup->nDeath		= -1;	
		pDup->ePlayerState	= PS_UNKNOWN;
		pDup->hObject		= LTNULL;
		pDup->nPing			= 0;
		pDup->bNewPlayer	= LTTRUE;
	
		return;
	}

	T_PLAYER_INFOptr pNew = new T_PLAYER_INFO;
	if( !pNew ) return;

	strcpy( pNew->szNickName, lpszNick );
	pNew->nServerIndex	= nServerIndex;	
	pNew->nGrade		= nGrade;
	pNew->nTeamID		= nTeamID;
	pNew->bCaptain		= bCaptain;

	pNew->bInGame		= LTFALSE;
	pNew->bRecvInfo		= LTFALSE;
	pNew->nPlayerID		= -1;
	pNew->nKill			= -1;
	pNew->nDeath		= -1;	
	pNew->ePlayerState	= PS_UNKNOWN;
	pNew->hObject		= LTNULL;
	pNew->nPing			= 0;	
	pNew->bNewPlayer	= LTTRUE;
	
	m_PlayerInfoList.push_back(pNew);
}

void CPlayerInfoMgr::RemovePlayer( int32 nServerIndex )
{
	if( g_GameDoc.IsHost() )
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_PLAYER_KICKBAN);
		cMsg.Writeint32(nServerIndex);
		g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
	}

	T_PLAYER_INFOptr ptr = LTNULL;
	PLAYER_INFO_LIST_ITR itr = m_PlayerInfoList.begin();

	while( itr!=m_PlayerInfoList.end() && (*itr)->nServerIndex!=nServerIndex )
		itr++;

	if( itr != m_PlayerInfoList.end() )
	{
		ptr = (T_PLAYER_INFOptr)(*itr);
		m_PlayerInfoList.erase(itr);

		SAFE_DELETE(ptr);
	}	
}

void CPlayerInfoMgr::RemoveAll()
{
    if( false == m_PlayerInfoList.empty() )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			SAFE_DELETE(ptr);
		}
		m_PlayerInfoList.clear();
		
		g_pScores->RemoveAll();
		g_pScores->SetUpdate(true);
	}
}

void CPlayerInfoMgr::SetPlayerInfoFromServer( int32 nServerIndex, uint16 nKill, uint16 nDeath,
											  uint8 nAModelID, uint8 nBModelID )

{
	if( m_PlayerInfoList.empty() ) return;

	T_PLAYER_INFOptr ptr = GetPlayerByServerIndex( nServerIndex );
	if( ptr )
	{
		ptr->nKill			= nKill;
		ptr->nDeath			= nDeath;
		ptr->nModelID[0]	= nAModelID;
		ptr->nModelID[1]	= nBModelID;
		ptr->bRecvInfo		= LTTRUE;

		if( LTFALSE == ptr->bInGame && ptr->nPlayerID >= 0 )
		{
			ptr->bInGame	= LTTRUE;
			RecvComplete( ptr );
		}
	}
}

void CPlayerInfoMgr::SetPlayerInfoFromP2P( int32 nServerIndex, int32 nPlayerID, HOBJECT hObj, PlayerState eState )
{
	if( m_PlayerInfoList.empty() ) return;

	T_PLAYER_INFOptr ptr = GetPlayerByServerIndex( nServerIndex );
	if( ptr )
	{
		ptr->nPlayerID		= nPlayerID;
		ptr->hObject		= hObj;
		ptr->ePlayerState	= eState;

		if( LTFALSE == ptr->bInGame && LTTRUE == ptr->bRecvInfo )
		{
			ptr->bInGame	= LTTRUE;
			RecvComplete( ptr );
		}
	}
}

void CPlayerInfoMgr::SetNewPlayer( int nServerIndex, LTBOOL bNew )
{
	T_PLAYER_INFOptr ptr = GetPlayerByServerIndex(nServerIndex);
	if( ptr ) ptr->bNewPlayer = bNew;
}

void CPlayerInfoMgr::SetPlayerHandle( uint8 nPlayerID, HOBJECT hObj )
{
	T_PLAYER_INFOptr ptr = GetPlayerByID( nPlayerID );
	if( ptr ) ptr->hObject = hObj;
}

void CPlayerInfoMgr::LeaveGame( int32 nServerIndex )
{
	if( m_PlayerInfoList.empty() ||
		nServerIndex == g_GameDoc.GetUniqueIndex() ) return;

	if( g_GameDoc.IsHost() )
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_PLAYER_KICKBAN);
		cMsg.Writeint32(nServerIndex);
		g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
	}

	T_PLAYER_INFOptr ptr = GetPlayerByServerIndex( nServerIndex );
	if( ptr )
	{
		if( ptr->nServerIndex != g_GameDoc.GetUniqueIndex() )
		{
			char szLeaveGame[128];
			//"[%s]´Ô ÅðÀå"
			sprintf( szLeaveGame, NF_GetMsg(SA_TEXT_PLAYER_EXIT_NICK), ptr->szNickName );
			g_pChatMsgs->AddMessage( szLeaveGame );			
		}

		g_pScores->RemovePlayer( ptr->nPlayerID );
		g_pScores->SetUpdate(true);
		
		g_pRadar->RemoveObject( ptr->hObject );

		if( ptr->nTeamID == g_GameDoc.GetTeam() )
		{
			g_pNickList->RemoveNick( ptr->hObject );
			CCharacterFX *pCharacter = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(ptr->hObject);
			if( pCharacter ) pCharacter->SetUpdateNick(false);
		}

		ptr->nPlayerID	= -1;
		ptr->nKill		= -1;
		ptr->nDeath		= -1;
		ptr->hObject	= LTNULL;
		ptr->bInGame	= LTFALSE;
		ptr->bRecvInfo	= LTFALSE;
		ptr->bNewPlayer = LTTRUE;
	}
}

void CPlayerInfoMgr::LeaveGameAll()
{
	if( false == m_PlayerInfoList.empty() )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			
			ptr->nPlayerID	= -1;
			ptr->hObject	= LTNULL;
			ptr->bInGame	= LTFALSE;
			ptr->bRecvInfo	= LTFALSE;
			ptr->bNewPlayer = LTTRUE;
		}
		
		g_pScores->RemoveAll();
	}
}

void CPlayerInfoMgr::ResetMandate()
{
	if( false == m_PlayerInfoList.empty() )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			
			ptr->nPlayerID	= -1;
			ptr->hObject	= LTNULL;
			ptr->bInGame	= LTFALSE;
		}
		
		g_pScores->RemoveAll();
	}
}

void CPlayerInfoMgr::SetPlayerPing( int nServerIndex, uint8 nPing )
{
	if( m_PlayerInfoList.empty() ) return;

	T_PLAYER_INFOptr ptr = GetPlayerByServerIndex( nServerIndex );
	if( ptr )
	{
		ptr->nPing = nPing;
		g_pScores->SetPing( ptr->nPlayerID, nPing );
	}
}

void CPlayerInfoMgr::SetCaptain( int nServerIndex )
{
	if( m_PlayerInfoList.empty() ) return;

	T_PLAYER_INFOptr ptr;
	PLAYER_INFO_LIST_ITR itr;
	for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
	{
		ptr = (T_PLAYER_INFOptr)(*itr);
		
		if( ptr->nServerIndex == nServerIndex )
		{
			ptr->bCaptain = LTTRUE;
		}
		else
		{
			ptr->bCaptain = LTFALSE;
		}
	}

	g_pScores->SetUpdate(true);
}

void CPlayerInfoMgr::SetPlayerState( HOBJECT hObj, PlayerState eState  )
{
	if( m_PlayerInfoList.empty() ) return;

	T_PLAYER_INFOptr ptr = GetPlayerByHandle( hObj );
	if( ptr ) ptr->ePlayerState = eState;
}

void CPlayerInfoMgr::SetPlayerTeam( int32 nServerIndex, uint8 nTeamID )
{
	if( m_PlayerInfoList.empty() ) return;

	T_PLAYER_INFOptr ptr = GetPlayerByServerIndex( nServerIndex );
	if( ptr ) ptr->nTeamID = nTeamID;
}

void CPlayerInfoMgr::SetPlayerTeamOnGame( int32 nServerIndex, uint8 nTeamID )
{
	if( m_PlayerInfoList.empty() ) return;

	CCharacterFX* pChar;
	T_PLAYER_INFOptr ptr = GetPlayerByServerIndex( nServerIndex );
	CSFXMgr* psfxMgr = g_pGameClientShell->GetSFXMgr();

	if( !ptr || !psfxMgr ) return;

	ptr->nTeamID = nTeamID;	

	if( g_GameDoc.GetUniqueIndex() == nServerIndex )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			if( ptr->nTeamID == nTeamID )
			{
				g_pNickList->AddNick( ptr->hObject, ptr->szNickName, ptr->nTeamID );
				pChar = psfxMgr->GetCharacterFX( ptr->hObject );
				if( pChar ) pChar->SetUpdateNick(true);
			}
			else
			{
				g_pNickList->RemoveNick( ptr->hObject );
				pChar = psfxMgr->GetCharacterFX( ptr->hObject );
				if( pChar ) pChar->SetUpdateNick(false);
			}
		}

		g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_TEAM_CHANGE), kMsgNotice );
	}
	else
	{
		g_pNickList->RemoveNick( ptr->hObject );
		pChar = psfxMgr->GetCharacterFX( ptr->hObject );
		if( pChar ) pChar->SetUpdateNick(false);
	}
}

T_PLAYER_INFOptr CPlayerInfoMgr::GetLocalPlayer()
{
	return GetPlayerByServerIndex( g_GameDoc.GetUniqueIndex() );
}

T_PLAYER_INFOptr CPlayerInfoMgr::GetFirstPlayer()
{
	if( m_PlayerInfoList.empty() ) return LTNULL;
	return (T_PLAYER_INFOptr)m_PlayerInfoList[0];
}

T_PLAYER_INFOptr CPlayerInfoMgr::GetPlayerByServerIndex( int32 nServerIndex )
{
	if( false == m_PlayerInfoList.empty() )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			if( ptr->nServerIndex == nServerIndex ) return ptr;
		}
	}
	
    return LTNULL;
}

T_PLAYER_INFOptr CPlayerInfoMgr::GetPlayerByID( uint8 nID )
{
	if( false == m_PlayerInfoList.empty() )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			if( ptr->nPlayerID == nID ) return ptr;
		}
	}
	
    return LTNULL;
}

T_PLAYER_INFOptr CPlayerInfoMgr::GetPlayerByNick( const char* lpszNick )
{
	if( false == m_PlayerInfoList.empty() )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			if( strcmp( ptr->szNickName, lpszNick ) == 0 ) return ptr;
		}
	}
	
    return LTNULL;
}

T_PLAYER_INFOptr CPlayerInfoMgr::GetPlayerByHandle( HOBJECT hObj )
{
	if( false == m_PlayerInfoList.empty() )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			if( ptr->hObject == hObj ) return ptr;
		}
	}
	
    return LTNULL;
}

HOBJECT CPlayerInfoMgr::GetHandleByID( uint8 nID )
{
	if( false == m_PlayerInfoList.empty() )
	{
		T_PLAYER_INFOptr ptr;
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			ptr = (T_PLAYER_INFOptr)(*itr);
			if( ptr->nPlayerID == nID ) return ptr->hObject;
		}
	}
	
    return LTNULL;
}

T_PLAYER_INFOptr CPlayerInfoMgr::GetPlayerByIndex( uint32 nIndex )
{
	if( m_PlayerInfoList.empty() ) return LTNULL;
	return (T_PLAYER_INFOptr)m_PlayerInfoList[nIndex];
}

uint32 CPlayerInfoMgr::GetNumPlayerInGame()
{
	if( m_PlayerInfoList.empty() ) return 0;

	uint32 nCount = 0;
	
	T_PLAYER_INFOptr ptr;
	PLAYER_INFO_LIST_ITR itr;
	for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
	{
		ptr = (T_PLAYER_INFOptr)(*itr);
		if( ptr->bInGame ) nCount++;
	}

	return nCount;
}

uint32 CPlayerInfoMgr::GetNumPlayers()
{
	if( m_PlayerInfoList.empty() ) return 0;
	return (uint32)m_PlayerInfoList.size();
}

char const* CPlayerInfoMgr::GetPlayerNick( int32 nServerIndex )
{
	if( m_PlayerInfoList.empty() ) return "";

	T_PLAYER_INFOptr ptr = GetPlayerByServerIndex( nServerIndex );
	if( ptr ) return ptr->szNickName;

	return "";
}

char const* CPlayerInfoMgr::GetPlayerNick( HOBJECT hObj )
{
	if( m_PlayerInfoList.empty() ) return "";
 
	T_PLAYER_INFOptr ptr = GetPlayerByHandle( hObj );
	if( ptr ) return ptr->szNickName;

	return "";
}

int32 CPlayerInfoMgr::GetPlayerServerIndex( uint8 nID )
{
	if( m_PlayerInfoList.empty() ) return -1;

	T_PLAYER_INFOptr ptr = GetPlayerByID( nID );
	if( ptr ) return ptr->nServerIndex;

	return -1;
}

void CPlayerInfoMgr::UpdatePlayerSort()
{
	std::sort( m_PlayerInfoList.begin(), m_PlayerInfoList.end(), MoreThenKill() );		
}

uint8 CPlayerInfoMgr::GetNumPlayersOnTeam( uint8 nTeam )
{
	uint8 nCount = 0;

	T_PLAYER_INFOptr ptr;
	PLAYER_INFO_LIST_ITR itr;
	for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
	{
		ptr = (T_PLAYER_INFOptr)(*itr);
		if( ptr->bInGame && ptr->nTeamID == nTeam ) nCount++;
	}

	return nCount;
}

void CPlayerInfoMgr::RecvComplete( T_PLAYER_INFOptr pPlayerInfo )
{
	if( pPlayerInfo->nServerIndex != g_GameDoc.GetUniqueIndex() &&
		pPlayerInfo->bNewPlayer )
	{
		char szEnterGame[128];
		//"[%s] ´Ô ÀÔÀå"
		sprintf( szEnterGame, NF_GetMsg(SA_TEXT_PLAYER_ENTER_NICK), pPlayerInfo->szNickName );
		g_pChatMsgs->AddMessage( szEnterGame );	
		pPlayerInfo->bNewPlayer = LTFALSE;
	}


#ifdef GAME_SHOW
	int nRedTeam	= 0;
	int nBlueTeam	= 0;
	
	if( TEAM_A == g_GameDoc.GetTeam() )
	{
		--nRedTeam;
	}
	else
	{
		--nBlueTeam;
	}

	if( g_GameDoc.IsCaptain() && g_GameDoc.IsHost() && false == m_PlayerInfoList.empty() )
	{
		PLAYER_INFO_LIST_ITR itr;
		for( itr=m_PlayerInfoList.begin() ; itr != m_PlayerInfoList.end() ; ++itr )
		{
			if( TEAM_A == (*itr)->nTeamID )
			{
				++nRedTeam;
			}
			else
			{
				++nBlueTeam;
			}
		}

		if( nRedTeam > 0 && nBlueTeam > 0 )
		{
			g_Network.CS_Kill( g_GameDoc.GetUniqueIndex(),
							   g_GameDoc.GetUniqueIndex(), 
							   false,
							   0 );
		}
	}	
	
	if( pPlayerInfo->bCaptain ) return;

#endif

	g_pScores->AddPlayer( pPlayerInfo->szNickName, pPlayerInfo->nPlayerID, pPlayerInfo->nGrade );
	g_pScores->SetUpdate(true);
	g_pRadar->AddPlayer( pPlayerInfo->hObject, pPlayerInfo->nPlayerID );	

	CCharacterFX *pCharacter = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(pPlayerInfo->hObject);
	
	if( pCharacter && false == pCharacter->IsRecvModelInfo() )
	{
#ifdef GAME_SHOW
		if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
		{
			g_pNickList->AddNick( pPlayerInfo->hObject, pPlayerInfo->szNickName, pPlayerInfo->nTeamID );
			if( pCharacter ) pCharacter->SetUpdateNick(true);
		}
		else
		{
			if( pPlayerInfo->nTeamID == g_GameDoc.GetTeam() )
			{
				g_pNickList->AddNick( pPlayerInfo->hObject, pPlayerInfo->szNickName, pPlayerInfo->nTeamID );
				if( pCharacter ) pCharacter->SetUpdateNick(true);
			}
		}
#else
		if( pPlayerInfo->nTeamID == g_GameDoc.GetTeam() )
		{
			g_pNickList->AddNick( pPlayerInfo->hObject, pPlayerInfo->szNickName, pPlayerInfo->nTeamID );
			if( pCharacter ) pCharacter->SetUpdateNick(true);
		}
#endif

		pCharacter->RecvModelInfo();
	}
}