#include "StdAfx.h"

#include "./OnAllKilled.h"

#include "../../../../IRoom.h"
#include "../../../../Room/IResult.h"
#include "../../../../Room/IProcess.h"
#include "../../../../Player/IResult.h"
#include "../../../../../Share/Packet/Service/Game.h"
#include "../../../../../Share/LogicObject/Room/Option.h"

#include "../../../../../Dispatcher/IConsole.h"
#include "../../../../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

void
OnAllKilled::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Game::CN_AllKilled
							CN_ALLKILLED;
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

	if( i_pMemento->GetPlayer() != i_pMemento->GetRoom()->GetSuperPeer() )
	{
		return;
	}

	IRoom * pRoom			= i_pMemento->GetRoom();
	if( pRoom->GetOption().GetMode() == E_MODE::RESPAWN )
	{
		return;
	}

	CN_ALLKILLED * pReq		= static_cast<CN_ALLKILLED *>( i_pPacket );

	i_pMemento->GetRoom()->GetResult()->SetAllKilled();
	IPlayer::E_TEAM::ENUM	eWinner;
	if( pReq->m_byWinnerTeam == IPlayer::E_TEAM::RED )
	{
		eWinner				= IPlayer::E_TEAM::RED;
	}
	else
	{
		eWinner				= IPlayer::E_TEAM::BLUE;
	}
	pRoom->GetResult()->SetWinRound( eWinner );
	Room::IPROCESS()->FinishRound( pRoom );

	STRING strLog;
	strLog.Printf( "Result\t%s\t%s\tR(%d)\t%s\t%d\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"AllKilled",
					pRoom->GetIndex(),
					i_pMemento->GetStock().GetNick(),
					eWinner );
	::Dispatcher::ICONSOLE()->Write( strLog );
}

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */