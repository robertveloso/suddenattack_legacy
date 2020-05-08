#ifndef __MURSUM_NETWORK_H__
#define __MURSUM_NETWORK_H__

#define DEFAULT_P2P_PORT	8000

#define PROVIDER_GAMEHI	0
#define PROVIDER_CJ		1
#define PROVIDER_7ON	2
#define PROVIDER_MORIA	3


//Net Module
#include "../Lib_Network/ClientGate.h"

//Packet
#include "../Lib_Packet/Login.h"
#include "../Lib_Packet/Manager.h"
#include "../Lib_Packet/Service/Channel.h"
#include "../Lib_Packet/Service/Room.h"
#include "../Lib_Packet/Service/Game.h"
#include "../Lib_Packet/Service/Play.h"
#include "../Lib_Packet/Service/Player.h"
#include "../Lib_Packet/Service/Item.h"



using namespace GamehiSolution::ClientModule;
using namespace LogicObject::Player;
using namespace LogicObject::Room;
using namespace Packet::Login;
using namespace Packet::Service::Channel;
using namespace Packet::Service::Room;
using namespace Packet::Service::Game;
using namespace Packet::Service::Play;
using namespace Packet::Service::Player;

#define __P_L	::Packet::Login
#define __P_C	::Packet::Service::Channel
#define __P_R	::Packet::Service::Room
#define __P_PR	::Packet::Service::Player
#define __P_P	::Packet::Service::Play
#define __P_G	::Packet::Service::Game
#define __P_I	::Packet::Service::Item
#define __P_M	::Packet::Manager

typedef struct _tagPACKET
{
	__P_L::CQ_Login				CS_LoginGameHI;
	__P_L::CQ_LoginNetmarble	CS_LoginNetmarble;
	__P_L::CN_Nick				CS_Nick;
	__P_L::CN_Character			CS_Character;
	
	__P_C::CQ_Join				CS_JoinChannel;
	__P_C::CN_Refresh			CS_RefreshChannelList;
	__P_C::CN_Away				CS_AwayChannel;
	__P_C::CQ_QuickJoin			CS_ChannelQuickJoin;
	
	__P_R::CQ_Create			CS_CreateRoom;
	__P_R::CQ_Join				CS_JoinRoom;
	__P_R::CQ_QuickJoin			CS_QuickJoin;
	__P_R::CQ_Away				CS_ExitGameRoom;
	__P_R::NN_Chat				CS_Chat;
	__P_R::NN_ChangeOption		CS_ChangeOption;
	__P_R::NN_ChangeTitle		CS_ChangeRoomTitle;
	__P_R::CQ_ChangeCaptain		CS_ChangeCaptain;
	__P_R::CQ_Info				CS_DbgPlayerInfo;

	__P_PR::CQ_Find				CS_SearchUser;
	__P_PR::NN_ChangeTeam		CS_ChangeTeam;
	__P_PR::NN_Whisper			CS_Whisper;
	__P_PR::CN_Out				CS_KickBan;
	__P_PR::CN_Info				CS_UserInfo;
	
	__P_P::NN_Ready				CS_Ready;	
	__P_P::NN_Inventory			CS_Inven;
	__P_P::CN_Start				CS_StartGame;
	__P_P::CN_Host				CS_StartHost;
	__P_P::CN_ReturnBuff		CS_GetRoomInfo;
	__P_P::CQ_ViewLobby			CS_GetLobbyUserList;
	__P_P::CN_Invite			CS_Invite;

	__P_G::NN_Kill				CS_Kill;
	__P_G::NN_Suicide			CS_Suicide;	
	__P_G::CN_LoadComplate		CS_LoadComplete;
	__P_G::CN_Timeout			CS_TimeOut;
	__P_G::CN_Exit				CS_ExitGame;
	__P_G::NN_Chat				CS_GameChat;
	__P_G::CN_Bomb				CS_BombState;
	__P_G::CN_BombPlanted		CS_PlantedBomb;
	__P_G::CN_NextRound			CS_NextRound;
	__P_G::NN_Radio				CS_Radio;
	__P_G::NN_WeaponChange		CS_WeaponChange;
	__P_G::CN_NetLoss			CS_NetLoss;
	__P_G::CN_ServerTick		CS_ServerTick;

	__P_I::CN_EnterShop			CS_EnterShop;
	__P_I::CN_ExitShop			CS_ExitShop;
	__P_I::CQ_Buy				CS_BuyItem;
	__P_I::CQ_Discard			CS_DiscardItem;
	__P_I::CQ_AddEquipment		CS_AddEquipment;
	__P_I::CQ_RemoveEquipment	CS_RemoveEquipment;

	__P_G::CN_Speed				CS_HackDetected;
	
} T_PACKET, *T_PACKETptr;

typedef struct _tagSYNCDATA
{
	__int64	dwSendTime;
	__int64	dwRecvTime;
	__int64	dwServerTime;
	__int64	dwDelayTime;
} T_SYNC_DATA, *T_SYNC_DATAptr;

#define SYNC_TIME_COUNT			5

#define GAME_TYPE_TDM			::LogicObject::Room::Option::E_MODE::ENUM::RESPAWN
#define GAME_TYPE_TSV			::LogicObject::Room::Option::E_MODE::ENUM::BOMB

#define PLAYER_STATE_BUFF		::LogicObject::Player::Type::E_STATE::ENUM::BUFF		//2
#define PLAYER_STATE_READY		::LogicObject::Player::Type::E_STATE::ENUM::READY		//3
#define PLAYER_STATE_INVEN		::LogicObject::Player::Type::E_STATE::ENUM::INVENTORY	//4
#define PLAYER_STATE_P_READY	::LogicObject::Player::Type::E_STATE::ENUM::PLAY_READY	//5
#define PLAYER_STATE_PLAY		::LogicObject::Player::Type::E_STATE::ENUM::PLAY		//6

#define GAME_RESULT_WIN			0
#define GAME_RESULT_LOSE		1
#define GAME_RESULT_DRAW		2

#define MAX_ID_LEN				::LogicObject::Player::Type::s_nUserNick
#define MIN_NICK_LEN			4
#define MAX_NICK_LEN			::LogicObject::Player::Type::s_nUserNick
#define MAX_USERNO_LEN			::LogicObject::Player::Type::s_nUserNo
#define MAX_CLAN_LEN			32
#define MAX_R_TITLE_LEN			::LogicObject::Room::Type::s_nName
#define MAX_PASS_LEN			::LogicObject::Room::Type::s_nPass
#define MAX_ROOM_USER			16
#define MAX_IP_LEN				16



typedef struct _tagLOCALINFO
{
	int		nProvider;
	char	szUserNo[MAX_USERNO_LEN];
	char	szServerIP[MAX_IP_LEN];
	uint16	nServerPort;
} T_LOCAL_INFO, *T_LOCAL_INFOptr;

#define MAX_PACKET_SIZE	8192

class CNetwork
{
public:
	CNetwork();
	~CNetwork();

	static CNetwork& GetSingleton()
	{
		static CNetwork __stMURSUM_CNetwork__;
		return __stMURSUM_CNetwork__;
	}

	LTBOOL			Initialize( HWND hWnd, ILTClient* pLTClient );
	LTBOOL			Connect();
	void			Close();
	LTBOOL			SendPacket( char* pPacket, int nPacketLen );
	LTBOOL			SendPacket( CPacket* pPacket );
	void			SendPing();

	LTBOOL			Update( float fTime );
	LTBOOL			HandleMessage( unsigned long wParam, unsigned long lParam );
	LTBOOL			NetworkRefresh();

	LTBOOL			SetLocalInfo( T_LOCAL_INFOptr pLocalInfo );
	T_LOCAL_INFOptr GetLocalInfo() { return &m_LocalInfo; }

	void			EnableNetwork( LTBOOL bEnable )
	{
		m_bEnable = bEnable;
		g_pLTClient->CPrint( "[NETWORK] Enable : %d", m_bEnable );
	}

