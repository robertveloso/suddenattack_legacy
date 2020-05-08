#ifndef __CLIENTINFOMGR_H
#define __CLIENTINFOMGR_H

/*
#include "ltbasedefs.h"
#include "ClientUtilities.h"
#include "SharedMission.h"
#include "SharedScoring.h"

class ILTClient;


struct CLIENT_INFO
{
    CLIENT_INFO()
	{
		nPing			= 0;
		nID				= 0;
		pPrev			= LTNULL;
		pNext			= LTNULL;		
		nTeamID			= 0;
		hObject			= LTNULL;
		ePlayerState	= PS_UNKNOWN;
	}

	uint16			nPing;
    uint32          nID;
	int32			nServerIndex;
	uint8			nGrade;
	std::string		sName;		// Nick (Unique)
	CPlayerScore	sScore;
	uint8			nTeamID;
	HOBJECT			hObject;
	PlayerState		ePlayerState;

	CLIENT_INFO*	pPrev;
	CLIENT_INFO*	pNext;
};

class CClientInfoMgr
{
public:

	CClientInfoMgr();
	~CClientInfoMgr();

    void    Init ();
	// 광식 : 등급 추가
	//[MURSUM] Nick
    void    AddClient      ( char const* pszName, uint32 nID, HOBJECT hObj, uint8 nState, uint8 nTeamID, int32 nServerIndex, uint8 nGrade );
	void	PlayerConnected( char const* pszName, uint32 nID, HOBJECT hObj, uint8 nState, uint8 nTeamID, int32 nServerIndex, uint8 nGrade );
    void    UpdateClient   ( char const* pszName, uint32 nID, HOBJECT hObj, uint8 nState, uint8 nTeamID, int32 nServerIndex, uint8 nGrade );
    void    RemoveClient (uint32 nID);
	void	RemoveAllClients();
	void	SetPlayerState( HOBJECT hObj, PlayerState eState );
	void	SetPlayerTeam( int nIndex, int nTeamID );

	CLIENT_INFO* GetLocalClient();
	CLIENT_INFO* GetFirstClient() {return m_pClients;}
    CLIENT_INFO* GetClientByID(uint32 nID, bool bUpdateOnFailure = true);
	CLIENT_INFO* GetClientByHandle( HOBJECT hObj );

    uint32  GetNumClients();
    char const* GetPlayerName (uint32 nID);
	char const* GetPlayerName (HOBJECT hObj);
	int32		GetPlayerServerIndex(uint32 nID);
	void	UpdateClientSort(CLIENT_INFO* pCur);

	uint8	GetNumPlayersOnTeam(uint8 nTeam = -1);

protected:


	CLIENT_INFO*		m_pClients;

	uint32				m_nLocalID;

};
*/

#endif