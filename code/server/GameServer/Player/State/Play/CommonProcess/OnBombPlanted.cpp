#include "StdAfx.h"

#include "./OnBombPlanted.h"

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
OnBombPlanted::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Game::CN_BombPlanted
							CN_BOMBPLANTED;
	
	if( i_pMemento->GetPlayer() != i_pMemento->GetRoom()->GetSuperPeer() )
	{
		return;
	}

	CN_BOMBPLANTED * pReq	= static_cast<CN_BOMBPLANTED *>( i_pPacket );

	IRoom * pRoom			= i_pMemento->GetRoom();

	if( pRoom->GetResult()->IsPlantedBomb() == TRUE )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "Bomb Already Planted\n" ) );
		return;
	}

	IPlayer * pPlanter		= pRoom->FindPlayer( pReq->m_nPlanterIndex );
	if( pPlanter == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "Planter Not Found\n" ) );
		return;
	}

	if( pPlanter->GetMemento()->GetTeam() != IPlayer::E_TEAM::RED )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "Blue Planted\n" ) );
		return;
	}

	STRING strLog;
	strLog.Printf( "Result\t%s\t%s\tR(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Planted",
					pRoom->GetIndex(),
					pPlanter->GetMemento()->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	pRoom->GetResult()->PlantBomb( pReq->m_nPlanterIndex );
	pPlanter->GetResult()->Mission();

	typedef					::Packet::Service::Game::SN_BombPlanted
							SN_BOMBPLANTED;
	SN_BOMBPLANTED * pNoti	= ::Packet::Builder::Build<SN_BOMBPLANTED>();
	pNoti->m_nPlayerID		= pReq->m_nPlayerID;
	pNoti->m_fDetonateTimer	= pReq->m_fDetonateTimer;
	pNoti->m_fTimeBombPosX	= pReq->m_fTimeBombPosX;
	pNoti->m_fTimeBombPosY	= pReq->m_fTimeBombPosY;
	pNoti->m_fTimeBombPosZ	= pReq->m_fTimeBombPosZ;

	IPlayer * pPlayer		= pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY )
		{
			ICASTER()->Cast( pPlayer, pNoti );
		}
		pPlayer				= pRoom->GetNextPlayer();
	}
}

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */