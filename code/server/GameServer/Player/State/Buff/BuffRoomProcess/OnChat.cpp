#include "StdAfx.h"

#include "./OnChat.h"

#include "../../../../ICaster.h"
#include "../../../../ILog.h"

#include "../../../../../Dispatcher/ITime.h"
#include "../../../../../Share/Packet/Service/Room.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

void
OnChat::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Room::NN_Chat
							NN_CHAT;
	NN_CHAT * pRelay		= static_cast<NN_CHAT *>( i_pPacket );

	ICASTER()->Cast( i_pMemento->GetRoom(), i_pMemento->GetPlayer(), pRelay );

	STRING					strLog;
	strLog.Printf( "%s\tBUFF CHAT\t%s\t%s\r\n",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							i_pMemento->GetStock().GetNick(),
							pRelay->GetMessage() );
	ILog::IFACADE()->Chatting( strLog );
}

} /* BuffRoomProcess */		} /* Buff */
} /* State */				} /* Player */				} /* GameServer */