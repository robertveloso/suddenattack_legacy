#include "StdAfx.h"

#include "./OnKill.h"

#include "../../../../IRoom.h"
#include "../../../../ICaster.h"
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
OnKill::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

	switch( i_pMemento->GetRoom()->GetOption().GetMode() )
	{
	case E_MODE::RESPAWN :
		OnKill::onRespawn( i_pMemento, i_pPacket );
		break;

	case E_MODE::BOMB :
		OnKill::onBomb( i_pMemento, i_pPacket );
		break;

	default :
		break;
	}
}

inline void
OnKill::onRespawn( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Game::NN_Kill
							NN_KILL;

	Assert( i_pPacket->GetCommand() == NN_KILL::s_nCode );

	IRoom * pRoom			= i_pMemento->GetRoom();
	if( i_pMemento->GetPlayer() != pRoom->GetSuperPeer() )
	{
		return;
	}

	NN_KILL * pReq			= static_cast<NN_KILL *>( i_pPacket );
	
	IPlayer * pKiller		= pRoom->FindPlayer( pReq->m_nKiller );
	if( pKiller == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnKill : Killer Index Not Found\n" ) );
		return;
	}
	IPlayer * pVictim		= pRoom->FindPlayer( pReq->m_nVictim );
	if( pVictim == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnKill : Victim Index Not Found\n" ) );
		return;
	}

	STRING strLog;
	strLog.Printf( "Result\t%s\t%s\tR(%d)\t%s\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Kill",
					pRoom->GetIndex(),
					pKiller->GetMemento()->GetStock().GetNick(),
					pVictim->GetMemento()->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	pKiller->GetResult()->Kill( pReq->m_bHeadShot );
	pVictim->GetResult()->Dead( pReq->m_bHeadShot );

	pRoom->GetResult()->Kill( pKiller->GetMemento()->GetTeam() );
	pRoom->GetResult()->Dead( pVictim->GetMemento()->GetTeam() );

	IPlayer * pPlayer		= pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() == IPlayer::E_STATE::PLAY )
		{
			ICASTER()->Cast( pPlayer, pReq );
		}
		pPlayer				= pRoom->GetNextPlayer();
	}

	CUINT nTotalDead		= pRoom->GetResult()->GetKillCount( pKiller->GetMemento()->GetTeam() );
	if( nTotalDead >= pRoom->GetOption().GetKillLimit() )
	{
		typedef				::LogicObject::Player::Type::E_TEAM
							E_TEAM;

		IRoom * pRoom		= i_pMemento->GetRoom();

		INT nRed			= pRoom->GetResult()->GetKillCount( E_TEAM::RED )
							- pRoom->GetResult()->GetDeadCount( E_TEAM::RED );

		INT nBlue			= pRoom->GetResult()->GetKillCount( E_TEAM::BLUE )
							- pRoom->GetResult()->GetDeadCount( E_TEAM::BLUE );

		if( nRed > nBlue )
		{
			pRoom->GetResult()->SetWinRound( E_TEAM::RED );
		}
		else if( nRed < nBlue )
		{
			pRoom->GetResult()->SetWinRound( E_TEAM::BLUE );
		}
		else
		{
			pRoom->GetResult()->SetWinRound( E_TEAM::DRAW );
		}
		Room::IPROCESS()->FinishRound( i_pMemento->GetRoom() );
	}
}

inline void
OnKill::onBomb( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Game::NN_Kill
							NN_KILL;

	Assert( i_pPacket->GetCommand() == NN_KILL::s_nCode );

	IRoom * pRoom			= i_pMemento->GetRoom();
	if( i_pMemento->GetPlayer() != pRoom->GetSuperPeer() )
	{
		return;
	}

	NN_KILL * pReq			= static_cast<NN_KILL *>( i_pPacket );
	
	IPlayer * pKiller		= pRoom->FindPlayer( pReq->m_nKiller );
	if( pKiller == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnKill : Killer Index Not Found\n" ) );
		return;
	}
	IPlayer * pVictim		= pRoom->FindPlayer( pReq->m_nVictim );
	if( pVictim == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnKill : Victim Index Not Found\n" ) );
		return;
	}

	if( pVictim->GetMemento()->IsDead() == TRUE )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnKill : Already Dead\n" ) );
		return;
	}

	STRING strLog;
	strLog.Printf( "Result\t%s\t%s\tR(%d)\t%s\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Kill",
					pRoom->GetIndex(),
					pKiller->GetMemento()->GetStock().GetNick(),
					pVictim->GetMemento()->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	pVictim->GetMemento()->SetDead( TRUE );
	pVictim->GetMemento()->SetSpectator( TRUE );

	pKiller->GetResult()->Kill( pReq->m_bHeadShot );
	pVictim->GetResult()->Dead( pReq->m_bHeadShot );

	pRoom->GetResult()->Kill( pKiller->GetMemento()->GetTeam() );
	pRoom->GetResult()->Dead( pVictim->GetMemento()->GetTeam() );

	IPlayer * pPlayer		= pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() == IPlayer::E_STATE::PLAY )
		{
			ICASTER()->Cast( pPlayer, pReq );
		}
		pPlayer				= pRoom->GetNextPlayer();
	}

	BOOL bAlive				= FALSE;
	pPlayer					= pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() == IPlayer::E_STATE::PLAY &&
			pPlayer->GetMemento()->GetTeam() == pVictim->GetMemento()->GetTeam() )
		{
			if( pPlayer->GetMemento()->IsDead() == FALSE &&
				pPlayer->GetMemento()->IsSpectator() == FALSE )
			{
				bAlive		= TRUE;
				break;
			}
		}
		pPlayer				= pRoom->GetNextPlayer();
	}

	if( bAlive == TRUE )
	{
		return;
	}

	if( pRoom->GetResult()->IsPlantedBomb() == TRUE &&
		pVictim->GetMemento()->GetTeam() == IPlayer::E_TEAM::RED )
	{
		return;
	}

	pRoom->GetResult()->SetAllKilled();
	IPlayer::E_TEAM::ENUM eWinner;
	if( pVictim->GetMemento()->GetTeam() == IPlayer::E_TEAM::RED )
	{
		eWinner				= IPlayer::E_TEAM::BLUE;
	}
	else
	{
		eWinner				= IPlayer::E_TEAM::RED;
	}
	pRoom->GetResult()->SetWinRound( eWinner );
	Room::IPROCESS()->FinishRound( pRoom );
}

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */