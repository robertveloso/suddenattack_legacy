#include "StdAfx.h"

#include "./Factory.h"
#include "../Handler/Provider.h"

namespace Dispatcher {		namespace Query {

Factory::Factory( Handler::Provider * i_pHandler, CINT i_nPoolCount )
{
	m_pHandler				= i_pHandler;

	m_pPool					= new POOL;
	m_pPool->Create( i_nPoolCount );

	this->install();
}

Factory::~Factory()
{
	this->uninstall();

	m_pPool->Destroy();
	SAFE_DELETE( m_pPool );
}

IQuery *
Factory::Create( CDWORD i_dwCode )
{
	Provider * pProvider	= m_pPool->Alloc();
	pProvider->Capture( i_dwCode, m_pHandler->AllocDbCommand() );

	return pProvider;
}

void
Factory::Destroy( IQuery * i_pQuery )
{
	Provider * pProvider	= static_cast<Provider *>( i_pQuery );
    	
	pProvider->Release();
	m_pPool->Free( pProvider );
}

} /* Query */				} /* Dispatcher */