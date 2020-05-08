#include "StdAfx.h"

#include "./ReturnBuff.h"
#include "./BuffRoom.h"
#include "../Fin.h"
#include "../../Memento.h"
#include "../../../IChannel.h"
#include "../../../IRoom.h"
#include "../../../Room/IProcess.h"
#include "../../../ICaster.h"
#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Service/Play.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {

ReturnBuff::ReturnBuff()
{
	this->install();
}

ReturnBuff::~ReturnBuff()
{
	this->uninstall();
}

CBOOL
ReturnBuff::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				P = ::Packet::Service::Play;

	switch( i_pPacket->GetCommand() )
	{
	case P::CN_ReturnBuff::s_nCode :
		this->onReturnBuff( i_pMemento );
		break;

	default :
		break;
	}

	return TRUE;
}

void
ReturnBuff::OnClose( Memento * i_pMemento )
{
//	namespace				R = ::Packet::Service::Room;

	IRoom * pOldRoom		= i_pMemento->GetRoom();
	IRoom * pLobby			= i_pMemento->GetChannel()->GetLobby();

	pOldRoom->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( NULL );

	if( pOldRoom->GetPlayerCount() == 0 )
	{
		pOldRoom->GetChannel()->RemoveRoom( pOldRoom );
		IRoom::IFACTORY()->Destroy( pOldRoom );
	}

	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
ReturnBuff::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
ReturnBuff::onReturnBuff( Memento * i_pMemento )
{
	typedef					::Packet::Service::Play::SN_ReturnBuff
							SN_RETURNBUFF;
	SN_RETURNBUFF * pNoti	= ::Packet::Builder::Build<SN_RETURNBUFF>();
	pNoti->m_byPlayerCount	= static_cast<BYTE>( i_pMemento->GetRoom()->GetPlayerCount() - 1 );
	pNoti->m_byTeam			= i_pMemento->GetTeam();
	pNoti->m_stOption		= i_pMemento->GetRoom()->GetOption();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );

	i_pMemento->SetState( Buff::BUFFROOM() );
	
	Room::IPROCESS()->CastStateForReturn( i_pMemento->GetPlayer(), i_pMemento->GetRoom() );
}

} /* Buff */
} /* State */				} /* Player */				} /* GameServer */