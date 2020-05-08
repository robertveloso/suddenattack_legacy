#include "StdAfx.h"
#include ".\Timer.h"

using namespace Session::Manage;
using namespace Session::Message;

CTimer::CTimer()
:	m_nTickTime		( 0		)
,	m_nMaxTickCount	( 0		)
,	m_nSize			( 0		)
,	m_nPos			( 0		)
,	m_paSignalSet	( NULL	)
,	m_pSignalList	( NULL	)
,	m_bStop			( FALSE	)
{
}

CTimer::~CTimer()
{
}

BOOL CTimer::Create( int nTickTime, int nMaxTickCount, int nSize, ITimerHandler* pTimerHandler )
{
	m_paSignalSet	= new TagSignal*[nMaxTickCount];
	if( m_paSignalSet == NULL )
		return FALSE;

	if( !m_SignalPool.Create( nSize ) )
	{
		delete[] m_paSignalSet;
		return FALSE;
	}

	for( int i = 0; i < nMaxTickCount; i++ )
	{
		m_paSignalSet[i] = NULL;
	}

	m_pSignalList	= NULL;
	m_nTickTime		= nTickTime;
	m_nMaxTickCount	= nMaxTickCount;
	m_nSize			= nSize;
	m_nPos			= 0;
	m_pTimerHandler	= pTimerHandler;

	m_hInitEvent = ::CreateEvent(
					NULL,         // no security attributes
					TRUE,         // manual-reset Signal
					TRUE,         // initial state is signaled
					NULL		  // object name
					);
	::ResetEvent( m_hInitEvent );
	m_bStop = FALSE;

	DWORD dwThreadId = 0;
	::CloseHandle( ::CreateThread( NULL, 0, ThreadStartForTime, this, 0, &dwThreadId ) );

	::InitializeCriticalSection( &m_cs );
	return TRUE;
}

void CTimer::Destroy()
{
	m_bStop = TRUE;
	::WaitForSingleObject( m_hInitEvent, INFINITE );

	m_SignalPool.Destroy();;

	delete[] m_paSignalSet;
	m_paSignalSet	= NULL;
	m_pSignalList	= NULL;
	m_nTickTime		= 0;
	m_nMaxTickCount	= 0;
	m_nSize			= 0;
	m_nPos			= 0;

	::DeleteCriticalSection( &m_cs );
}

ULONG_PTR CTimer::SetTimer( int nTickCount, const CSignal& signal ) //��m_pSignalList֮ǰ����ڵ�
{
	TagSignal*	pSignal;

	::EnterCriticalSection( &m_cs );

	// Signal Node �Ҵ�
	pSignal = m_SignalPool.Alloc();
	pSignal->signal		= signal;
	pSignal->signal.SetTimerKey( reinterpret_cast<ULONG_PTR>( pSignal ) );
	pSignal->nTickCount	= nTickCount;

	pSignal->pNext		= m_pSignalList;
	m_pSignalList		= pSignal;

	::LeaveCriticalSection( &m_cs );

	return reinterpret_cast<ULONG_PTR>( pSignal );
}

// Ÿ�̸� ������
DWORD __stdcall CTimer::ThreadStartForTime(PVOID pvParam)
{
	CTimer* pThis = (CTimer*)pvParam;

	int			nTickPos;
	TagSignal*	pSignalNext;

	int			nTickTime		= pThis->m_nTickTime;
	int			nMaxTickCount	= pThis->m_nMaxTickCount;
	int			nSize			= pThis->m_nSize;
	int			nPos			= pThis->m_nPos;
	TagSignal**	paSignalSet		= pThis->m_paSignalSet;

	ITimerHandler*
				pTimerHandler	= pThis->m_pTimerHandler;

	while( !pThis->m_bStop )
	{
		::Sleep( nTickTime );

		TagSignal* pCurrent;
		TagSignal* pBefore;
		TagSignal* pNext;
		pCurrent = paSignalSet[nPos];
//		paSignalSet[nPos] = NULL;

		if( nPos == 0 || nPos == 1 )
			int a = 3;

		pBefore = NULL;
		while( pCurrent )
		{
			if( pCurrent->nTickCount == 0 )
			{
				pTimerHandler->OnTimer( pCurrent->signal ); //�����źŽ�m_RecvMessageQueue����
				pNext	= pCurrent->pNext;
				if( pBefore == NULL )
				{
					paSignalSet[nPos] = pNext;
				}
				else
				{
					pBefore->pNext = pNext;
				}
				pThis->m_SignalPool.Free( pCurrent );
			}
			else
			{
				pCurrent->nTickCount--;
				pNext		= pCurrent->pNext;
				pBefore		= pCurrent;
			}
			
			pCurrent	= pNext;
		}
		nPos++;
		if( nPos == nMaxTickCount )
			nPos = 0;

		::EnterCriticalSection( &pThis->m_cs );

		while( pThis->m_pSignalList )
		{
			//��m_pSignalList��ժ�µ�һ���ڵ㣬���ýڵ���m_paSignalSet������Ӧ��˫�����
			pSignalNext	= pThis->m_pSignalList->pNext;

			// Tick Position�� Tick Count ����
			nTickPos	= pThis->m_pSignalList->nTickCount % nMaxTickCount;
			nTickPos	+= nPos;
			if( nTickPos >= nMaxTickCount )
				nTickPos -= nMaxTickCount;
			pThis->m_pSignalList->nTickCount /= nMaxTickCount;

			if( nTickPos ==  1 )
				int a = 3;
			// Signal Node ����
			pThis->m_pSignalList->pNext		= pThis->m_paSignalSet[nTickPos];
			pThis->m_paSignalSet[nTickPos]	= pThis->m_pSignalList;

			// ���� Signal Node�� List���� ����
			pThis->m_pSignalList				= pSignalNext;
		}

		::LeaveCriticalSection( &pThis->m_cs );
	}

	::SetEvent( pThis->m_hInitEvent );
	return 0;
}