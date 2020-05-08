#include "StdAfx.h"

#include "./BuffRoom.h"
#include "./PlayReady.h"
#include "./Inventory.h"
#include "../Login/Lobby.h"
#include "../Play/RespawnRoom.h"
#include "../Play/BombRoom.h"
#include "../Fin.h"

#include "../CommonProcess/OnWhisper.h"
#include "../CommonProcess/OnFind.h"
#include "./BuffRoomProcess/OnChangeOption.h"
#include "./BuffRoomProcess/OnChangeTeam.h"
#include "./BuffRoomProcess/OnReady.h"
#include "./BuffRoomProcess/OnStart.h"
#include "./BuffRoomProcess/OnChat.h"

#include "../../../IChannel.h"
#include "../../../IRoom.h"
#include "../../../Room/IProcess.h"
#include "../../../Player/IResult.h"
#include "../../../ICaster.h"

#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Service/Play.h"
#include "../../../../Share/Packet/Service/Player.h"
#include "../../../../Share/Packet/Service/Room.h"
#include "../../../../Share/Packet/Service/Game.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {

BuffRoom::BuffRoom()
{
	this->install();
}

BuffRoom::~BuffRoom()
{
	this->uninstall();
}

CBOOL
BuffRoom::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				P = ::Packet::Service::Play;
	namespace				R = ::Packet::Service::Room;
	namespace				U = ::Packet::Service::Player;
	namespace				G = ::Packet::Service::Game;

	switch( i_pPacket->GetCommand() )
	{
	case R::NN_ChangeOption::s_nCode :
		BuffRoomProcess::OnChangeOption::Execute( i_pMemento, i_pPacket );
		break;

	case R::CQ_Away::s_nCode :
		this->onAway( i_pMemento, i_pPacket );
		break;

	case R::NN_ChangeTitle::s_nCode :
		this->onChangeTitle( i_pMemento, i_pPacket );
		break;

	case R::CQ_ChangeCaptain::s_nCode :
		this->onChangeCaptain( i_pMemento, i_pPacket );
		break;

	case R::NN_Chat::s_nCode :
		BuffRoomProcess::OnChat::Execute( i_pMemento, i_pPacket );
		break;

	case P::NN_Ready::s_nCode :	//非caption准备
		BuffRoomProcess::OnReady::Execute( i_pMemento, i_pPacket );
		break;

	case P::NN_Inventory::s_nCode :
		this->onInventory( i_pMemento, i_pPacket );
		break;

	case P::CN_Start::s_nCode ://caption准备
		BuffRoomProcess::OnStart::Execute( i_pMemento, i_pPacket );
		break;

	case P::SS_Start::s_nCode ://caption准备中触发的signal
		this->onStart( i_pMemento );
		break;

	case U::NN_ChangeTeam::s_nCode :
		BuffRoomProcess::OnChangeTeam::Execute( i_pMemento, i_pPacket );
		break;

	case U::NN_Whisper::s_nCode :
		CommonProcess::OnWhisper::Execute( i_pMemento, i_pPacket );
		break;

	case U::CQ_Find::s_nCode :
		CommonProcess::OnFind::Execute( i_pMemento, i_pPacket );
		break;

	case U::CN_Out::s_nCode :
		this->onOut( i_pMemento, i_pPacket );
		break;

	case U::SS_Out::s_nCode :
		this->onOut( i_pMemento );
		break;

	case U::CN_Info::s_nCode :
		this->onInfo( i_pMemento, i_pPacket );
		break;

	case U::SS_ChangeTeam::s_nCode :
		this->onChangeTeam( i_pMemento );
		break;

	case G::SS_Play::s_nCode :
		this->onPlay( i_pMemento );
		break;

	case G::SS_End::s_nCode :
		this->onEnd( i_pMemento );
		break;
	case P::CQ_ViewLobby::s_nCode:	//服务器没有，查询lobby中的角色信息
		OnViewLobby(i_pMemento );
		break;

	default:
		return TRUE;
	}

	return TRUE;
}

void
BuffRoom::OnClose( Memento * i_pMemento )
{
	namespace				R = ::Packet::Service::Room;
	namespace				P = ::Packet::Service::Player;

	IRoom * pOldRoom		= i_pMemento->GetRoom();
	IRoom * pLobby			= i_pMemento->GetChannel()->GetLobby();

	pOldRoom->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetRoom( NULL );

	if( pOldRoom->GetPlayerCount() == 0 )
	{
		pOldRoom->GetChannel()->RemoveRoom( pOldRoom );
		IRoom::IFACTORY()->Destroy( pOldRoom );
	}

	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
BuffRoom::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
BuffRoom::onInventory( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Play::NN_Inventory
							NN_INVENTORY;
	Assert( i_pPacket->GetCommand() == NN_INVENTORY::s_nCode );

	NN_INVENTORY * pRelay	= static_cast<NN_INVENTORY *>( i_pPacket );

	if( pRelay->m_bInventory == false )
	{
		return;
	}

	if( pRelay->m_nIndex != i_pMemento->GetPlayer()->GetIndex() )
	{
		return;
	}

	if( i_pMemento->IsReady() == TRUE )
	{
		i_pMemento->SetReady( FALSE );
//		return;
	}

	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() == IPlayer::E_STATE::BUFF ||
			pPlayer->GetState() == IPlayer::E_STATE::READY )
		{
			ICASTER()->Cast( pPlayer, pRelay );
		}
		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}
	i_pMemento->SetState( INVENTORY() );
	
}

inline void
BuffRoom::onAway( Memento * i_pMemento, PACKET * i_pPacket )
{
#ifdef _DEVDEBUG
RETURN:
	IRoom * pRoom			= i_pMemento->GetChannel()->GetFirstRoom();
	while( pRoom != NULL )
	{
		if( pRoom->GetPlayerCount() == 0 )
		{
			i_pMemento->GetChannel()->RemoveRoom( pRoom );
			goto RETURN;
		}
		pRoom				= i_pMemento->GetChannel()->GetNextRoom();
	}
#endif

	namespace				R = ::Packet::Service::Room;
	namespace				P = ::Packet::Service::Player;
	R::SA_Away * pAck		= ::Packet::Builder::Build<R::SA_Away>();

	IRoom * pOldRoom		= i_pMemento->GetRoom();
	IRoom * pLobby			= i_pMemento->GetChannel()->GetLobby();
	pAck->m_nRoomCount		= i_pMemento->GetChannel()->GetRoomCount();
	pAck->m_nUserCount		= pLobby->GetPlayerCount();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );

	pLobby->AddPlayer( i_pMemento->GetPlayer() );
	pOldRoom->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetReady( FALSE );
	i_pMemento->SetRoom( pLobby );

	if( pOldRoom->GetPlayerCount() == 0 )
	{
		pOldRoom->GetChannel()->RemoveRoom( pOldRoom );
		IRoom::IFACTORY()->Destroy( pOldRoom );
	}

	i_pMemento->SetState( Login::LOBBY() );
}

inline void
BuffRoom::onStart( Memento * i_pMemento )
{
	typedef					::Packet::Service::Play::SN_PlayReady
							SN_PLAYREADY;

	if( i_pMemento->IsReady() == FALSE )
	{
		typedef				::Packet::Service::Room::SN_Play
							SN_PLAY;
		SN_PLAY * pNoti		= ::Packet::Builder::Build<SN_PLAY>();
		pNoti->m_nRoomIndex	= i_pMemento->GetRoom()->GetIndex();
		ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
		return;
	}

	i_pMemento->GetPlayer()->InitPlay();

	SN_PLAYREADY * pNoti	= ::Packet::Builder::Build<SN_PLAYREADY>();

	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );

	if( i_pMemento->GetPlayer() == i_pMemento->GetRoom()->GetTempSuperPeer() )
	{
		typedef				::Packet::Service::Play::SN_Host
							SN_HOST;
		SN_HOST * pNoti		= ::Packet::Builder::Build<SN_HOST>();
		ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
	}

	i_pMemento->SetState( PLAYREADY() );

	i_pMemento->SetIntrusion( FALSE );

	Room::IPROCESS()->CastStateForPlay( i_pMemento->GetPlayer(), i_pMemento->GetRoom() );

}

inline void
BuffRoom::onEnd( Memento * i_pMemento )
{
	typedef					::Packet::Service::Room::SN_Buff
							SN_BUFF;
	SN_BUFF * pNoti			= ::Packet::Builder::Build<SN_BUFF>();
	pNoti->m_nRoomIndex		= i_pMemento->GetRoom()->GetIndex();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
}

inline void
BuffRoom::onPlay( Memento * i_pMemento )
{
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

	i_pMemento->GetPlayer()->InitPlay();

	switch( i_pMemento->GetRoom()->GetOption().GetMode() )
	{
	case E_MODE::RESPAWN :
		i_pMemento->SetState( Play::RESPAWNROOM() );
		break;
	
	case E_MODE::BOMB :
		i_pMemento->SetState( Play::BOMBROOM() );
		break;
	
	default:
		DEBUG_BREAK;
		break;
	}

}

inline void
BuffRoom::onOut( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				P = ::Packet::Service::Player;
	Assert( i_pPacket->GetCommand() == P::CN_Out::s_nCode );

	if( i_pMemento->GetPlayer() != i_pMemento->GetRoom()->GetCaptain() )
	{
		return;
	}

	P::CN_Out * pReq		= static_cast<P::CN_Out *>( i_pPacket );
	IPlayer * pPlayer		=i_pMemento->GetRoom()->FindPlayer( pReq->m_nIndex );
	if( pPlayer == NULL )
	{
		return;
	}

	ICASTER()->Signal( pPlayer, P::SS_Out::s_nCode );
}

inline void
BuffRoom::onOut( Memento * i_pMemento )
{
	namespace				P = ::Packet::Service::Player;
	P::SN_Out * pAck		= ::Packet::Builder::Build<P::SN_Out>();

	IRoom * pOldRoom		= i_pMemento->GetRoom();
	IRoom * pLobby			= i_pMemento->GetChannel()->GetLobby();
	pAck->m_nRoomCount		= i_pMemento->GetChannel()->GetRoomCount();
	pAck->m_nUserCount		= pLobby->GetPlayerCount();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );

	pLobby->AddPlayer( i_pMemento->GetPlayer() );
	pOldRoom->RemovePlayer( i_pMemento->GetPlayer() );
	i_pMemento->SetReady( FALSE );
	i_pMemento->SetRoom( pLobby );

	if( pOldRoom->GetPlayerCount() == 0 )
	{
		pOldRoom->GetChannel()->RemoveRoom( pOldRoom );
		IRoom::IFACTORY()->Destroy( pOldRoom );
	}

	i_pMemento->SetState( Login::LOBBY() );
}

inline void
BuffRoom::onInfo( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				P = ::Packet::Service::Player;
	Assert( i_pPacket->GetCommand() == P::CN_Info::s_nCode );

	P::CN_Info * pReq		= static_cast<P::CN_Info *>( i_pPacket );
	IPlayer * pPlayer		= i_pMemento->GetRoom()->FindPlayer( pReq->m_nIndex );
	if( pPlayer == NULL )
	{
		return;
	}

	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	P::SN_Info * pAck		= ::Packet::Builder::Build<P::SN_Info>();
	pAck->m_nIndex			= pPlayer->GetIndex();
	pAck->m_stRed			= pPlayer->GetMemento()->GetCharacters()[ E_TEAM::RED ];
	pAck->m_stBlue			= pPlayer->GetMemento()->GetCharacters()[ E_TEAM::BLUE ];
	pAck->m_stResult		= * pPlayer->GetResult()->GetTotalResult();

	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
}

inline void
BuffRoom::onChangeTeam( Memento * i_pMemento )
{
	const IPlayer::E_TEAM::ENUM
	eSourceTeam				= i_pMemento->GetTeam();
	
	IPlayer::E_TEAM::ENUM	eDestTeam;
	if( eSourceTeam == IPlayer::E_TEAM::RED )
	{
		eDestTeam			= IPlayer::E_TEAM::BLUE;
	}
	else
	{
		eDestTeam			= IPlayer::E_TEAM::RED;
	}

	UINT nDestTeam			= 0;
	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->GetTeam() == eDestTeam )
		{
			nDestTeam ++;
		}
		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}

	if( i_pMemento->GetRoom()->GetOption().GetTeamMax() <= nDestTeam )
	{
		return;
	}

	i_pMemento->SetTeam( eDestTeam );

	namespace				P = ::Packet::Service::Player;
	
	P::SN_ChangeTeamOnStart *
	pNoti					= ::Packet::Builder::Build<P::SN_ChangeTeamOnStart>();
	pNoti->m_nIndex			= i_pMemento->GetPlayer()->GetIndex();
	pNoti->m_byTeam			= i_pMemento->GetTeam();
	ICASTER()->Cast( i_pMemento->GetRoom(), pNoti );

}

inline void
BuffRoom::onChangeTitle( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				R = ::Packet::Service::Room;

	if( i_pMemento->GetPlayer() != i_pMemento->GetRoom()->GetCaptain() )
	{
		return;
	}

	R::NN_ChangeTitle *
	pRelay					= static_cast<R::NN_ChangeTitle *>( i_pPacket );
	i_pMemento->GetRoom()->SetName( IRoom::CNAME( pRelay->m_strTitle ) );
	ICASTER()->Cast( i_pMemento->GetRoom(), pRelay );

	R::SN_ChangeTitle *
	pNoti					= ::Packet::Builder::Build<R::SN_ChangeTitle>();
	pNoti->m_nRoomIndex		= i_pMemento->GetRoom()->GetIndex();
	pNoti->Set( pRelay->m_strTitle );
	ICASTER()->Cast( i_pMemento->GetChannel()->GetLobby(), pNoti );

}

inline void
BuffRoom::onChangeCaptain( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				R = ::Packet::Service::Room;

	if( i_pMemento->GetPlayer() != i_pMemento->GetRoom()->GetCaptain() )
	{
		R::SA_ChangeCaptain *
		pAck				= ::Packet::Builder::Build<R::SA_ChangeCaptain>();
		pAck->m_byResult	= R::SA_ChangeCaptain::E_RESULT::FAIL_NOT_CAPTAIN;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	R::CQ_ChangeCaptain *
	pReq					= static_cast<R::CQ_ChangeCaptain *>( i_pPacket );

	IPlayer *
	pChangeCaptain			= i_pMemento->GetRoom()->FindPlayer( pReq->m_nPlayerIndex );

	if( pChangeCaptain == NULL )
	{
		R::SA_ChangeCaptain *
		pAck				= ::Packet::Builder::Build<R::SA_ChangeCaptain>();
		pAck->m_byResult	= R::SA_ChangeCaptain::E_RESULT::FAIL_NOT_EXIST;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}
	if( pChangeCaptain == i_pMemento->GetPlayer() )
	{
		R::SA_ChangeCaptain *
		pAck				= ::Packet::Builder::Build<R::SA_ChangeCaptain>();
		pAck->m_byResult	= R::SA_ChangeCaptain::E_RESULT::FAIL_CAPTAIN_INDEX;
		ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );
		return;
	}

	i_pMemento->GetRoom()->SetCaptain( pChangeCaptain );

	R::SA_ChangeCaptain *
	pAck					= ::Packet::Builder::Build<R::SA_ChangeCaptain>();
	pAck->m_byResult		= R::SA_ChangeCaptain::E_RESULT::SUCCESS;
	ICASTER()->Cast( i_pMemento->GetPlayer(), pAck );

	R::NN_ChangeCaptain *
	pNotiRoom				= ::Packet::Builder::Build<R::NN_ChangeCaptain>();
	pNotiRoom->m_nCaptainIndex
							= i_pMemento->GetRoom()->GetCaptain()->GetIndex();
	ICASTER()->Cast( i_pMemento->GetRoom(), pNotiRoom );

	R::SN_ChangeCaptain *
	pNotiLobby				= ::Packet::Builder::Build<R::SN_ChangeCaptain>();
	pNotiLobby->m_nRoomIndex= i_pMemento->GetRoom()->GetIndex();
	pNotiLobby->SetNick( i_pMemento->GetRoom()->GetCaptain()->GetMemento()->GetStock().GetNick() );
	ICASTER()->Cast( i_pMemento->GetChannel()->GetLobby(), pNotiLobby );
}

inline void
BuffRoom::OnViewLobby( Memento * i_pMemento)
{
	namespace				P = ::Packet::Service::Play;

	P::SA_ViewLobby * pReq	= ::Packet::Builder::Build<P::SA_ViewLobby>();
	pReq->m_byResult		= P::SA_ViewLobby::E_RESULT::SUCCESS;
	pReq->m_nCount			= i_pMemento->GetChannel()->GetLobby()->GetPlayerCount();

	ICASTER()->Cast( i_pMemento->GetPlayer(), pReq );


}

} /* Buff */
} /* State */				} /* Player */				} /* GameServer */