#include "StdAfx.h"

#include "./Factory.h"
#include "./Provider.h"
#include "./Facade.h"

namespace GameServer {		namespace Log {

Factory::Factory()
{
	m_pFacade				= new Log::Facade;

	this->install();
}

Factory::~Factory()
{
	this->uninstall();

	SAFE_DELETE( m_pFacade );
}

ILog *
Factory::Create( const STRING & strPath )
{
	Provider * pProvider		= new Provider;
	pProvider->Capture( strPath );
	return pProvider;
}

void
Factory::Destroy( ILog * pLog )
{
	Provider * pProvider		= static_cast< Provider * >( pLog );
	pProvider->Release();

	delete pProvider;
}

} /* Log */					} /* GameServer */
