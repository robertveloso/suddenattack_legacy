#include "StdAfx.h"

#include "./OnReady.h"

#include "../../../Memento.h"
#include "../../../IState.h"
#include "../../../State/Buff/PlayReady.h"
#include "../../../../IRoom.h"
#include "../../../../ICaster.h"
#include "../../../../Room/IResult.h"
#include "../../../../Room/IProcess.h"
#include "../../../../Player/IResult.h"
#include "../../../../../Share/Packet/Builder.h"
#include "../../../../../Share/Packet/Service/Room.h"
#include "../../../../../Share/Packet/Service/Play.h"
#include "../../../../../Share/Packet/Service/Game.h"
#include "../../../../../Share/LogicObject/Room/Option.h"

#include "../../../../../Dispatcher/IConsole.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

void
OnReady::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	if( i_pMemento->GetPlayer() == i_pMemento->GetRoom()->GetCaptain() )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "WARNNING : OnReady 不能用于caption\n" ) );
		return;
	}

	typedef					::Packet::Service::Play::NN_Ready
							NN_READY;
	NN_READY * pRelay		= static_cast<NN_READY *>( i_pPacket );
	Assert( i_pPacket->GetCommand() == NN_READY::s_nCode );

	if( pRelay->m_nIndex != i_pMemento->GetPlayer()->GetIndex() )
	{
		return;
	}

	if( i_pMemento->GetRoom()->GetOption().IsPlaying() == TRUE )
	{
		if( pRelay->m_bReady == FALSE )
		{
			return;
		}
		i_pMemento->SetReady( pRelay->m_bReady );
		if( i_pMemento->GetRoom()->GetSuperPeer() == NULL ||
			i_pMemento->GetRoom()->IsPause() == TRUE )
		{
			OnReady::doStart( i_pMemento );
		}
		else
		{
			OnReady::doGame( i_pMemento );
		}
	}
	else
	{
		if( i_pMemento->IsReady() == static_cast<BOOL>( pRelay->m_bReady ) )
		{
			return;
		}
		i_pMemento->SetReady( pRelay->m_bReady );
		ICASTER()->Cast( i_pMemento->GetRoom(), pRelay );
	}
}

inline void
OnReady::doStart( Memento * i_pMemento )
{
	namespace				P = ::Packet::Service::Play;

	ICASTER()->Signal( i_pMemento->GetPlayer(), P::SS_Start::s_nCode );
}

inline void
OnReady::doGame( Memento * i_pMemento )
{
	if( i_pMemento->GetRoom()->IsNat() == TRUE )
	{
		if( i_pMemento->GetIP() != i_pMemento->GetRoom()->GetCaptain()->GetMemento()->GetIP() )
		{
			typedef				::Packet::Service::Room::SN_JoinPlayFail
								SN_PLAYFAIL;
			SN_PLAYFAIL * pNoti	= ::Packet::Builder::Build<SN_PLAYFAIL>();
			ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
			
			i_pMemento->SetReady( FALSE );
			return;
		}
	}


	namespace				P = ::Packet::Service::Play;
	namespace				G = ::Packet::Service::Game;
	typedef					::LogicObject::Player::Type::E_STATE
							E_STATE;

	i_pMemento->SetSpectator( i_pMemento->GetRoom()->IsSpectatorTick() );
	i_pMemento->SetIntrusion( TRUE );

	G::SN_Play * pPlay		= ::Packet::Builder::Build<G::SN_Play>();
	pPlay->m_nHostIndex		= i_pMemento->GetRoom()->GetSuperPeer()->GetIndex();
	pPlay->m_nPort			= i_pMemento->GetRoom()->GetSuperPeer()->GetMemento()->GetPort();
	pPlay->m_bSpectator		= i_pMemento->IsSpectator() == TRUE;
	pPlay->SetIP( i_pMemento->GetRoom()->GetSuperPeer()->GetMemento()->GetClientIP().GetCSTR() );
	ICASTER()->Cast( i_pMemento->GetPlayer(), pPlay );

	//G::SN_CurrentRoundResult * pResult = ::Packet::Builder::Build<G::SN_CurrentRoundResult>();
	//pResult->m_byRedWin		= i_pMemento->GetRoom()->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::RED );
	//pResult->m_byBlueWin	= i_pMemento->GetRoom()->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::BLUE );
	//ICASTER()->Cast( i_pMemento->GetPlayer(), pResult );

	//P::SN_State * pNoti		= ::Packet::Builder::Build<P::SN_State>();
	//pNoti->m_nIndex			= i_pMemento->GetPlayer()->GetIndex();
	//pNoti->m_byState		= IPlayer::E_STATE::PLAY;
	//pNoti->m_byGrade		= i_pMemento->GetStock().m_byGrade;
	//IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	//while( pPlayer != NULL )
	//{
	//	if( pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY )
	//	{
	//		ICASTER()->Cast( pPlayer, pNoti );
	//	}
	//	pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	//}
	if( i_pMemento->GetRoom()->GetOption().GetMode() == IRoom::COPTION::E_MODE::RESPAWN )
	{
		G::SN_RespawnInfo *
		pInfo				= ::Packet::Builder::Build<G::SN_RespawnInfo>();
		pInfo->m_nRedKill	= i_pMemento->GetRoom()->GetResult()->GetKillCount( IPlayer::E_TEAM::RED );
		pInfo->m_nRedDead	= i_pMemento->GetRoom()->GetResult()->GetDeadCount( IPlayer::E_TEAM::RED );
		pInfo->m_nBlueKill	= i_pMemento->GetRoom()->GetResult()->GetKillCount( IPlayer::E_TEAM::BLUE );
		pInfo->m_nBlueDead	= i_pMemento->GetRoom()->GetResult()->GetDeadCount( IPlayer::E_TEAM::BLUE );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pInfo );
	}
	else if( i_pMemento->GetRoom()->GetOption().GetMode() == IRoom::COPTION::E_MODE::BOMB )
	{
		G::SN_BombInfo *
		pInfo				= ::Packet::Builder::Build<G::SN_BombInfo>();
		pInfo->m_byRound	= i_pMemento->GetRoom()->GetResult()->GetRound();
		pInfo->m_byRedWin	= i_pMemento->GetRoom()->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::RED );
		pInfo->m_byBlueWin	= i_pMemento->GetRoom()->GetResult()->GetWinRoundCount( IPlayer::E_TEAM::BLUE );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pInfo );
	}
	else
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnReady : Wrong Room Option Mode\r\n" ) );
		DEBUG_BREAK;
	}

	i_pMemento->GetPlayer()->GetResult()->Reset();
	i_pMemento->SetState( Buff::PLAYREADY() );
	Room::IPROCESS()->CastStateForPlay( i_pMemento->GetPlayer(), i_pMemento->GetRoom() );

	ICASTER()->Signal( i_pMemento->GetPlayer(), G::SS_Play::s_nCode );
}

} /* BuffRoomProcess */		} /* Buff */
} /* State */				} /* Player */				} /* GameServer */