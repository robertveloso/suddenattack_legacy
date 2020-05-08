#pragma once

#include "../../IState.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class NoneNick
:	public					IState
,	public					Singleton<NoneNick>
{
public:
							NoneNick();
	virtual					~NoneNick();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					requestNick( Memento * i_pMemento );

};
static inline				NoneNick *
NONENICK()					{ return NoneNick::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */