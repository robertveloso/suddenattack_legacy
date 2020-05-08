#include "StdAfx.h"

#include "./Factory.h"

namespace GameServer {		namespace Channel {

Factory::Factory()
{
	m_pPool					= new POOL;

	this->install();
}

Factory::~Factory()
{
	this->uninstall();

	SAFE_DELETE( m_pPool );
}

IChannel *
Factory::Create( CBYTE i_byIndex, CINT i_nMaxUser )
{
	Provider * pProvider	= m_pPool->Capture();
	pProvider->Capture( i_byIndex, i_nMaxUser );

	return pProvider;
}

void
Factory::Destroy( IChannel * i_pChannel )
{
	Provider * pProvider	= static_cast<Provider *>( i_pChannel );
	
	pProvider->Release();
	m_pPool->Release( pProvider );
}

} /* Channel */				} /* GameServer */