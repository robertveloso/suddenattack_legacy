#pragma once

#include "../../IState.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class NoneCharacter
:	public					IState
,	public					Singleton<NoneCharacter>
{
public:
							NoneCharacter();
	virtual					~NoneCharacter();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					requestCharacter( Memento * i_pMemento );

};
static inline				NoneCharacter *
NONECHARACTER()				{ return NoneCharacter::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */