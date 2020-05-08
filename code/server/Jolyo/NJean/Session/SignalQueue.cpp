#include "StdAfx.h"
#include ".\Signalqueue.h"

using namespace Session::Message;

CSignalQueue::CSignalQueue(void)
:	m_aSignal	( NULL	)
,	m_nSize		( 0		)
,	m_nHead		( 0		)
,	m_nRear		( 0		)
{
}

CSignalQueue::~CSignalQueue(void)
{
}

BOOL CSignalQueue::Create( int nSize )
{
	m_nSize		= nSize;
	m_aSignal	= new CSignal[nSize];
	if( m_aSignal == NULL )
		return FALSE;

	m_nHead		= 0;
	m_nRear		= 0;

	::InitializeCriticalSection( &m_cs );
	return TRUE;
}

void CSignalQueue::Destroy( void )
{
	if( m_aSignal )
		delete[] m_aSignal;

	m_aSignal	= NULL;
	m_nSize		= 0;
	m_nHead		= 0;
	m_nRear		= 0;

	::DeleteCriticalSection( &m_cs );
}

BOOL CSignalQueue::EnqueueSignal( const CSignal& signal )
{
	// Queue가 Full이면 Queue를 2배로 다시 잡는다.
	::EnterCriticalSection( &m_cs );

	if( m_nRear == m_nHead + 1 ||
		m_nRear == m_nHead - m_nSize + 1 )
	{
		m_nSize *= 2;
		CSignal* aSignal = NULL;
		aSignal = new CSignal[m_nSize];
		if( aSignal == NULL )
		{
			m_nSize /= 2;
			::LeaveCriticalSection( &m_cs );
			return FALSE;
		}

		for( int i = 0; i < m_nSize / 2; i++ )
		{
			aSignal[i] = m_aSignal[i];
		}
		delete[] m_aSignal;
		m_aSignal = aSignal;

	}

	m_aSignal[m_nHead] = signal;
	m_nHead++;
	if( m_nHead == m_nSize )
		m_nHead = 0;

	::LeaveCriticalSection( &m_cs );
	return TRUE;
}

BOOL CSignalQueue::DequeueSignal( CSignal& outSignal )
{
	if( m_nHead == m_nRear )
		return FALSE;

	outSignal = m_aSignal[m_nRear];
	
	m_nRear++;
	if( m_nRear == m_nSize )
		m_nRear = 0;

	return TRUE;
}