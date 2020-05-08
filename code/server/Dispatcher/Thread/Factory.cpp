#include "StdAfx.h"

#include "./Factory.h"

namespace Dispatcher {		namespace Thread {

Factory::Factory()
{
	this->install();
}

Factory::~Factory()
{
	this->uninstall();
}

IThread *
Factory::Create( IOBSERVER * i_pObserver )
{
	Provider * pThread		= new Provider;
	pThread->Capture( i_pObserver );
	
	return pThread;
}

void
Factory::Destroy( IThread * i_pThread )
{
	Provider * pThread	= static_cast<Provider *>( i_pThread );

	pThread->Release();

	SAFE_DELETE( pThread );
}

} /* Thread */				} /* Dispatcher */
