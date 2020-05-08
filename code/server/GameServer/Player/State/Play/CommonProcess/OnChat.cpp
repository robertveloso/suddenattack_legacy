#include "StdAfx.h"

#include "./OnChat.h"
//#include "./OnGameEnd.h"

#include "../../../../ICaster.h"
#include "../../../../IRoom.h"
#include "../../../../ILog.h"
#include "../../../../Player/IResult.h"

#include "../../../../../Dispatcher/ITime.h"
#include "../../../../../Share/Packet/Service/Game.h"
#include "../../../../../Share/Packet/Service/Room.h"
#include "../../../../../Share/LogicObject/Room/Option.h"

//#include "../../../../../Dispatcher/IConsole.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

void
OnChat::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Game::NN_Chat
							NN_CHAT;
	NN_CHAT * pRelay		= static_cast<NN_CHAT *>( i_pPacket );

	switch( pRelay->m_byType )
	{
	case NN_CHAT::E_TYPE::ALL :
		OnChat::castAll( i_pMemento, i_pPacket );
		break;

	case NN_CHAT::E_TYPE::TEAM :
		OnChat::castTeam( i_pMemento, i_pPacket );
		break;

	case NN_CHAT::E_TYPE::GHOST_ALL :
		OnChat::castGhostAll( i_pMemento, i_pPacket );
		break;

	case NN_CHAT::E_TYPE::GHOST_TEAM :
		OnChat::castGhostTeam( i_pMemento, i_pPacket );
		break;

	default:
		break;
	}

	STRING					strLog;
	strLog.Printf( "%s\tROOM CHAT\t%s\t%s\r\n",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							i_pMemento->GetStock().GetNick(),
							pRelay->GetMessage() );
	ILog::IFACADE()->Chatting( strLog );
}

inline void
OnChat::castAll( Memento * i_pMemento, PACKET * i_pPacket )
{
	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		if( pPlayer == i_pMemento->GetPlayer() )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		ICASTER()->Cast( pPlayer, i_pPacket );

		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}

}

inline void
OnChat::castTeam( Memento * i_pMemento, PACKET * i_pPacket )
{
	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		if( pPlayer == i_pMemento->GetPlayer() )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		if( pPlayer->GetMemento()->GetTeam() != i_pMemento->GetPlayer()->GetMemento()->GetTeam() )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		ICASTER()->Cast( pPlayer, i_pPacket );

		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}
}

inline void
OnChat::castGhostAll( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

	if( i_pMemento->GetRoom()->GetOption().GetMode() == E_MODE::RESPAWN )
	{
		return;
	}
	if( i_pMemento->IsDead() == FALSE )
	{
		return;
	}

	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		if( pPlayer == i_pMemento->GetPlayer() )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		if( pPlayer->GetMemento()->IsDead() == FALSE )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		ICASTER()->Cast( pPlayer, i_pPacket );

		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}
}

inline void
OnChat::castGhostTeam( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

	if( i_pMemento->GetRoom()->GetOption().GetMode() == E_MODE::RESPAWN )
	{
		return;
	}
	if( i_pMemento->IsDead() == FALSE )
	{
		return;
	}

	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		if( pPlayer == i_pMemento->GetPlayer() )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		if( pPlayer->GetResult()->GetDeadCount() == 0 )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		if( pPlayer->GetMemento()->GetTeam() != i_pMemento->GetPlayer()->GetMemento()->GetTeam() )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}

		ICASTER()->Cast( pPlayer, i_pPacket );

		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}
}


} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */