#include "StdAfx.h"

#include "./Noti.h"

#include "../../ICaster.h"

#include "../../IChannel.h"
#include "../../IRoom.h"
#include "../../IServer.h"
#include "../../IPlayer.h"

#include "../../../Share/Packet/Builder.h"
#include "../../../Share/Packet/Manager.h"

namespace GameServer {		namespace Manager {			namespace Process {

Noti::Noti()
{
}

Noti::~Noti()
{
}

CBOOL
Noti::Execute( IManager * i_pManager, PACKET * i_pPacket )
{
	namespace				M = ::Packet::Manager;

	Assert( i_pPacket->GetCommand() == M::CN_Noti::s_nCode );

	M::SN_NoticeSuccess *
	pAck					= ::Packet::Builder::Build<M::SN_NoticeSuccess>();
	ICASTER()->Cast( i_pManager, pAck );

	M::CN_Noti * pReq		= static_cast<M::CN_Noti *>( i_pPacket );

	if( pReq->m_byType == M::CN_Noti::E_TYPE::PERSON )
	{
		typedef				IPlayer::IContainer::NICK
							NICK;
		IPlayer * pPlayer	= IPlayer::ICONTAINER()->Find( NICK( pReq->m_strNick ) );
		if( pPlayer == NULL )
		{
			return TRUE;
		}
		M::SN_Noti * pNoti	= ::Packet::Builder::Build<M::SN_Noti>();
		pNoti->SetNoti( pReq->m_strNoti );
		ICASTER()->Cast( pPlayer, pNoti );
		return TRUE;
	}

	IChannel * pChannel		= ISERVER()->GetFirstChannel();
	while( pChannel != NULL )
	{
		IRoom * pRoom		= pChannel->GetFirstRoom();
		while( pRoom != NULL )
		{
			this->cast( pRoom, pReq );

			pRoom			= pChannel->GetNextRoom();
		}
		pChannel			= ISERVER()->GetNextChannel();
	}

	return TRUE;
}

CINT
Noti::GetCode()
{
	namespace				M = ::Packet::Manager;

	return M::CN_Noti::s_nCode;
}

inline void
Noti::cast( IRoom * i_pRoom, PACKET * i_pPacket )
{
	namespace				M = ::Packet::Manager;
	M::CN_Noti * pReq		= static_cast<M::CN_Noti *>( i_pPacket );

	M::SN_Noti * pNoti		= ::Packet::Builder::Build<M::SN_Noti>();
	pNoti->SetNoti( pReq->m_strNoti );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pReq->m_byType == M::CN_Noti::E_TYPE::ALL )
		{
			ICASTER()->Cast( pPlayer, pNoti );
		}
		else if( pReq->m_byType == M::CN_Noti::E_TYPE::LOBBY )
		{
			if( pPlayer->GetState() == IPlayer::E_STATE::LOBBY || 
				pPlayer->GetState() == IPlayer::E_STATE::LOGIN )
			{
				ICASTER()->Cast( pPlayer, pNoti );
			}
		}
		else if( pReq->m_byType == M::CN_Noti::E_TYPE::BUFF )
		{
			if( pPlayer->GetState() == IPlayer::E_STATE::BUFF ||
				pPlayer->GetState() == IPlayer::E_STATE::RETURN_BUFF ||
				pPlayer->GetState() == IPlayer::E_STATE::READY ||
				pPlayer->GetState() == IPlayer::E_STATE::INVENTORY )
			{
				ICASTER()->Cast( pPlayer, pNoti );
			}
		}
		else if( pReq->m_byType == M::CN_Noti::E_TYPE::GAME )
		{
			if( pPlayer->GetState() == IPlayer::E_STATE::PLAY_READY ||
				pPlayer->GetState() == IPlayer::E_STATE::PLAY )
			{
				ICASTER()->Cast( pPlayer, pNoti );
			}
		}

		pPlayer				= i_pRoom->GetNextPlayer();
	}
}

} /* Process */				} /* Manager */				} /* GamerServer */