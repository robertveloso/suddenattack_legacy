#pragma once

#include "../IState.h"

namespace GameServer {		namespace Player {			namespace State {

class Fin
:	public					IState
,	public					Singleton<Fin>
{
public:
							Fin();
	virtual					~Fin();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

};
static inline				Fin *
FIN()						{ return Fin::GetInstance(); }

} /* State */				} /* Player */				} /* GameServer */
