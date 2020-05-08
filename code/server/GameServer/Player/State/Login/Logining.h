#pragma once

#include "../../IState.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class Logining
:	public					IState
,	public					Singleton<Logining>
{
public:
							Logining();
	virtual					~Logining();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					requestResult( Memento * i_pMemento );
	CBOOL					setUserResult( Memento * i_pMemento, IQUERY * i_pQuery );
	void					castNoneNick( Memento * i_pMemento );
	void					castNoneCharacter( Memento * i_pMemento );

};
static inline				Logining *
LOGINING()					{ return Logining::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */