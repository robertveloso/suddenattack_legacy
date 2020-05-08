#include "StdAfx.h"

#include "./Ready.h"
#include "./Lobby.h"
#include "../Fin.h"

#include "../../Memento.h"
#include "../../../IChannel.h"
#include "../../../IServer.h"
#include "../../../IRoom.h"
#include "../../../ICaster.h"
#include "../../../IPlayer.h"
#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Service/Channel.h"
#include "../../../../Share/Packet/Service/Room.h"
#include "../../../../Share/Packet/Service/Player.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

Ready::Ready()
{
	this->install();
}

Ready::~Ready()
{
	this->uninstall();
}

CBOOL
Ready::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				C = ::Packet::Service::Channel;

	switch( i_pPacket->GetCommand() )
	{
	case C::CQ_Join::s_nCode :
		this->onJoin( i_pMemento, i_pPacket );
		break;

	case C::CN_Refresh::s_nCode :
		this->onRefresh( i_pMemento, i_pPacket );
		break;

	default:
		break;
	}

	return TRUE;
}

void
Ready::OnClose( Memento * i_pMemento )
{
	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
Ready::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
Ready::onJoin( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				C = ::Packet::Service::Channel;

	Assert( i_pPacket->GetCommand() == C::CQ_Join::s_nCode );

	C::CQ_Join * pReq		= static_cast<C::CQ_Join *>( i_pPacket );

	IChannel * pChannel		= ISERVER()->Search( pReq->m_byChannelIndex );
	CBYTE byResult			= this->joinable( pChannel );
	if( byResult != C::SA_Join::E_RESULT::SUCCESS )
	{
		this->castJoinFail( i_pMemento, byResult );
		return;
	}
	this->castJoinSuccess( i_pMemento, pChannel );

	pChannel->GetLobby()->AddPlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( pChannel->GetLobby() );
	i_pMemento->SetChannel( pChannel );

	i_pMemento->SetState( LOBBY() );
}

inline void
Ready::onRefresh( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				C = ::Packet::Service::Channel;

	Assert( i_pPacket->GetCommand() == C::CN_Refresh::s_nCode );
	
	C::SN_Info * pInfo		= ::Packet::Builder::Build<C::SN_Info>();
	pInfo->m_byChannelCount	= ISERVER()->GetChannelCount();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pInfo );

	IChannel * pChannel		= ISERVER()->GetFirstChannel();
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
Ready::castJoinFail( Memento * i_pMemento, CBYTE i_byResult )
{
	typedef					::Packet::Service::Channel::SA_Join
							SA_JOIN;
	SA_JOIN * pAck			= ::Packet::Builder::Build<SA_JOIN>();
	pAck->m_byResult		= i_byResult;

	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
}

inline void
Ready::castJoinSuccess( Memento * i_pMemento, IChannel * i_pChannel )
{
#ifdef _DEVDEBUG
RETURN:
	IRoom * pRoom			= i_pChannel->GetFirstRoom();
	while( pRoom != NULL )
	{
		if( pRoom->GetPlayerCount() == 0 )
		{
			i_pChannel->RemoveRoom( pRoom );
			goto RETURN;
		}
		pRoom				= i_pChannel->GetNextRoom();
	}
#endif
	typedef					::Packet::Service::Channel::SA_Join
							SA_JOIN;
	SA_JOIN * pAck			= ::Packet::Builder::Build<SA_JOIN>();
	pAck->m_byResult		= static_cast<BYTE>( SA_JOIN::E_RESULT::SUCCESS );
	pAck->m_nRoomCount		= i_pChannel->GetRoomCount();
	pAck->m_nUserCount		= i_pChannel->GetLobby()->GetPlayerCount();

	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
}

inline CBYTE
Ready::joinable( IChannel * i_pChannel )
{
	typedef					::Packet::Service::Channel::SA_Join::E_RESULT
							E_RESULT;

	if( i_pChannel == NULL )
	{
		return E_RESULT::WRONG_INDEX;
	}

	if( i_pChannel->GetMaxUser() == i_pChannel->GetCurrentUser() )
	{
		return E_RESULT::FULL;
	}

	return E_RESULT::SUCCESS;
}

} /* Login */
} /* State */				} /* Player */				} /* GameServer */