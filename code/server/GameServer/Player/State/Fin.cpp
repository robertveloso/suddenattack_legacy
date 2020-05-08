#include "StdAfx.h"

#include "./Fin.h"
//#include "../Memento.h"
//#include "../../IChannel.h"
//#include "../../../Share/Packet/Login.h"
//#include "../../../Dispatcher/IQuery.h"

namespace GameServer {		namespace Player {			namespace State {

Fin::Fin()
{
	this->install();
}

Fin::~Fin()
{
	this->uninstall();
}

CBOOL
Fin::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	DEBUG_BREAK;

	return TRUE;
}

void
Fin::OnClose( Memento * i_pMemento )
{
}

void
Fin::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

} /* State */				} /* Player */				} /* GameServer */