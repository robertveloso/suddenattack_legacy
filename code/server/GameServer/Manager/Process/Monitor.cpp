#include "StdAfx.h"

#include "./Monitor.h"

#include "../../ICaster.h"

#include "../../IChannel.h"
#include "../../IServer.h"

#include "../../../Share/Packet/Builder.h"
#include "../../../Share/Packet/Manager.h"

namespace GameServer {		namespace Manager {			namespace Process {

Monitor::Monitor()
{
}

Monitor::~Monitor()
{
}

CBOOL
Monitor::Execute( IManager * i_pManager, PACKET * i_pPacket )
{
	namespace				M = ::Packet::Manager;

	Assert( i_pPacket->GetCommand() == M::CN_Monitor::s_nCode );

	M::CN_Monitor * pReq		= static_cast<M::CN_Monitor *>( i_pPacket );

	switch( pReq->m_byType )
	{
	case M::CN_Monitor::E_TYPE::CONNECTIONS :
		this->monitorPlayers( i_pManager );
		break;

	case M::CN_Monitor::E_TYPE::OPEN_ROOMS :
		this->monitorRooms( i_pManager );
		break;

	default :
		break;
	}

	return TRUE;
}

CINT
Monitor::GetCode()
{
	namespace				M = ::Packet::Manager;

	return M::CN_Monitor::s_nCode;
}

inline void
Monitor::monitorPlayers( IManager * i_pManager )
{
	namespace				M = ::Packet::Manager;

	IChannel * pChannel		= ISERVER()->GetFirstChannel();
	while( pChannel != NULL )
	{
		M::SN_MonitorConnections *
		pNoti					= ::Packet::Builder::Build<M::SN_MonitorConnections>();
		pNoti->m_byChannelIndex	= pChannel->GetIndex();
		pNoti->m_nConnections	= pChannel->GetCurrentUser();
		ICASTER()->Cast( i_pManager, pNoti );
		pChannel			= ISERVER()->GetNextChannel();
	}

}

inline void
Monitor::monitorRooms( IManager * i_pManager )
{
	namespace				M = ::Packet::Manager;

	IChannel * pChannel		= ISERVER()->GetFirstChannel();
	while( pChannel != NULL )
	{
		M::SN_MonitorOpenRooms *
		pNoti					= ::Packet::Builder::Build<M::SN_MonitorOpenRooms>();
		pNoti->m_byChannelIndex	= pChannel->GetIndex();
		pNoti->m_nRooms			= pChannel->GetRoomCount();
		ICASTER()->Cast( i_pManager, pNoti );
		pChannel			= ISERVER()->GetNextChannel();
	}
}

} /* Process */				} /* Manager */				} /* GamerServer */