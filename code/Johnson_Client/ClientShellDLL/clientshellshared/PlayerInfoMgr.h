#ifndef __PLAYERINFOMGR_H__
#define __PLAYERINFOMGR_H__

#include "ltbasedefs.h"
#include "ClientUtilities.h"
#include "SharedMission.h"
//#include "SharedScoring.h"

class ILTClient;

typedef struct _tagPLAYER_INFO
{
	LTBOOL				bInGame;
	int32				nServerIndex;				// SC_AddRoomUser
	char				szNickName[MAX_NICK_LEN];	// SC_AddRoomUser
	uint8				nGrade;						// SC_AddRoomUser
	int8				nTeamID;					// SC_AddRoomUser	(버퍼룸에서 변함)

	int32				nPlayerID;					// Recv P2P Host
	HOBJECT				hObject;					// Recv P2P Host
	PlayerState			ePlayerState;				// Recv P2P Host

	LTBOOL				bRecvInfo;
	int32				nKill;						// SC_PlayerInfo	(게임중 변함)
	int32				nDeath;						// SC_PlayerInfo	(게임중 변함)
	uint8				nModelID[2];				// SC_PlayerInfo
	uint8				nPing;
	LTBOOL				bCaptain;
	LTBOOL				bNewPlayer;

} T_PLAYER_INFO, *T_PLAYER_INFOptr;

typedef std::vector<T_PLAYER_INFOptr>	PLAYER_INFO_LIST;
typedef PLAYER_INFO_LIST::iterator			PLAYER_INFO_LIST_ITR;


class CPlayerInfoMgr
{
public:

	CPlayerInfoMgr();
	~CPlayerInfoMgr();

    void				Init();
    void				AddPlayer( int32 nServerIndex, const char* lpszNick, uint8 nGrade, uint8 nTeamID, LTBOOL bCaptain );
	void				RemovePlayer( int32 nServerIndex );
	void				RemoveAll();

	void				SetPlayerInfoFromServer( int32 nServerIndex, uint16 nKill, uint16 nDeath, uint8 nAModelID, uint8 nBModeID );
	void				SetPlayerInfoFromP2P( int32 nServerIndex, int32 nPlayerID, HOBJECT hObj, PlayerState eState );
	void				LeaveGame( int32 nServerIndex );
	void				LeaveGameAll();
	void				ResetMandate();
	
	void				SetPlayerHandle( uint8 nPlayerID, HOBJECT hObj );
	void				SetPlayerPing( int nServerIndex, uint8 nPing );
	void				SetPlayerState( HOBJECT hObj, PlayerState eState );
	void				SetPlayerTeam( int32 nServerIndex, uint8 nTeamID );
	void				SetPlayerTeamOnGame( int32 nServerIndex, uint8 nTeamID );
	void				SetCaptain( int nServerIndex );

	T_PLAYER_INFOptr	GetLocalPlayer();
	T_PLAYER_INFOptr	GetFirstPlayer();
	T_PLAYER_INFOptr	GetPlayerByServerIndex( int32 nServerIndex );
    T_PLAYER_INFOptr	GetPlayerByID( uint8 nID );
	T_PLAYER_INFOptr	GetPlayerByNick( const char* lpszNick );
	T_PLAYER_INFOptr	GetPlayerByHandle( HOBJECT hObj );
	T_PLAYER_INFOptr	GetPlayerByIndex( uint32 nIndex );
	HOBJECT				GetHandleByID( uint8 nID );

    uint32				GetNumPlayerInGame();
    char const*			GetPlayerNick( int32 nServerIndex );
	char const*			GetPlayerNick( HOBJECT hObj );
	int32				GetPlayerServerIndex( uint8 nID );
	void				UpdatePlayerSort();

	uint8				GetNumPlayersOnTeam( uint8 nTeam = -1 );
	uint32				GetNumPlayers();
	
	void				SetNewPlayer( int nServerIndex, LTBOOL bNew );

protected:
	void				RecvComplete( T_PLAYER_INFOptr pPlayerInfo );

	PLAYER_INFO_LIST	m_PlayerInfoList;
};

#endif