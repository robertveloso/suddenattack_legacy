#include "StdAfx.h"

#include "./OnInit.h"
//#include "./OnGameEnd.h"

#include "../../../../IRoom.h"
#include "../../../../Player/IResult.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

void
OnInit::Execute( Memento * i_pMemento )
{
	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		pPlayer->InitPlay();

		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}
}


} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */