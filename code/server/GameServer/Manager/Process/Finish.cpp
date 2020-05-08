#include "StdAfx.h"

#include "./Finish.h"

#include "../../ICaster.h"

#include "../../IChannel.h"
#include "../../IRoom.h"
#include "../../IServer.h"
#include "../../IPlayer.h"

#include "../../../Share/Packet/Builder.h"
#include "../../../Share/Packet/Manager.h"

namespace GameServer {		namespace Manager {			namespace Process {

Finish::Finish()
{
}

Finish::~Finish()
{
}

CBOOL
Finish::Execute( IManager * i_pManager, PACKET * i_pPacket )
{
	namespace				M = ::Packet::Manager;

	Assert( i_pPacket->GetCommand() == M::CQ_Finish::s_nCode );

	M::CQ_Finish * pReq		= static_cast<M::CQ_Finish *>( i_pPacket );
	M::SA_Finish * pAck		= ::Packet::Builder::Build<M::SA_Finish>();
	pAck->m_bCancel			= pReq->m_bCancel;
	if( pReq->m_bCancel == TRUE )
	{
		if( IPlayer::IBUILDER()->IsLock() == TRUE )
		{
			IPlayer::IBUILDER()->SetLock( FALSE, 0, 0 );
			ICASTER()->Cast( i_pManager, pAck );
		}
		return TRUE;
	}

	if( IPlayer::IBUILDER()->IsLock() == FALSE )
	{
		return TRUE;
	}
	
	//IChannel * pChannel		= ISERVER()->GetFirstChannel();
	//while( pChannel != NULL )
	//{
	//	IRoom * pRoom		= pChannel->GetFirstRoom();
	//	while( pRoom != NULL )
	//	{
	//		IPlayer * pPlayer = pRoom->GetFirstPlayer();
	//		while( pPlayer != NULL )
	//		{
	//			IPlayer::ISESSION()->Close( pPlayer );
	//			pPlayer		= pRoom->GetNextPlayer();
	//		}

	//		pRoom			= pChannel->GetNextRoom();
	//	}
	//	pChannel			= ISERVER()->GetNextChannel();
	//}
	IPlayer * pPlayer		= IPlayer::ICONTAINER()->First();
	while( pPlayer != NULL )
	{
		IPlayer::ISESSION()->Close( pPlayer );
		pPlayer				= IPlayer::ICONTAINER()->Next();
	}

	return TRUE;
}

CINT
Finish::GetCode()
{
	namespace				M = ::Packet::Manager;

	return M::CQ_Finish::s_nCode;
}

} /* Process */				} /* Manager */				} /* GamerServer */