#pragma once

#include "../../IState.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class Closing
:	public					IState
,	public					Singleton<Closing>
{
public:
							Closing();
	virtual					~Closing();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

};
static inline				Closing *
CLOSING()					{ return Closing::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */