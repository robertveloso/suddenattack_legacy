#pragma once

#include "../../System.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {		namespace Player {
class Memento;
} /* Player */				} /* GameServer */

namespace Dispatcher {
class IQuery;
} /* Dispatcher */

namespace GameServer {		namespace Player {

class IState	//某个状态下对packet的反应
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket ) = 0;
	virtual void			OnClose( Memento * i_pMemento ) = 0;

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery ) = 0;

};

} /* Player */				} /* GameServer */