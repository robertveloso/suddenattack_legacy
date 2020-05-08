#include "StdAfx.h"

#include "./Inventory.h"
#include "./BuffRoom.h"
#include "../Fin.h"
#include "../Login/Lobby.h"
#include "../../Memento.h"
#include "../../../IPlayer.h"
#include "../../../IRoom.h"
#include "../../../Room/IProcess.h"
#include "../../../IChannel.h"
#include "../../../ICaster.h"
#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Service/Play.h"
#include "../../../../Share/Packet/Service/Player.h"
#include "../../../../Share/Packet/Service/Room.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {

Inventory::Inventory()
{
	this->install();
}

Inventory::~Inventory()
{
	this->uninstall();
}

CBOOL
Inventory::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Play::NN_Inventory
							NN_INVENTORY;
	NN_INVENTORY * pRelay	= static_cast<NN_INVENTORY *>( i_pPacket );
	if( i_pPacket->GetCommand() != NN_INVENTORY::s_nCode )
	{
		return TRUE;
	}

	if( pRelay->m_bInventory == TRUE )
	{
		return TRUE;
	}

	i_pMemento->SetState( BUFFROOM() );
	
	ICASTER()->Cast( i_pMemento->GetPlayer(), pRelay );

	typedef					::Packet::Service::Play::SN_ReturnBuff
							SN_RETURNBUFF;

	SN_RETURNBUFF * pNoti	= ::Packet::Builder::Build<SN_RETURNBUFF>();
	pNoti->m_byPlayerCount	= i_pMemento->GetRoom()->GetPlayerCount() - 1;
	pNoti->m_byTeam			= i_pMemento->GetTeam();
	pNoti->m_stOption		= i_pMemento->GetRoom()->GetOption();

	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );

	Room::IPROCESS()->CastStateForReturn( i_pMemento->GetPlayer(), i_pMemento->GetRoom() );

	return TRUE;
}

void
Inventory::OnClose( Memento * i_pMemento )
{
	namespace				R = ::Packet::Service::Room;
	namespace				P = ::Packet::Service::Player;

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
Inventory::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
Inventory::onOut( Memento * i_pMemento )
{
	namespace				P = ::Packet::Service::Player;
	P::SN_Out * pAck		= ::Packet::Builder::Build<P::SN_Out>();

	IRoom * pOldRoom		= i_pMemento->GetRoom();
	IRoom * pLobby			= i_pMemento->GetChannel()->GetLobby();
	pAck->m_nRoomCount		= i_pMemento->GetChannel()->GetRoomCount();
	pAck->m_nUserCount		= pLobby->GetPlayerCount();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );

	pLobby->AddPlayer( i_pMemento->GetPlayer() );
	pOldRoom->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetReady( FALSE );
	i_pMemento->SetRoom( pLobby );

	if( pOldRoom->GetPlayerCount() == 0 )
	{
		pOldRoom->GetChannel()->RemoveRoom( pOldRoom );
		IRoom::IFACTORY()->Destroy( pOldRoom );
	}

	i_pMemento->SetState( Login::LOBBY() );
}

} /* Buff */
} /* State */				} /* Player */				} /* GameServer */