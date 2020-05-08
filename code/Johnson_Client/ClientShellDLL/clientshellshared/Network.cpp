#include "StdAfx.h"
#include "./network.h"

//extern================
#include "InterfaceMgr.h"
#include "..\\sa\\ScreenMain.h"
#include "..\\sa\\ScreenRoom.h"
#include "..\\sa\\ScreenResult.h"
#include "..\\sa\\ScreenCharacter.h"
#include "..\\sa\\ScreenInventory.h"
#include "..\\sa\\ScreenChannel.h"
#include "ClientMultiplayerMgr.h"
#include "MissionMgr.h"
//======================

//------------------------------------------- (B) Novice (minjin) [03/12/2005]
#include "GameMessage.h"
#include "GameText.h"
//#include "../../../CasualServerLib/SuddenAttack/AdminPacket.h"
//------------------------------------------- (E) Novice (minjin) [03/12/2005]

#define CHANGE_TEAM_BUFF	0
#define CHANGE_TEAM_START	1
#define CHANGE_TEAM_PLAY	2

#define PARSE_ITEM_SHOP		0
#define PARSE_ITEM_INVEN	1

#ifdef _FINAL
#define DEFAULT_TEAM_BALANCE	1
#else
#define DEFAULT_TEAM_BALANCE	0
#endif

#define __TIME_SYNC__

char	g_szStartTime[16]={0,};

CNetwork::CNetwork()
{
	m_nDeltaTime	= 0;
	m_bCompleteTimeSync	= LTFALSE;
	m_bEnable		= LTFALSE;
	m_nLastPingTick	= 0;
	m_nPingDelta	= 10000;
	m_pLTCLient		= LTNULL;
	m_bConnected	= LTFALSE;

#ifdef __VIRTUAL_IP__
	//[MURSUM]	ConeTest
	m_bEnableCheck	= LTFALSE;
	m_bConeChecked	= LTFALSE;
#endif

	m_bConnected	= LTFALSE;		
	m_nJoinRoomIndex = -1;

	m_cDiscardItemClass	= LTNULL;
	m_nDiscardItemIndex = -1;
	
	m_nCheckFireGuard   = 10;

	memset( &m_LocalInfo, 0, sizeof(T_LOCAL_INFO) );
}

CNetwork::~CNetwork()
{
	m_bEnable			= LTFALSE;
	m_bCheckComplete	= LTFALSE;
	m_ClientModule.Destroy();
}

LTBOOL CNetwork::Initialize( HWND hWnd, ILTClient* pLTCLient )
{
	if( !hWnd || !pLTCLient ) return LTFALSE;
	m_hMainWnd	= hWnd;
	m_pLTCLient	= pLTCLient;
	m_bEnable	= LTTRUE;

	SetPacketValue();

	m_nLastPingTick = ::GetTickCount();

	g_pLTClient->CPrint( "[NETWORK] Init" );
	return m_ClientModule.Create( m_hMainWnd, (WM_USER+100)/*WM_NETWORK*/, MAX_PACKET_SIZE );
}

LTBOOL CNetwork::Connect()
{
	g_pLTClient->CPrint( "[NETWORK] Connect %s, %d", m_LocalInfo.szServerIP, m_LocalInfo.nServerPort );
	return m_ClientModule.Connect( m_LocalInfo.szServerIP, m_LocalInfo.nServerPort, CClientGate::ServicerType::Client );
}

void CNetwork::Close()
{
	g_pLTClient->CPrint( "[NETWORK] Close" );
	m_ClientModule.Close();
}

LTBOOL CNetwork::SendPacket( CPacket* pPacket )
{
	if( pPacket )
	{
		m_ClientModule.Send( pPacket );
#ifdef _DEBUG
		g_pLTClient->CPrint( "<== %d",pPacket->GetCommand() );
#endif	
	}
	return LTTRUE;
}

LTBOOL CNetwork::SendPacket( char* pPacket, int nPacketLen )
{
	if( !pPacket || nPacketLen <= 0 ) return LTFALSE;
	m_ClientModule.Send( pPacket, nPacketLen );
	return LTTRUE;
}

void CNetwork::SendPing()
{
	if( LTFALSE == m_bConnected ) return;
	
	uint32 nCurPingDelta;
	nCurPingDelta = ::GetTickCount() - m_nLastPingTick;
	if( nCurPingDelta > 20000 ) //ÇÎÀ» ¾Èº¸³½Áö 20ÃÊ ÀÌ»ó Áö³µÀ¸¸é ·Î±×¿¡ ±â·Ï.
	{
		g_pLTClient->CPrint( "[NETWORK] Ping Delta ( %d ) +++++++++++++++++++++++++++++", nCurPingDelta );
	}
	
	/*
	__int64 nCurrTick = ::GetTickCount();
	
	uint32 nServerTick = m_bCompleteTimeSync ? uint32( nCurrTick+m_nDeltaTime ) : 0 ;
	*/

	m_ClientModule.Pinging( 0 );
	m_nLastPingTick = ::GetTickCount();
}

LTBOOL CNetwork::Update( float fTime )
{
	uint32 nCurPingDelta;

	while( NetworkRefresh() ) {};	
	
	nCurPingDelta = ::GetTickCount() - m_nLastPingTick;
	if( nCurPingDelta > m_nPingDelta )
	{
#ifdef USE_FIRE_GUARD
		if( --m_nCheckFireGuard < 0 )
		{
			m_nCheckFireGuard = -1;
			g_pLTClient->CheckFireGuard();
		}
#endif
		SendPing();
	}

#ifdef __VIRTUAL_IP__
	//[MURSUM]	ConeTest
	if( m_bEnableCheck && ::GetTickCount() - m_dwConeCheckStart > 2000 )
	{
		m_bEnableCheck	= LTFALSE;
		g_pLTClient->StopCheckConeType();
		g_pInterfaceMgr->CloseMessageBox(TRUE);
	}
#endif
	return LTTRUE;
}

LTBOOL CNetwork::HandleMessage( unsigned long wParam, unsigned long lParam )
{
	m_ClientModule.FirePacket( wParam, lParam );
	return LTTRUE;
}

LTBOOL CNetwork::NetworkRefresh()
{
	if( LTFALSE == m_bEnable ||
		LTFALSE == m_ClientModule.RefreshPackets() ) return LTFALSE;
	
	CClientGate::PacketType packetType;
	char*	pPacket;
	CPeer*	pPeer;			////
	int		nPacketSize;
	INT		nServicerIndex;	////

	m_ClientModule.GetRecentPacket( nServicerIndex,
									pPeer,
									packetType,
									pPacket,
									nPacketSize );

	switch( packetType )
	{
	case CClientGate::PacketType::ReceiveType:
		OnPacket( (CPacket*)pPacket );
		return LTTRUE;

	case CClientGate::PacketType::ConnectType:
		OnConnect();
		return LTTRUE;

	case CClientGate::PacketType::CloseType:
		OnClose();
		return LTTRUE;
	}

	return LTFALSE;
}

LTBOOL CNetwork::OnConnect()
{
	g_pLTClient->CPrint( "[NETWORK] Send LoginRequest" );
	m_bConnected	= LTTRUE;

	switch( m_LocalInfo.nProvider )
	{
	case PROVIDER_GAMEHI:
		CS_LoginGameHI();
		break;
	case PROVIDER_CJ:
		CS_LoginNetmarble();
		break;
	case PROVIDER_7ON:
		break;
	case PROVIDER_MORIA:
		break;
	}
	return LTTRUE;
}

LTBOOL CNetwork::OnClose()
{
	//Close Msg...
	if( m_bConnected )
	{
		//Áß°£¿¡ ¿¬°áÀÌ ²÷±ä°æ¿ì
		g_pLTClient->CPrint( "[NETWORK] OnClose  CurrentTick(%d) LastPingTick(%d) Delta(%d)",
							 ::GetTickCount(), m_nLastPingTick, ::GetTickCount()-m_nLastPingTick );

		if( SCREEN_ID_RESULT == g_pInterfaceMgr->GetCurrentScreen() )
		{
			g_pInterfaceMgr->ShowMessageBox( "¼­¹ö¿Í ¿¬°áÀÌ ²÷¾îÁ³½À´Ï´Ù.\r\n\r\n"
											 "Á¤È®ÇÑ ¿¡·¯ ºÐ¼®À» À§ÇÏ¿© ¿À·ù·Î±×¸¦ º¸³»½Ã°Ú½À´Ï±î?",
											 LTMB_YESNO, CNetwork::SendLogCallBack, LTNULL );
		}
		else
		{
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_DISCONNECTED,
											 LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
		}		
	}
	else
	{
		//¿¬°áÁ¶Â÷ ¾ÈµÈ °æ¿ì
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ERROR_ON_CONNECT_SERVER,
					LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
	}

	m_bConnected = LTFALSE;
	
	return LTTRUE;
}

LTBOOL CNetwork::OnPacket( CPacket* pPacket )
{
	switch( pPacket->GetCommand() )
	{
//LOGIN ====================================================
	case __P_L::SA_Login::s_nCode:
		SC_LoginReply( pPacket );						
		break;

	case __P_L::SN_Nick::s_nCode:
		SC_Nick( pPacket );
		break;

	case __P_L::SN_Character::s_nCode:
		SC_Character( pPacket );	//ÉèÖÃ½ÇÉ«µÄÄ£ÐÍÊôÐÔ modify by zhoujb 0814
		break;

	case __P_L::SN_MyProperty::s_nCode:
		SC_MyProperty( pPacket );
		break;

	case __P_L::SN_GameResultFail::s_nCode:
		SC_MyProperty( LTNULL );
		break;

	case __P_L::SN_Ranking::s_nCode:
		SC_Ranking( pPacket );
		break;

	case __P_C::SN_Info::s_nCode:
		SC_ChannelListInfo( pPacket );
		break;

	case __P_C::SN_Enter::s_nCode:
		SC_ChannelInfo( pPacket );
		break;

	case __P_C::SA_QuickJoin::s_nCode:
		SC_ChannelQuickJoin( pPacket );
		break;

	case __P_C::SA_Join::s_nCode:
		SC_JoinChannel( pPacket );
		break;

	case __P_R::SA_Away::s_nCode:
		SC_ExitGameRoom( pPacket );
		break;

	case __P_R::SN_Enter::s_nCode:
		SC_RoomList( pPacket );
		break;

	case __P_R::SN_Leave::s_nCode:
		SC_RemoveRoom( pPacket );
		break;		

	case __P_R::SN_RemoveUser::s_nCode:
		SC_DecPlayerCount( pPacket );
		break;		

	case __P_R::NN_ChangeTitle::s_nCode:
		SC_ChangeRoomTitle_R( pPacket );
		break;

	case __P_R::SN_ChangeTitle::s_nCode:
		SC_ChangeRoomTitle_L( pPacket );
		break;

	case __P_R::SA_ChangeCaptain::s_nCode:
		SC_ChangeCaptainResult( pPacket );
		break;
	
	case __P_PR::SN_EnterLobby::s_nCode:
		SC_UserList( pPacket );
		break;

	case __P_PR::SN_LeaveLobby::s_nCode:
		SC_RemoveUser( pPacket );
		break;

	case __P_R::SN_AddUser::s_nCode:
		SC_IncPlayerCount( pPacket );
		break;

	case __P_R::NN_ChangeCaptain::s_nCode:
		SC_ChangeCaptain( pPacket, LTFALSE );
		break;

	case __P_R::SN_ChangeCaptain::s_nCode:
		SC_ChangeCaptain( pPacket, LTTRUE );
		break;

	case __P_R::NN_ChangeOption::s_nCode:
		SC_ChangeOption( pPacket, LTFALSE );
		break;

	case __P_R::SN_ChangeOption::s_nCode:
		SC_ChangeOption( pPacket, LTTRUE );
		break;

	case __P_R::SN_ChangeOptionFail::s_nCode:
		break;
		
	case __P_R::SN_JoinPlayFail::s_nCode:
		SC_JoinPlayFail( pPacket );
		break;

	case __P_PR::SN_Enter::s_nCode:
		SC_AddRoomUser( pPacket );
		break;

	case __P_PR::SN_Leave::s_nCode:
		SC_RemoveRoomUser( pPacket );
		break;

	case __P_PR::SN_Result::s_nCode:
		SC_RefreshUserInfo( pPacket );

	case __P_P::SN_State::s_nCode:
		SC_PlayerStateChange( pPacket );
		break;

	case __P_P::NN_Inventory::s_nCode:
		SC_Inven( pPacket );
		break;

	case __P_R::SA_Create::s_nCode:
		SC_CreateRoom( pPacket );
		break;

	case __P_P::SA_ViewLobby::s_nCode:
		SC_LobbyUserInfo( pPacket );
		break;

	case __P_P::SN_LobbyPlayer::s_nCode:
		SC_LobbyUser( pPacket );
		break;

	case __P_P::SN_Invite::s_nCode:
		SC_Invite( pPacket );
		break;

	case __P_R::SA_Join::s_nCode:
		SC_JoinRoom( pPacket );
		break;

	case __P_R::SA_QuickJoin::s_nCode:
		SC_QuickJoin( pPacket );
		break;

	case __P_R::SN_Play::s_nCode:
		SC_RoomStateChange( pPacket, LTTRUE );
		break;

	case __P_R::SN_Buff::s_nCode:
		SC_RoomStateChange( pPacket, LTFALSE );
		break;

	case __P_R::NN_Chat::s_nCode:
		SC_Chat( pPacket );
		break;

	case __P_PR::SA_Find::s_nCode:
		SC_SearchUser( pPacket );
		break;

	case __P_PR::NN_ChangeTeam::s_nCode:		
		SC_ChangeTeam( pPacket );
		break;

	case __P_PR::SN_ChangeTeamOnStart::s_nCode:
		SC_ChangeTeamOnStart( pPacket );
		break;

	case __P_PR::SN_ChangeTeamOnGame::s_nCode:
		SC_ChangeTeamOnGame( pPacket );
		break;

	case __P_P::NN_Ready::s_nCode:
		SC_Ready( pPacket );
		break;

	case __P_P::SN_ReturnBuff::s_nCode:
		SC_ReturnBufferRoom( pPacket );
		break;

	case __P_P::SN_Host::s_nCode:
		SC_StartHost( pPacket );
		break;

	case __P_G::SN_Play::s_nCode:
		SC_StartGame( pPacket );
		break;

	case __P_P::SN_PlayReady::s_nCode:
		SC_PlayReady( pPacket );
		break;

	case __P_P::SN_StartFail::s_nCode:
		SC_FailStartGame( pPacket );
		break;

	case __P_G::SN_ResultTotal::s_nCode:
		SC_GameResultTeam( pPacket );
		break;

	case __P_G::SN_ResultPlayer::s_nCode:
		SC_GameResultUser( pPacket );
		break;

	case __P_G::SN_RoundResult::s_nCode:
		SC_EndRound( pPacket );
		break;

	case __P_G::SN_Pause::s_nCode:
		SC_MandateHost( pPacket );
		break;

	case __P_G::NN_Chat::s_nCode:
		SC_GameChat( pPacket );
		break;

	case __P_G::SN_BombPlanted::s_nCode:
		SC_PlantedBomb( pPacket );
		break;

	case __P_G::SN_Bomb::s_nCode:
		SC_SetBombState( pPacket );
		break;

	case __P_G::SN_PlayerInfo::s_nCode:
		SC_PlayerInfo( pPacket );
		break;	
		
	case __P_G::SN_RespawnInfo::s_nCode:
		SC_DeathMatchInfo( pPacket );
		break;

	case __P_G::SN_BombInfo::s_nCode:
		SC_TimeBombMissionInfo( pPacket );
		break;

	case __P_G::NN_Kill::s_nCode:
		SC_Kill( pPacket );
		break;

	case __P_G::NN_Suicide::s_nCode:
		SC_Suicide( pPacket );
		break;

	case __P_G::SN_Exit::s_nCode:
		SC_RemovePlayer( pPacket );
		break;

	case __P_G::NN_WeaponChange::s_nCode:
		SC_WeaponChange( pPacket );
		break;

	case __P_G::NN_Radio::s_nCode:
		SC_Radio( pPacket );
		break;

	case __P_G::SN_NetLossPlayer::s_nCode:
		SC_NetLossPlayer( pPacket );
		break;

	case __P_G::SN_NetLossRoom::s_nCode:
		SC_NetLossRoom( pPacket );
		break;

	case __P_PR::NN_Whisper::s_nCode:
		SC_Whisper( pPacket );
		break;

	case __P_PR::SN_WhisperFail::s_nCode:
		SC_WhisperFail( pPacket );
		break;

	case __P_PR::SN_Out::s_nCode:
		SC_KickBan( pPacket );
		break;

	case __P_PR::SN_Info::s_nCode:
		SC_UserInfo( pPacket );
		break;

	case __P_M::SN_Noti::s_nCode:
		SC_ManagerNotify( pPacket );
		break;

	case __P_G::SN_ServerTick::s_nCode:
		SC_ServerTick( pPacket );
		break;

	case __P_G::SN_Speed::s_nCode:
		SC_SpeedHackDetected( pPacket );
		break;

	case __P_I::SN_Shop::s_nCode:
		SC_ShopItemList( pPacket );
		break;

	case __P_I::SN_EnterShop::s_nCode:
		SC_EnterShop( pPacket );
		break;

	case __P_I::SN_ExitShop::s_nCode:
		SC_ExitShop( pPacket );
		break;

	case __P_I::SA_Buy::s_nCode:
		SC_BuyItem( pPacket );
		break;

	case __P_I::SA_Discard::s_nCode:
		SC_DiscardItem( pPacket );
		break;

	case __P_I::SN_AddItem::s_nCode:
		SC_AddItem( pPacket );
		break;

	case __P_I::SN_RemoveItem::s_nCode:
		SC_RemoveItem( pPacket );
		break;

	case __P_I::SN_Equipment::s_nCode:
		SC_Equipment( pPacket );
		break;

	case __P_I::SA_AddEquipment::s_nCode:
		SC_AddEquipment( pPacket );
		break;

	case __P_I::SA_RemoveEquipment::s_nCode:
		SC_RemoveEquipment( pPacket );
		break;	
		
	case __P_I::SN_Inven::s_nCode:
		SC_MyInvenInfo( pPacket );
		break;

	case __P_I::SN_Item::s_nCode:
		SC_MyInvenItem( pPacket );
		break;

	case __P_R::SA_Info::s_nCode:
		SC_DbgRoomInfo( pPacket );
		break;

	case __P_R::SN_PlayerInfo::s_nCode:
		SC_DbgPlayerInfo( pPacket );
		break;

	default:
		g_pLTClient->CPrint( "[ERROR] Unknown Packet Command (%d)", pPacket->GetCommand() );
		ASSERT(!"Unknown Packet Command");
		break;
	}
	return LTTRUE;
}

LTBOOL CNetwork::SetLocalInfo( T_LOCAL_INFOptr pLocalInfo )
{
	if( !pLocalInfo ) return LTFALSE;
	
	memcpy( &m_LocalInfo, pLocalInfo, sizeof(T_LOCAL_INFO) );
	return LTTRUE;
}

LTBOOL CNetwork::CheckRecvComplete()
{
	if (m_nLobbyUserCount <= 0 && m_nLobbyRoomCount <= 0 && g_GameDoc.IsAllItemSet())
	{
		//·ëÁ¤º¸, »ç¿ëÀÚÁ¤º¸ ¹Þ±â ¿Ï·á
		g_pScreenMain->SortRoomList();

		if( g_pInterfaceMgr->IsExitMessage() )
		{
			g_pInterfaceMgr->ShowMessageBox( g_pInterfaceMgr->GetExitMessage(), LTMB_OK, LTNULL, LTNULL );
			g_pInterfaceMgr->SetExitMessage( LTNULL );
		}
		else
		{
			g_pInterfaceMgr->CloseMessageBox(TRUE);
		}	
		
#ifdef __VIRTUAL_IP__
		//[MURSUM]	ConeTest
		if( LTFALSE == m_bConeChecked )
		{
			m_bConeChecked	= LTTRUE;

			if( LTTRUE == g_GameDoc.CanBeHost() )
			{
				CS_HostInfo( UTC_REAL_IP, g_pLTClient->GetHostPort() );
			}
			else
			{				
				m_bEnableCheck	= LTTRUE;
				m_dwConeCheckStart	= ::GetTickCount();
				g_pLTClient->StartCheckConeType( m_LocalInfo.szServerIP, m_LocalInfo.nServerPort+1,
												 LTNULL, 0, g_GameDoc.GetID(), MAX_ID_LEN );
				// "Check Cone Type..."
				g_pInterfaceMgr->ShowMessageBox( SA_TEXT_CHK_CONE_TYPE, LTMB_MESSAGE, LTNULL, LTNULL );
			}
		}
#endif

		g_pScreenMain->OnRecvComplete(m_nJoinRoomIndex);
		m_nJoinRoomIndex = -1;
		
		m_bCheckComplete	= LTFALSE;
		return LTTRUE;
	}

	return LTFALSE;
}

// CLIENT -> SERVER ========================================================================
void CNetwork::CS_LoginGameHI()
{
	g_pLTClient->CPrint( "[NETWORK] Send Login GameHI" );
	m_Packets.CS_LoginGameHI.Set( m_ClientModule.GetLocalAddr(), m_LocalInfo.szUserNo, g_GameDoc.GetClientHWInfo() );
	SendPacket( &m_Packets.CS_LoginGameHI );

	//m_Packets.CS_LoginNetmarble.Set( m_ClientModule.GetLocalAddr(), 
	//								 "Certificate=f1553a2f8bd18a59&Sex=f1553a2f8bd18a59&Name=bb2eceba47b0c5f3&UniID=dba883cc54ad28ce7b3c93c4d489771b&Age=1489fa5ce12aeab7&IDState=3f01788a2482ba3a&UserID=ec2ce6231c3b59bc",
	//								 g_GameDoc.GetClientHWInfo() );
	//SendPacket( &m_Packets.CS_LoginNetmarble );
}

void CNetwork::CS_LoginNetmarble()
{
	g_pLTClient->CPrint( "[NETWORK] Send Login Netmarble" );
	m_Packets.CS_LoginNetmarble.Set( m_ClientModule.GetLocalAddr(), g_pLTClient->GetCpCookie(), g_GameDoc.GetClientHWInfo() );
	SendPacket( &m_Packets.CS_LoginNetmarble );
}

void CNetwork::CS_JoinChannel( int nIndex )
{
	if( SCREEN_ID_CHANNEL == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send JoinChannel" );
		g_GameDoc.SetChannelIndex( nIndex );
		m_Packets.CS_JoinChannel.m_byChannelIndex = nIndex;
		SendPacket( &m_Packets.CS_JoinChannel );
	}
}

void CNetwork::CS_RefreshChannel()
{
	if( SCREEN_ID_CHANNEL == GetCurrentScreen() )
	{
		SendPacket( &m_Packets.CS_RefreshChannelList );
	}
}

void CNetwork::CS_AwayChannel()
{
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send AwayChannel" );
		SendPacket( &m_Packets.CS_AwayChannel );
	}
}

void CNetwork::CS_ChannelQuickJoin( int nChannelIndex, int nRoomIndex )
{
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send ChannelQuickJoin" );
		m_nJoinRoomIndex = nRoomIndex;
		m_Packets.CS_ChannelQuickJoin.m_byChannelIndex = nChannelIndex;
		SendPacket( &m_Packets.CS_ChannelQuickJoin );
	}
}

void CNetwork::CS_StartGame()
{
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send StartGame" );
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NOW_ON_GAME_ENTER,
					LTMB_MESSAGE, LTNULL, LTNULL );
		
		SendPacket( &m_Packets.CS_StartGame );
	}
}

void CNetwork::CS_StartHostReply( LTBOOL bStarted )
{
	g_pLTClient->CPrint( "[NETWORK] Send StartHostReply  Started(%d)", bStarted );

	m_Packets.CS_StartHost.m_byResult	= bStarted ?
										__P_P::CN_Host::E_RESULT::ENUM::SUCCESS : 
										  __P_P::CN_Host::E_RESULT::ENUM::FAIL;
	m_Packets.CS_StartHost.m_nPort		= g_pLTClient->GetHostPort();

	SendPacket( &m_Packets.CS_StartHost );
	if( bStarted ) g_GameDoc.SetHost(LTTRUE);
}

void CNetwork::CS_GetRoomInfo()
{
	SendPacket( &m_Packets.CS_GetRoomInfo );
}

void CNetwork::CS_UserInfo( int nIndex, const char* lpszNick, unsigned int nGrade )
{
	g_GameDoc.SetUserInfoNick( lpszNick );
	g_GameDoc.SetUserInfoGrade( nGrade );
	m_Packets.CS_UserInfo.m_nIndex = nIndex;
	SendPacket( &m_Packets.CS_UserInfo );
}

void CNetwork::CS_CreateRoom( const char* lpszTitle, const char* lpszPassword,
							  int16	nMaxUsers, uint8 nMapIndex, uint8 nGameType, uint8 nTimeLimit, uint8 nKillLimit, uint8 nRoundLimit )
{
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send CreateRoom" );
		g_GameDoc.SetTitle( lpszTitle );
		g_GameDoc.SetPassword( lpszPassword );

		g_GameDoc.SetCreateRoomParam( nMapIndex, nGameType, nMaxUsers, nTimeLimit, nKillLimit, nRoundLimit );

		if( lpszPassword && strlen(lpszPassword) > 0 )
		{
			m_Packets.CS_CreateRoom.m_stOption.SetPassword( true );
			strcpy( m_Packets.CS_CreateRoom.m_strPass, lpszPassword );
		}
		else
		{
			m_Packets.CS_CreateRoom.m_stOption.SetPassword( false );
			m_Packets.CS_CreateRoom.m_strPass[0] = LTNULL;
		}
		memset( m_Packets.CS_CreateRoom.m_strName, 0, MAX_R_TITLE_LEN );
		strncpy( m_Packets.CS_CreateRoom.m_strName, lpszTitle, MAX_R_TITLE_LEN-1 );

		m_Packets.CS_CreateRoom.m_stOption.SetTeamMaxForValue( int(nMaxUsers*0.5f) );
		m_Packets.CS_CreateRoom.m_stOption.SetMode( (::LogicObject::Room::Option::E_MODE::ENUM)nGameType );
		m_Packets.CS_CreateRoom.m_stOption.SetTimeForValue( nTimeLimit );
		m_Packets.CS_CreateRoom.m_stOption.SetKillLimitForValue( nKillLimit );	
		m_Packets.CS_CreateRoom.m_stOption.SetMapIndex( nMapIndex );
		m_Packets.CS_CreateRoom.m_stOption.SetRoundLimit( nRoundLimit );
#ifdef GAME_SHOW
		m_Packets.CS_CreateRoom.m_stOption.SetTeamBalance( false );
		m_Packets.CS_CreateRoom.m_stOption.SetRunnableInBattle( false );
		m_Packets.CS_CreateRoom.m_stOption.SetFree( false );
#else		
		m_Packets.CS_CreateRoom.m_stOption.SetTeamBalance( DEFAULT_TEAM_BALANCE );
		m_Packets.CS_CreateRoom.m_stOption.SetRunnableInBattle( true );
		m_Packets.CS_CreateRoom.m_stOption.SetFree( true );
#endif
		
		SendPacket( &m_Packets.CS_CreateRoom );
	
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NOW_ON_ROOM_ENTER,
					LTMB_MESSAGE, LTNULL, LTNULL );
	}
}

void CNetwork::CS_JoinRoom( int nRoomIndex, const char* lpszPassword )
{
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send JoinRoom" );
		if( LTNULL == lpszPassword )
		{
			m_Packets.CS_JoinRoom.m_strPass[0] = LTNULL;
		}
		else
		{
			strcpy( m_Packets.CS_JoinRoom.m_strPass, lpszPassword );
		}
		m_Packets.CS_JoinRoom.m_nRoomIndex = nRoomIndex;

		SendPacket( &m_Packets.CS_JoinRoom );

		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NOW_ON_ROOM_ENTER,
					LTMB_MESSAGE, LTNULL, LTNULL );
	}
}

void CNetwork::CS_QuickJoin()
{
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send QuickJoin" );
		SendPacket( &m_Packets.CS_QuickJoin );				

		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NOW_ON_ROOM_ENTER,
					LTMB_MESSAGE, LTNULL, LTNULL );
	}
}

void CNetwork::CS_ChangeOption( short nRoomIndex,
							    unsigned char nGameType,
								unsigned char nMapIndex,
								unsigned char nMaxUsers, 
								unsigned char nTimeLimit,
								unsigned char nKillLimit, 
								unsigned char nRoundLimit, 
								bool bIntrusion,
								bool bTeamBalance,
								bool bFreeSpec )
{
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		m_Packets.CS_ChangeOption.m_nRoomIndex = nRoomIndex;
		m_Packets.CS_ChangeOption.m_stOption.SetTeamMaxForValue( nMaxUsers );
		m_Packets.CS_ChangeOption.m_stOption.SetMode( (::LogicObject::Room::Option::E_MODE::ENUM)nGameType );
		m_Packets.CS_ChangeOption.m_stOption.SetTimeForValue( nTimeLimit );
		m_Packets.CS_ChangeOption.m_stOption.SetKillLimitForValue( nKillLimit );		
		m_Packets.CS_ChangeOption.m_stOption.SetMapIndex( nMapIndex );
		m_Packets.CS_ChangeOption.m_stOption.SetRoundLimit( nRoundLimit );
		m_Packets.CS_ChangeOption.m_stOption.SetTeamBalance( bTeamBalance );
		m_Packets.CS_ChangeOption.m_stOption.SetRunnableInBattle( bIntrusion );
		m_Packets.CS_ChangeOption.m_stOption.SetFree( bFreeSpec );

		SendPacket( &m_Packets.CS_ChangeOption );
	}
}

void CNetwork::CS_ChangeRoomTitle( const char* lpszTitle )
{
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		m_Packets.CS_ChangeRoomTitle.Set( lpszTitle );
		SendPacket( &m_Packets.CS_ChangeRoomTitle );
	}
}

void CNetwork::CS_ChangeCaptain( int nPlayerIndex )
{
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		m_Packets.CS_ChangeCaptain.m_nPlayerIndex = nPlayerIndex;
		SendPacket( &m_Packets.CS_ChangeCaptain );
	}
}

void CNetwork::CS_ChangeTeam( int nTeam )
{
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		m_Packets.CS_ChangeTeam.m_byTeam = nTeam;
		SendPacket( &m_Packets.CS_ChangeTeam );
	}
}

void CNetwork::CS_Ready( LTBOOL	bReady )
{
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		m_Packets.CS_Ready.m_bReady	= !!bReady;
		SendPacket( &m_Packets.CS_Ready );
	}	
}

void CNetwork::CS_Kill( int32 nKiller, int32 nVictim, bool bHeadShot, uint16 nWeaponID )
{
	if( nKiller == nVictim )
	{
		m_Packets.CS_Suicide.m_nVictim	= nVictim;
		m_Packets.CS_Suicide.m_nModelID	= nWeaponID;
		SendPacket( &m_Packets.CS_Suicide );
	}
	else
	{
		m_Packets.CS_Kill.m_nKiller		= nKiller;
		m_Packets.CS_Kill.m_nVictim		= nVictim;
		m_Packets.CS_Kill.m_bHeadShot	= bHeadShot;
		m_Packets.CS_Kill.m_nModelID	= nWeaponID;
		
		SendPacket( &m_Packets.CS_Kill );
	}
}

void CNetwork::CS_HackDetected()
{
	SendPacket( &m_Packets.CS_HackDetected );
}

void CNetwork::CS_DbgPlayerInfo()
{
	SendPacket( &m_Packets.CS_DbgPlayerInfo );
}

void CNetwork::CS_TimeOut()
{
	if( LTFALSE == m_bSendTimeOut )
	{
		g_pLTClient->CPrint( "[NETWORK] Send TimeOut" );
		SendPacket( &m_Packets.CS_TimeOut );
		m_bSendTimeOut = LTTRUE;
	}
}

void CNetwork::CS_ExitGameRequest()
{
	g_pLTClient->CPrint( "[NETWORK] Send ExitGameRequest" );
	SendPacket( &m_Packets.CS_ExitGame );
}

void CNetwork::CS_ExitGameRoom()
{
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send ExitGameRoom" );
		SendPacket( &m_Packets.CS_ExitGameRoom );

		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NOW_ON_ROOM_EXIT,
						LTMB_MESSAGE, LTNULL, LTNULL );
	}
}

void CNetwork::CS_LoadComplete()
{
	g_pLTClient->CPrint( "[NETWORK] Send LoadComplete" );
	SendPacket( &m_Packets.CS_LoadComplete );
}

void CNetwork::CS_StartNextRound()
{
	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		m_bSendTimeOut = LTFALSE;
		g_pLTClient->CPrint( "[NETWORK] Send StartNextRound" );
		SendPacket( &m_Packets.CS_NextRound );
	}
}

void CNetwork::CS_SetBombState( int nState, int nIndex )
{
	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send SetBombState State: %d  Idx: %d", nState, nIndex );
		m_Packets.CS_BombState.m_byType		= nState;
		m_Packets.CS_BombState.m_nPlayer	= nIndex;
		
		SendPacket( &m_Packets.CS_BombState );
	}
}

void CNetwork::CS_PlantedBomb( int nServerIndex, uint8 nPlayerID, float fTime, float fPosX, float fPosY, float fPosZ )
{
	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pLTClient->CPrint( "[NETWORK] Send PlantedBomb  Idx: %d", nServerIndex );
		m_Packets.CS_PlantedBomb.m_nPlanterIndex	= nServerIndex;
		m_Packets.CS_PlantedBomb.m_nPlayerID		= nPlayerID;
		m_Packets.CS_PlantedBomb.m_fDetonateTimer	= fTime;
		m_Packets.CS_PlantedBomb.m_fTimeBombPosX	= fPosX;
		m_Packets.CS_PlantedBomb.m_fTimeBombPosY	= fPosY;
		m_Packets.CS_PlantedBomb.m_fTimeBombPosZ	= fPosZ;

		SendPacket( &m_Packets.CS_PlantedBomb );
	}
}

void CNetwork::CS_Chat( const char* lpszMessage )
{
	m_Packets.CS_Chat.Set( lpszMessage );
	SendPacket( &m_Packets.CS_Chat );
}

void CNetwork::CS_GameChat( const char* lpszMessage )
{
	m_Packets.CS_GameChat.m_byType = __P_G::NN_Chat::E_TYPE::ENUM::ALL;
	m_Packets.CS_GameChat.Set( lpszMessage );
	SendPacket( &m_Packets.CS_GameChat );
}

void CNetwork::CS_TeamChat( const char* lpszMessage )
{
	m_Packets.CS_GameChat.m_byType = __P_G::NN_Chat::E_TYPE::ENUM::TEAM;
	m_Packets.CS_GameChat.Set( lpszMessage );
	SendPacket( &m_Packets.CS_GameChat );
}

void CNetwork::CS_GhostChat( const char* lpszMessage )
{
#ifndef GAME_SHOW
	m_Packets.CS_GameChat.m_byType = __P_G::NN_Chat::E_TYPE::ENUM::GHOST_ALL;
	m_Packets.CS_GameChat.Set( lpszMessage );
	SendPacket( &m_Packets.CS_GameChat );
#endif
}

void CNetwork::CS_GhostTeamChat( const char* lpszMessage )
{
#ifndef GAME_SHOW
	m_Packets.CS_GameChat.m_byType = __P_G::NN_Chat::E_TYPE::ENUM::GHOST_TEAM;
	m_Packets.CS_GameChat.Set( lpszMessage );
	SendPacket( &m_Packets.CS_GameChat );
#endif
}

void CNetwork::CS_Whisper( const char* lpszReceiverNick, const char* lpszMessage )
{
	m_Packets.CS_Whisper.Set( lpszReceiverNick, lpszMessage );	
	g_GameDoc.SetLastSendNick( lpszReceiverNick );
	SendPacket( &m_Packets.CS_Whisper );
}

void CNetwork::CS_Radio( uint16 nPlayerID, uint16 nRadioID, uint16 nTeamID )
{
	m_Packets.CS_Radio.m_nPlayerIndex	= nPlayerID;
	m_Packets.CS_Radio.m_nRadioID		= nRadioID;
	m_Packets.CS_Radio.m_nTeamID		= nTeamID;
	SendPacket( &m_Packets.CS_Radio );
}

void CNetwork::CS_NetLoss( uint8 nValue )
{
	m_Packets.CS_NetLoss.m_byNetLoss = nValue;
	SendPacket( &m_Packets.CS_NetLoss );
}

void CNetwork::CS_WeaponChange( uint16 nPlayerID, uint16 nWeaponID, uint16 nAmmoCount, uint16 nAmmoInClip )
{
	m_Packets.CS_WeaponChange.m_nPlayerIndex = nPlayerID;
	m_Packets.CS_WeaponChange.m_nWeaponID = nWeaponID;
	m_Packets.CS_WeaponChange.m_nWeaponModel = 0;
	m_Packets.CS_WeaponChange.m_nAmmoCount = nAmmoCount;
	m_Packets.CS_WeaponChange.m_nAmmoInClip = nAmmoInClip;

	SendPacket( &m_Packets.CS_WeaponChange );
}

void CNetwork::CS_SearchUser( const char* lpszNick )
{
	m_Packets.CS_SearchUser.SetNick( lpszNick );
	g_GameDoc.SetSearchUser( lpszNick );
	SendPacket( &m_Packets.CS_SearchUser );	
}

void CNetwork::CS_KickBan( int nIndex )
{
	if( SCREEN_ID_SCREEN_ROOM != GetCurrentScreen() ) return;

	m_Packets.CS_KickBan.m_nIndex = nIndex;
	SendPacket( &m_Packets.CS_KickBan );
}

void CNetwork::CS_HostInfo( uint16 nConeType, uint16 nHostPort )
{
	//m_Packets.HostInfo.nHostPort	= nHostPort;
	//m_Packets.HostInfo.nConeType	= nConeType;
	//SendPacket( reinterpret_cast<char*>( &m_Packets.HostInfo ),
	//			m_Packets.HostInfo.GetLength() );
}

void CNetwork::CS_FirstNickName( const char* lpszNickName )
{
	m_Packets.CS_Nick.Set( lpszNickName );
	SendPacket( &m_Packets.CS_Nick );	
}

void CNetwork::CS_EnterInven()
{
	g_pLTClient->CPrint( "[NETWORK] Send EnterInven" );
	m_Packets.CS_Inven.m_bInventory	= LTTRUE;
	SendPacket( &m_Packets.CS_Inven );
}

void CNetwork::CS_LeaveInven()
{
	g_pLTClient->CPrint( "[NETWORK] Send LeaveInven" );
	m_Packets.CS_Inven.m_bInventory	= LTFALSE;
	SendPacket( &m_Packets.CS_Inven );
}

void CNetwork::CS_GetLobbyUserList()
{
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		SendPacket( &m_Packets.CS_GetLobbyUserList );
	}
}

void CNetwork::CS_SendInvite( const char* lpszNick )
{
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		m_Packets.CS_Invite.SetNick( lpszNick );
		SendPacket( &m_Packets.CS_Invite );
	}
}

void CNetwork::CS_EnterShop()
{
	g_pLTClient->CPrint( "[NETWORK] Send EnterShop" );
	SendPacket( &m_Packets.CS_EnterShop );
}

void CNetwork::CS_ExitShop()
{
	g_pLTClient->CPrint( "[NETWORK] Send ExitShop" );
	SendPacket( &m_Packets.CS_ExitShop );
}

