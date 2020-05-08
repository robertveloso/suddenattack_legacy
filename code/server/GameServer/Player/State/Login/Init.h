#pragma once

#include "../../IState.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class Init
:	public					IState
,	public					Singleton<Init>
{
public:
							Init();
	virtual					~Init();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	CBOOL					onGamehiLogin( Memento * i_pMemento, PACKET * i_pPacket );
	CBOOL					onNetmarbleLogin( Memento * i_pMemento, PACKET * i_pPacket );

	void					onDevLogin( Memento * i_pMemento, PACKET * i_pPacket );

};
static inline				Init *
INIT()						{ return Init::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */