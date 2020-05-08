#include "StdAfx.h"

#include "./OnSuicide.h"

#include "../../../../IRoom.h"
#include "../../../../ICaster.h"
#include "../../../../Room/IResult.h"
#include "../../../../Player/IResult.h"
#include "../../../../Room/IProcess.h"
#include "../../../../../Share/Packet/Service/Game.h"

#include "../../../../../Dispatcher/IConsole.h"
#include "../../../../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

void
OnSuicide::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

	switch( i_pMemento->GetRoom()->GetOption().GetMode() )
	{
	case E_MODE::RESPAWN :
		OnSuicide::onRespawn( i_pMemento, i_pPacket );
		break;

	case E_MODE::BOMB :
		OnSuicide::onBomb( i_pMemento, i_pPacket );
		break;

	default :
		break;
	}
}

inline void
OnSuicide::onRespawn( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Game::NN_Suicide
							NN_SUICIDE;

	Assert( i_pPacket->GetCommand() == NN_SUICIDE::s_nCode );

	IRoom * pRoom			= i_pMemento->GetRoom();
	if( i_pMemento->GetPlayer() != pRoom->GetSuperPeer() )
	{
		return;
	}

	NN_SUICIDE * pReq		= static_cast<NN_SUICIDE *>( i_pPacket );
	
	IPlayer * pVictim		= pRoom->FindPlayer( pReq->m_nVictim );
	if( pVictim == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnSuicide : Victim Index Not Found\n" ) );
		return;
	}

	STRING strLog;
	strLog.Printf( "Result\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Siucide",
					pRoom->GetIndex(),
					pVictim->GetMemento()->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	pVictim->GetResult()->Dead( FALSE );
	pVictim->GetResult()->Suicide();
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

	CUINT nTotalDead		= pRoom->GetResult()->GetKillCount( pVictim->GetMemento()->GetTeam() );
	if( nTotalDead >= pRoom->GetOption().GetKillLimit() )
	{
		Room::IPROCESS()->FinishRound( i_pMemento->GetRoom() );
		return;
	}

}

inline void
OnSuicide::onBomb( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Game::NN_Suicide
							NN_SUICIDE;

	Assert( i_pPacket->GetCommand() == NN_SUICIDE::s_nCode );

	IRoom * pRoom			= i_pMemento->GetRoom();
	if( i_pMemento->GetPlayer() != pRoom->GetSuperPeer() )
	{
		return;
	}

	NN_SUICIDE * pReq		= static_cast<NN_SUICIDE *>( i_pPacket );
	
	IPlayer * pVictim		= pRoom->FindPlayer( pReq->m_nVictim );
	if( pVictim == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnSuicide : Victim Index Not Found\n" ) );
		return;
	}

	if( pVictim->GetMemento()->IsDead() == TRUE )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnKill : Already Dead\n" ) );
		return;
	}

	STRING strLog;
	strLog.Printf( "Result\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Siucide",
					pRoom->GetIndex(),
					pVictim->GetMemento()->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	pVictim->GetMemento()->SetDead( TRUE );

	pVictim->GetResult()->Dead( FALSE );
	pVictim->GetResult()->Suicide();
	pVictim->GetMemento()->SetSpectator( TRUE );
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