void CNetwork::CS_BuyItem( char cClass1, char cClass2_1, char cClass2_2, uint8 nCode,
						   int nDuration, bool bDefault, bool bDisplay )
{
#ifndef _FINAL
#ifdef DEBUG_SHOP
	return;
#endif
#endif

	g_pLTClient->CPrint( "[NETWORK] Send BuyItem %c|%c%c Idx(%d) Period(%d) Default(%d), Display(%d)",
						 cClass1, cClass2_1, cClass2_2, nCode, nDuration, bDefault, bDisplay );

	m_Packets.CS_BuyItem.m_stItem.m_cClass1				= cClass1;
	m_Packets.CS_BuyItem.m_stItem.m_stClass2[0]			= cClass2_1;
	m_Packets.CS_BuyItem.m_stItem.m_stClass2[1]			= cClass2_2;
	m_Packets.CS_BuyItem.m_stItem.m_byCode				= nCode;
	m_Packets.CS_BuyItem.m_nPeriod						= nDuration;
	m_Packets.CS_BuyItem.m_cDefault						= bDefault ? 'Y' : 'N';
	m_Packets.CS_BuyItem.m_cDisplay						= bDisplay ? 'Y' : 'N';

	SendPacket( &m_Packets.CS_BuyItem );
}

void CNetwork::CS_DiscardItem( char cClass1, char cClass2_1, char cClass2_2, uint8 nCode )
{
#ifndef _FINAL
#ifdef DEBUG_SHOP
	return;
#endif
#endif

	g_pLTClient->CPrint( "[NETWORK] Send DiscardItem %c|%c%c Idx(%d)",
						 cClass1, cClass2_1, cClass2_2, nCode );

	m_Packets.CS_DiscardItem.m_stItem.m_cClass1			= cClass1;
	m_Packets.CS_DiscardItem.m_stItem.m_stClass2[0]		= cClass2_1;
	m_Packets.CS_DiscardItem.m_stItem.m_stClass2[1]		= cClass2_2;
	m_Packets.CS_DiscardItem.m_stItem.m_byCode			= nCode;

	SendPacket( &m_Packets.CS_DiscardItem );

	m_cDiscardItemClass	= cClass1;
	m_nDiscardItemIndex = nCode;
}

void CNetwork::CS_AddEquipment( int nSlotIndex, char cClass1, char cClass2_1, char cClass2_2, uint8 nCode )
{
#ifndef _FINAL
#ifdef DEBUG_SHOP
	if( IsWeaponByInt(nSlotIndex) )
	{
		g_pScreenInventory->AddEquipWeapon( nSlotIndex-ITEM_IDX_CHAR_B,
											nCode );
	}
	else if( IsCharacterByInt(nSlotIndex) )
	{
		g_pScreenInventory->AddEquipCharacter( nCode, nSlotIndex );			
	}
	g_GameDoc.SetEquipItem( nCode, nSlotIndex );
	if( m_bCheckComplete ) CheckRecvComplete();
	return;
#endif
#endif

	g_pLTClient->CPrint( "[NETWORK] Send AddEquip %c|%c%c Idx(%d) SlotIdx(%d)",
						 cClass1, cClass2_1, cClass2_2, nCode, nSlotIndex );

	m_Packets.CS_AddEquipment.m_byIndex					= nSlotIndex;
	m_Packets.CS_AddEquipment.m_stItem.m_cClass1		= cClass1;
	m_Packets.CS_AddEquipment.m_stItem.m_stClass2[0]	= cClass2_1;
	m_Packets.CS_AddEquipment.m_stItem.m_stClass2[1]	= cClass2_2;
	m_Packets.CS_AddEquipment.m_stItem.m_byCode			= nCode;

	SendPacket( &m_Packets.CS_AddEquipment );
}

void CNetwork::CS_RemoveEquipment()
{
}

// SERVER -> CLIENT ========================================================================
void CNetwork::SC_LoginReply( CPacket* pPacket )
{
	__P_L::SA_Login* pLogin = static_cast< __P_L::SA_Login * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv LoginReply ACK: %d", pLogin->m_byResult );
	switch( pLogin->m_byResult )
	{
	case __P_L::SA_Login::E_RESULT::SUCCESS:
		g_GameDoc.SetUniqueIndex( pLogin->m_nIndex );
		g_pLTClient->CPrint( "[NETWORK] My Unique Index: %d", g_GameDoc.GetUniqueIndex() );
		g_pGameClientShell->ScreenRender( "Please wait." );
		break;
	case __P_L::SA_Login::E_RESULT::FAIL_LOGINED:
		//"Á¢¼ÓÇÏ½Å ¾ÆÀÌµð°¡ »ç¿ë Áß ÀÔ´Ï´Ù.\r\n"...
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ID_IN_USE,
					LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
		break;
	case __P_L::SA_Login::E_RESULT::FAIL_LOGINED_DB:
		//"Á¢¼ÓÇÏ½Å ¾ÆÀÌµð°¡ »ç¿ë Áß ÀÔ´Ï´Ù.\r\n"...
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ID_IN_USE,
					LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
		break;
	case __P_L::SA_Login::E_RESULT::FAIL_WRONG_ID:
		//"µî·ÏµÇÁö ¾ÊÀº ¾ÆÀÌµð ÀÔ´Ï´Ù."
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_INVALID_ID,
					LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
		break;

	case __P_L::SA_Login::E_RESULT::FAIL_NETMARBLE_TIMEOUT:
		//"·Î±×ÀÎ ½Ã°£ÀÌ ¸¸·áµÇ¾ú½À´Ï´Ù.\r\n\r\n³Ý¸¶ºí À¥ÆäÀÌÁö¿¡¼­ ´Ù½Ã ·Î±×ÀÎÇØÁÖ½Ã±â ¹Ù¶ø´Ï´Ù."
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_LOGIN_TIME_EXPIRED,
					LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
		break;
	case __P_L::SA_Login::E_RESULT::FAIL_NETMARBLE_DEC:
		//"»ç¿ëÀÚ Á¤º¸¸¦ ÇØ¼®ÇÒ ¼ö ¾ø½À´Ï´Ù.\r\n\r\nÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù."
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_WRONG_USERINFO,
					LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
		break;
	case __P_L::SA_Login::E_RESULT::FAIL_BAD_PLAYER://ºÒ·® »ç¿ëÀÚ
		g_pInterfaceMgr->ShowMessageBox( pLogin->m_strText,
					LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
		break;
	case __P_L::SA_Login::E_RESULT::FAIL_DB:
	case __P_L::SA_Login::E_RESULT::FAIL_ADO:
	case __P_L::SA_Login::E_RESULT::FAIL_QUERY:
	case __P_L::SA_Login::E_RESULT::FAIL_DB_ERROR:
	default:
		this->Close();
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ERROR_ON_CONNECT_SERVER,
					LTMB_OK, CNetwork::DisconnectCallBack, LTNULL, LTTRUE );
		break;
	}
}

void CNetwork::SC_Nick( CPacket* pPacket )
{
	__P_L::SN_Nick* pNick = static_cast< __P_L::SN_Nick * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv Nick: %d", pNick->m_byResult );

	switch( pNick->m_byResult )
	{
	case __P_L::SN_Nick::E_RESULT::ENUM::EMPTY:
		g_GameDoc.SetUniqueIndex( pNick->m_nIndex );
		g_pGameClientShell->SetDrawLogo(false);
		g_pScreenChannel->SetNickName(LTTRUE);
		g_pInterfaceMgr->SwitchToScreen( SCREEN_ID_CHANNEL );
		break;
		
	case __P_L::SN_Nick::E_RESULT::ENUM::EXIST:
		g_pScreenChannel->ResponseNickName( 1 );
		break;

	case __P_L::SN_Nick::E_RESULT::ENUM::BAD_NAME:
		g_GameDoc.SetUniqueIndex( pNick->m_nIndex );
		g_pGameClientShell->SetDrawLogo(false);
		g_pScreenChannel->SetNickName(LTFALSE);
		g_pInterfaceMgr->SwitchToScreen( SCREEN_ID_CHANNEL );
		g_pScreenChannel->ResponseNickName( 2, pNick->GetNick(), pNick->GetText() );
		break;

	case __P_L::SN_Nick::E_RESULT::ENUM::RESET:
		g_GameDoc.SetUniqueIndex( pNick->m_nIndex );
		g_pGameClientShell->SetDrawLogo(false);
		g_pScreenChannel->SetNickName(LTFALSE);
		g_pInterfaceMgr->SwitchToScreen( SCREEN_ID_CHANNEL );
		g_pScreenChannel->ResponseNickName( 3, pNick->GetNick(), pNick->GetText() );
		break;

	case __P_L::SN_Nick::E_RESULT::ENUM::FAIL_ADO:
	case __P_L::SN_Nick::E_RESULT::ENUM::FAIL_DB1:
	case __P_L::SN_Nick::E_RESULT::ENUM::FAIL_DB2:
	case __P_L::SN_Nick::E_RESULT::ENUM::FAIL_DB3:
		g_pScreenChannel->ResponseNickName( 10 );
		break;
	}
}
//modify by zhoujb 0814
void CNetwork::SC_Character( CPacket* pPacket )
{
	g_pLTClient->CPrint( "[NETWORK] Recv Character  ==== Ã»ÓÐ´¦Àí" );
	m_Packets.CS_Character.stRed.m_nModel = 0;
	m_Packets.CS_Character.stRed.m_nHeadSkin = 0;
	m_Packets.CS_Character.stRed.m_nBodySkin = 0;
	
	m_Packets.CS_Character.stBlue.m_nModel = 0;
	m_Packets.CS_Character.stBlue.m_nHeadSkin = 0;
	m_Packets.CS_Character.stBlue.m_nBodySkin = 0;
	SendPacket( &m_Packets.CS_Character);
	
}
void CNetwork::SC_MyProperty( CPacket* pPacket )
{
	if( !pPacket )
	{
		//"»ç¿ëÀÚ Á¤º¸¸¦ °¡Á®¿À´ø Áß\r\n"
		//"ÇÏ³ª ÀÌ»óÀÇ ¿À·ù°¡ ¹ß»ýÇÏ¿´½À´Ï´Ù.\r\n"
		//"ÇÁ·Î±×·¥À» Á¾·á ÇÕ´Ï´Ù."
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_USERINFO_ERROR,
										 LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
		return;
	}

	__P_L::SN_MyProperty* pProperty = static_cast< __P_L::SN_MyProperty * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv MyProperty" );
	g_pLTClient->CPrint( "\tNick: %s", pProperty->m_stStock.GetNick() );
	g_pLTClient->CPrint( "\tAge: %d  Real: %d", pProperty->m_byAge, pProperty->m_bRealName );
	g_pLTClient->CPrint( "\tGrade: %d", pProperty->m_stStock.m_byGrade );
	g_pLTClient->CPrint( "\tExp: %15.0f", (double)pProperty->m_stResult.m_llExp );
	g_pLTClient->CPrint( "\tMoney: %15.0f", (double)pProperty->m_stResult.m_llMoney );
	g_pLTClient->CPrint( "\tKill: %d", pProperty->m_stResult.m_nKill );
	g_pLTClient->CPrint( "\tDeath: %d", pProperty->m_stResult.m_nDeath );	
	g_pLTClient->CPrint( "\tWin: %d", pProperty->m_stResult.m_nWin );
	g_pLTClient->CPrint( "\tLose: %d", pProperty->m_stResult.m_nLose );
	g_pLTClient->CPrint( "\tDraw: %d", pProperty->m_stResult.m_nDraw );
	g_pLTClient->CPrint( "\tModel_A: %d\t%d\t%d", pProperty->m_stRed.m_nModel, pProperty->m_stRed.m_nHeadSkin, pProperty->m_stRed.m_nBodySkin );
	g_pLTClient->CPrint( "\tModel_B: %d\t%d\t%d", pProperty->m_stBlue.m_nModel, pProperty->m_stBlue.m_nHeadSkin, pProperty->m_stBlue.m_nBodySkin );
	
	g_GameDoc.SetNick( pProperty->m_stStock.GetNick() );
	g_GameDoc.SetGrade( pProperty->m_stStock.m_byGrade );
	
	g_GameDoc.SetCanBeHost( LTTRUE );
	g_GameDoc.SetFirst(LTFALSE);
	g_GameDoc.SetExp( pProperty->m_stResult.m_llExp );
	g_GameDoc.SetKill( pProperty->m_stResult.m_nKill );
	g_GameDoc.SetDeath( pProperty->m_stResult.m_nDeath );
	g_GameDoc.SetMoney( pProperty->m_stResult.m_llMoney );
	g_GameDoc.SetWinCount( pProperty->m_stResult.m_nWin );
	g_GameDoc.SetLoseCount( pProperty->m_stResult.m_nLose );
	g_GameDoc.SetDrawCount( pProperty->m_stResult.m_nDraw );
	g_GameDoc.SetUserModelId( pProperty->m_stRed.m_nModel, TEAM_A);
	g_GameDoc.SetUserModelId( pProperty->m_stBlue.m_nModel, TEAM_B);

	//18¼¼ ÀÌ»ó
	if( pProperty->m_byAge >= 18 /*&& pProperty->m_bRealName*/ )
	{
		g_GameDoc.SetLowViolence(LTFALSE);
	}
	else
	{
		g_GameDoc.SetLowViolence(LTTRUE);
	}
	
	g_GameDoc.SetChannelName( pProperty->GetServerName() );
	g_GameDoc.SetID( pProperty->GetID() );

	char szDumpName[MAX_ID_LEN+16];
//	sprintf( szDumpName, "%s_%s", g_GameDoc.GetID(), g_szStartTime );
	sprintf( szDumpName, "%s_%s", g_GameDoc.GetNick(), g_szStartTime );

	g_pLTClient->SetUserNickName( szDumpName );
	//g_pLTClient->SetFireGuardID( szDumpName );

#ifdef USE_FIRE_GUARD
	g_pLTClient->CPrint( "[FIREGUARD] Init FireGuard" );
	long lResult = g_pLTClient->InitFireGuard( "suddenattack", g_GameDoc.GetID() );
	if( 0L != lResult )
	{
		g_pLTClient->CPrint( "[FIREGUARD] Init FireGuard Result(0x%08x)", lResult );
		g_pLTClient->ShowFireGuardErr( lResult, false );
		g_pLTClient->Shutdown();
	}
	else
	{
		g_pLTClient->CPrint( "[FIREGUARD] Init FireGuard Success" );

		g_pLTClient->CPrint( "[FIREGUARD] Start FireGuard" );
		lResult = g_pLTClient->StartFireGuard();
		if( 0L != lResult )
		{
			g_pLTClient->CPrint( "[FIREGUARD] Start FireGuard Result(0x%08x)", lResult );
			g_pLTClient->ShowFireGuardErr( lResult, true );
			g_pLTClient->Shutdown();
		}
		else
		{
			g_pLTClient->CPrint( "[FIREGUARD] Start FireGuard Success" );
		}
	}	
#endif

	m_Packets.CS_Inven.m_nIndex			= g_GameDoc.GetUniqueIndex();
	m_Packets.CS_Ready.m_nIndex			= g_GameDoc.GetUniqueIndex();
	m_Packets.CS_ChangeTeam.m_nIndex	= g_GameDoc.GetUniqueIndex();

	{
		g_GameDoc.SetEquipItem(0, ITEM_IDX_CHAR_A);
		g_GameDoc.SetEquipItem(0, ITEM_IDX_CHAR_B);

		g_GameDoc.SetEquipItem(0, ITEM_IDX_WPN_1);
		g_GameDoc.SetEquipItem(0, ITEM_IDX_WPN_2);
		g_GameDoc.SetEquipItem(0, ITEM_IDX_WPN_3);
		g_GameDoc.SetEquipItem(0, ITEM_IDX_WPN_4);

		// Update the player's stats...
		g_pPlayerStats->UpdateAmmo(0, 0, 0);
		g_pPlayerStats->UpdatePlayerWeapon(0, 0);
	}

	g_pGameClientShell->ScreenRender( "Please wait...." );
}

void CNetwork::SC_ChannelListInfo( CPacket* pPacket )
{
	__P_C::SN_Info* pInfo = static_cast< __P_C::SN_Info * >( pPacket );
	
	g_pScreenChannel->SetNickName(LTFALSE);

	if( SCREEN_ID_CHANNEL != GetCurrentScreen() )
	{
		g_pInterfaceMgr->SwitchToScreen( SCREEN_ID_CHANNEL );
	}

	if( SCREEN_ID_CHANNEL == GetCurrentScreen() )
	{
		g_pScreenChannel->SetChannelCount( pInfo->m_byChannelCount );
	}
}

void CNetwork::SC_ChannelInfo( CPacket* pPacket )
{
	__P_C::SN_Enter* pChannel = static_cast< __P_C::SN_Enter * >( pPacket );

	char szChannelName[64];
	if( SCREEN_ID_CHANNEL == GetCurrentScreen() )
	{
		sprintf( szChannelName, "%s-%d", g_GameDoc.GetChannelName(), pChannel->m_byIndex+1 );
		g_pScreenChannel->AddChannel( pChannel->m_byIndex,
									  szChannelName,
									  pChannel->m_nCurrentUser,
									  pChannel->m_nMaxUser );
	}
}

void CNetwork::SC_JoinChannel( CPacket* pPacket )
{
	__P_C::SA_Join* pJoin = static_cast< __P_C::SA_Join * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv JoinChannel: %d", pJoin->m_byResult );

	switch (pJoin->m_byResult)
	{
	case __P_C::SA_Join::E_RESULT::ENUM::SUCCESS:
		g_pGameClientShell->SetDrawLogo(false);
		g_pInterfaceMgr->SwitchToScreen( SCREEN_ID_MAIN );
				
		m_bCheckComplete	= LTTRUE;
		m_nLobbyRoomCount	= pJoin->m_nRoomCount;
		m_nLobbyUserCount	= pJoin->m_nUserCount;
		
		g_pLTClient->CPrint( "[NETWORK] RoomCnt: %d   UserCnt: %d", m_nLobbyRoomCount, m_nLobbyUserCount );

		if (LTFALSE == g_GameDoc.IsAllItemSet())
		{
			if (g_GameDoc.GetEquipCharacter_A() >= g_pModelButeMgr->GetNumModels())
				g_pScreenInventory->SetDefaultCharacter(TEAM_A);
			
			if (g_GameDoc.GetEquipCharacter_B() >= g_pModelButeMgr->GetNumModels())
				g_pScreenInventory->SetDefaultCharacter(TEAM_B);

			if (g_GameDoc.GetEquipWeapon(1) >= g_pWeaponMgr->GetNumWeapons())
				g_pScreenInventory->SetDefaultWeapon(1);

			if (g_GameDoc.GetEquipWeapon(2) >= g_pWeaponMgr->GetNumWeapons())
				g_pScreenInventory->SetDefaultWeapon(2);

			if (g_GameDoc.GetEquipWeapon(3) >= g_pWeaponMgr->GetNumWeapons())
				g_pScreenInventory->SetDefaultWeapon(3);

			if (g_GameDoc.GetEquipWeapon(4) >= g_pWeaponMgr->GetNumWeapons())
				g_pScreenInventory->SetDefaultWeapon(4);
		}

		if (LTFALSE == CheckRecvComplete())
		{
			//g_pInterfaceMgr->ShowMessageBox(SA_TEXT_GET_ROOM_INFO, LTMB_MESSAGE, LTNULL, LTNULL);
		}
		break;

	case __P_C::SA_Join::E_RESULT::ENUM::WRONG_INDEX:
		g_GameDoc.SetChannelIndex( -1 );
		//"Á¸ÀçÇÏÁö ¾Ê´Â Ã¤³Î ÀÔ´Ï´Ù.\r\n\r\n´Ù½Ã ½ÃµµÇØ ÁÖ½Ê½Ã¿À"
		g_pInterfaceMgr->ShowMessageBox(SA_TEXT_HELP_CHANNEL_IS_NOT_EXIST, LTMB_OK, LTNULL, LTNULL);
		break;

	case __P_C::SA_Join::E_RESULT::ENUM::FULL:
		g_GameDoc.SetChannelIndex( -1 );
		//"ÀÔÀå °¡´ÉÇÑ ÀÎ¿øÀ» ÃÊ°ú Çß½À´Ï´Ù.\r\n\r\n´Ù¸¥ Ã¤³Î¿¡ ÀÔÀåÇØ ÁÖ½Ê½Ã¿À"
		g_pInterfaceMgr->ShowMessageBox(SA_TEXT_HELP_CHANNEL_USER_EXCEEDED, LTMB_OK, LTNULL, LTNULL);
		break;
	}
}

void CNetwork::SC_ChannelQuickJoin( CPacket* pPacket )
{
	__P_C::SA_QuickJoin* pQuickJoin = static_cast< __P_C::SA_QuickJoin * >( pPacket );
	
	g_pLTClient->CPrint( "[NETWORK] Recv ChannelQuickJoin" );

	switch( pQuickJoin->m_byResult )
	{
	case __P_C::SA_QuickJoin::E_RESULT::ENUM::SUCCESS:
		g_pScreenMain->ClearScreenMain();
		m_bCheckComplete	= LTTRUE;
		m_nLobbyRoomCount	= pQuickJoin->m_nRoomCount;
		m_nLobbyUserCount	= pQuickJoin->m_nUserCount;
		g_pLTClient->CPrint( "\tRoomCnt: %d  UserCnt: %d", m_nLobbyRoomCount, m_nLobbyUserCount );
		if( LTFALSE == CheckRecvComplete() )
		{
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_GET_ROOM_INFO,
				LTMB_MESSAGE, LTNULL, LTNULL );
		}
		break;

	case __P_C::SA_QuickJoin::E_RESULT::ENUM::FAIL_CHANNEL_FULL:
		//"ÀÔÀå °¡´ÉÇÑ ÀÎ¿øÀ» ÃÊ°ú Çß½À´Ï´Ù.\r\n\r\nÃ¤³Î¿¡ ÀÔÀåÇÒ ¼ö ¾ø½À´Ï´Ù."
		m_nJoinRoomIndex = -1;
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_CHANNEL_USER_EXCEEDED,
										 LTMB_OK, LTNULL, LTNULL );
		break;

	case __P_C::SA_QuickJoin::E_RESULT::ENUM::FAIL_WRONG_CHANNEL:
	default:
		m_nJoinRoomIndex = -1;
		g_pInterfaceMgr->CloseMessageBox(LTTRUE);
		break;
	}
}

void CNetwork::SC_RoomList( CPacket* pPacket )
{
	__P_R::SN_Enter* pRoom = static_cast< __P_R::SN_Enter * >( pPacket );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->AddRoom( pRoom->m_nRoomIndex,
							    pRoom->GetName(),
								pRoom->GetNick(),
								pRoom->m_stOption.GetMapIndex(),
								pRoom->m_byUsers,
								pRoom->m_stOption.GetTeamMax() * 2,
								pRoom->m_stOption.GetMode(),
								pRoom->m_byNetLoss,
								pRoom->m_stOption.IsPlaying(),
								pRoom->m_stOption.IsPassword(),
								pRoom->m_stOption.GetRoundLimit(),
								pRoom->m_stOption.GetKillLimit(),
								pRoom->m_stOption.CanRunnableInBattle(),
								pRoom->m_stOption.IsTeamBalance() );
	}

	--m_nLobbyRoomCount;
	if( m_bCheckComplete ) CheckRecvComplete();
}

void CNetwork::SC_UserList( CPacket* pPacket )
{
	__P_PR::SN_EnterLobby* pPlayer = static_cast< __P_PR::SN_EnterLobby * >( pPacket );
		
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->AddUser( pPlayer->m_nIndex, pPlayer->m_strNick, pPlayer->m_byGrade );		
	}

	--m_nLobbyUserCount;
	if( m_bCheckComplete ) CheckRecvComplete();
}

void CNetwork::SC_CreateRoom( CPacket* pPacket )
{
	__P_R::SA_Create* pCreateRoom = static_cast< __P_R::SA_Create * >( pPacket );
	
	g_pLTClient->CPrint( "[NETWORK] Recv CreateRoom : %d", pCreateRoom->m_byResult );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		T_CREATEROOM_PARAMptr pParam = g_GameDoc.GetCreateRoomParam();

		switch( pCreateRoom->m_byResult )
		{
		case __P_R::SA_Create::E_RESULT::ENUM::SUCCESS:
			g_GameDoc.SetRoomIndex( pCreateRoom->m_nIndex );
			g_GameDoc.SetCaptain( LTTRUE );
			g_GameDoc.SetTeam( pCreateRoom->m_byTeam );
			g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_SCREEN_ROOM);
#ifdef GAME_SHOW
			g_pScreenRoom->ChangeOption( pParam->nGameType,
										 pParam->nMapIndex,
										 pParam->nMaxUsers,
										 pParam->nTimeLimit,
										 pParam->nKillLimit,
										 pParam->nRoundLimit,
										 LTFALSE,
										 LTFALSE,
										 LTFALSE );
#else
			g_pScreenRoom->ChangeOption( pParam->nGameType,
										 pParam->nMapIndex,
										 pParam->nMaxUsers,
										 pParam->nTimeLimit,
										 pParam->nKillLimit,
										 pParam->nRoundLimit,
										 LTTRUE,
										 DEFAULT_TEAM_BALANCE,
										 LTTRUE );
#endif
			g_pScreenRoom->SetRecvUserCount( 0, pCreateRoom->m_byTeam, PLAYER_STATE_READY );
			g_pScreenRoom->EnterBufferRoom( LTFALSE );
			break;
		
		case __P_R::SA_Create::E_RESULT::ENUM::FAIL:
		default:
			// Msg : "¹æ »ý¼º Áß ¿À·ù°¡ ¹ß»ýÇã¿´½À´Ï´Ù."
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ERROR_IN_CREATING_ROOM,
						LTMB_OK, LTNULL, LTNULL );
			break;
		}
	}
}

void CNetwork::SC_JoinRoom( CPacket* pPacket )
{
	__P_R::SA_Join* pJoinRoom = static_cast< __P_R::SA_Join * >( pPacket );
		
	g_pLTClient->CPrint( "[NETWORK] Recv JoinRoom : %d", pJoinRoom->m_byResult );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		switch( pJoinRoom->m_byResult )
		{
		case __P_R::SA_Join::E_RESULT::ENUM::SUCCESS:
			g_pLTClient->CPrint( "\tUserCnt: %d", pJoinRoom->m_byPlayerCount );
			g_GameDoc.SetRoomIndex( pJoinRoom->m_nRoomIndex );
			g_GameDoc.SetCaptain( LTFALSE );
			g_GameDoc.SetTeam( pJoinRoom->m_byTeam );
			g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_SCREEN_ROOM);
			g_pScreenRoom->ChangeOption( pJoinRoom->m_stOption.GetMode(),
										 pJoinRoom->m_stOption.GetMapIndex(),
										 pJoinRoom->m_stOption.GetTeamMax() * 2,
										 pJoinRoom->m_stOption.GetTime(),
										 pJoinRoom->m_stOption.GetKillLimit(),
										 pJoinRoom->m_stOption.GetRoundLimit(),
										 pJoinRoom->m_stOption.CanRunnableInBattle(),
										 pJoinRoom->m_stOption.IsTeamBalance(),
										 pJoinRoom->m_stOption.IsFree() );
			
			g_pScreenRoom->SetRecvUserCount( pJoinRoom->m_byPlayerCount, pJoinRoom->m_byTeam, PLAYER_STATE_BUFF );
			g_pScreenRoom->EnterBufferRoom( pJoinRoom->m_stOption.IsPlaying() );
			break;

		case __P_R::SA_Join::E_RESULT::ENUM::FULL:
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_EXCESSIVE_NUM_PLAYER,
						LTMB_OK, LTNULL, LTNULL );
			break;

		case __P_R::SA_Join::E_RESULT::ENUM::WRONG_PASS:
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_INVALID_PASSWORD,
						LTMB_OK, LTNULL, LTNULL );
			break;

		case __P_R::SA_Join::E_RESULT::ENUM::NOT_EXIST:
			//"Á¸ÀçÇÏÁö ¾Ê´Â ¹æ ÀÔ´Ï´Ù."
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_ROOM_IS_NOT_EXIST,
						LTMB_OK, LTNULL, LTNULL );
			break;
			
		case __P_R::SA_Join::E_RESULT::ENUM::PLAYING:
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_CANNOT_ENTER_PLAYING,
						LTMB_OK, LTNULL, LTNULL );
			break;

		default:
			//"¹æ ÀÔÀå Áß ¿À·ù°¡ ¹ß»ýÇÏ¿´½À´Ï´Ù."
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ERROR_IN_ENTER_ROOM,
						LTMB_OK, LTNULL, LTNULL );
			break;
		}
	}
}

void CNetwork::SC_QuickJoin( CPacket* pPacket )
{
	__P_R::SA_QuickJoin* pQuickJoin = static_cast< __P_R::SA_QuickJoin * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv QuickJoin : %d", pQuickJoin->m_byResult );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		switch( pQuickJoin->m_byResult )
		{
		case __P_R::SA_QuickJoin::E_RESULT::ENUM::SUCCESS:
			g_pLTClient->CPrint( "\tUserCnt: %d", pQuickJoin->m_byPlayerCount );
			g_GameDoc.SetRoomIndex( pQuickJoin->m_nRoomIndex );
			g_GameDoc.SetCaptain( LTFALSE );
			g_GameDoc.SetTeam( pQuickJoin->m_byTeam );
			g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_SCREEN_ROOM);
			g_pScreenRoom->ChangeOption( pQuickJoin->m_stOption.GetMode(),
										 pQuickJoin->m_stOption.GetMapIndex(),
										 pQuickJoin->m_stOption.GetTeamMax() * 2,
										 pQuickJoin->m_stOption.GetTime(),
										 pQuickJoin->m_stOption.GetKillLimit(),
										 pQuickJoin->m_stOption.GetRoundLimit(),
										 pQuickJoin->m_stOption.CanRunnableInBattle(),
										 pQuickJoin->m_stOption.IsTeamBalance(),
										 pQuickJoin->m_stOption.IsFree() );
			g_pScreenRoom->SetRecvUserCount( pQuickJoin->m_byPlayerCount, pQuickJoin->m_byTeam, PLAYER_STATE_BUFF );
			g_pScreenRoom->EnterBufferRoom( pQuickJoin->m_stOption.IsPlaying() );
			break;

		default:
			//"ÀÔÀå °¡´ÉÇÑ ¹æÀÌ ¾ø½À´Ï´Ù."
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NO_AVAILABLE_ROOM,
						LTMB_OK, LTNULL, LTNULL );
			break;
		}
	}
}

void CNetwork::SC_ReturnBufferRoom( CPacket* pPacket )
{
	__P_P::SN_ReturnBuff* pReturnBuff = static_cast< __P_P::SN_ReturnBuff * >( pPacket );
	
	g_pLTClient->CPrint( "[NETWORK] Recv ReturnBufferRoom  UserCnt: %d", pReturnBuff->m_byPlayerCount );
	
	g_pInterfaceMgr->ExitGame();
	
	g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_SCREEN_ROOM);
	g_pScreenRoom->ChangeOption( pReturnBuff->m_stOption.GetMode(),
								 pReturnBuff->m_stOption.GetMapIndex(),
								 pReturnBuff->m_stOption.GetTeamMax() * 2,
								 pReturnBuff->m_stOption.GetTime(),								 
								 pReturnBuff->m_stOption.GetKillLimit(),
								 pReturnBuff->m_stOption.GetRoundLimit(),
								 pReturnBuff->m_stOption.CanRunnableInBattle(),
								 pReturnBuff->m_stOption.IsTeamBalance(),
								 pReturnBuff->m_stOption.IsFree() );
	g_pScreenRoom->SetRecvUserCount( pReturnBuff->m_byPlayerCount, pReturnBuff->m_byTeam, PLAYER_STATE_BUFF );
	g_pScreenRoom->EnterBufferRoom( pReturnBuff->m_stOption.IsPlaying() );
}

void CNetwork::SC_RemoveUser( CPacket* pPacket )
{
	__P_PR::SN_Leave* pRemoveUser = static_cast< __P_PR::SN_Leave * >( pPacket );
	
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->RemoveUser( pRemoveUser->m_nPlayerIndex );
	}
}


void CNetwork::SC_RemoveRoom( CPacket* pPacket )
{
	__P_R::SN_Leave* pRemoveRoom = static_cast< __P_R::SN_Leave * >( pPacket );
	
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->RemoveRoom( pRemoveRoom->m_nRoomIndex );
	}
}

void CNetwork::SC_AddRoomUser( CPacket* pPacket )
{
	__P_PR::SN_Enter* pPlayer = static_cast< __P_PR::SN_Enter * >( pPacket );

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->AddRoomUser( pPlayer->m_nIndex,
									pPlayer->m_strNick,
									(int)pPlayer->m_byGrade,
									(int)pPlayer->m_byTeam,
									(int)pPlayer->m_byState,
									(LTBOOL)pPlayer->m_bCaptain );
	}

	g_pInterfaceMgr->GetPlayerInfoMgr()->AddPlayer( pPlayer->m_nIndex,
													pPlayer->m_strNick,
													pPlayer->m_byGrade,
													pPlayer->m_byTeam,
													pPlayer->m_bCaptain );
}

void CNetwork::SC_RemoveRoomUser( CPacket* pPacket )
{
	__P_PR::SN_Leave* pPlayer = static_cast< __P_PR::SN_Leave * >( pPacket );

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->RemoveRoomUser( pPlayer->m_nPlayerIndex );
	}

	g_pInterfaceMgr->GetPlayerInfoMgr()->RemovePlayer( pPlayer->m_nPlayerIndex );
}

void CNetwork::SC_IncPlayerCount( CPacket* pPacket )
{
	__P_R::SN_AddUser* pInc = static_cast< __P_R::SN_AddUser * >( pPacket );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->IncRoomUser( pInc->m_nRoomIndex );
	}
}

void CNetwork::SC_DecPlayerCount( CPacket* pPacket )
{
	__P_R::SN_RemoveUser* pDec = static_cast< __P_R::SN_RemoveUser * >( pPacket );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->DecRoomUser( pDec->m_nRoomIndex );
	}
}

void CNetwork::SC_ExitGameRoom( CPacket* pPacket )
{
	__P_R::SA_Away* pExitRoom = static_cast< __P_R::SA_Away * >( pPacket );

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_MAIN);

		m_bCheckComplete	= LTTRUE;
		m_nLobbyUserCount = pExitRoom->m_nUserCount;
		m_nLobbyRoomCount = pExitRoom->m_nRoomCount;
		g_pLTClient->CPrint( "[NETWORK] Recv ExitGameRoom RoomCnt: %d  UserCnt:%d", m_nLobbyUserCount, m_nLobbyRoomCount );
		
		if( LTFALSE == CheckRecvComplete() )
		{
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_GET_ROOM_INFO,
						LTMB_MESSAGE, LTNULL, LTNULL );
		}
	}
}

void CNetwork::SC_ChangeCaptain( CPacket* pPacket, LTBOOL bLobby )
{
	if( bLobby )
	{
		if( SCREEN_ID_MAIN == GetCurrentScreen() )
		{
			__P_R::SN_ChangeCaptain* pChangeCaptain = static_cast< __P_R::SN_ChangeCaptain * >( pPacket );
			g_pScreenMain->ChangeCaptain( pChangeCaptain->m_nRoomIndex, pChangeCaptain->m_strNick );
		}
	}
	else
	{
		__P_R::NN_ChangeCaptain* pChangeCaptain = static_cast< __P_R::NN_ChangeCaptain * >( pPacket );
		g_pLTClient->CPrint( "[NETWORK] Recv ChangeCaptain %d", pChangeCaptain->m_nCaptainIndex );

		if( g_GameDoc.GetUniqueIndex() == pChangeCaptain->m_nCaptainIndex )
		{
			g_GameDoc.SetCaptain(LTTRUE);
		}
		else
		{
			g_GameDoc.SetCaptain(LTFALSE);
		}
		
		if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
		{
			g_pScreenRoom->CaptainChange( pChangeCaptain->m_nCaptainIndex );
		}
		
		g_pInterfaceMgr->GetPlayerInfoMgr()->SetCaptain( pChangeCaptain->m_nCaptainIndex );
	}
}

void CNetwork::SC_ChangeOption( CPacket* pPacket, LTBOOL bLobby )
{
	if( bLobby )
	{
		__P_R::SN_ChangeOption* pChangeOption = static_cast< __P_R::SN_ChangeOption * >( pPacket );

		if( SCREEN_ID_MAIN == GetCurrentScreen() )
		{
			g_pScreenMain->ChangeOption( pChangeOption->m_nRoomIndex,
										 pChangeOption->m_stOption.GetMode(),
										 pChangeOption->m_stOption.GetMapIndex(),
										 pChangeOption->m_stOption.GetTeamMax() * 2,
										 pChangeOption->m_stOption.GetTime(),
										 pChangeOption->m_stOption.GetKillLimit(),
										 pChangeOption->m_stOption.GetRoundLimit(),
										 pChangeOption->m_stOption.CanRunnableInBattle(),
										 pChangeOption->m_stOption.IsTeamBalance(),
										 pChangeOption->m_stOption.IsFree() );
		}
	}
	else
	{
		__P_R::NN_ChangeOption* pChangeOption = static_cast< __P_R::NN_ChangeOption * >( pPacket );

		if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
		{
			g_pScreenRoom->ChangeOption( pChangeOption->m_stOption.GetMode(),
										 pChangeOption->m_stOption.GetMapIndex(),
										 pChangeOption->m_stOption.GetTeamMax() * 2,
										 pChangeOption->m_stOption.GetTime(),
										 pChangeOption->m_stOption.GetKillLimit(),
										 pChangeOption->m_stOption.GetRoundLimit(),
										 pChangeOption->m_stOption.CanRunnableInBattle(),
										 pChangeOption->m_stOption.IsTeamBalance(),
										 pChangeOption->m_stOption.IsFree() );
		}
	}
}

void CNetwork::SC_PlayerStateChange( CPacket* pPacket )
{
	__P_P::SN_State* pPlayerState = static_cast< __P_P::SN_State * >( pPacket );
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->SetPlayerState( pPlayerState->m_nIndex, pPlayerState->m_byState, pPlayerState->m_byGrade );
	}
}

void CNetwork::SC_Inven( CPacket* pPacket )
{
	__P_P::NN_Inventory* pInven = static_cast< __P_P::NN_Inventory * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv Inven UserIdx: %d  %d", pInven->m_nIndex, pInven->m_bInventory );
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() || SCREEN_ID_MAIN == GetCurrentScreen())
	{		
		g_pScreenRoom->EnterInven( pInven->m_nIndex, pInven->m_bInventory );
	}
	/*
	else if( SCREEN_ID_INVENTORY == GetCurrentScreen() && pInven->m_nIndex == g_GameDoc.GetUniqueIndex() )
	{
		g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_SCREEN_ROOM);
	}
	*/
}

void CNetwork::SC_ChangeTeam( CPacket* pPacket )
{
	__P_PR::NN_ChangeTeam* pChangeTeam = static_cast< __P_PR::NN_ChangeTeam * >( pPacket );
	g_pInterfaceMgr->GetPlayerInfoMgr()->SetPlayerTeam( pChangeTeam->m_nIndex, pChangeTeam->m_byTeam );

	if( pChangeTeam->m_nIndex == g_GameDoc.GetUniqueIndex() )
	{
		if( g_GameDoc.GetTeam() == pChangeTeam->m_byTeam )
		{
			return;		
		}
		g_GameDoc.SetTeam( pChangeTeam->m_byTeam );
	}	
	
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{	
		g_pScreenRoom->ChangeTeam( pChangeTeam->m_nIndex, pChangeTeam->m_byTeam );	
	}
}