	void			CS_LoginGameHI();
	void			CS_LoginNetmarble();
	void			CS_UserInfo( int nIndex, const char* lpszNick, unsigned int nGrade );
	void			CS_CreateRoom( const char* lpszTitle,
								   const char* lpszPassword,
								   int16 nMaxUsers, uint8 nMapIndex,
								   uint8 nGameType, uint8 nTimeLimit,
								   uint8 nKillLimit, uint8 nRoundLimit );
	void			CS_JoinRoom( int nRoomIndex, const char* lpszPassword=LTNULL );
	void			CS_QuickJoin();	
	void			CS_ChangeOption( short nRoomIndex,
									 unsigned char nGameType,
									 unsigned char nMapIndex,
									 unsigned char nMaxUsers,
									 unsigned char nTimeLimit,
									 unsigned char nKillLimit,
									 unsigned char nRoundLimit,
									 bool bIntrusion,
									 bool bTeamBalance,
									 bool bFreeSpec );
	void			CS_ChangeRoomTitle( const char* lpszTitle );
	void			CS_ChangeCaptain( int nPlayerIndex );
	
	void			CS_NetLoss( uint8 nValue );							
	void			CS_ChangeTeam( int nTeam );
	void			CS_WeaponChange( uint16 nPlayerID, uint16 nWeaponID, uint16 nAmmoCount, uint16 nAmmoInClip );
	void			CS_Ready( LTBOOL bReady );
	void			CS_ExitGameRoom();
	void			CS_StartGame();
	void			CS_StartHostReply( LTBOOL bStarted );
	void			CS_Kill( int32 nKiller, int32 nVictim, bool bHeadShot, uint16 nWeaponID );
	void			CS_TimeOut();
	void			CS_ExitGameRequest();
	void			CS_LoadComplete();
	void			CS_StartNextRound();
	void			CS_SetBombState( int nState, int nIndex );
	void			CS_PlantedBomb( int nServerIndex, uint8 nPlayerID, float fTime, float fPosX, float fPosY, float fPosZ );
	void			CS_Chat( const char* lpszMessage );
	void			CS_GameChat( const char* lpszMessage );
	void			CS_TeamChat( const char* lpszMessage );
	void			CS_GhostChat( const char* lpszMessage );
	void			CS_GhostTeamChat( const char* lpszMessage );
	void			CS_KickBan( int nIndex );
	void			CS_Whisper( const char* lpszReceiverNick, const char* lpszMessage );
	void			CS_HostInfo( uint16 nConeType, uint16 nHostPort );
	void			CS_FirstNickName( const char* lpszNickName );
	void			CS_EnterInven();
	void			CS_LeaveInven();
	void			CS_SearchUser( const char* lpszNick );

	void			CS_JoinChannel( int nIndex );
	void			CS_RefreshChannel();
	void			CS_AwayChannel();
	void			CS_ChannelQuickJoin( int nChannelIndex, int nRoomIndex );
	void			CS_GetRoomInfo();
	void			CS_Radio( uint16 nPlayerID, uint16 nRadioID, uint16 nTeamID );
	void			CS_SendInvite( const char* lpszNick );
	void			CS_GetLobbyUserList();

	void			CS_EnterShop();
	void			CS_ExitShop();
	void			CS_BuyItem( char cClass1, char cClass2_1, char cClass2_2, uint8 nCode,
								int nDuration, bool bDefault, bool bDisplay );
	void			CS_DiscardItem( char cClass1, char cClass2_1, char cClass2_2, uint8 nCode );
	void			CS_AddEquipment( int nSlotIndex, char cClass1, char cClass2_1, char cClass2_2, uint8 nCode );
	void			CS_RemoveEquipment();

	void			CS_HackDetected();

	void			CS_DbgPlayerInfo();

	void			BeginTimeSync();
	void			CS_GetServerTick();
	void			EndTimeSync();
	
	void			SetRecvResult( LTBOOL b );
	LTBOOL			IsRecvResult();
	const char*		GetIP();	
	static void		DisconnectCallBack( LTBOOL bReturn, void* pData ); 
	static void		SendLogCallBack( LTBOOL bReturn, void* pData );
	
	void			ProcessMandate();
	void			ProcessRejoin( const char* lpszIP, uint16 nPort, bool bSpectator );
	
#ifndef _FINAL
	void			TestMandate();
#endif
	
protected:
	LTBOOL			OnPacket( CPacket* pPacket );
	LTBOOL			OnConnect();
	LTBOOL			OnClose();
	LTBOOL			CheckRecvComplete();

	void			SC_LoginReply( CPacket* pPacket );
	void			SC_UserList( CPacket* pPacket );
	void			SC_UserInfo( CPacket* pPacket );
	void			SC_MyUserInfo( CPacket* pPacket );
	void			SC_RemoveUser( CPacket* pPacket );
	void			SC_StartHost( CPacket* pPacket );
	void			SC_MandateHost( CPacket* pPacket );
	void			SC_StartGame( CPacket* pPacket );
	void			SC_FailStartGame( CPacket* pPacket );
	void			SC_RoomList( CPacket* pPacket );
	void			SC_RemoveRoom( CPacket* pPacket );
	void			SC_RoomUserProperty( CPacket* pPacket );
	void			SC_ChangeTeam( CPacket* pPacket );
	void			SC_ChangeTeamOnStart( CPacket* pPacket );
	void			SC_ChangeTeamOnGame( CPacket* pPacket );
	void			SC_ChangeRoomTitle_R( CPacket* pPacket );
	void			SC_ChangeRoomTitle_L( CPacket* pPacket );
	void			SC_ExitGameRoom( CPacket* pPacket );
	void			SC_JoinRoom( CPacket* pPacket );
	void			SC_QuickJoin( CPacket* pPacket );
	void			SC_RemoveRoomUser( CPacket* pPacket );
	void			SC_ChangeCaptain( CPacket* pPacket, LTBOOL bLobby );
	void			SC_EndRound( CPacket* pPacket );
	void			SC_GameResultUser( CPacket* pPacket );
	void			SC_GameResultTeam( CPacket* pPacket );
	void			SC_Chat( CPacket* pPacket );	
	void			SC_GameChat( CPacket* pPacket );
	void			SC_Whisper( CPacket* pPacket );
	void			SC_WhisperFail( CPacket* pPacket );
	void			SC_RoomStateChange( CPacket* pPacket, LTBOOL bPlaying );
	void			SC_KickBan( CPacket* pPacket );
	void			SC_Inven( CPacket* pPacket );
	void			SC_PlayerStateChange( CPacket* pPacket );
	void			SC_SearchUser( CPacket* pPacket );
	void			SC_ChangeCaptainResult( CPacket* pPacket );

	void			SC_Nick( CPacket* pPacket );
	void			SC_Character( CPacket* pPacket );
	void			SC_MyProperty( CPacket* pPacket );
	void			SC_ChannelListInfo( CPacket* pPacket );
	void			SC_ChannelInfo( CPacket* pPacket );
	void			SC_JoinChannel( CPacket* pPacket );
	void			SC_CreateRoom( CPacket* pPacket );
	void			SC_AddRoomUser( CPacket* pPacket );
	void			SC_IncPlayerCount( CPacket* pPacket );
	void			SC_DecPlayerCount( CPacket* pPacket );
	void			SC_ChangeOption( CPacket* pPacket, LTBOOL bLobby );
	void			SC_Ready( CPacket* pPacket );
	void			SC_ReturnBufferRoom( CPacket* pPacket );
	void			SC_ChannelQuickJoin( CPacket* pPacket );
	void			SC_NetLossPlayer( CPacket* pPacket );
	void			SC_NetLossRoom( CPacket* pPacket );

	void			SC_PlayerInfo( CPacket* pPacket );
	void			SC_DeathMatchInfo( CPacket* pPacket );
	void			SC_TimeBombMissionInfo( CPacket* pPacket );
	void			SC_Kill( CPacket* pPacket );
	void			SC_Suicide( CPacket* pPacket );
	void			SC_RemovePlayer( CPacket* pPacket );
	void			SC_Radio( CPacket* pPacket );
	void			SC_WeaponChange( CPacket* pPacket );

	void			SC_SetBombState( CPacket* pPacket );
	void			SC_PlantedBomb( CPacket* pPacket );

	void			SC_ManagerNotify( CPacket* pPacket );
		
	void			SC_JoinPlayFail( CPacket* pPacket );
	void			SC_PlayReady( CPacket* pPacket );
	void			SC_RefreshUserInfo( CPacket* pPacket );
	
