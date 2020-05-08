#include "StdAfx.h"

#include "./Closing.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

Closing::Closing()
{
	this->install();
}

Closing::~Closing()
{
	this->uninstall();
}

CBOOL
Closing::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	// 무시
	return TRUE;
}

void
Closing::OnClose( Memento * i_pMemento )
{
	// 무시
}

void
Closing::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	// 무시
}

} /* Login */
} /* State */				} /* Player */				} /* GameServer */