void CNetwork::SC_ChangeTeamOnStart( CPacket* pPacket )
{
	__P_PR::SN_ChangeTeamOnStart* pChangeTeam = static_cast< __P_PR::SN_ChangeTeamOnStart * >( pPacket );

	if( pChangeTeam->m_nIndex == g_GameDoc.GetUniqueIndex() )
	{
		if( g_GameDoc.GetTeam() == pChangeTeam->m_byTeam )
		{
			return;		
		}
		g_GameDoc.SetTeam( pChangeTeam->m_byTeam );
	}

	g_pInterfaceMgr->GetPlayerInfoMgr()->SetPlayerTeam( pChangeTeam->m_nIndex, pChangeTeam->m_byTeam );

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->ChangeTeam( pChangeTeam->m_nIndex, pChangeTeam->m_byTeam );	
	}
}

void CNetwork::SC_ChangeTeamOnGame( CPacket* pPacket )
{
	__P_PR::SN_ChangeTeamOnGame* pChangeTeam = static_cast< __P_PR::SN_ChangeTeamOnGame * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv ChangeTeamOnGame %d  %d", pChangeTeam->m_nIndex,
																   pChangeTeam->m_byTeam );

	if( GS_PLAYING != g_pInterfaceMgr->GetGameState() ) return;

	if( pChangeTeam->m_nIndex == g_GameDoc.GetUniqueIndex() )
	{
		if( g_GameDoc.GetTeam() == pChangeTeam->m_byTeam )
		{
			return;		
		}
		g_GameDoc.SetTeam( pChangeTeam->m_byTeam );
	}

	g_pInterfaceMgr->GetPlayerInfoMgr()->SetPlayerTeamOnGame( pChangeTeam->m_nIndex, pChangeTeam->m_byTeam );

	g_pInterfaceMgr->HandlePlayerTeamChange( pChangeTeam->m_nIndex,
											 pChangeTeam->m_byTeam );
}

void CNetwork::SC_ChangeRoomTitle_L( CPacket* pPacket )
{
	__P_R::SN_ChangeTitle* pChangeTitle = static_cast< __P_R::SN_ChangeTitle * >( pPacket );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->ChangeRoomTitle( pChangeTitle->m_nRoomIndex, pChangeTitle->m_strTitle );
	}
}

void CNetwork::SC_ChangeRoomTitle_R( CPacket* pPacket )
{
	__P_R::NN_ChangeTitle* pChangeTitle = static_cast< __P_R::NN_ChangeTitle * >( pPacket );

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->ChangeRoomTitle( pChangeTitle->m_strTitle );
	}
}

void CNetwork::SC_Ready( CPacket* pPacket )
{
	__P_P::NN_Ready* pReady = static_cast< __P_P::NN_Ready * >( pPacket );

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->SetPlayerReady( pReady->m_nIndex, pReady->m_bReady );	
	}
}

void CNetwork::SC_StartHost( CPacket* pPacket )
{
	m_bSendTimeOut = LTFALSE;

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] StartHost" );

		g_pInterfaceMgr->CloseMessageBox(LTTRUE);
		g_pScreenRoom->StartHost();
		m_bRecvResult	= LTFALSE;
		m_bRecvPausePacket = LTFALSE;

		g_pInterfaceMgr->GetPlayerInfoMgr()->SetPlayerInfoFromServer( g_GameDoc.GetUniqueIndex(),
																	  0, 0,
																	  g_GameDoc.GetUserModelId(0),
																	  g_GameDoc.GetUserModelId(1) );
	}
	else if( m_bRecvPausePacket && GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pLTClient->CPrint( "[NETWORK] MandateHost" );

		m_bRecvPausePacket = LTFALSE;

		ProcessMandate();
	}
}


void CNetwork::SC_StartGame( CPacket* pPacket )
{
	__P_G::SN_Play* pStartGame = static_cast< __P_G::SN_Play * >( pPacket );
	
	m_bSendTimeOut = LTFALSE;
	
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pLTClient->CPrint( "[NETWORK] StartGame IP(%s) Port(%d) Spectator(%d)",
							 pStartGame->m_strIP,
							 pStartGame->m_nPort,
							 pStartGame->m_bSpectator );

		g_pInterfaceMgr->CloseMessageBox(LTTRUE);
		g_pInterfaceMgr->GetPlayerInfoMgr()->SetPlayerInfoFromServer( g_GameDoc.GetUniqueIndex(),
																	  0, 0,
																	  g_GameDoc.GetUserModelId(0),
																	  g_GameDoc.GetUserModelId(1) );
		g_GameDoc.SetHost(LTFALSE);
		
		g_pScreenRoom->StartGameAsClient( pStartGame->m_strIP, pStartGame->m_nPort,
										  pStartGame->m_bSpectator );
		m_bRecvResult	= LTFALSE;
		m_bRecvPausePacket = LTFALSE;
	}
	else if( m_bRecvPausePacket && GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		//¹æÀåÀ§ÀÓ
		g_pLTClient->CPrint( "[NETWORK] ReJoin IP(%s) Port(%d) Spectator(%d)", pStartGame->m_strIP, pStartGame->m_nPort, pStartGame->m_bSpectator );

		m_bRecvPausePacket = LTFALSE;
		
		ProcessRejoin( pStartGame->m_strIP, pStartGame->m_nPort, pStartGame->m_bSpectator );
	}	
}

void CNetwork::ProcessMandate()
{
	//¹æÀåÀ§ÀÓ
	g_GameDoc.BackupCurrentState();	
	ResetMandate();

	g_pGameClientShell->SetSwitchingWorldState( eSwitchingWorldsStateNone );
	if( !g_pClientMultiplayerMgr->SetMultiplayerData( g_pGameClientShell->GetGameType(),
												 "127.0.0.1", 
												 g_pLTClient->GetHostPort() ) )
	{
		CS_StartHostReply(LTFALSE);
		return;
	}
	if( !g_pClientMultiplayerMgr->InitNetworking(false) )
	{
		CS_StartHostReply(LTFALSE);
		return;
	}
	if( !g_pClientMultiplayerMgr->SetupMandateHost() )
	{
		CS_StartHostReply(LTFALSE);
		return;
	}	
	if( !g_pClientMultiplayerMgr->StartClientServer() )
	{
		CS_StartHostReply(LTFALSE);
		return;
	}
	g_pInterfaceMgr->ChangeState(GS_MANDATEHOST);
}

void CNetwork::ProcessRejoin( const char* lpszIP, uint16 nPort, bool bSpectator )
{
	m_bRecvPausePacket = LTFALSE;

	g_GameDoc.BackupCurrentState();

	ResetMandate();

	g_pGameClientShell->SetSwitchingWorldState( eSwitchingWorldsStateNone );
	g_pClientMultiplayerMgr->SetMultiplayerData( g_pGameClientShell->GetGameType(),
												 lpszIP, 
												 nPort );
	g_pClientMultiplayerMgr->InitNetworking(false);
	g_pClientMultiplayerMgr->SetupRejoin( lpszIP,
										  g_GameDoc.GetTitle(),
										  "",
										  bSpectator );
	g_pClientMultiplayerMgr->StartClientServer();
	
	g_pInterfaceMgr->ChangeState(GS_REJOINHOST);
}

void CNetwork::ResetMandate()
{
	g_pGameClientShell->GetDamageFXMgr()->Clear();
	g_pGameClientShell->GetSFXMgr()->RemoveAll();
	g_pGameClientShell->GetClientFXMgr()->ShutdownAllFX();
	g_pInterfaceMgr->GetPlayerInfoMgr()->ResetMandate();
	g_pRadar->Reset();
	g_pNickList->Clear();
}

void CNetwork::SC_FailStartGame( CPacket* pPacket )
{
	__P_P::SN_StartFail* pFailStartGame = static_cast< __P_P::SN_StartFail * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv FailStartGame : %d", pFailStartGame->m_byResult );
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_CANNOT_FIND_HOST_CONDITION, LTMB_OK, LTNULL, LTNULL );

		switch( pFailStartGame->m_byResult )
		{
		case __P_P::SN_StartFail::E_RESULT::ENUM::NO_HOST:
			// "P2P Host¸¦ ÇÒ ¼ö ÀÖ´Â Á¶°ÇÀ» °¡Áø À¯Àú°¡\r\n\r\nÁ¸ÀçÇÏÁö ¾Ê½À´Ï´Ù."
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_CANNOT_FIND_HOST_CONDITION, LTMB_OK, LTNULL, LTNULL );
			break;

		case __P_P::SN_StartFail::E_RESULT::ENUM::ALONE:
			//"»ó´ëÆÀ Áß Àû¾îµµ ÇÑ¸í ÀÌ»óÀÌ ·¹µð¸¦ ÇØ¾ß¸¸\r\n\r\n°ÔÀÓ ½ÃÀÛÀÌ °¡´ÉÇÕ´Ï´Ù."
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_YOU_HAVE_NO_ENEMY,
											 LTMB_OK,
											 LTNULL,
											 LTNULL );
			break;

		default:
			break;
		}
	}
	else if( GS_LOADINGLEVEL == g_pInterfaceMgr->GetGameState() )
	{
		switch( pFailStartGame->m_byResult )
		{
		case __P_P::SN_StartFail::E_RESULT::ENUM::NO_HOST:
			//[TODO] Á¾·áÃ³¸®
			break;
		}
	}
}

void CNetwork::SC_RoomStateChange( CPacket* pPacket, LTBOOL bPlaying )
{
	__P_R::SN_Play* pRoomStateChange = static_cast< __P_R::SN_Play * >( pPacket );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->ChangeRoomState( pRoomStateChange->m_nRoomIndex, bPlaying );
	}
	else if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() &&
			 pRoomStateChange->m_nRoomIndex == g_GameDoc.GetRoomIndex() )
	{
		g_pScreenRoom->ChangeRoomState( bPlaying );
	}
}

void CNetwork::SC_GameResultUser( CPacket* pPacket )
{
	__P_G::SN_ResultPlayer* pGameResultUser = static_cast< __P_G::SN_ResultPlayer * >( pPacket );
	
	g_pLTClient->CPrint( "[NETWORK] Recv GameResultUser %s T:%d G:%d K:%d D:%d E:%d M:%d",
							pGameResultUser->m_strNick, 
							pGameResultUser->m_byTeam,
							pGameResultUser->m_byGrade,
							pGameResultUser->m_nKill,
							pGameResultUser->m_nDead,
							pGameResultUser->m_nExp,
							pGameResultUser->m_nMoney );

	if( SCREEN_ID_NONE == GetCurrentScreen() )
	{
		g_pScreenResult->SetResult_User( pGameResultUser->m_strNick,
										 pGameResultUser->m_byTeam,
										 pGameResultUser->m_byGrade,
										 pGameResultUser->m_nKill,
										 pGameResultUser->m_nDead,
										 pGameResultUser->m_byMission,										 
										 pGameResultUser->m_nExp,
										 pGameResultUser->m_nMoney,
										 0 ); //MiscFlags
	}
}

void CNetwork::SC_GameResultTeam( CPacket* pPacket )
{
	__P_G::SN_ResultTotal* pGameResultTeam = static_cast< __P_G::SN_ResultTotal * >( pPacket );
	
	g_pLTClient->CPrint( "[NETWORK] Recv GameResultTeam : %d", pGameResultTeam->m_byResultCount );

	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pScreenResult->SetResult_Team( pGameResultTeam->m_byWinnerTeam, pGameResultTeam->m_byResultCount );	
	}	
	else if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		//1234
		g_pScreenRoom->EndGame();
	}
}

void CNetwork::SC_EndRound( CPacket* pPacket )
{	
	__P_G::SN_RoundResult* pRoundResult = static_cast< __P_G::SN_RoundResult * >( pPacket );

	int nResult;

	g_pLTClient->CPrint( "[NETWORK] Recv EndRound" );
	if( pRoundResult->m_byWinnerTeam == ::LogicObject::Player::Type::E_TEAM::ENUM::DRAW )
	{
		nResult = GAME_RESULT_DRAW;
	}
	else if( pRoundResult->m_byWinnerTeam == g_GameDoc.GetTeam() )
	{
		nResult = GAME_RESULT_WIN;
	}
	else
	{
		nResult = GAME_RESULT_LOSE;
	}

	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() ||
		GS_MANDATEHOST == g_pInterfaceMgr->GetGameState() ||
		GS_REJOINHOST == g_pInterfaceMgr->GetGameState() )
	{
		m_bRecvResult	= LTFALSE;
		g_pMissionMgr->EndRound(nResult,
								(int)pRoundResult->m_byRedWin,
								(int)pRoundResult->m_byBlueWin,
								!!(pRoundResult->m_byRestRound > 0),
								pRoundResult->m_byType );
		if( 0 >= pRoundResult->m_byRestRound && g_pScreenResult )
		{
			g_pScreenResult->Result_Init();
			g_pScreenResult->SetRoundScore( pRoundResult->m_byRedWin, pRoundResult->m_byBlueWin );
		}
	}
}

void CNetwork::SC_Whisper( CPacket* pPacket )
{
	if( GetConsoleInt( "RejectWhisper", 0 ) > 0 ) return;

	__P_PR::NN_Whisper* pWhisper = static_cast< __P_PR::NN_Whisper * >( pPacket );
	
	char	szWhisper[MAX_TEXT_BUFFER+MAX_ID_LEN+16];
	sprintf( szWhisper, " [From:%s] %s", pWhisper->GetNick(), pWhisper->GetChat() );
	g_GameDoc.SetLastRecvNick( pWhisper->GetNick() );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->AddChat( szWhisper, CHAT_TYPE_WHISPER );
	}
	else if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->AddChat( szWhisper, CHAT_TYPE_WHISPER );
	}
	else if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pChatMsgs->AddMessage( szWhisper, kMsgWhisper );
	}
}

void CNetwork::SC_WhisperFail( CPacket* pPacket )
{
	__P_PR::SN_WhisperFail* pWhisper = static_cast< __P_PR::SN_WhisperFail * >( pPacket );

	char	szWhisper[MAX_TEXT_BUFFER];

	switch( pWhisper->m_byResult )
	{
	case __P_PR::SN_WhisperFail::E_RESULT::ENUM::NOT_EXIST:
		//" [%s]´ÔÀº Á¸ÀçÇÏÁö ¾Ê´Â »ç¿ëÀÚ ÀÌ°Å³ª ¼­¹ö¿¡ Á¢¼ÓÁßÀÌÁö ¾Ê½À´Ï´Ù."
		sprintf( szWhisper, NF_GetMsg(SA_TEXT_HELP_USER_IS_NOT_EXIST), 
							pWhisper->m_strNick );
		break;

	case __P_PR::SN_WhisperFail::E_RESULT::ENUM::INVENTORY:
		//" [%s]´ÔÀº ÇöÀç ÀÎº¥Åä¸®¿¡ ÀÖ¾î¼­ ±Ó¸»À» º¸³¾ ¼ö ¾ø½À´Ï´Ù."
		sprintf( szWhisper, NF_GetMsg(SA_TEXT_HELP_USER_IS_IN_INVENTORY), 
							pWhisper->m_strNick );
		break;
	}

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->AddChat( szWhisper, CHAT_TYPE_WHISPER );
	}
	else if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->AddChat( szWhisper, CHAT_TYPE_WHISPER );
	}
}

void CNetwork::SC_UserInfo( CPacket* pPacket )
{
	__P_PR::SN_Info* pUserInfo = static_cast< __P_PR::SN_Info * >( pPacket );
	
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{	
		g_pScreenMain->UserInfoReply( g_GameDoc.GetUserInfoNick(),
									  g_GameDoc.GetUserInfoGrade(),
									  pUserInfo->m_stResult.m_nKill,
									  pUserInfo->m_stResult.m_nDeath,
									  pUserInfo->m_stResult.m_nWin,
									  pUserInfo->m_stResult.m_nLose,
									  pUserInfo->m_stResult.m_nDraw,
									  pUserInfo->m_stResult.m_llExp,
									  pUserInfo->m_byItemCode[ITEM_IDX_CHAR_A],
									  pUserInfo->m_byItemCode[ITEM_IDX_CHAR_B] );
	}
	else if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->UserInfoReply( g_GameDoc.GetUserInfoNick(),
									  g_GameDoc.GetUserInfoGrade(),
									  pUserInfo->m_stResult.m_nKill,
									  pUserInfo->m_stResult.m_nDeath,
									  pUserInfo->m_stResult.m_nWin,
									  pUserInfo->m_stResult.m_nLose,
									  pUserInfo->m_stResult.m_nDraw,
									  pUserInfo->m_stResult.m_llExp,
									  pUserInfo->m_byItemCode[ITEM_IDX_CHAR_A],
									  pUserInfo->m_byItemCode[ITEM_IDX_CHAR_B] );
	}
}

void CNetwork::SC_MandateHost( CPacket* pPacket )
{
	g_pLTClient->CPrint( "[NETWORK] Recv MandateHost" );
	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		m_bRecvPausePacket = LTTRUE;
		if( g_pPlayerMgr ) g_pPlayerMgr->DisconnectFromServer();
		if( g_pMissionMgr ) g_pMissionMgr->DisconnectFromServer();
	}
}

void CNetwork::DisconnectCallBack( LTBOOL bReturn, void* pData )
{
	g_pLTClient->Shutdown();
}

void CNetwork::SendLogCallBack( LTBOOL bReturn, void* pData )
{
	if( LTTRUE == bReturn )
	{
		g_pLTClient->SetDumpDir( "NetClose" );
		g_pLTClient->SendAddedFiles();
	}

	g_pLTClient->Shutdown();
}

void CNetwork::SC_Chat( CPacket* pPacket )
{
	__P_R::NN_Chat* pChat = static_cast< __P_R::NN_Chat * >( pPacket );	

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->AddChat( pChat->GetMessage(), CHAT_TYPE_NORMAL );
	}
	else if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->AddChat( pChat->GetMessage(), CHAT_TYPE_NORMAL );
	}
}

void CNetwork::SC_GameChat( CPacket* pPacket )
{
	__P_G::NN_Chat* pChat = static_cast< __P_G::NN_Chat * >( pPacket );	

	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		eChatMsgType eType = kMsgChat;

		switch( pChat->m_byType )
		{
		case __P_G::NN_Chat::E_TYPE::ENUM::TEAM:
			eType = kMsgTeam;
			break;
		case __P_G::NN_Chat::E_TYPE::ENUM::GHOST_ALL:
			eType = kMsgGhost;
			break;
		case __P_G::NN_Chat::E_TYPE::ENUM::GHOST_TEAM:
			eType = kMsgGhost;
			break;
		}
		
		g_pChatMsgs->AddMessage( pChat->GetMessage(), eType );
	}	
}


void CNetwork::SC_KickBan( CPacket* pPacket )
{
	__P_PR::SN_Out* pKickBan = static_cast< __P_PR::SN_Out * >( pPacket );	

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pInterfaceMgr->SetExitMessage( NF_GetMsg(SA_TEXT_KICKED_BY_HOST) );
		g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_MAIN);

		m_bCheckComplete	= LTTRUE;
		m_nLobbyUserCount = pKickBan->m_nUserCount;
		m_nLobbyRoomCount = pKickBan->m_nRoomCount;
		if( LTFALSE == CheckRecvComplete() )
		{
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_GET_ROOM_INFO,
						LTMB_MESSAGE, LTNULL, LTNULL );
		}
	}
}

void CNetwork::SC_PlayerInfo( CPacket* pPacket )
{
	__P_G::SN_PlayerInfo* pInfo = static_cast< __P_G::SN_PlayerInfo * >( pPacket );
	
	g_pInterfaceMgr->GetPlayerInfoMgr()->SetPlayerInfoFromServer( pInfo->m_nIndex,
																  pInfo->m_nKill,
																  pInfo->m_nDead,
																  pInfo->m_byItemCode[ITEM_IDX_CHAR_A],
																  pInfo->m_byItemCode[ITEM_IDX_CHAR_B] );
}

void CNetwork::SC_RemovePlayer( CPacket* pPacket )
{
	__P_G::SN_Exit* pRemovePlayer = static_cast< __P_G::SN_Exit * >( pPacket );

	g_pInterfaceMgr->GetPlayerInfoMgr()->LeaveGame( pRemovePlayer->m_nPlayerIndex );
}

void CNetwork::SC_DeathMatchInfo( CPacket* pPacket )
{
	__P_G::SN_RespawnInfo* pInfo = static_cast< __P_G::SN_RespawnInfo * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv DeathMatchInfo k: %d  D: %d  K: %d  D: %d",
						pInfo->m_nRedKill, pInfo->m_nRedDead, pInfo->m_nBlueKill, pInfo->m_nBlueDead );

	g_pScores->SetKillDeath( pInfo->m_nRedKill, pInfo->m_nBlueKill );
}

void CNetwork::SC_TimeBombMissionInfo( CPacket* pPacket )
{
	__P_G::SN_BombInfo* pInfo = static_cast< __P_G::SN_BombInfo * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv TimeBombMissionInfo Round: %d  R: %d  B: %d",
						pInfo->m_byRound, pInfo->m_byRedWin, pInfo->m_byBlueWin );

	g_pMissionMgr->SetRoundScore( pInfo->m_byRedWin, pInfo->m_byBlueWin, pInfo->m_byRound );
}

void CNetwork::SC_Kill( CPacket* pPacket )
{
	__P_G::NN_Kill* pKill = static_cast< __P_G::NN_Kill * >( pPacket );
	
	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pInterfaceMgr->HandlePlayerScored( pKill->m_nKiller, pKill->m_nVictim,
											pKill->m_bHeadShot, (uint8)pKill->m_nModelID );
	}
}

void CNetwork::SC_Suicide( CPacket* pPacket )
{
	__P_G::NN_Suicide* pSuicide = static_cast< __P_G::NN_Suicide * >( pPacket );

	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pInterfaceMgr->HandlePlayerScored( pSuicide->m_nVictim, pSuicide->m_nVictim,
											false, (uint8)pSuicide->m_nModelID );
	}
}

void CNetwork::SC_Radio( CPacket* pPacket )
{
	__P_G::NN_Radio* pRadio = static_cast< __P_G::NN_Radio * >( pPacket );

	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pInterfaceMgr->HandlePlayerRadio( pRadio->m_nPlayerIndex, pRadio->m_nRadioID, pRadio->m_nTeamID );
	}
}

void CNetwork::SC_WeaponChange( CPacket* pPacket )
{
	__P_G::NN_WeaponChange* pWeaponChange = static_cast< __P_G::NN_WeaponChange * >( pPacket );

	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pGameClientShell->HandleWeaponChange( pWeaponChange->m_nPlayerIndex,
												pWeaponChange->m_nWeaponID,
												pWeaponChange->m_nAmmoCount,
												pWeaponChange->m_nAmmoInClip );
	}
}

void CNetwork::SC_NetLossPlayer( CPacket* pPacket )
{
	__P_G::SN_NetLossPlayer* pNetLoss = static_cast< __P_G::SN_NetLossPlayer * >( pPacket );

	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pInterfaceMgr->GetPlayerInfoMgr()->SetPlayerPing( pNetLoss->m_nPlayerIndex, pNetLoss->m_byNetLoss );
	}
}

void CNetwork::SC_NetLossRoom( CPacket* pPacket )
{
	__P_G::SN_NetLossRoom* pNetLoss = static_cast< __P_G::SN_NetLossRoom * >( pPacket );

	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->SetPing( pNetLoss->m_nRoomIndex, pNetLoss->m_byNetLoss );
	}
}

void CNetwork::SC_SetBombState( CPacket* pPacket )
{
	__P_G::SN_Bomb* pBomb = static_cast< __P_G::SN_Bomb * >( pPacket );	

	g_pLTClient->CPrint( "[NETWORK] Recv SetBombState %d", pBomb->m_byType );
	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pMissionMgr->SetBombState( pBomb->m_byType );
	}
}

void CNetwork::SC_PlantedBomb( CPacket* pPacket )
{
	__P_G::SN_BombPlanted* pPlantedBomb = static_cast< __P_G::SN_BombPlanted * >( pPacket );	

	g_pLTClient->CPrint( "[NETWORK] Recv PlantedBomb" );
	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pMissionMgr->PlantedBomb( pPlantedBomb->m_fDetonateTimer,
									pPlantedBomb->m_fTimeBombPosX,
									pPlantedBomb->m_fTimeBombPosY,
									pPlantedBomb->m_fTimeBombPosZ );
	}
}

void CNetwork::SC_SearchUser( CPacket* pPacket )
{
	char szMsg[256];
	int nChannelIndex	= -1;
	int nRoomIndex		= -1;

	__P_PR::SA_Find* pSearchUser = static_cast< __P_PR::SA_Find * >( pPacket );
	
	switch( pSearchUser->m_byResult )
	{
	case __P_PR::SA_Find::E_RESULT::ENUM::FAIL:
		//"[ %s ]´ÔÀ» Ã£À» ¼ö ¾ø½À´Ï´Ù."
		sprintf( szMsg, NF_GetMsg(SA_TEXT_SEARCH_USER_RESULT_NONE),
						g_GameDoc.GetSearchUser() );
		break;
	case __P_PR::SA_Find::E_RESULT::ENUM::CHANNEL:
		//"[%s]´ÔÀº\r\n\r\nÇöÀç Ã¤³Î ¼±ÅÃÁß ÀÔ´Ï´Ù."
		sprintf( szMsg, NF_GetMsg(SA_TEXT_SEARCH_USER_RESULT_CHOOSING_CHANNEL), 
						g_GameDoc.GetSearchUser() );
		break;
	case __P_PR::SA_Find::E_RESULT::ENUM::LOBBY:
		if( g_GameDoc.GetChannelIndex() == pSearchUser->m_byChannelIndex )
		{
			//"[ %s ]´ÔÀº\r\n\r\nÇöÀç ´ë±â½Ç¿¡ ÀÖ½À´Ï´Ù."		
			sprintf( szMsg, NF_GetMsg(SA_TEXT_SEARCH_USER_RESULT_LOBBY),
							g_GameDoc.GetSearchUser() );
		}
		else
		{
			//"[%s]´ÔÀº\r\n\r\n%d¹ø Ã¤³Î ´ë±â½Ç¿¡ ÀÖ½À´Ï´Ù."
			sprintf( szMsg, NF_GetMsg(SA_TEXT_SEARCH_USER_RESULT_CHANNEL), 
							g_GameDoc.GetSearchUser(),
							pSearchUser->m_byChannelIndex+1 );
			nChannelIndex = pSearchUser->m_byChannelIndex;
		}

		break;
	case __P_PR::SA_Find::E_RESULT::ENUM::ROOM:
		if( g_GameDoc.GetChannelIndex() == pSearchUser->m_byChannelIndex )
		{
			//"[ %s ]´ÔÀº\r\n\r\nÇöÀç %d¹ø ¹æ¿¡ ÀÖ½À´Ï´Ù."
			sprintf( szMsg, NF_GetMsg(SA_TEXT_SEARCH_USER_RESULT_ROOM),
							g_GameDoc.GetSearchUser(),
							pSearchUser->m_nRoomIndex ); 
		}
		else
		{
			//"[%s]´ÔÀº\r\n\r\n%d¹ø Ã¤³Î %d¹ø ¹æ¿¡ ÀÖ½À´Ï´Ù."
			sprintf( szMsg, NF_GetMsg(SA_TEXT_SEARCH_USER_RESULT_CHANNEL_ROOM),
							g_GameDoc.GetSearchUser(),
							pSearchUser->m_byChannelIndex+1,
							pSearchUser->m_nRoomIndex );
			nChannelIndex = pSearchUser->m_byChannelIndex;
		}
		nRoomIndex = pSearchUser->m_nRoomIndex;
		break;
	case __P_PR::SA_Find::E_RESULT::ENUM::PLAY:
		if( g_GameDoc.GetChannelIndex() == pSearchUser->m_byChannelIndex )
		{
			//"[%s]´ÔÀº\r\n\r\nÇöÀç %d¹ø¹æ¿¡¼­ °ÔÀÓ Áß ÀÔ´Ï´Ù."
			sprintf( szMsg, NF_GetMsg(SA_TEXT_SEARCH_RESULT_PLAYING_ROOM),
							g_GameDoc.GetSearchUser(),
							pSearchUser->m_nRoomIndex );
		}
		else
		{
			//"[%s]´ÔÀº\r\n\r\n%d¹ø Ã¤³Î %d¹ø¹æ¿¡¼­ °ÔÀÓ Áß ÀÔ´Ï´Ù."
			sprintf( szMsg, NF_GetMsg(SA_TEXT_SEARCH_RESULT_PLAYING_CHANNEL_ROOM),
							g_GameDoc.GetSearchUser(),
							pSearchUser->m_byChannelIndex+1,
							pSearchUser->m_nRoomIndex );
			nChannelIndex = pSearchUser->m_byChannelIndex;
		}
		nRoomIndex = pSearchUser->m_nRoomIndex;
		break;
	}
	
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->SearchResult( pSearchUser->m_byResult==__P_PR::SA_Find::E_RESULT::ENUM::FAIL ? LTFALSE:LTTRUE,
									 szMsg, nChannelIndex, nRoomIndex );
	}
	else if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->SearchResult( pSearchUser->m_byResult==__P_PR::SA_Find::E_RESULT::ENUM::FAIL ? LTFALSE:LTTRUE,
									 szMsg );
	}
}

void CNetwork::SC_ManagerNotify( CPacket* pPacket )
{
	__P_M::SN_Noti* pNoti = static_cast< __P_M::SN_Noti * >( pPacket );

	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		g_pHelpMsgs->AddMessage( pNoti->GetNoti(), 12, kMsgNotice, 10.0f );
	}
	else if( g_pScreenMain && SCREEN_ID_MAIN == g_pInterfaceMgr->GetCurrentScreen() )
	{
		g_pScreenMain->AddNotify( pNoti->GetNoti() );
	}
	else if( g_pScreenRoom && SCREEN_ID_SCREEN_ROOM == g_pInterfaceMgr->GetCurrentScreen() )
	{
		g_pScreenRoom->AddNotify( pNoti->GetNoti() );
	}
}

void CNetwork::SC_JoinPlayFail( CPacket* pPacket )
{
	g_pLTClient->CPrint( "[NETWORK] Recv JoinPlayFail" );
	g_pInterfaceMgr->ShowMessageBox( SA_TEXT_IN_NAT,
									 LTMB_OK, LTNULL, LTNULL );
}

void CNetwork::SC_PlayReady( CPacket* pPacket )
{
	g_pLTClient->CPrint( "[NETWORK] Recv PlayReady" );
	g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ENTER_GAME,
									 LTMB_MESSAGE, LTNULL, LTNULL );
}

void CNetwork::SC_RefreshUserInfo( CPacket* pPacket )
{
	__P_PR::SN_Result* pResult = static_cast< __P_PR::SN_Result * >( pPacket );

	g_GameDoc.SetGrade( pResult->m_byGrade );
	g_GameDoc.SetExp( pResult->m_stResult.m_llExp );
	g_GameDoc.SetMoney( pResult->m_stResult.m_llMoney );
	g_GameDoc.SetKill( pResult->m_stResult.m_nKill );
	g_GameDoc.SetDeath( pResult->m_stResult.m_nDeath );
	g_GameDoc.SetWinCount( pResult->m_stResult.m_nWin );
	g_GameDoc.SetLoseCount( pResult->m_stResult.m_nLose );
	g_GameDoc.SetDrawCount( pResult->m_stResult.m_nDraw );

	g_pLTClient->CPrint( "[NETWORK] Recv RefreshUserInfo" );
	g_pLTClient->CPrint( "\tGrade: %d", pResult->m_byGrade );
	g_pLTClient->CPrint( "\tExp: %15.0f", (double)pResult->m_stResult.m_llExp );
	g_pLTClient->CPrint( "\tMoney: %15.0f", (double)pResult->m_stResult.m_llMoney );
	g_pLTClient->CPrint( "\tKill: %d", pResult->m_stResult.m_nKill );
	g_pLTClient->CPrint( "\tDeath: %d", pResult->m_stResult.m_nDeath );	
	g_pLTClient->CPrint( "\tWin: %d", pResult->m_stResult.m_nWin );
	g_pLTClient->CPrint( "\tLose: %d", pResult->m_stResult.m_nLose );
	g_pLTClient->CPrint( "\tDraw: %d", pResult->m_stResult.m_nDraw );
}

void CNetwork::SC_Ranking( CPacket* pPacket )
{
	__P_L::SN_Ranking* pRanking = static_cast< __P_L::SN_Ranking * >( pPacket );

	LTBOOL bFirst = !g_GameDoc.IsRankingInfo();
	g_GameDoc.SetRanking( pRanking->m_strText );

	if( bFirst )
	{
		if( g_pScreenChannel && SCREEN_ID_CHANNEL == g_pInterfaceMgr->GetCurrentScreen() )
		{
			g_pScreenChannel->SetRanking();
		}
		else if( g_pScreenMain && SCREEN_ID_MAIN == g_pInterfaceMgr->GetCurrentScreen() )
		{
			g_pScreenMain->SetRanking();
		}
		else if( g_pScreenRoom && SCREEN_ID_SCREEN_ROOM == g_pInterfaceMgr->GetCurrentScreen() )
		{
			g_pScreenRoom->SetRanking();
		}
		else if( g_pScreenInventory && SCREEN_ID_INVENTORY == g_pInterfaceMgr->GetCurrentScreen() )
		{
			g_pScreenInventory->SetRanking();
		}
	}
}

void CNetwork::SC_ChangeCaptainResult( CPacket* pPacket )
{
	__P_R::SA_ChangeCaptain* pResult = static_cast< __P_R::SA_ChangeCaptain * >( pPacket );

	switch( pResult->m_byResult )
	{
	case __P_R::SA_ChangeCaptain::E_RESULT::ENUM::SUCCESS:
		break;
	case __P_R::SA_ChangeCaptain::E_RESULT::ENUM::FAIL_NOT_CAPTAIN:
		break;
	case __P_R::SA_ChangeCaptain::E_RESULT::ENUM::FAIL_NOT_EXIST:
		break;
	case __P_R::SA_ChangeCaptain::E_RESULT::ENUM::FAIL_CAPTAIN_INDEX:
		break;
	}
}

void CNetwork::SC_LobbyUserInfo( CPacket* pPacket )
{
	__P_P::SA_ViewLobby* pLobbyUserInfo = static_cast< __P_P::SA_ViewLobby * >( pPacket );

	if( __P_P::SA_ViewLobby::E_RESULT::ENUM::SUCCESS == pLobbyUserInfo->m_byResult &&
		SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->SetLobbyUserCount( pLobbyUserInfo->m_nCount );
	}
}

void CNetwork::SC_LobbyUser( CPacket* pPacket )
{
	__P_P::SN_LobbyPlayer* pLobbyUser = static_cast< __P_P::SN_LobbyPlayer * >( pPacket );

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() )
	{
		g_pScreenRoom->AddLobbyUser( pLobbyUser->m_strNick, pLobbyUser->m_byGrade );
	}
}

void CNetwork::SC_Invite( CPacket* pPacket )
{
	if( GetConsoleInt( "RejectInvite", 0 ) > 0 ) return;

	__P_P::SN_Invite* pInvite = static_cast< __P_P::SN_Invite * >( pPacket );

	if( g_pScreenMain && SCREEN_ID_MAIN == g_pInterfaceMgr->GetCurrentScreen() )
	{
		g_pScreenMain->ShowInviteMsg( pInvite->GetNick(), pInvite->m_nRoomIndex, pInvite->GetPass() );
	}
}

void CNetwork::SC_SpeedHackDetected( CPacket* pPacket )
{
	__P_G::SN_Speed* pSpeed = static_cast< __P_G::SN_Speed * >( pPacket );

	if( GS_PLAYING == g_pInterfaceMgr->GetGameState() )
	{
		if( g_GameDoc.GetUniqueIndex() == pSpeed->m_nPlayerIndex )
		{
			g_pInterfaceMgr->SetExitMessage( NF_GetMsg(SA_TEXT_HACK_DETECTED) );
		}
		else
		{
			T_PLAYER_INFOptr pInfo = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByServerIndex(pSpeed->m_nPlayerIndex);

			if( pInfo )
			{
				char szTemp[256];				
				sprintf( szTemp, NF_GetMsg(SA_TEXT_HACK_KICKED),
								 pInfo->szNickName );
				g_pChatMsgs->AddMessage( szTemp, kMsgNotice );
			}
		}
	}
	g_pLTClient->CPrint( "Speed Hack Detected!!! ------------ %d", pSpeed->m_nPlayerIndex );
}

void CNetwork::SC_DbgRoomInfo( CPacket* pPacket )
{
	__P_R::SA_Info* pRoomInfo = static_cast< __P_R::SA_Info * >( pPacket );
	g_pLTClient->CPrint( "[DEBUG INFO] -----------------------------------------------------" );
	g_pLTClient->CPrint( "Planted: %d", pRoomInfo->m_bPlanted );
	g_pLTClient->CPrint( "Player Cnt: %d", pRoomInfo->m_byPlayerCount );
}

void CNetwork::SC_DbgPlayerInfo( CPacket* pPacket )
{
	__P_R::SN_PlayerInfo* pPlayerInfo = static_cast< __P_R::SN_PlayerInfo * >( pPacket );

	g_pLTClient->CPrint( "Player_Info" );
	g_pLTClient->CPrint( "\tTeam: %d", pPlayerInfo->m_byTeam );
	g_pLTClient->CPrint( "\tBuff: %d", pPlayerInfo->m_bBuff );
	g_pLTClient->CPrint( "\tLoad: %d", pPlayerInfo->m_bLoading );
	g_pLTClient->CPrint( "\tDead: %d", pPlayerInfo->m_bDead );
	g_pLTClient->CPrint( "\tSpec: %d", pPlayerInfo->m_bSpectator );
	g_pLTClient->CPrint( "\tNick: %s", pPlayerInfo->m_strNick );
}

void CNetwork::SC_MyInvenInfo( CPacket* pPacket )
{
	__P_I::SN_Inven* pInvenInfo = static_cast< __P_I::SN_Inven * >( pPacket );
	
	g_pLTClient->CPrint( "[NETWORK] Recv MyInvenInfo : %d  %04d-%02d-%02d %02d:%02d",
						 pInvenInfo->m_nCount,
						 pInvenInfo->m_stServerTime.GetYear(),
						 pInvenInfo->m_stServerTime.GetMonth(),
						 pInvenInfo->m_stServerTime.GetDay(),
						 pInvenInfo->m_stServerTime.GetHour(),
						 pInvenInfo->m_byServerMin );

	sprintf( g_szStartTime, "%04d%02d%02d", 
						pInvenInfo->m_stServerTime.GetYear(),
						pInvenInfo->m_stServerTime.GetMonth(),
						pInvenInfo->m_stServerTime.GetDay() );
	g_pLTClient->SetDumpDir( g_szStartTime );

	sprintf( g_szStartTime, "%02d%02d", 
							pInvenInfo->m_stServerTime.GetHour(),
							pInvenInfo->m_byServerMin );

	g_pGameClientShell->ScreenRender( "Please wait.." );
}