	void			SC_Ranking( CPacket* pPacket );	
	void			SC_ServerTick( CPacket* pPacket );
	void			SC_SpeedHackDetected( CPacket* pPacket );

	void			SC_LobbyUserInfo( CPacket* pPacket );
	void			SC_LobbyUser( CPacket* pPacket );
	void			SC_Invite( CPacket* pPacket );

	void			SC_ShopItemList( CPacket* pPacket );
	void			SC_EnterShop( CPacket* pPacket );
	void			SC_ExitShop( CPacket* pPacket );
	void			SC_BuyItem( CPacket* pPacket );
	void			SC_DiscardItem( CPacket* pPacket );
	void			SC_AddItem( CPacket* pPacket );
	void			SC_RemoveItem( CPacket* pPacket );
	void			SC_Equipment( CPacket* pPacket );
	void			SC_AddEquipment( CPacket* pPacket );
	void			SC_RemoveEquipment( CPacket* pPacket );
	void			SC_MyInvenInfo( CPacket* pPacket );
	void			SC_MyInvenItem( CPacket* pPacket );

	void			SC_DbgRoomInfo( CPacket* pPacket );
	void			SC_DbgPlayerInfo( CPacket* pPacket );
				
	int				GetCurrentScreen();		

	void			SetPacketValue();
	
	void			ResetMandate();

	void			ParseItemList( int nType, const char* lpszItemList );
	void			AddShopItem( char* lpszClass1,
								 char* lpszClass2,
								 int nItemIndex,
								 int nPeriod,
								 LTBOOL	bDefault,
								 LTBOOL	bBuyItem,
								 int nPrice,
								 int nSortSeq );
	void			AddInvenItem( char cClass1, char cClass2_1, char cClass2_2, int nItemIndex, 
								  int nY, int nM, int nD, int nH );

	void			RemoveInvenItem( char cClass1, char cClass2_1, char cClass2_2, int nItemIndex );
	
	LTBOOL			IsWeapon( char c );
	LTBOOL			IsCharacter( char c );
	LTBOOL			IsWeaponByInt( int nClass );
	LTBOOL			IsCharacterByInt( int nClass );
	int				GetWeaponClass( const char* lpszClass2 );

	HWND			m_hMainWnd;
	ILTClient*		m_pLTCLient;
	
	GamehiSolution::ClientModule::CClientGate	m_ClientModule;
	T_PACKET		m_Packets;
	LTBOOL			m_bEnable;
	LTBOOL			m_bConnected;

	T_LOCAL_INFO	m_LocalInfo;
	uint32			m_nPingDelta;
	uint32			m_nLastPingTick;
	
	int				m_nLobbyUserCount;
	int				m_nLobbyRoomCount;

	LTBOOL			m_bRecvResult;
	LTBOOL			m_bCheckComplete;

	LTBOOL			m_bRecvPausePacket;

	int				m_nJoinRoomIndex;

	LTBOOL			m_bCompleteTimeSync;
	int				m_nSyncCount;
	__int64			m_nDeltaTime;
	T_SYNC_DATA		m_SyncData[SYNC_TIME_COUNT];

	LTBOOL			m_bSendTimeOut;

	char			m_szTextBuffer[4096];

	char			m_cDiscardItemClass;
	int				m_nDiscardItemIndex;
	
	int				m_nCheckFireGuard;
	
#ifdef __VIRTUAL_IP__
	//[MURSUM]	ConeTest
	LTBOOL			m_bEnableCheck;
	LTBOOL			m_bConeChecked;
	DWORD			m_dwConeCheckStart;
#endif
};

#define g_Network				CNetwork::GetSingleton()
#define g_Network_Init			CNetwork::GetSingleton().Initialize
#define g_Network_Connect		CNetwork::GetSingleton().Connect
#define g_Network_Close			CNetwork::GetSingleton().Close
#define g_Network_SendPacket	CNetwork::GetSingleton().SendPacket
#define g_Network_Update		CNetwork::GetSingleton().Update
#define g_Network_HandleMsg		CNetwork::GetSingleton().HandleMessage
#define g_Network_GetIP			CNetwork::GetSingleton().GetIP

#endif	//__MURSUM_NETWORK_H__
