#include "StdAfx.h"

#include "./OnBomb.h"

#include "../../../../IRoom.h"
#include "../../../../ICaster.h"
#include "../../../../Room/IResult.h"
#include "../../../../Room/IProcess.h"
#include "../../../../Player/IResult.h"
#include "../../../../../Share/Packet/Builder.h"
#include "../../../../../Share/Packet/Service/Game.h"

#include "../../../../../Dispatcher/IConsole.h"
#include "../../../../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

void
OnBomb::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Game::CN_Bomb
							CN_BOMB;
	
	if( i_pMemento->GetPlayer() != i_pMemento->GetRoom()->GetSuperPeer() )
	{
		return;
	}

	CN_BOMB * pReq			= static_cast<CN_BOMB *>( i_pPacket );
	switch( pReq->m_byType )
	{
	case CN_BOMB::E_TYPE::DEFUSED :
		OnBomb::onDefused( i_pMemento->GetRoom(), pReq->m_nPlayer );
		break;

	case CN_BOMB::E_TYPE::DETONATED :
		OnBomb::onDetonated( i_pMemento->GetRoom(), pReq->m_nPlayer );
		break;

	default :
		::Dispatcher::ICONSOLE()->Output( STRING( "OnBomb Wrong Type\n" ) );
		return;
	}
}

void
OnBomb::onDefused( IRoom * i_pRoom, CINT i_nPlayer )
{
	if( i_pRoom->GetResult()->IsPlantedBomb() == FALSE )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnDefused : Bomb Is Not Planted\n" ) );
		return;
	}

	IPlayer * pDefuser		= i_pRoom->FindPlayer( i_nPlayer );
	if( pDefuser == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "Defuser Not Exist\n" ) );
		return;
	}
	if( pDefuser->GetMemento()->GetTeam() != IPlayer::E_TEAM::BLUE )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "Red Defused\n" ) );
		return;
	}

	typedef					::Packet::Service::Game::SN_Bomb
							SN_BOMB;
	SN_BOMB * pNoti			= ::Packet::Builder::Build<SN_BOMB>();
	pNoti->m_byType			= SN_BOMB::E_TYPE::DEFUSED;
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY )
		{
			ICASTER()->Cast( pPlayer, pNoti );
		}
		pPlayer				= i_pRoom->GetNextPlayer();
	}

	STRING strLog;
	strLog.Printf( "Result\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Defused",
					i_pRoom->GetIndex(),
					pDefuser->GetMemento()->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	i_pRoom->GetResult()->DefusedBomb( i_nPlayer );
	pDefuser->GetResult()->Mission();

	i_pRoom->GetResult()->SetWinRound( IPlayer::E_TEAM::BLUE );
	Room::IPROCESS()->FinishRound( i_pRoom );
}

void
OnBomb::onDetonated( IRoom * i_pRoom, CINT i_nPlayer )
{
	if( i_pRoom->GetResult()->IsPlantedBomb() == FALSE )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnDefused : Bomb Is Not Planted\n" ) );
		return;
	}

	typedef					::Packet::Service::Game::SN_Bomb
							SN_BOMB;
	SN_BOMB * pNoti			= ::Packet::Builder::Build<SN_BOMB>();
	pNoti->m_byType			= SN_BOMB::E_TYPE::DETONATED;
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY )
		{
			ICASTER()->Cast( pPlayer, pNoti );
		}
		pPlayer				= i_pRoom->GetNextPlayer();
	}

	STRING strLog;
	strLog.Printf( "Result\t%s\t%s\tR(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Detonated",
					i_pRoom->GetIndex() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	i_pRoom->GetResult()->DetonatedBomb();
	i_pRoom->GetResult()->SetWinRound( IPlayer::E_TEAM::RED );
	Room::IPROCESS()->FinishRound( i_pRoom );
}

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */