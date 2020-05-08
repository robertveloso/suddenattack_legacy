#include "StdAfx.h"

#include "./FinishNoti.h"

#include "../../ICaster.h"

#include "../../IChannel.h"
#include "../../IRoom.h"
#include "../../IServer.h"
#include "../../IPlayer.h"

#include "../../../Share/Packet/Builder.h"
#include "../../../Share/Packet/Manager.h"

namespace GameServer {		namespace Manager {			namespace Process {

FinishNoti::FinishNoti()
{
}

FinishNoti::~FinishNoti()
{
}

CBOOL
FinishNoti::Execute( IManager * i_pManager, PACKET * i_pPacket )
{
	namespace				M = ::Packet::Manager;

	Assert( i_pPacket->GetCommand() == M::NN_FinishNoti::s_nCode );

	M::NN_FinishNoti * pRelay	= static_cast<M::NN_FinishNoti *>( i_pPacket );

	if( IPlayer::IBUILDER()->IsLock() == TRUE )
	{
		return TRUE;
	}

	IPlayer::IBUILDER()->SetLock( TRUE, pRelay->m_byFinishMin, pRelay->m_byType );

	//IChannel * pChannel		= ISERVER()->GetFirstChannel();
	//while( pChannel != NULL )
	//{
	//	ICASTER()->Cast( pChannel->GetLobby(), pRelay );
	//	IRoom * pRoom		= pChannel->GetFirstRoom();
	//	while( pRoom != NULL )
	//	{
	//		ICASTER()->Cast( pRoom, pRelay );

	//		pRoom			= pChannel->GetNextRoom();
	//	}
	//	pChannel			= ISERVER()->GetNextChannel();
	//}
	IPlayer * pPlayer		= IPlayer::ICONTAINER()->First();
	while( pPlayer != NULL )
	{
		ICASTER()->Cast( pPlayer, pRelay );
		pPlayer				= IPlayer::ICONTAINER()->Next();
	}

	//pChannel				= ISERVER()->GetFirstChannel();
	//while( pChannel != NULL )
	//{
	//	IRoom * pRoom		= pChannel->GetFirstRoom();
	//	while( pRoom != NULL )
	//	{
	//		IPlayer * pPlayer = pRoom->GetFirstPlayer();
	//		while( pPlayer != NULL )
	//		{
	//			this->disconnect( pPlayer );
	//			pPlayer		= pRoom->GetNextPlayer();
	//		}

	//		pRoom			= pChannel->GetNextRoom();
	//	}
	//	IPlayer * pPlayer	= pChannel->GetLobby()->GetFirstPlayer();
	//	while( pPlayer != NULL )
	//	{
	//		this->disconnect( pPlayer );
	//		pPlayer			= pChannel->GetLobby()->GetNextPlayer();
	//	}

	//	pChannel			= ISERVER()->GetNextChannel();
	//}
	pPlayer					= IPlayer::ICONTAINER()->First();
	while( pPlayer != NULL )
	{
		this->disconnect( pPlayer );
		pPlayer				= IPlayer::ICONTAINER()->Next();
	}

	return TRUE;
}

CINT
FinishNoti::GetCode()
{
	namespace				M = ::Packet::Manager;

	return M::NN_FinishNoti::s_nCode;
}

inline void
FinishNoti::disconnect( IPlayer * i_pPlayer )
{
	if( i_pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY )
	{
		IPlayer::ISESSION()->Close( i_pPlayer );
	}
}

} /* Process */				} /* Manager */				} /* GamerServer */