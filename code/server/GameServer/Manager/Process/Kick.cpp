#include "StdAfx.h"

#include "./Kick.h"

#include "../../ICaster.h"
#include "../../IPlayer.h"

#include "../../../Share/Packet/Builder.h"
#include "../../../Share/Packet/Manager.h"

namespace GameServer {		namespace Manager {			namespace Process {

Kick::Kick()
{
}

Kick::~Kick()
{
}

CBOOL
Kick::Execute( IManager * i_pManager, PACKET * i_pPacket )
{
	namespace				M = ::Packet::Manager;

	Assert( i_pPacket->GetCommand() == M::CQ_Kick::s_nCode );

	M::CQ_Kick * pReq		= static_cast<M::CQ_Kick *>( i_pPacket );

	M::SA_Kick * pAck		= ::Packet::Builder::Build<M::SA_Kick>();

	typedef					IPlayer::IContainer::NICK
							NICK;
	IPlayer * pPlayer		= IPlayer::ICONTAINER()->Find( NICK( pReq->m_strNick ) );
	if( pPlayer == NULL )
	{
		pAck->m_bSuccess	= FALSE;
		ICASTER()->Cast( i_pManager, pAck );
		return TRUE;
	}

	pAck->m_bSuccess		= TRUE;
	ICASTER()->Cast( i_pManager, pAck );

	M::SN_Kick * pNoti		= ::Packet::Builder::Build<M::SN_Kick>();
	for( INT i = 0; i < 41; i++ )
	{
		pNoti->m_strNoti[ i ] = pReq->m_strNoti[ i ];
	}

	ICASTER()->Cast( pPlayer, pNoti );

	IPlayer::ISESSION()->Close( pPlayer );

	return TRUE;
}

CINT
Kick::GetCode()
{
	namespace				M = ::Packet::Manager;

	return M::CQ_Kick::s_nCode;
}

} /* Process */				} /* Manager */				} /* GamerServer */