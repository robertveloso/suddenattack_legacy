#include "StdAfx.h"

#include "./Lobby.h"
#include "./Ready.h"
#include "../Buff/BuffRoom.h"
#include "../Fin.h"
#include "../CommonProcess/OnWhisper.h"
#include "../CommonProcess/OnFind.h"
#include "../../../IChannel.h"
#include "../../../IServer.h"
#include "../../../IRoom.h"
#include "../../../ICaster.h"
#include "../../../ILog.h"
#include "../../../Player/IResult.h"

#include "../../../../Dispatcher/ITime.h"
#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Service/Player.h"
#include "../../../../Share/Packet/Service/Room.h"
#include "../../../../Share/Packet/Service/Channel.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

Lobby::Lobby()
{
	this->install();
}

Lobby::~Lobby()
{
	this->uninstall();
}

CBOOL
Lobby::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				R = ::Packet::Service::Room;
	namespace				C = ::Packet::Service::Channel;
	namespace				P = ::Packet::Service::Player;
	switch( i_pPacket->GetCommand() )
	{
	case C::CN_Away::s_nCode :
		this->onAwayChannel( i_pMemento );
		break;

	case C::CQ_QuickJoin::s_nCode :
		this->onQuickChannel( i_pMemento, i_pPacket );
		break;

	case R::CQ_Join::s_nCode :
		this->onJoinRoom( i_pMemento, i_pPacket );
		break;

	case R::CQ_QuickJoin::s_nCode :
		this->onQuickJoinRoom( i_pMemento, i_pPacket );
		break;

	case R::CQ_Create::s_nCode :
		this->onCreateRoom( i_pMemento, i_pPacket );
		break;

	case P::CQ_Find::s_nCode :
		CommonProcess::OnFind::Execute( i_pMemento, i_pPacket );
		break;

	case R::NN_Chat::s_nCode :
		this->onChat( i_pMemento, i_pPacket );
		break;

	case P::NN_Whisper::s_nCode :
		CommonProcess::OnWhisper::Execute( i_pMemento, i_pPacket );
		break;

	case P::CN_Info::s_nCode :
		this->onInfo( i_pMemento, i_pPacket );
		break;

	default :
		return TRUE;
	}

	return TRUE;
}

void
Lobby::OnClose( Memento * i_pMemento )
{
	IChannel * pChannel		= i_pMemento->GetChannel();
	pChannel->GetLobby()->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( NULL );
	i_pMemento->SetChannel( NULL );

	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
Lobby::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
Lobby::onAwayChannel( Memento * i_pMemento )
{
	namespace				C = ::Packet::Service::Channel;

	IChannel * pChannel		= i_pMemento->GetChannel();
	pChannel->GetLobby()->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( NULL );
	i_pMemento->SetChannel( NULL );
	i_pMemento->SetState( READY() );

	C::SN_Info * pInfo		= ::Packet::Builder::Build<C::SN_Info>();
	pInfo->m_byChannelCount	= ISERVER()->GetChannelCount();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pInfo );

	pChannel				= ISERVER()->GetFirstChannel();
	while( pChannel != NULL )
	{
		C::SN_Enter * pNoti		= ::Packet::Builder::Build<C::SN_Enter>();
		pNoti->m_byIndex		= pChannel->GetIndex();
		pNoti->m_nMaxUser		= pChannel->GetMaxUser();
		pNoti->m_nCurrentUser	= pChannel->GetCurrentUser();
		ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
		
		pChannel				= ISERVER()->GetNextChannel();
	}
}

inline void
Lobby::onJoinRoom( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				R = ::Packet::Service::Room;
	namespace				P = ::Packet::Service::Player;
	R::CQ_Join * pReq		= static_cast<R::CQ_Join *>( i_pPacket );
	R::SA_Join * pAck		= ::Packet::Builder::Build<R::SA_Join>();

	IRoom * pRoom			= i_pMemento->GetChannel()->Search( pReq->m_nRoomIndex );

	if( pRoom == NULL )
	{
		pAck->m_byResult	= R::SA_Join::E_RESULT::NOT_EXIST;
		pAck->m_nRoomIndex	= pReq->m_nRoomIndex;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	switch( pRoom->IsJoinable( pReq->m_strPass ) )
	{
	case IRoom::E_JOIN::SUCCESS :
		break;

	case IRoom::E_JOIN::FULL :
		pAck->m_byResult	= R::SA_Join::E_RESULT::FULL;
		pAck->m_nRoomIndex	= pReq->m_nRoomIndex;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;

	case IRoom::E_JOIN::WRONG_PASS :
		pAck->m_byResult	= R::SA_Join::E_RESULT::WRONG_PASS;
		pAck->m_nRoomIndex	= pReq->m_nRoomIndex;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;

	case IRoom::E_JOIN::PLAYING :
		pAck->m_byResult	= R::SA_Join::E_RESULT::PLAYING;
		pAck->m_nRoomIndex	= pReq->m_nRoomIndex;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;

	default :
		DEBUG_BREAK;
		break;
	}

	i_pMemento->SetTeam( this->choiceTeam( pRoom ) );

	pAck->m_byResult		= R::SA_Join::E_RESULT::SUCCESS;
	pAck->m_nRoomIndex		= pRoom->GetIndex();
	pAck->m_byPlayerCount	= pRoom->GetPlayerCount();
	pAck->m_byTeam			= i_pMemento->GetTeam();
	pAck->m_stOption		= pRoom->GetOption();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );

	i_pMemento->SetReady( FALSE );
	pRoom->AddPlayer( i_pMemento->GetPlayer() );	
	pRoom->GetChannel()->GetLobby()->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( pRoom );

	i_pMemento->SetState( Buff::BUFFROOM() );
}

inline void
Lobby::onQuickJoinRoom( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				R = ::Packet::Service::Room;
	namespace				P = ::Packet::Service::Player;
	R::CQ_QuickJoin * pReq	= static_cast<R::CQ_QuickJoin *>( i_pPacket );
	R::SA_QuickJoin * pAck	= ::Packet::Builder::Build<R::SA_QuickJoin>();

	IRoom * pRoom			= this->findFreeRoom( i_pMemento );

	if( pRoom == NULL )
	{
		pAck->m_byResult	= R::SA_QuickJoin::E_RESULT::FULL;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	i_pMemento->SetTeam( this->choiceTeam( pRoom ) );

	pAck->m_byResult		= R::SA_QuickJoin::E_RESULT::SUCCESS;
	pAck->m_nRoomIndex		= pRoom->GetIndex();
	pAck->m_byPlayerCount	= pRoom->GetPlayerCount();
	pAck->m_byTeam			= i_pMemento->GetTeam();
	pAck->m_stOption		= pRoom->GetOption();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );

	i_pMemento->SetReady( FALSE );
	pRoom->AddPlayer( i_pMemento->GetPlayer() );	
	pRoom->GetChannel()->GetLobby()->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( pRoom );

	i_pMemento->SetState( Buff::BUFFROOM() );
}

inline void
Lobby::onCreateRoom( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				R = ::Packet::Service::Room;
	namespace				P = ::Packet::Service::Player;
	R::CQ_Create * pReq		= static_cast<R::CQ_Create *>( i_pPacket );

	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

	switch( pReq->m_stOption.GetMode() )
	{
	case E_MODE::RESPAWN :
	case E_MODE::BOMB :
		break;

	default :
		{
			R::SA_Create * pAck	= ::Packet::Builder::Build<R::SA_Create>();
			pAck->m_byResult	= R::SA_Create::E_RESULT::FAIL;
			ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		}
		return;
	}

	i_pMemento->GetChannel()->GetLobby()->RemovePlayer( i_pMemento->GetPlayer() );

	IRoom * pRoom			= IRoom::IFACTORY()->Create( i_pMemento->GetChannel(), i_pMemento->GetPlayer(), pReq->m_strName, pReq->m_strPass, pReq->m_stOption );
	if( pRoom == NULL )
	{
		R::SA_Create * pAck	= ::Packet::Builder::Build<R::SA_Create>();
		pAck->m_byResult	= R::SA_Create::E_RESULT::FAIL;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}
	i_pMemento->SetRoom( pRoom );

	i_pMemento->GetChannel()->AddRoom( pRoom );

//	i_pMemento->SetTeam( this->choiceTeam( pRoom ) );
	i_pMemento->SetTeam( IPlayer::E_TEAM::RED );
	i_pMemento->SetReady( FALSE );

	R::SA_Create * pAck		= ::Packet::Builder::Build<R::SA_Create>();
	pAck->m_byResult		= R::SA_Create::E_RESULT::SUCCESS;
	pAck->m_nIndex			= pRoom->GetIndex();
	pAck->m_byTeam			= i_pMemento->GetTeam();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );

	i_pMemento->SetState( Buff::BUFFROOM() );
}

inline Lobby::E_TEAM::ENUM
Lobby::choiceTeam( IRoom * i_pRoom )
{
	INT nRed				= 0;
	INT nBlue				= 0;
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->GetTeam() == E_TEAM::RED )
		{
			nRed ++;
		}
		else
		{
			nBlue ++;
		}
		pPlayer				= i_pRoom->GetNextPlayer();
	}
	if( nRed <= nBlue )
	{
		return E_TEAM::RED;
	}
	else
	{
		return E_TEAM::BLUE;
	}
}

inline IRoom *
Lobby::findFreeRoom( Memento * i_pMemento )
{
	IChannel * pChannel		= i_pMemento->GetChannel();
	IRoom * pRoom			= pChannel->GetFirstRoom();
	while( pRoom != NULL )
	{
		if( pRoom->GetOption().CanRunnableInBattle() == FALSE )
		{
			pRoom			= pChannel->GetNextRoom();
			continue;
		}
		if( pRoom->IsJoinable( IRoom::CPASS( "" ) ) == IRoom::E_JOIN::SUCCESS )
		{
			return pRoom;
		}

		pRoom				= pChannel->GetNextRoom();
	}

	pRoom			= pChannel->GetFirstRoom();
	while( pRoom != NULL )
	{
		if( pRoom->IsJoinable( IRoom::CPASS( "" ) ) == IRoom::E_JOIN::SUCCESS )
		{
			return pRoom;
		}

		pRoom				= pChannel->GetNextRoom();
	}
	return NULL;
}

inline void
Lobby::onChat( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Room::NN_Chat
							NN_CHAT;
	NN_CHAT * pRelay		= static_cast<NN_CHAT *>( i_pPacket );

	ICASTER()->Cast( i_pMemento->GetRoom(), i_pMemento->GetPlayer(), pRelay );

	STRING					strLog;
	strLog.Printf( "%s\tLOBBY CHAT\t%s\t%s\r\n",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							i_pMemento->GetStock().GetNick(),
							pRelay->GetMessage() );
	ILog::IFACADE()->Chatting( strLog );

}

inline void
Lobby::onInfo( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				P = ::Packet::Service::Player;
	Assert( i_pPacket->GetCommand() == P::CN_Info::s_nCode );

	P::CN_Info * pReq		= static_cast<P::CN_Info *>( i_pPacket );
	IPlayer * pPlayer		= i_pMemento->GetRoom()->FindPlayer( pReq->m_nIndex );
	if( pPlayer == NULL )
	{
		return;
	}

	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	P::SN_Info * pAck		= ::Packet::Builder::Build<P::SN_Info>();
	pAck->m_nIndex			= pPlayer->GetIndex();
	pAck->m_stRed			= pPlayer->GetMemento()->GetCharacters()[ E_TEAM::RED ];
	pAck->m_stBlue			= pPlayer->GetMemento()->GetCharacters()[ E_TEAM::BLUE ];
	pAck->m_stResult		= * pPlayer->GetResult()->GetTotalResult();

	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
}

inline void
Lobby::onQuickChannel( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				C = ::Packet::Service::Channel;
	C::CQ_QuickJoin * pReq	= static_cast<C::CQ_QuickJoin *>( i_pPacket );
	C::SA_QuickJoin * pAck	= ::Packet::Builder::Build<C::SA_QuickJoin>();

	IChannel * pChannel		= ISERVER()->Search( pReq->m_byChannelIndex );
	if( pChannel == NULL )
	{
		pAck->m_byResult	= C::SA_QuickJoin::E_RESULT::FAIL_WRONG_CHANNEL;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	if( pChannel->GetMaxUser() == pChannel->GetCurrentUser() )
	{
		pAck->m_byResult	= C::SA_QuickJoin::E_RESULT::FAIL_CHANNEL_FULL;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	pAck->m_byResult		= C::SA_QuickJoin::E_RESULT::SUCCESS;
	pAck->m_nRoomCount		= pChannel->GetRoomCount();
	pAck->m_nUserCount		= pChannel->GetLobby()->GetPlayerCount();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );

	i_pMemento->GetChannel()->GetLobby()->RemovePlayer( i_pMemento->GetPlayer() );
	pChannel->GetLobby()->AddPlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( pChannel->GetLobby() );
	i_pMemento->SetChannel( pChannel );

}

} /* Login */
} /* State */				} /* Player */				} /* GameServer */