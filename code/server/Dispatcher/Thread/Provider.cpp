#include "StdAfx.h"

#include "./Provider.h"

namespace Dispatcher {		namespace Thread {

Provider::Provider()
{
	m_hThread				= NULL;
}

Provider::~Provider()
{
}

void
Provider::Capture( IObserver * i_pObserver )
{
	ASSERT( m_hThread == NULL );
	m_pObserver	= i_pObserver;
	m_bRun		= TRUE;

	m_hThread	= CreateThread( NULL, 0, Provider::bridge, this, 0, NULL );
	ASSERT( m_hThread != NULL );
}

void
Provider::Release()
{
	ASSERT( m_hThread != NULL );
	ASSERT( m_bRun == FALSE );

	CloseHandle( m_hThread );

	m_pObserver			= NULL;
}

void
Provider::Stop()
{
	m_pObserver->OnStop();

	ULONG ulWaitResult		= WaitForSingleObject( m_hThread, INFINITE );
	ASSERT( ulWaitResult == WAIT_OBJECT_0 );
}

inline ULONG WINAPI
Provider::bridge( void * pArgument )
{
	Provider * pThread		= reinterpret_cast<Provider *>( pArgument );

	pThread->m_pObserver->OnRun();
	pThread->m_bRun			= FALSE;

	return 0;
}

} /* Thread */				} /* Dispatcher */
