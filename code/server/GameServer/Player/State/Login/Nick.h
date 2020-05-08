#pragma once

#include "../../IState.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class Nick
:	public					IState
,	public					Singleton<Nick>
{
public:
							Nick();
	virtual					~Nick();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					castCharacter( Memento * i_pMemento );

};
static inline				Nick *
NICK()						{ return Nick::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */