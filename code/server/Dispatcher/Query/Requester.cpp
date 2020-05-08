#include "StdAfx.h"

#include "./Requester.h"
#include "../Handler/Provider.h"
#include "../IClient.h"
#include "../Handler/Peer.h"

namespace Dispatcher {		namespace Query {

Requester::Requester( Handler::Provider * i_pHandler )
{
	m_pHandler				= i_pHandler;
	m_pObserver				= NULL;
//	m_pMap					= new MAP;

	this->install();
}

Requester::~Requester()
{
	this->uninstall();

//	SAFE_DELETE( m_pMap );
}

Requester::IOBSERVER *
Requester::GetObserver()
{
	return m_pObserver;
}

void
Requester::SetObserver( IOBSERVER * i_pObserver )
{
	Assert( i_pObserver != NULL );
	Assert( m_pObserver == NULL );

	m_pObserver				= i_pObserver;
}

void
Requester::Regist( IQuery * i_pQuery, IClient * i_pClient )
{
	Assert( i_pQuery != NULL );
	Assert( i_pClient != NULL );

	m_pHandler->Request( i_pClient->GetPeer(), i_pQuery->GetCode(), i_pQuery->GetCommand(), reinterpret_cast<void *>( i_pQuery ) );
}

void
Requester::Regist( IQuery * i_pQuery )
{
	Assert( m_pObserver != NULL );

	m_pHandler->Request( NULL, i_pQuery->GetCode(), i_pQuery->GetCommand(), reinterpret_cast<void *>( i_pQuery ) );
}

} /* Query */				} /* Dispatcher */