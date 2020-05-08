#include "StdAfx.h"

#include "./Login.h"

#include "../../../Share/Packet/Builder.h"
#include "../../../Share/Packet/Manager.h"

namespace GameServer {		namespace Manager {			namespace Process {

Login::Login()
{
}

Login::~Login()
{
}

CBOOL
Login::Execute( IManager * i_pManager, PACKET * i_pPacket )
{
	namespace				M = ::Packet::Manager;

	Assert( i_pPacket->GetCommand() == M::CQ_Login::s_nCode );

	M::CQ_Login * pReq		= static_cast<M::CQ_Login *>( i_pPacket );

	return TRUE;
}

CINT
Login::GetCode()
{
	namespace				M = ::Packet::Manager;

	return M::CQ_Login::s_nCode;
}

} /* Process */				} /* Manager */				} /* GamerServer */