void CNetwork::SC_MyInvenItem( CPacket* pPacket )
{
	__P_I::SN_Item* pInvenItem = static_cast< __P_I::SN_Item * >( pPacket );

	//AddItemToInven
	AddInvenItem( pInvenItem->m_stItem.m_cClass1,
				  pInvenItem->m_stItem.m_stClass2[0],
				  pInvenItem->m_stItem.m_stClass2[1],
				  pInvenItem->m_stItem.m_byCode,
				  pInvenItem->m_stExpireTime.GetYear(),
				  pInvenItem->m_stExpireTime.GetMonth(),
				  pInvenItem->m_stExpireTime.GetDay(),
				  pInvenItem->m_stExpireTime.GetHour() );
}

void CNetwork::AddInvenItem( char cClass1, char cClass2_1, char cClass2_2, int nItemIndex,
							 int nY, int nM, int nD, int nH )
{
#ifndef _FINAL
#ifdef DEBUG_SHOP
	return;
#endif
#endif
	
	g_pLTClient->CPrint( "[NETWORK] AddInvenItem :  %c|%c%c Idx(%d)  %4d-%02d-%02d %02d",
						 cClass1, cClass2_1, cClass2_2, nItemIndex, nY, nM, nD, nH );
	
	if( IsWeapon( cClass1 ) )
	{
		sShopWeapon_Set* pWeapon = g_pScreenInventory->GetShopWeapon(nItemIndex);
		sInven_Set* pSet	= new sInven_Set;
		pSet->bBuyItem		= pWeapon->bBuyItem;
		pSet->bDefaultItem	= pWeapon->bDefaultItem;
		pSet->nWeaponId		= nItemIndex;
		pSet->szClass1[0]	= cClass1;
		pSet->szClass1[1]	= LTNULL;
		pSet->szClass2[0]	= cClass2_1;
		pSet->szClass2[1]	= cClass2_2;
		pSet->szClass2[2]	= LTNULL;
		pSet->nClassNum		= GetWeaponClass(pSet->szClass2);
		if( pSet->bDefaultItem )
		{
			//[TODO] Msg
			sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: Á¦ÇÑ¾øÀ½" );
		}
		else
		{
			if( nH > 12 )
			{
				sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: %d³â%d¿ù%dÀÏ ¿ÀÈÄ%d½Ã", nY, nM, nD, nH-12 );
			}
			else if( nH == 12 )
			{
				sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: %d³â%d¿ù%dÀÏ ³·%d½Ã", nY, nM, nD, nH );
			}
			else if( nH == 0 )
			{
				sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: %d³â%d¿ù%dÀÏ ¹ã12½Ã", nY, nM, nD );
			}
			else
			{
				sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: %d³â%d¿ù%dÀÏ ¿ÀÀü%d½Ã", nY, nM, nD, nH );
			}
		}
		g_pScreenInventory->AddWeapon( pSet );
	}
	else if( IsCharacter( cClass1 ) )
	{
		sShopChar_Set* pCharacter = g_pScreenInventory->GetShopCharTab(nItemIndex,ITEM_CHAR_SLOT);
		sChar_Set* pSet		= new sChar_Set;
		pSet->bBuyItem		= pCharacter->bBuyItem;
		pSet->bDefaultItem	= pCharacter->bDefaultItem;
		pSet->nClassNum		= ITEM_CHAR_SLOT;
		pSet->nItemId		= nItemIndex;
		pSet->szClass1[0]	= cClass1;
		pSet->szClass1[1]	= LTNULL;
		pSet->szClass2[0]	= cClass2_1;
		pSet->szClass2[1]	= cClass2_2;
		pSet->szClass2[2]	= LTNULL;
		if( pSet->bDefaultItem )
		{
			//[TODO] Msg
			sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: Á¦ÇÑ¾øÀ½" );
		}
		else
		{
			if( nH > 12 )
			{
				sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: %d³â%d¿ù%dÀÏ ¿ÀÈÄ%d½Ã", nY, nM, nD, nH-12 );
			}
			else if( nH == 12 )
			{
				sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: %d³â%d¿ù%dÀÏ ³·%d½Ã", nY, nM, nD, nH );
			}
			else if( nH == 0 )
			{
				sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: %d³â%d¿ù%dÀÏ ¹ã12½Ã", nY, nM, nD );
			}
			else
			{
				sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: %d³â%d¿ù%dÀÏ ¿ÀÀü%d½Ã", nY, nM, nD, nH );
			}
		}
		g_pScreenInventory->AddCharTab( pSet );
	}

	g_pScreenInventory->ItemBuy();
}

void CNetwork::SC_ShopItemList( CPacket* pPacket )
{
	__P_I::SN_Shop* pShop = static_cast< __P_I::SN_Shop * >( pPacket );
	
	g_pScreenInventory->RemoveAllShop();
	ParseItemList( PARSE_ITEM_SHOP, pShop->m_strTable );
}

void CNetwork::SC_EnterShop( CPacket* pPacket )
{
	__P_I::SN_EnterShop* pEnterShop = static_cast< __P_I::SN_EnterShop * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv EnterShop" );
	if( SCREEN_ID_MAIN == GetCurrentScreen() )
	{
		g_pScreenMain->EnterShop();
	}
}

void CNetwork::SC_ExitShop( CPacket* pPacket )
{
	__P_I::SN_ExitShop* pExitShop = static_cast< __P_I::SN_ExitShop * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv ExitShop  Result(%d)  Money(%.0f)",
						 pExitShop->m_byResult, (double)pExitShop->m_llRestMoney );

	switch( pExitShop->m_byResult )
	{
	case __P_I::SN_ExitShop::E_RESULT::ENUM::SUCCESS:
		g_GameDoc.SetMoney(pExitShop->m_llRestMoney);
		g_pScreenInventory->ExitShop();
		break;
	
	case __P_I::SN_ExitShop::E_RESULT::ENUM::FAIL_NOT_EXIST:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_NOT_EXIST, LTMB_OK, LTNULL, LTNULL );
		break;

	case __P_I::SN_ExitShop::E_RESULT::ENUM::FAIL_DEFICIT:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_FAIL_DEFICIT, LTMB_OK, LTNULL, LTNULL );
		break;

	case __P_I::SN_ExitShop::E_RESULT::ENUM::FAIL_DB:
	default:
		{
			char szTemp[256];
			sprintf( szTemp, NF_GetMsg(SA_TEXT_ITEM_EXIT_SHOP_ERR),
				pExitShop->m_byResult );
			g_pInterfaceMgr->ShowMessageBox( szTemp, LTMB_OK, LTNULL, LTNULL );
		}	
		break;
	}
}

void CNetwork::SC_BuyItem( CPacket* pPacket )
{
#ifndef _FINAL
#ifdef DEBUG_SHOP
	return;
#endif
#endif

	__P_I::SA_Buy* pBuyItem = static_cast< __P_I::SA_Buy * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv BuyItem  Result(%d)  Money(%.0f)",
						 pBuyItem->m_byResult, (double)pBuyItem->m_llRestMoney );

	g_GameDoc.SetMoney(pBuyItem->m_llRestMoney);
	g_pScreenInventory->SetGameMoney(pBuyItem->m_llRestMoney);
	
	switch( pBuyItem->m_byResult )
	{
	case __P_I::SA_Buy::E_RESULT::ENUM::SUCCESS:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_BUY_SUCCESS,
										 LTMB_OK, LTNULL, LTNULL );
		break;

	case __P_I::SA_Buy::E_RESULT::ENUM::FAIL_NOT_EXIST:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_NOT_EXIST,
										 LTMB_OK, LTNULL, LTNULL );
		break;

	case __P_I::SA_Buy::E_RESULT::ENUM::FAIL_OWN:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_FAIL_OWN,
										 LTMB_OK, LTNULL, LTNULL );
		break;
		
	case __P_I::SA_Buy::E_RESULT::ENUM::FAIL_DEFICIT:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_FAIL_DEFICIT,
										 LTMB_OK, LTNULL, LTNULL );
		break;
		
	case __P_I::SA_Buy::E_RESULT::ENUM::FAIL_DB1:
	case __P_I::SA_Buy::E_RESULT::ENUM::FAIL_DB2:
	case __P_I::SA_Buy::E_RESULT::ENUM::FAIL_DB3:
	default:
		{
			char szTemp[256];
			sprintf( szTemp, NF_GetMsg(SA_TEXT_ITEM_BUY_ERR),
						pBuyItem->m_byResult );
			g_pInterfaceMgr->ShowMessageBox( szTemp, LTMB_OK, LTNULL, LTNULL );
		}
		break;
	}
}

void CNetwork::SC_DiscardItem( CPacket* pPacket )
{
#ifndef _FINAL
#ifdef DEBUG_SHOP
	return;
#endif
#endif
	
	__P_I::SA_Discard* pDiscardItem = static_cast< __P_I::SA_Discard * >( pPacket );
	
	g_pLTClient->CPrint( "[NETWORK] Recv DiscardItem  Result(%d)", pDiscardItem->m_byResult );
	switch( pDiscardItem->m_byResult )
	{
	case __P_I::SA_Discard::E_RESULT::ENUM::SUCCESS:
		if( m_nDiscardItemIndex < 0 || m_cDiscardItemClass == LTNULL )
		{
			g_pLTClient->CPrint( "[NETWORK] ERROR m_nDiscardItemIndex < 0" );
			return;
		}

		if( IsWeapon( m_cDiscardItemClass ) )
		{
			g_pScreenInventory->WeaponThrow(m_nDiscardItemIndex);
		}
		else if( IsCharacter( m_cDiscardItemClass ) )
		{
			g_pScreenInventory->CharacterThrow(m_nDiscardItemIndex);
		}
		break;

	case __P_I::SA_Discard::E_RESULT::ENUM::FAIL_NOT_EXIST:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_NOT_EXIST,
										 LTMB_OK, LTNULL, LTNULL );
		break;

	case __P_I::SA_Discard::E_RESULT::ENUM::FAIL_DB1:
	case __P_I::SA_Discard::E_RESULT::ENUM::FAIL_DB2:
	default:
		{
			char szTemp[256];
			sprintf( szTemp, NF_GetMsg(SA_TEXT_ITEM_DISCARD_ERR),
						pDiscardItem->m_byResult );
			g_pInterfaceMgr->ShowMessageBox( szTemp, LTMB_OK, LTNULL, LTNULL );
		}
		break;
	}

	m_cDiscardItemClass = LTNULL;
	m_nDiscardItemIndex = -1;
}

void CNetwork::SC_AddItem( CPacket* pPacket )
{
#ifndef _FINAL
#ifdef DEBUG_SHOP
	return;
#endif
#endif
	
	__P_I::SN_AddItem* pAddItem = static_cast< __P_I::SN_AddItem * >( pPacket );

	AddInvenItem( pAddItem->m_stItem.m_cClass1,
				  pAddItem->m_stItem.m_stClass2[0],
				  pAddItem->m_stItem.m_stClass2[1],
				  pAddItem->m_stItem.m_byCode,
				  pAddItem->m_stExpireTime.GetYear(),
				  pAddItem->m_stExpireTime.GetMonth(),
				  pAddItem->m_stExpireTime.GetDay(),
				  pAddItem->m_stExpireTime.GetHour() );
}

void CNetwork::SC_RemoveItem( CPacket* pPacket )
{
#ifndef _FINAL
#ifdef DEBUG_SHOP
	return;
#endif
#endif

	__P_I::SN_RemoveItem* pRemoveItem = static_cast< __P_I::SN_RemoveItem * >( pPacket );

	RemoveInvenItem( pRemoveItem->m_stItem.m_cClass1,
					 pRemoveItem->m_stItem.m_stClass2[0],
					 pRemoveItem->m_stItem.m_stClass2[1],
					 pRemoveItem->m_stItem.m_byCode );
}

void CNetwork::RemoveInvenItem( char cClass1, char cClass2_1, char cClass2_2, int nItemIndex )
{
#ifndef _FINAL
#ifdef DEBUG_SHOP
	return;
#endif
#endif

	g_pLTClient->CPrint( "[NETWORK] RemoveInvenItem :  %c|%c%c Idx(%d)",
						 cClass1, cClass2_1, cClass2_2, nItemIndex );

	if( IsWeapon( cClass1 ) )
	{
		g_pScreenInventory->RemoveWeapon( nItemIndex );
	}
	else if( IsCharacter( cClass1 ) )
	{
		g_pScreenInventory->RemoveCharTab( nItemIndex, ITEM_CHAR_SLOT );
	}
}

void CNetwork::SC_Equipment( CPacket* pPacket )
{
	__P_I::SN_Equipment* pEquipment = static_cast< __P_I::SN_Equipment * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv Equipments(%d)\n"
						"\t1: %d\n"
						"\t2: %d\n"
						"\t3: %d\n"
						"\t4: %d\n"
						"\t5: %d\n"
						"\t6: %d\n"
						"\t7: %d\n"
						"\t8: %d\n"
						"\t9: %d\n"
						"\t10: %d\n"
						"\t11: %d\n"
						"\t12: %d\n",
						pEquipment->m_byCount,
						pEquipment->m_byCode[0], pEquipment->m_byCode[1], pEquipment->m_byCode[2],
						pEquipment->m_byCode[3], pEquipment->m_byCode[4], pEquipment->m_byCode[5],
						pEquipment->m_byCode[6], pEquipment->m_byCode[7], pEquipment->m_byCode[8],
						pEquipment->m_byCode[9], pEquipment->m_byCode[10], pEquipment->m_byCode[11] );

	for( int i=0 ; i<pEquipment->m_byCount ; ++i )
	{	
		g_GameDoc.SetEquipItem( pEquipment->m_byCode[i], i );

		if( pEquipment->m_byCode[i] == INVALID_ITEM_CODE ) continue;
		
		switch( i )
		{
		case ITEM_IDX_CHAR_A:
		case ITEM_IDX_CHAR_B:
			g_pScreenInventory->EquipCharacter( pEquipment->m_byCode[i], i );				
			break;

		case ITEM_IDX_WPN_1:			
		case ITEM_IDX_WPN_2:
		case ITEM_IDX_WPN_3:
		case ITEM_IDX_WPN_4:
		case ITEM_IDX_WPN_5:		
			g_pScreenInventory->EquipWeapon( pEquipment->m_byCode[i] );
			break;
		}
	}

	if( g_GameDoc.IsAllItemSet() )
	{
		g_GameDoc.SetWaitEquipList(LTFALSE);
	}
	else
	{
		g_GameDoc.SetWaitEquipList(LTTRUE);
	}

	g_pGameClientShell->ScreenRender( "Please wait..." );
}

void CNetwork::SC_AddEquipment( CPacket* pPacket )
{
	__P_I::SA_AddEquipment* pAddEquipment = static_cast< __P_I::SA_AddEquipment * >( pPacket );

	g_pLTClient->CPrint( "[NETWORK] Recv AddEquip  Result(%d)  SlotIdx(%d)",
						 pAddEquipment->m_byResult, pAddEquipment->m_byIndex );

	switch( pAddEquipment->m_byResult )
	{
	case __P_I::SA_AddEquipment::E_RESULT::ENUM::SUCCESS:
		if( IsWeaponByInt(pAddEquipment->m_byIndex) )
		{
			g_pScreenInventory->AddEquipWeapon( pAddEquipment->m_byIndex-ITEM_IDX_CHAR_B, pAddEquipment->m_byCode );
		}
		else if( IsCharacterByInt(pAddEquipment->m_byIndex) )
		{
			g_pScreenInventory->AddEquipCharacter( pAddEquipment->m_byCode, pAddEquipment->m_byIndex );			
		}
		g_GameDoc.SetEquipItem( pAddEquipment->m_byCode, pAddEquipment->m_byIndex );
		if( m_bCheckComplete ) CheckRecvComplete();
		break;
		
	case __P_I::SA_AddEquipment::E_RESULT::ENUM::FAIL_NO_INVEN:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_NO_INVEN, LTMB_OK, LTNULL, LTNULL );
		break;

	case __P_I::SA_AddEquipment::E_RESULT::ENUM::FAIL_WRONG_INDEX:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_WRONG_INDEX, LTMB_OK, LTNULL, LTNULL );
		break;

	default:
		{
			char szTemp[256];
			sprintf( szTemp, NF_GetMsg(SA_TEXT_ITEM_EQUIP_ERR), pAddEquipment->m_byResult );
			g_pInterfaceMgr->ShowMessageBox( szTemp, LTMB_OK, LTNULL, LTNULL );
		}
		break;
	}
}

void CNetwork::SC_RemoveEquipment( CPacket* pPacket )
{
	__P_I::SA_RemoveEquipment* pRemoveEquipment = static_cast< __P_I::SA_RemoveEquipment * >( pPacket );

	//pRemoveEquipment->m_byIndex
}

void CNetwork::ParseItemList( int nType, const char* lpszItemList )
{
	char*	lpszClass1;
	char*	lpszClass2;
	char*	lpszTemp;
	int		nItemIndex;
	int		nPeriod;
	LTBOOL	bDefault;
	LTBOOL	bBuyItem;
	int		nPrice;
	int		nSortSeq;
	LTBOOL	bFirst = LTTRUE;
		
	if( !lpszItemList || strlen( lpszItemList )<=0 ) return;

	strcpy( m_szTextBuffer, lpszItemList );

	lpszClass1	= strtok( m_szTextBuffer, "|" );
	if( !lpszClass1 ) return;

	do
	{
		if( bFirst )
		{
			bFirst = LTFALSE;
		}
		else
		{
			//´ëºÐ·ù
			lpszClass1	= strtok( LTNULL, "|" );
			if( !lpszClass1 ) return;
		}

		//ÁßºÐ·ù
		lpszClass2	= strtok( LTNULL, "|" );
		if( !lpszClass2 ) return;

		//¾ÆÀÌÅÛ ÀÎµ¦½º
		lpszTemp	= strtok( LTNULL, "|" );
		if( !lpszTemp ) return;
		nItemIndex	= atoi(lpszTemp);

		//»ç¿ë±â°£
		lpszTemp	= strtok( LTNULL, "|" );
		if( !lpszTemp ) return;
		nPeriod	= atoi(lpszTemp);

		//±âº»¿©ºÎ
		lpszTemp	= strtok( LTNULL, "|" );
		if( !lpszTemp ) return;
		bDefault	= stricmp(lpszTemp, "Y" ) ? LTFALSE : LTTRUE;

		//ÆÇ¸Å¿©ºÎ
		lpszTemp	= strtok( LTNULL, "|" );
		if( !lpszTemp ) return;
		bBuyItem	= stricmp(lpszTemp, "Y" ) ? LTFALSE : LTTRUE;

		//°¡°Ý
		lpszTemp	= strtok( LTNULL, "|" );
		if( !lpszTemp ) return;
		nPrice		= atoi(lpszTemp);

		//Á¤·Ä¼ø¼­
		lpszTemp	= strtok( LTNULL, "\r" );
		if( !lpszTemp ) return;
		nSortSeq	= atoi(lpszTemp);

		//AddItem
		AddShopItem( lpszClass1, lpszClass2,
					 nItemIndex, nPeriod,
					 bDefault, bBuyItem, nPrice, nSortSeq );
	} while( LTTRUE );
}

void CNetwork::AddShopItem( char* lpszClass1, char* lpszClass2, 
						    int nItemIndex, int nDuration,
							LTBOOL bDefault, LTBOOL bBuyItem,
							int nPrice, int nSortSeq )
{
#ifndef _FINAL
	g_pLTClient->CPrint( "[NETWORK] AddShopItem :  %c|%c%c Idx(%d) Period(%d) Default(%d) Display(%d) Price(%d)",
						 lpszClass1[0], lpszClass2[0], lpszClass2[1], nItemIndex, nDuration, bDefault, bBuyItem, nPrice );
#endif

	if( IsWeapon( lpszClass1[0] ) )
	{
		sShopWeapon_Set* pWeapon = new sShopWeapon_Set;	
		strcpy( pWeapon->szClass1, lpszClass1 );
		strcpy( pWeapon->szClass2, lpszClass2 );
		pWeapon->nClassNum		= GetWeaponClass(lpszClass2);
		pWeapon->nWeaponId		= nItemIndex;
		pWeapon->bDefaultItem	= !!bDefault;	
		pWeapon->bBuyItem		= !!bBuyItem;
		if( LTTRUE == g_pScreenInventory->AddShopWeapon( pWeapon, nDuration, nPrice ) )
		{
#ifndef _FINAL		
#ifdef DEBUG_SHOP
		sInven_Set* pSet	= new sInven_Set;
		pSet->bBuyItem		= false;
		pSet->bDefaultItem	= true;
		pSet->nWeaponId		= nItemIndex;
		strcpy( pSet->szClass1, pWeapon->szClass1 );
		strcpy( pSet->szClass2, pWeapon->szClass2 );
		pSet->nClassNum		= GetWeaponClass(pSet->szClass2);
		//[TODO] Msg
		sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: Á¦ÇÑ¾øÀ½" );
		g_pScreenInventory->AddWeapon( pSet );
#endif
#endif
		}
	}
	else if( IsCharacter( lpszClass1[0] ) )
	{
		sShopChar_Set* pCharacter = new sShopChar_Set;
		strcpy( pCharacter->szClass1, lpszClass1 );
		strcpy( pCharacter->szClass2, lpszClass2 );
		pCharacter->nClassNum		= ITEM_CHAR_SLOT;
		pCharacter->nItemId		= nItemIndex;
		pCharacter->bDefaultItem	= !!bDefault;
		pCharacter->bBuyItem		= !!bBuyItem;
		if( LTTRUE == g_pScreenInventory->AddShopCharTab( pCharacter, nDuration, nPrice ) )
		{
#ifndef _FINAL
#ifdef DEBUG_SHOP
		sChar_Set* pSet		= new sChar_Set;
		pSet->bBuyItem		= false;
		pSet->bDefaultItem	= true;
		pSet->nClassNum		= ITEM_CHAR_SLOT;
		pSet->nItemId		= nItemIndex;
		strcpy( pSet->szClass1, pCharacter->szClass1 );
		strcpy( pSet->szClass2, pCharacter->szClass2 );
		//[TODO] Msg
		sprintf( pSet->szExpireTime, "»ç¿ë±âÇÑ: Á¦ÇÑ¾øÀ½" );
		g_pScreenInventory->AddCharTab( pSet );
#endif
#endif
		}
	}
}

LTBOOL CNetwork::IsWeapon( char c )
{
	return (c=='w' || c=='W');
}

LTBOOL CNetwork::IsWeaponByInt( int nClass )
{
	if( nClass > ITEM_IDX_CHAR_B && nClass <=ITEM_IDX_WPN_6 )
	{
		return LTTRUE;
	}

	return LTFALSE;
}

LTBOOL CNetwork::IsCharacterByInt( int nClass )
{
	if( nClass <= ITEM_IDX_CHAR_B )
	{
		return LTTRUE;
	}

	return LTFALSE;
}

LTBOOL CNetwork::IsCharacter( char c )
{
	return (c=='c' || c=='C');
}

int CNetwork::GetWeaponClass( const char* lpszClass2 )
{
	if( stricmp( lpszClass2, "PW" ) == 0 )
	{
		return 1;
	}
	else if( stricmp( lpszClass2, "SW" ) == 0 )
	{
		return 2;
	}
	else if( stricmp( lpszClass2, "KN" ) == 0 )
	{
		return 3;
	}
	else if( stricmp( lpszClass2, "ET" ) == 0 )
	{
		return 4;
	}

	return 255;
}

void CNetwork::BeginTimeSync()
{
#ifdef __TIME_SYNC__
	for( int i=0 ; i<SYNC_TIME_COUNT ; ++i )
	{
		m_SyncData[i].dwServerTime	= 0;
		m_SyncData[i].dwDelayTime	= 0x7FFFFFFF;
	}

	m_nSyncCount = SYNC_TIME_COUNT-1;

	WriteConsoleFloat( "LockFPS", 0.0f );

	CS_GetServerTick();
#else
	g_pInterfaceMgr->CloseMessageBox(LTTRUE);
#endif
}

void CNetwork::CS_GetServerTick()
{
#ifdef __TIME_SYNC__
	m_SyncData[m_nSyncCount].dwSendTime = ::GetTickCount();
#endif
	SendPacket( &m_Packets.CS_ServerTick );
}

void CNetwork::SC_ServerTick( CPacket* pPacket  )
{
	__P_G::SN_ServerTick* pTick = static_cast< __P_G::SN_ServerTick * >( pPacket );

#ifdef __TIME_SYNC__

	if( m_nSyncCount < 0 ) 
	{
#ifndef _FINAL
		ASSERT(!"m_nSyncCount < 0");
#endif
		EndTimeSync();
		return;
	}
	
	m_SyncData[m_nSyncCount].dwRecvTime		= ::GetTickCount();
	m_SyncData[m_nSyncCount].dwServerTime	= pTick->m_dwTick;
	m_SyncData[m_nSyncCount].dwDelayTime	= (m_SyncData[m_nSyncCount].dwRecvTime-m_SyncData[m_nSyncCount].dwSendTime)/2;

	if( --m_nSyncCount < 0 )
	{
		EndTimeSync();
	}
	else
	{
		CS_GetServerTick();
	}
#else
	__int64 nCurrTick	= ::GetTickCount();
	__int64 nServerTick	= pTick->m_dwTick;
	m_nDeltaTime = nServerTick - nCurrTick;

	m_bCompleteTimeSync = LTTRUE;

	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() ) g_pScreenRoom->RecvComplete();
#endif
}

void CNetwork::EndTimeSync()
{
#ifdef __TIME_SYNC__
	int			i;
	int			j;
	int			nMiddleIndex;
	int			nMin;
	__int64		nSum;
	T_SYNC_DATA	MinData;
	
	// Selection Sort ===========================================	
	for( i=0 ; i<SYNC_TIME_COUNT-1 ; ++i )
	{
		nMin = i;
		memcpy( &MinData, &m_SyncData[i], sizeof(T_SYNC_DATA) );
		for( j=i+1 ; j<SYNC_TIME_COUNT ; ++j )
		{
			if( MinData.dwDelayTime > m_SyncData[j].dwDelayTime )
			{
				nMin	= j;
				memcpy( &MinData, &m_SyncData[j], sizeof(T_SYNC_DATA) );
			}
		}
		memcpy( &m_SyncData[nMin], &m_SyncData[i], sizeof(T_SYNC_DATA) );
		memcpy( &m_SyncData[i], &MinData, sizeof(T_SYNC_DATA) );
	}

	// Áß°£°ªº¸´Ù 1.5¹è°¡ µÇ´Â °ªµéÀÇ Æò±ÕÀ» °è»ê =================
	j				= 0; //Count
	nSum			= 0;
	nMiddleIndex	= SYNC_TIME_COUNT/2;

	for( i=0 ; i<SYNC_TIME_COUNT ; ++i )
	{
		if( (m_SyncData[nMiddleIndex].dwDelayTime*1.5) >= m_SyncData[i].dwDelayTime )
		{
			++j;
			nSum += (int)( (m_SyncData[i].dwServerTime+m_SyncData[i].dwDelayTime)-m_SyncData[i].dwRecvTime );
		}
	}

	//½Ã°£Â÷ÀÌ
	m_nDeltaTime = nSum/j;

	WriteConsoleFloat( "LockFPS", 0.014f );

	m_bCompleteTimeSync = LTTRUE;
	
	if( SCREEN_ID_SCREEN_ROOM == GetCurrentScreen() ) g_pScreenRoom->RecvComplete();
#endif
}

const char* CNetwork::GetIP()
{
	return m_ClientModule.GetLocalAddr();
}

LTBOOL CNetwork::IsRecvResult()
{
	return m_bRecvResult;
}

void CNetwork::SetRecvResult( LTBOOL b )
{
	m_bRecvResult	= b;
}

int CNetwork::GetCurrentScreen()
{
	if( SCREEN_ID_MAIN == g_pInterfaceMgr->GetCurrentScreen() && g_pScreenMain )
	{
		return SCREEN_ID_MAIN;
	}
	else if( SCREEN_ID_SCREEN_ROOM == g_pInterfaceMgr->GetCurrentScreen() && g_pScreenRoom )
	{
		return SCREEN_ID_SCREEN_ROOM;
	}
	else if( SCREEN_ID_RESULT == g_pInterfaceMgr->GetCurrentScreen() && g_pScreenResult )
	{
		return SCREEN_ID_RESULT;
	}
	else if( SCREEN_ID_INVENTORY == g_pInterfaceMgr->GetCurrentScreen() && g_pScreenInventory )
	{
		if( SCREEN_ID_MAIN == g_pInterfaceMgr->GetScreenMgr()->GetPreviousScreenID() )
		{
			return SCREEN_ID_MAIN;
		}
		else
		{
			return SCREEN_ID_INVENTORY;
		}
	}
	else if( SCREEN_ID_CHANNEL == g_pInterfaceMgr->GetCurrentScreen() && g_pScreenChannel )
	{
		return SCREEN_ID_CHANNEL;
	}

	return SCREEN_ID_NONE;
}

void CNetwork::SetPacketValue()
{
	m_Packets.CS_LoginGameHI.SetCommand( m_Packets.CS_LoginGameHI.s_nCode );
	m_Packets.CS_LoginGameHI.SetLength( sizeof(m_Packets.CS_LoginGameHI) );
	m_Packets.CS_LoginNetmarble.SetCommand( m_Packets.CS_LoginNetmarble.s_nCode );
	m_Packets.CS_LoginNetmarble.SetLength( sizeof(m_Packets.CS_LoginNetmarble) );
	m_Packets.CS_Nick.SetCommand( m_Packets.CS_Nick.s_nCode );
	m_Packets.CS_Nick.SetLength( sizeof(m_Packets.CS_Nick) );
	m_Packets.CS_Character.SetCommand( m_Packets.CS_Character.s_nCode );	//modify by zhoujb 0814
	m_Packets.CS_Character.SetLength( sizeof(m_Packets.CS_Character) );
	m_Packets.CS_JoinChannel.SetCommand( m_Packets.CS_JoinChannel.s_nCode );
	m_Packets.CS_JoinChannel.SetLength( sizeof(m_Packets.CS_JoinChannel) );
	m_Packets.CS_RefreshChannelList.SetCommand( m_Packets.CS_RefreshChannelList.s_nCode );
	m_Packets.CS_RefreshChannelList.SetLength( sizeof(m_Packets.CS_RefreshChannelList) );
	m_Packets.CS_AwayChannel.SetCommand( m_Packets.CS_AwayChannel.s_nCode );
	m_Packets.CS_AwayChannel.SetLength( sizeof(m_Packets.CS_AwayChannel) );
	m_Packets.CS_CreateRoom.SetCommand( m_Packets.CS_CreateRoom.s_nCode );
	m_Packets.CS_CreateRoom.SetLength( sizeof(m_Packets.CS_CreateRoom) );
	m_Packets.CS_JoinRoom.SetCommand( m_Packets.CS_JoinRoom.s_nCode );
	m_Packets.CS_JoinRoom.SetLength( sizeof(m_Packets.CS_JoinRoom) );
	m_Packets.CS_QuickJoin.SetCommand( m_Packets.CS_QuickJoin.s_nCode );
	m_Packets.CS_QuickJoin.SetLength( sizeof(m_Packets.CS_QuickJoin) );
	m_Packets.CS_Chat.SetCommand( m_Packets.CS_Chat.s_nCode );
	m_Packets.CS_Chat.SetLength( sizeof(m_Packets.CS_Chat) );	
	m_Packets.CS_ExitGameRoom.SetCommand( m_Packets.CS_ExitGameRoom.s_nCode );
	m_Packets.CS_ExitGameRoom.SetLength( sizeof(m_Packets.CS_ExitGameRoom) );
	m_Packets.CS_SearchUser.SetCommand( m_Packets.CS_SearchUser.s_nCode );
	m_Packets.CS_SearchUser.SetLength( sizeof(m_Packets.CS_SearchUser) );	
	m_Packets.CS_ChangeOption.SetCommand( m_Packets.CS_ChangeOption.s_nCode );
	m_Packets.CS_ChangeOption.SetLength( sizeof(m_Packets.CS_ChangeOption) );
	m_Packets.CS_Inven.SetCommand( m_Packets.CS_Inven.s_nCode );
	m_Packets.CS_Inven.SetLength( sizeof(m_Packets.CS_Inven) );
	m_Packets.CS_Ready.SetCommand( m_Packets.CS_Ready.s_nCode );
	m_Packets.CS_Ready.SetLength( sizeof(m_Packets.CS_Ready) );
	m_Packets.CS_ChangeTeam.SetCommand( m_Packets.CS_ChangeTeam.s_nCode );
	m_Packets.CS_ChangeTeam.SetLength( sizeof(m_Packets.CS_ChangeTeam) );
	m_Packets.CS_StartGame.SetCommand( m_Packets.CS_StartGame.s_nCode );
	m_Packets.CS_StartGame.SetLength( sizeof(m_Packets.CS_StartGame) );
	m_Packets.CS_StartHost.SetCommand( m_Packets.CS_StartHost.s_nCode );
	m_Packets.CS_StartHost.SetLength( sizeof(m_Packets.CS_StartHost) );	
	m_Packets.CS_Kill.SetCommand( m_Packets.CS_Kill.s_nCode );
	m_Packets.CS_Kill.SetLength( sizeof(m_Packets.CS_Kill) );	
	m_Packets.CS_Suicide.SetCommand( m_Packets.CS_Suicide.s_nCode );
	m_Packets.CS_Suicide.SetLength( sizeof(m_Packets.CS_Suicide) );
	m_Packets.CS_LoadComplete.SetCommand( m_Packets.CS_LoadComplete.s_nCode );
	m_Packets.CS_LoadComplete.SetLength( sizeof(m_Packets.CS_LoadComplete) );
	m_Packets.CS_TimeOut.SetCommand( m_Packets.CS_TimeOut.s_nCode );
	m_Packets.CS_TimeOut.SetLength( sizeof(m_Packets.CS_TimeOut) );
	m_Packets.CS_ExitGame.SetCommand( m_Packets.CS_ExitGame.s_nCode );
	m_Packets.CS_ExitGame.SetLength( sizeof(m_Packets.CS_ExitGame) );
	m_Packets.CS_GameChat.SetCommand( m_Packets.CS_GameChat.s_nCode );
	m_Packets.CS_GameChat.SetLength( sizeof(m_Packets.CS_GameChat) );
	m_Packets.CS_Whisper.SetCommand( m_Packets.CS_Whisper.s_nCode );
	m_Packets.CS_Whisper.SetLength( sizeof(m_Packets.CS_Whisper) );
	m_Packets.CS_BombState.SetCommand( m_Packets.CS_BombState.s_nCode );
	m_Packets.CS_BombState.SetLength( sizeof(m_Packets.CS_BombState) );
	m_Packets.CS_NextRound.SetCommand( m_Packets.CS_NextRound.s_nCode );
	m_Packets.CS_NextRound.SetLength( sizeof(m_Packets.CS_NextRound) );
	m_Packets.CS_KickBan.SetCommand( m_Packets.CS_KickBan.s_nCode );
	m_Packets.CS_KickBan.SetLength( sizeof(m_Packets.CS_KickBan) );
	m_Packets.CS_UserInfo.SetCommand( m_Packets.CS_UserInfo.s_nCode );
	m_Packets.CS_UserInfo.SetLength( sizeof(m_Packets.CS_UserInfo) );
	m_Packets.CS_GetRoomInfo.SetCommand( m_Packets.CS_GetRoomInfo.s_nCode );
	m_Packets.CS_GetRoomInfo.SetLength( sizeof(m_Packets.CS_GetRoomInfo) );
	m_Packets.CS_ChannelQuickJoin.SetCommand( m_Packets.CS_ChannelQuickJoin.s_nCode );
	m_Packets.CS_ChannelQuickJoin.SetLength( sizeof(m_Packets.CS_ChannelQuickJoin) );
	m_Packets.CS_PlantedBomb.SetCommand( m_Packets.CS_PlantedBomb.s_nCode );
	m_Packets.CS_PlantedBomb.SetLength( sizeof(m_Packets.CS_PlantedBomb) );
	m_Packets.CS_Radio.SetCommand( m_Packets.CS_Radio.s_nCode );
	m_Packets.CS_Radio.SetLength( sizeof(m_Packets.CS_Radio) );
	m_Packets.CS_WeaponChange.SetCommand( m_Packets.CS_WeaponChange.s_nCode );
	m_Packets.CS_WeaponChange.SetLength( sizeof(m_Packets.CS_WeaponChange) );
	m_Packets.CS_NetLoss.SetCommand( m_Packets.CS_NetLoss.s_nCode );
	m_Packets.CS_NetLoss.SetLength( sizeof(m_Packets.CS_NetLoss) );
	m_Packets.CS_ServerTick.SetCommand( m_Packets.CS_ServerTick.s_nCode );
	m_Packets.CS_ServerTick.SetLength( sizeof(m_Packets.CS_ServerTick) );
	m_Packets.CS_ChangeRoomTitle.SetCommand( m_Packets.CS_ChangeRoomTitle.s_nCode );
	m_Packets.CS_ChangeRoomTitle.SetLength( sizeof(m_Packets.CS_ChangeRoomTitle) );
	m_Packets.CS_ChangeCaptain.SetCommand( m_Packets.CS_ChangeCaptain.s_nCode );
	m_Packets.CS_ChangeCaptain.SetLength( sizeof(m_Packets.CS_ChangeCaptain) );
	m_Packets.CS_GetLobbyUserList.SetCommand( m_Packets.CS_GetLobbyUserList.s_nCode );
	m_Packets.CS_GetLobbyUserList.SetLength( sizeof(m_Packets.CS_GetLobbyUserList) );
	m_Packets.CS_Invite.SetCommand( m_Packets.CS_Invite.s_nCode );
	m_Packets.CS_Invite.SetLength( sizeof(m_Packets.CS_Invite) );
	m_Packets.CS_HackDetected.SetCommand( m_Packets.CS_HackDetected.s_nCode );
	m_Packets.CS_HackDetected.SetLength( sizeof(m_Packets.CS_HackDetected) );

	m_Packets.CS_EnterShop.SetCommand( m_Packets.CS_EnterShop.s_nCode );
	m_Packets.CS_EnterShop.SetLength( sizeof(m_Packets.CS_EnterShop) );
	m_Packets.CS_ExitShop.SetCommand( m_Packets.CS_ExitShop.s_nCode );
	m_Packets.CS_ExitShop.SetLength( sizeof(m_Packets.CS_ExitShop) );
	m_Packets.CS_BuyItem.SetCommand( m_Packets.CS_BuyItem.s_nCode );
	m_Packets.CS_BuyItem.SetLength( sizeof(m_Packets.CS_BuyItem) );
	m_Packets.CS_DiscardItem.SetCommand( m_Packets.CS_DiscardItem.s_nCode );
	m_Packets.CS_DiscardItem.SetLength( sizeof(m_Packets.CS_DiscardItem) );
	m_Packets.CS_AddEquipment.SetCommand( m_Packets.CS_AddEquipment.s_nCode );
	m_Packets.CS_AddEquipment.SetLength( sizeof(m_Packets.CS_AddEquipment) );
	m_Packets.CS_RemoveEquipment.SetCommand( m_Packets.CS_RemoveEquipment.s_nCode );
	m_Packets.CS_RemoveEquipment.SetLength( sizeof(m_Packets.CS_RemoveEquipment) );
	
	m_Packets.CS_DbgPlayerInfo.SetCommand( m_Packets.CS_DbgPlayerInfo.s_nCode );
	m_Packets.CS_DbgPlayerInfo.SetLength( sizeof(m_Packets.CS_DbgPlayerInfo) );
}

#ifndef _FINAL

void CNetwork::TestMandate()
{
	m_bRecvPausePacket = LTFALSE;
	ProcessMandate();
}

#endif