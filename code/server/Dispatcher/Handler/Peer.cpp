#include "StdAfx.h"

#include "./Peer.h"

namespace Dispatcher {		namespace Handler {

Peer::Peer()
{
}

Peer::~Peer()
{
}

void
Peer::Capture()
{
	m_pClient				= NULL;
}

void
Peer::Release()
{
	m_pClient				= NULL;
}

IClient *
Peer::GetClient()
{
	return m_pClient;
}

void
Peer::SetClient( IClient * i_pClient )
{
	m_pClient				= i_pClient;
}


} /* Handler */				} /* Dispatcher */