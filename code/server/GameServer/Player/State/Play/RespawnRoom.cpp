#include "StdAfx.h"

#include "./RespawnRoom.h"
#include "./CommonProcess/OnKill.h"
#include "./CommonProcess/OnSuicide.h"
#include "./CommonProcess/OnChat.h"
#include "./CommonProcess/OnChangeHost.h"
#include "./CommonProcess/OnInit.h"
#include "./CommonProcess/OnLoadComplate.h"
#include "./CommonProcess/OnRelay.h"
#include "./CommonProcess/OnNetLoss.h"
#include "../CommonProcess/OnWhisper.h"
#include "../CommonProcess/OnFind.h"
#include "../Buff/BuffRoom.h"
#include "../Buff/ReturnBuff.h"
#include "../Buff/PlayReady.h"
#include "../Fin.h"
#include "../../Memento.h"
#include "../../../Player/IResult.h"
#include "../../../Room/IResult.h"
#include "../../../Room/IProcess.h"
#include "../../../IChannel.h"
#include "../../../IRoom.h"
#include "../../../ICaster.h"
#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Service/Game.h"
#include "../../../../Share/Packet/Service/Play.h"
#include "../../../../Share/Packet/Service/Player.h"
#include "../../../../Share/Packet/Service/Room.h"

#include "../../../../Dispatcher/IConsole.h"
#include "../../../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {

RespawnRoom::RespawnRoom()
{
	this->install();
}

RespawnRoom::~RespawnRoom()
{
	this->uninstall();
}

CBOOL
RespawnRoom::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				G = ::Packet::Service::Game;
	namespace				P = ::Packet::Service::Player;
	namespace				R = ::Packet::Service::Room;

	switch( i_pPacket->GetCommand() )
	{
	case G::NN_Kill::s_nCode :
		CommonProcess::OnKill::Execute( i_pMemento, i_pPacket );
		break;

	case G::NN_Suicide::s_nCode :
		CommonProcess::OnSuicide::Execute( i_pMemento, i_pPacket );
		break;

	case G::CN_Timeout::s_nCode :
		this->onTimeout( i_pMemento );
		break;

	case G::SS_End::s_nCode :
		i_pMemento->GetPlayer()->CastResult();
		i_pMemento->GetPlayer()->RecordResult( i_pMemento->GetRoom()->GetResult()->GetWinnerTeam() );
		this->onEnd( i_pMemento );
		break;

	case G::CN_Exit::s_nCode :
		i_pMemento->GetPlayer()->RecordPenalty();
		i_pMemento->GetPlayer()->InitPlay();
		this->onEnd( i_pMemento );
		if( Room::IPROCESS()->InspactFinish( i_pMemento->GetRoom() ) == TRUE )
		{
			break;
		}
		if( i_pMemento->GetPlayer() == i_pMemento->GetRoom()->GetCaptain() )
		{
			Room::IPROCESS()->ChangeCaptain( i_pMemento->GetRoom() );
		}
		if( i_pMemento->GetPlayer() == i_pMemento->GetRoom()->GetSuperPeer() )
		{
			CommonProcess::OnChangeHost::Execute( i_pMemento, i_pMemento->GetRoom() );
		}
		break;

	case G::NN_Chat::s_nCode :
		CommonProcess::OnChat::Execute( i_pMemento, i_pPacket );
		break;

	case P::NN_Whisper::s_nCode :
		State::CommonProcess::OnWhisper::Execute( i_pMemento, i_pPacket );
		break;

	case P::CQ_Find::s_nCode :
		State::CommonProcess::OnFind::Execute( i_pMemento, i_pPacket );
		break;

	case G::SS_ChangeHost::s_nCode :
		this->onChangeHost( i_pMemento );
		break;

	case G::CN_LoadComplate::s_nCode :
		CommonProcess::OnLoadComplate::Execute( i_pMemento, i_pPacket );
		break;

	case R::SS_Init::s_nCode :
		CommonProcess::OnInit::Execute( i_pMemento );
		break;

	case G::NN_Radio::s_nCode :
	case G::NN_WeaponChange::s_nCode :
		CommonProcess::OnRelay::Execute( i_pMemento, i_pPacket );
		break;

	case G::CN_NetLoss::s_nCode :
		CommonProcess::OnNetLoss::Execute( i_pMemento, i_pPacket );
		break;

	default :
		break;
	}

	return TRUE;
}

void
RespawnRoom::OnClose( Memento * i_pMemento )
{
	namespace				R = ::Packet::Service::Room;
	namespace				P = ::Packet::Service::Player;

	IRoom * pOldRoom		= i_pMemento->GetRoom();
	IRoom * pLobby			= i_pMemento->GetChannel()->GetLobby();

	i_pMemento->GetPlayer()->RecordPenalty();
	pOldRoom->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( NULL );

	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );

	if( pOldRoom->GetPlayerCount() == 0 )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "Room Count is 0\r\n" ) );
		pOldRoom->GetChannel()->RemoveRoom( pOldRoom );
		IRoom::IFACTORY()->Destroy( pOldRoom );
		return;
	}

	if( Room::IPROCESS()->InspactFinish( pOldRoom ) == TRUE )
	{
		return;
	}

#ifdef _DEVDEBUG
	if( pOldRoom->GetPlayerCount() == 0 )
	{
		pOldRoom->GetChannel()->RemoveRoom( pOldRoom );
		IRoom::IFACTORY()->Destroy( pOldRoom );
	}
#endif

	if( i_pMemento->GetPlayer() == pOldRoom->GetSuperPeer() )
	{
		CommonProcess::OnChangeHost::Execute( i_pMemento, pOldRoom );
	}

}

void
RespawnRoom::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
RespawnRoom::onEnd( Memento * i_pMemento )
{
	if( i_pMemento->IsLoading() == TRUE )
	{
		i_pMemento->SetState( Buff::RETURNBUFF() );
		return;
	}

	typedef					::Packet::Service::Play::SN_ReturnBuff
							SN_RETURNBUFF;
	SN_RETURNBUFF * pNoti	= ::Packet::Builder::Build<SN_RETURNBUFF>();
	pNoti->m_byPlayerCount	= static_cast<BYTE>( i_pMemento->GetRoom()->GetPlayerCount() - 1 );
	pNoti->m_byTeam			= i_pMemento->GetTeam();
	pNoti->m_stOption		= i_pMemento->GetRoom()->GetOption();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );

	typedef					::Packet::Service::Game::SN_Exit
							SN_EXIT;
	SN_EXIT * pExit			= ::Packet::Builder::Build<SN_EXIT>();
	pExit->m_nPlayerIndex	= i_pMemento->GetPlayer()->GetIndex();
	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY )
		{
			ICASTER()->Cast( pPlayer, pExit );
		}
		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}

	i_pMemento->SetState( Buff::BUFFROOM() );

	Room::IPROCESS()->CastStateForReturn( i_pMemento->GetPlayer(), i_pMemento->GetRoom() );

}

inline void
RespawnRoom::onChangeHost( Memento * i_pMemento )
{
	//typedef					::Packet::Service::Game::SN_Pause
	//						SN_PAUSE;

	//SN_PAUSE * pNoti		= ::Packet::Builder::Build<SN_PAUSE>();

	//ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );

	i_pMemento->SetState( Buff::PLAYREADY() );
}

inline void
RespawnRoom::onTimeout( Memento * i_pMemento )
{
	if( i_pMemento->GetPlayer() != i_pMemento->GetRoom()->GetSuperPeer() )
	{
		return;
	}

	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	IRoom * pRoom			= i_pMemento->GetRoom();

	INT nRed				= pRoom->GetResult()->GetKillCount( E_TEAM::RED )
							- pRoom->GetResult()->GetDeadCount( E_TEAM::RED );

	INT nBlue				= pRoom->GetResult()->GetKillCount( E_TEAM::BLUE )
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

} /* Play */
} /* State */				} /* Player */				} /* GameServer */