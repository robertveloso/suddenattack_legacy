#include "StdAfx.h"

#include "./OnFind.h"

#include "../../../ICaster.h"
#include "../../../IChannel.h"
#include "../../../IRoom.h"

#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Service/Player.h"

namespace GameServer {		namespace Player {			namespace State {
namespace CommonProcess {

void
OnFind::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				P = ::Packet::Service::Player;
	typedef					IPlayer::IContainer::NICK
							NICK;

	P::CQ_Find * pReq		= static_cast<P::CQ_Find *>( i_pPacket );
	Assert( i_pPacket->GetCommand() == P::CQ_Find::s_nCode );

	P::SA_Find * pAck		= ::Packet::Builder::Build<P::SA_Find>();

	IPlayer * pPlayer		= IPlayer::ICONTAINER()->Find( NICK( pReq->GetNick() ) );
	if( pPlayer == NULL )
	{
		pAck->m_byResult	= P::SA_Find::E_RESULT::FAIL;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	IChannel * pChannel		= pPlayer->GetMemento()->GetChannel();
	IRoom * pRoom			= pPlayer->GetMemento()->GetRoom();

	if( pChannel == NULL )
	{
		pAck->m_byResult	= P::SA_Find::E_RESULT::CHANNEL;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	if( pRoom == pChannel->GetLobby() )
	{
		pAck->m_byResult		= P::SA_Find::E_RESULT::LOBBY;
		pAck->m_byChannelIndex	= pChannel->GetIndex();
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY )
	{
		pAck->m_byResult		= P::SA_Find::E_RESULT::PLAY;
		pAck->m_byChannelIndex	= pChannel->GetIndex();
		pAck->m_nRoomIndex		= pRoom->GetIndex();
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	pAck->m_byResult		= P::SA_Find::E_RESULT::ROOM;
	pAck->m_byChannelIndex	= pChannel->GetIndex();
	pAck->m_nRoomIndex		= pRoom->GetIndex();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
}

} /* CommonProcess */
} /* State */				} /* Player */				} /* GameServer */