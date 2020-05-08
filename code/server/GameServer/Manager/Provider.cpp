#include "StdAfx.h"

#include "./Provider.h"
#include "./IProcess.h"

#include "../../Share/Packet/Manager.h"

namespace GameServer {		namespace Manager {

Provider::Provider()
{
}

Provider::~Provider()
{
}

void
Provider::Capture( PEER * i_pPeer )
{
	m_pPeer					= i_pPeer;
}

void
Provider::Release()
{
	m_pPeer					= NULL;
}

Provider::PEER *
Provider::GetPeer()
{
	return m_pPeer;
}

void
Provider::OnOpen()
{
}

void
Provider::OnPacket( PACKET * i_pPacket )
{
	IProcess * pProcess		= IProcess::ICONTAINER()->Search( i_pPacket->GetCommand() );
	if( pProcess == NULL )
	{
		IClient::ISESSION()->Close( this );
	}

	CBOOL bResult			= pProcess->Execute( this, i_pPacket );

	if( bResult == FALSE )
	{
		IClient::ISESSION()->Close( this );
	}
}

void
Provider::OnClose()
{
	IBUILDER()->Destroy( this );
}

void
Provider::OnSignal( CUINT i_uiMessage )
{
	//typedef					::Packet::Login::CQ_Login
	//						TEST;
	//PACKET * pPacket		= ::Packet::Builder::Build<TEST>();
	//pPacket->SetCommand( i_uiMessage );
	
}

void
Provider::OnResponse( IQUERY * i_pQuery )
{
}

} /* Manager */				} /* GamerServer */