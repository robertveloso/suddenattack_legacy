#include "stdafx.h"
/*
#include "ClientInfoMgr.h"
#include "iltclient.h"
#include "GameClientShell.h"
#include "CharacterFX.h"

#include <stdio.h>

#include "PlayerCamera.h"

#define VERT_SPACING 3

extern CGameClientShell* g_pGameClientShell;

namespace
{
	char szUnknownPlayer[32] = "";
	const int kNumTeams = 2;
}


CClientInfoMgr::CClientInfoMgr()
{
    m_pClients = LTNULL;
}

CClientInfoMgr::~CClientInfoMgr()
{
    CLIENT_INFO* ptr = LTNULL;
	while (m_pClients)
	{
		ptr = m_pClients->pNext;
		debug_delete(m_pClients);
		m_pClients = ptr;
	}

}

void CClientInfoMgr::Init()
{

	LoadString(IDS_UNKNOWN_PLAYER,szUnknownPlayer,sizeof(szUnknownPlayer) );

	m_nLocalID = 0;
}

void CClientInfoMgr::UpdateClient( char const* pszName,
								   uint32 nID,
								   HOBJECT hObj,
								   uint8 nState,
								   uint8 nTeamID,
								   int32 nServerIndex,
								   uint8 nGrade )
{
	// if we have this client in the list, update their info
	CLIENT_INFO* pClient = GetClientByID(nID, false);
	if (pClient)
	{
		pClient->sName			= pszName;
		pClient->nTeamID		= nTeamID;
		pClient->hObject		= hObj;
		pClient->ePlayerState	= (PlayerState)nState;
		pClient->nServerIndex	= nServerIndex;
		pClient->nGrade			= nGrade;
				
		UpdateClientSort(pClient);
	}
	else
	{
		//hmmm... we should have found them...
		g_pLTClient->CPrint("CClientInfoMgr::UpdateClient () : Unknown Client %d (%s)", nID, pszName);
	}
}

void CClientInfoMgr::PlayerConnected( char const* pszName,
									  uint32 nID,
									  HOBJECT hObj,
									  uint8 nState,
									  uint8 nTeamID,
									  int32 nServerIndex,
									  uint8	nGrade )
{
	m_nLocalID = 0;
	g_pLTClient->GetLocalClientID (&m_nLocalID);

	// if we already have this client in the list, just return

	CLIENT_INFO* pDup = GetClientByID(nID, false);
	if (pDup)
	{
		pDup->sName			= pszName;
		pDup->hObject		= hObj;
		pDup->nTeamID		= nTeamID;
		pDup->ePlayerState	= (PlayerState)nState;
		pDup->nGrade		= nGrade;
		pDup->nServerIndex	= nServerIndex;
	}
	else
	{
		// create the new object

		CLIENT_INFO* pNew = debug_new(CLIENT_INFO);
		if (!pNew) return;

		pNew->nID			= nID;
		pNew->sName			= pszName;
		pNew->hObject		= hObj;
		pNew->nTeamID		= nTeamID;
		pNew->ePlayerState	= (PlayerState)nState;
		pNew->nGrade		= nGrade;
		pNew->nServerIndex	= nServerIndex;

		pNew->sScore.Init(nID);
		

		// if we don't have a list yet, set the list pointer to the new object
		if (!m_pClients)
		{
			m_pClients = pNew;
			return;
		}

		// we do have a list - insert the object
		CLIENT_INFO* ptr = m_pClients;

		//insert at head?
		if (ptr->sScore.GetScore() < pNew->sScore.GetScore())
		{
			pNew->pNext = m_pClients;
			m_pClients = pNew;
			return;
		}

		CLIENT_INFO* pNext = ptr->pNext;
		while (pNext && pNext->sScore.GetScore() >= pNew->sScore.GetScore())
		{
			ptr = pNext;
			pNext = ptr->pNext;
		}
		if (pNext)
		{
			pNext->pPrev = pNew;
		}
		ptr->pNext = pNew;
		pNew->pNext = pNext;
		pNew->pPrev = ptr;
	}

	if( nTeamID == g_GameDoc.GetTeam() )
	{
		g_pNickList->AddNick( hObj, pszName, nTeamID );
		CCharacterFX *pCharacter = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(hObj);
		if( pCharacter ) pCharacter->SetUpdateNick(true);
	}
}


void CClientInfoMgr::AddClient( char const* pszName,
								uint32 nID,
								HOBJECT hObj,
								uint8 nState,
								uint8 nTeamID,
								int32 nServerIndex,
								uint8 nGrade )
{
	m_nLocalID = 0;
	g_pLTClient->GetLocalClientID (&m_nLocalID);

	// if we already have this client in the list, then it's an update, not an add.
	CLIENT_INFO* pDup = GetClientByID(nID, false);
	if (pDup)
	{
		pDup->sScore.Init(nID);
		pDup->hObject	= hObj;
		UpdateClient( pszName, nID, hObj, nState, nTeamID, nServerIndex, nGrade );
		
		if( nTeamID == g_GameDoc.GetTeam() )
		{
			g_pNickList->AddNick( hObj, pszName, nTeamID );
			CCharacterFX *pCharacter = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(hObj);
			if( pCharacter ) pCharacter->SetUpdateNick(true);
		}
		return;
	}

	// create the new object

	CLIENT_INFO* pNew = debug_new(CLIENT_INFO);
	if (!pNew) return;

	pNew->nID			= nID;
	pNew->sName			= pszName;
	pNew->nTeamID		= nTeamID;
	pNew->ePlayerState	= (PlayerState)nState;
	pNew->nServerIndex	= nServerIndex;
	pNew->nGrade		= nGrade;

	pNew->hObject = hObj;

	pNew->sScore.Init(nID);

	// if we don't have a list yet, set the list pointer to the new object
	if (!m_pClients)
	{
		m_pClients = pNew;
		return;
	}

	// we do have a list - insert the object
	CLIENT_INFO* ptr = m_pClients;

	//insert at head?
	if (ptr->sScore.GetScore() < pNew->sScore.GetScore())
	{
		pNew->pNext = m_pClients;
		m_pClients = pNew;
		return;
	}

	CLIENT_INFO* pNext = ptr->pNext;
	while (pNext && pNext->sScore.GetScore() >= pNew->sScore.GetScore())
	{
		ptr = pNext;
		pNext = ptr->pNext;
	}
	if (pNext)
	{
		pNext->pPrev = pNew;
	}
	ptr->pNext = pNew;
	pNew->pNext = pNext;
	pNew->pPrev = ptr;


	if( nTeamID == g_GameDoc.GetTeam() )
	{
		g_pNickList->AddNick( hObj, pszName, nTeamID );
		CCharacterFX *pCharacter = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(hObj);
		if( pCharacter ) pCharacter->SetUpdateNick(true);
	}
}

void CClientInfoMgr::RemoveClient (uint32 nID)
{
	if (!m_pClients) return;

	m_nLocalID = 0;
	g_pLTClient->GetLocalClientID (&m_nLocalID);

	// find the client

	CLIENT_INFO* ptr = GetClientByID(nID, false);
	if (!ptr) return;

	if( ptr->nTeamID == g_GameDoc.GetTeam() )
	{
		g_pNickList->RemoveNick( ptr->hObject );
		CCharacterFX *pCharacter = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(ptr->hObject);
		if( pCharacter ) pCharacter->SetUpdateNick(false);
	}

	if (ptr->pNext) 
		ptr->pNext->pPrev = ptr->pPrev;
	if (ptr->pPrev) 
		ptr->pPrev->pNext = ptr->pNext;
	if (m_pClients == ptr) 
		m_pClients = ptr->pNext;

	debug_delete(ptr);
}

void CClientInfoMgr::SetPlayerState( HOBJECT hObj, PlayerState eState )
{
	CLIENT_INFO* pCI = GetFirstClient();

	while( pCI )
	{
		if( pCI->hObject == hObj )
		{
			pCI->ePlayerState	= eState;
			return;
		}
		pCI = pCI->pNext;
	}
}

void CClientInfoMgr::SetPlayerTeam( int nIndex, int nTeamID )
{
	CLIENT_INFO* pCI = GetFirstClient();

	while( pCI )
	{
		if( pCI->nServerIndex == nIndex )
		{
			pCI->nTeamID = nTeamID;
			return;
		}
		pCI = pCI->pNext;
	}
}

void CClientInfoMgr::RemoveAllClients()
{

    CLIENT_INFO* ptr = LTNULL;
	while (m_pClients)
	{
		ptr = m_pClients->pNext;
		debug_delete(m_pClients);
		m_pClients = ptr;
	}

	g_pHUDMgr->QueueUpdate(kHUDScores);	
}

void CClientInfoMgr::UpdateClientSort(CLIENT_INFO* pCur)
{
	if (!pCur) return;

	g_pHUDMgr->QueueUpdate(kHUDScores);

	// put this client in the correct position in the list (most frags to least frags)
	CLIENT_INFO* pTmp = pCur;

	LTBOOL bBack = LTFALSE;
	//try going forward
	while (pTmp->pPrev && pCur->sScore.GetScore() > pTmp->pPrev->sScore.GetScore())	
		pTmp = pTmp->pPrev;
	if (pTmp == pCur)
	{
		//try going backward
		while (pTmp->pNext && pCur->sScore.GetScore() < pTmp->pNext->sScore.GetScore())	
			pTmp = pTmp->pNext;
		if (pTmp == pCur) return;
		bBack = LTTRUE;

	}

	// we found a new position - remove current from the list
	if (pCur->pPrev)
		pCur->pPrev->pNext = pCur->pNext;
	else
		m_pClients = pCur->pNext;
	if (pCur->pNext) 
		pCur->pNext->pPrev = pCur->pPrev;

	// put us back in in the correct position
	if (bBack)
	{
		pCur->pPrev = pTmp;
		pCur->pNext = pTmp->pNext;
		pTmp->pNext = pCur;
		if (pCur->pNext)
			pCur->pNext->pPrev = pCur;
	}
	else
	{
		pCur->pPrev = pTmp->pPrev;
		pCur->pNext = pTmp;
		if (pTmp->pPrev) 
			pTmp->pPrev->pNext = pCur;
		else
			m_pClients = pCur;
		pTmp->pPrev = pCur;
	}


}


uint32 CClientInfoMgr::GetNumClients()
{
	if (!m_pClients) return 0;

	CLIENT_INFO* ptr = m_pClients;

    uint32 nCount = 0;
	while (ptr)
	{
		nCount++;
		ptr = ptr->pNext;
	}

	return nCount;
}

char const* CClientInfoMgr::GetPlayerName (uint32 nID)
{
    if (!m_pClients) return LTNULL;

	CLIENT_INFO* ptr = GetClientByID(nID,false);
	if (ptr)
		return ptr->sName.c_str( );

	return szUnknownPlayer;
}

char const* CClientInfoMgr::GetPlayerName( HOBJECT hObj )
{
    if (!m_pClients) return LTNULL;

	CLIENT_INFO* ptr = m_pClients;
	while (ptr)
	{
		if (ptr->hObject == hObj)
			return ptr->sName.c_str();

		ptr = ptr->pNext;
	}
	
	return szUnknownPlayer;
}

int32 CClientInfoMgr::GetPlayerServerIndex( uint32 nID )
{
	if (!m_pClients) return LTNULL;

	CLIENT_INFO* ptr = GetClientByID(nID,false);
	if (ptr)
		return ptr->nServerIndex;

	return -1;
}

CLIENT_INFO* CClientInfoMgr::GetLocalClient()
{
	return GetClientByID(m_nLocalID);
}

CLIENT_INFO* CClientInfoMgr::GetClientByID(uint32 nID, bool bUpdateOnFailure)
{
	CLIENT_INFO* ptr = m_pClients;
	while (ptr)
	{
		if (ptr->nID == nID)
			return ptr;

		ptr = ptr->pNext;
	}
	if (bUpdateOnFailure)
		g_pInterfaceMgr->UpdateClientList();
    return LTNULL;
}

CLIENT_INFO* CClientInfoMgr::GetClientByHandle( HOBJECT hObj )
{
	CLIENT_INFO* ptr = m_pClients;
	while (ptr)
	{
		if (ptr->hObject  == hObj)
			return ptr;

		ptr = ptr->pNext;
	}

	return LTNULL;
}

uint8 CClientInfoMgr::GetNumPlayersOnTeam(uint8 nTeam)
{
	if (nTeam > kNumTeams)
	{
		CLIENT_INFO* pLocal = GetLocalClient();
		if (pLocal)
			nTeam = pLocal->nTeamID;
	}

	uint8 nCount = 0;

	CLIENT_INFO* ptr = m_pClients;
	while (ptr)
	{
		if (ptr->nTeamID == nTeam)
			nCount++;

		ptr = ptr->pNext;
	}

	return nCount;
}
*/