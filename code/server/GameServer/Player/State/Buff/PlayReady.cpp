#include "StdAfx.h"

#include "./PlayReadyProcess/OnHost.h"
#include "./PlayReady.h"
#include "./BuffRoom.h"
#include "../Play/CommonProcess/OnChangeHost.h"
#include "../Play/CommonProcess/OnLoadComplate.h"
#include "../Buff/ReturnBuff.h"
#include "../Play/RespawnRoom.h"
#include "../Play/BombRoom.h"
#include "../Play/BombReady.h"
#include "../Fin.h"
#include "../../Memento.h"
#include "../../../IPlayer.h"
#include "../../../IChannel.h"
#include "../../../IRoom.h"
#include "../../../Room/IResult.h"
#include "../../../Room/IProcess.h"
#include "../../../ICaster.h"
#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Service/Play.h"
#include "../../../../Share/Packet/Service/Player.h"
#include "../../../../Share/Packet/Service/Room.h"
#include "../../../../Share/Packet/Service/Game.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {

PlayReady::PlayReady()
{
	this->install();
}

PlayReady::~PlayReady()
{
	this->uninstall();
}

CBOOL
PlayReady::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				P = ::Packet::Service::Play;
	namespace				G = ::Packet::Service::Game;

	switch( i_pPacket->GetCommand() )
	{
	case P::CN_Host::s_nCode :
		PlayReadyProcess::OnHost::Execute( i_pMemento, i_pPacket );
		break;

	case G::SS_Play::s_nCode:
		this->onPlay( i_pMemento );
		break;

	case P::SS_StartFail::s_nCode :
		i_pMemento->GetPlayer()->RecordResult( i_pMemento->GetRoom()->GetResult()->GetWinnerTeam() );
		this->returnBuff( i_pMemento );
		break;

	case P::CN_Cancel::s_nCode :
		i_pMemento->GetPlayer()->RecordPenalty();
		this->returnBuff( i_pMemento );
		if( Room::IPROCESS()->InspactFinish( i_pMemento->GetRoom() ) == FALSE &&
			i_pMemento->GetPlayer() == i_pMemento->GetRoom()->GetTempSuperPeer() )
		{
			Play::CommonProcess::OnChangeHost::Execute( i_pMemento, i_pMemento->GetRoom() );
		}
		i_pMemento->GetPlayer()->InitPlay();
		break;

	case G::SS_ChangeHost::s_nCode :
		i_pMemento->SetState( Buff::PLAYREADY() );
		break;

	case G::CN_LoadComplate::s_nCode :
		Play::CommonProcess::OnLoadComplate::Execute( i_pMemento, i_pPacket );
		break;

	case G::CN_Exit::s_nCode :
		this->onExit( i_pMemento );
		break;

	case G::SS_End::s_nCode :
		i_pMemento->GetPlayer()->CastResult();
		i_pMemento->GetPlayer()->RecordResult( i_pMemento->GetRoom()->GetResult()->GetWinnerTeam() );
		this->onEnd( i_pMemento );
		break;
	//case G::SS_End::s_nCode :
	//	i_pMemento->GetPlayer()->CastResult();
	//	i_pMemento->GetPlayer()->RecordResult( i_pMemento->GetRoom()->GetResult()->GetWinnerTeam() );
	//	this->onEnd( i_pMemento );
//		i_pMemento->GetPlayer()->InitPlay();
		break;


	default :
		break;
	}

	return TRUE;
}

void
PlayReady::OnClose( Memento * i_pMemento )
{
	namespace				R = ::Packet::Service::Room;

	i_pMemento->GetPlayer()->RecordPenalty();

	IRoom * pOldRoom		= i_pMemento->GetRoom();
	IRoom * pLobby			= i_pMemento->GetChannel()->GetLobby();

	pOldRoom->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( NULL );

	if( pOldRoom->GetPlayerCount() == 0 )
	{
		pOldRoom->GetChannel()->RemoveRoom( pOldRoom );
		IRoom::IFACTORY()->Destroy( pOldRoom );
	}
	else
	{
		if( Room::IPROCESS()->InspactFinish( pOldRoom ) == FALSE )
		{
			if( i_pMemento->GetPlayer() == pOldRoom->GetTempSuperPeer() )
			{
				Play::CommonProcess::OnChangeHost::Execute( i_pMemento, pOldRoom );
			}
		}
	}


	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
PlayReady::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
PlayReady::returnBuff( Memento * i_pMemento )
{
	typedef					::Packet::Service::Play::SN_ReturnBuff
							SN_RETURNBUFF;

	SN_RETURNBUFF * pNoti	= ::Packet::Builder::Build<SN_RETURNBUFF>();
	pNoti->m_byPlayerCount	= i_pMemento->GetRoom()->GetPlayerCount() - 1;
	pNoti->m_byTeam			= i_pMemento->GetTeam();
	pNoti->m_stOption		= i_pMemento->GetRoom()->GetOption();
	
	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );

	i_pMemento->SetState( BUFFROOM() );

	if( i_pMemento->GetPlayer() == i_pMemento->GetRoom()->GetCaptain() )
	{
		Room::IPROCESS()->ChangeCaptain( i_pMemento->GetRoom() );
	}
	Room::IPROCESS()->CastStateForReturn( i_pMemento->GetPlayer(), i_pMemento->GetRoom() );
}

inline void
PlayReady::onEnd( Memento * i_pMemento )
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

	i_pMemento->SetState( Buff::BUFFROOM() );
	
	Room::IPROCESS()->CastStateForReturn( i_pMemento->GetPlayer(), i_pMemento->GetRoom() );



//	i_pMemento->SetState( Buff::RETURNBUFF() );



	//typedef					::Packet::Service::Play::SN_ReturnBuff
	//						SN_RETURNBUFF;
	//SN_RETURNBUFF * pNoti	= ::Packet::Builder::Build<SN_RETURNBUFF>();
	//pNoti->m_byPlayerCount	= static_cast<BYTE>( i_pMemento->GetRoom()->GetPlayerCount() );
	//pNoti->m_byTeam			= i_pMemento->GetTeam();
	//pNoti->m_stOption		= i_pMemento->GetRoom()->GetOption();
	//ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );

	//i_pMemento->SetState( Buff::BUFFROOM() );
	//
	//Room::IPROCESS()->CastStateForReturn( i_pMemento->GetPlayer(), i_pMemento->GetRoom() );
}

inline void
PlayReady::onPlay( Memento * i_pMemento )
{
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

//	i_pMemento->SetSpectator( FALSE );
	i_pMemento->SetIntrusion( FALSE );

	switch( i_pMemento->GetRoom()->GetOption().GetMode() )
	{
	case E_MODE::RESPAWN :
		i_pMemento->SetState( Play::RESPAWNROOM() );
		break;
	
	case E_MODE::BOMB :
		if( i_pMemento->GetRoom()->IsBombReady() == TRUE )
		{
			i_pMemento->SetState( Play::BOMBREADY() );
		}
		else
		{
			i_pMemento->SetState( Play::BOMBROOM() );
		}
		break;
	
	default:
		DEBUG_BREAK;
		break;
	}

}

inline void
PlayReady::onExit( Memento * i_pMemento )
{
	i_pMemento->GetPlayer()->RecordPenalty();
	i_pMemento->GetPlayer()->InitPlay();

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

	if( Room::IPROCESS()->InspactFinish( i_pMemento->GetRoom() ) == TRUE )
	{
		return;
	}
	if( i_pMemento->GetPlayer() == i_pMemento->GetRoom()->GetCaptain() )
	{
		Room::IPROCESS()->ChangeCaptain( i_pMemento->GetRoom() );
	}
	if( i_pMemento->GetPlayer() == i_pMemento->GetRoom()->GetTempSuperPeer() )
	{
		Play::CommonProcess::OnChangeHost::Execute( i_pMemento, i_pMemento->GetRoom() );
	}
}

} /* Buff */
} /* State */				} /* Player */				} /* GameServer */