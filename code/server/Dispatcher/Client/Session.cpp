#include "StdAfx.h"

#include "./Session.h"
#include "../Handler/Provider.h"
#include "../Handler/Peer.h"
#include "../IConsole.h"
#include "../../Share/Packet/Builder.h"

namespace Dispatcher {		namespace Client {

Session::Session( Handler::Provider * i_pHandler )
{
	m_pHandler				= i_pHandler;

	this->install();
}

Session::~Session()
{
	this->uninstall();
}

void
Session::Alarm( CUINT i_uiMessage, CUINT i_uiSecond, IClient * i_pClient )
{
	m_pHandler->Alarm( i_uiMessage, i_uiSecond, reinterpret_cast<ULONG_PTR>( i_pClient ) );
}

void
Session::Signal( CUINT i_uiMessage, IClient * i_pClient )
{
	m_pHandler->Signal( i_uiMessage, reinterpret_cast<ULONG_PTR>( i_pClient ) );
}

void
Session::Send( PACKET * i_pPacket, IClient * i_pClient )
{
	m_pHandler->Send( i_pClient->GetPeer(), reinterpret_cast<CHAR *>( i_pPacket ), i_pPacket->GetLength() );
#ifdef _DEBUG
	STRING strOut;
	strOut.Printf( " <== %d\r\n",i_pPacket->GetCommand());
	::Dispatcher::ICONSOLE()->Write( strOut );
#endif
	::Packet::Builder::Destroy( i_pPacket );
}

void
Session::Close( IClient * i_pClient )
{
//#pragma message( "===============================\r\n" )
//#pragma message( "비정상 종료를 위한 임시로그\r\n" )
//#pragma message( "===============================\r\n" )
//	::Dispatcher::ICONSOLE()->WriteUnsafe( "DoClose\tSession\r\n" );
	m_pHandler->Close( i_pClient->GetPeer() );
}

CDWORD
Session::GetRTT( IClient * i_pClient )
{
	return m_pHandler->GetRTT( i_pClient->GetPeer() );
}

} /* Client */				} /* Dispatcher */