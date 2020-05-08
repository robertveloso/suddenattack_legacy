#include "StdAfx.h"
#include ".\asyncIocp.h"

using namespace IO::IOCP;

/**************************************************************************************
	생성자 / 소멸자
***************************************************************************************/

CAsyncIocp::CAsyncIocp( VOID )
:	CIocp			( )
,	m_nNumOfThreads ( 0		)
,	c_lpThreadDie	( 1		)
,	m_pIocpHandler	( NULL	)
{
}

CAsyncIocp::~CAsyncIocp( VOID )
{
	CIocp::~CIocp();
}


/**************************************************************************************
	Public Operations
***************************************************************************************/

// IOCP를 생성한다.
//		nNumberOfThreads : 쓰레드의 개수 : 0이면 CPU개수 * 2로 설정된다.
//		dwWaitMilliSeconds : wait 시간
BOOL CAsyncIocp::Create( IIocpHandler* pIocpHandler, int nNumberOfThreads/* = 0*/, DWORD dwWaitMilliSeconds/* = INFINITE*/ )
{
	m_pIocpHandler = pIocpHandler;

	if( !CIocp::Create( dwWaitMilliSeconds ) )
		return FALSE;

	//
	// Thread 개수 설정
	//
	m_nNumOfThreads = nNumberOfThreads;

	if( m_nNumOfThreads <= 0 )
	{
		SYSTEM_INFO si;
		::GetSystemInfo(&si);

		// 디폴트 쓰레드 수로 2 * 프로세서수 + 2 의 공식을 따랐음
		m_nNumOfThreads = si.dwNumberOfProcessors * 2 + 2;
	}


	//
	// Thread Pool 생성
	//
	for( int i=0; i < m_nNumOfThreads; i++ )
	{
		DWORD dwThreadId = 0;
		::CloseHandle( ::CreateThread( NULL, 0, IocpWorkerThreadStartingPoint, this, 0, &dwThreadId ) );
	}

	return TRUE;
}

// IOCP를 제거한다.
void CAsyncIocp::Destroy()
{
	for( int i = 0; i < m_nNumOfThreads; i++ )
		::PostQueuedCompletionStatus( m_hIOCP, 0, c_lpThreadDie, NULL );

	CIocp::Destroy();
}


/**************************************************************************************
	Private Operations
***************************************************************************************/

// IOCP를 수행하는 쓰레드
DWORD __stdcall CAsyncIocp::IocpWorkerThreadStartingPoint(PVOID pvParam)
{
	CAsyncIocp* pIocp = (CAsyncIocp*) pvParam;

	ULONG_PTR		lpCompletionKey	= NULL;
	LPOVERLAPPED	lpOverlapped	= NULL;
	DWORD			dwBytesTransferred = 0;

	while( TRUE )
	{
		//
		// IO Completion Packet 얻어온다.
		//
		BOOL bRet = pIocp->GetQueuedCompletionStatus(
			&dwBytesTransferred,
			&lpCompletionKey,
			&lpOverlapped );

		if( bRet )
		{
			if( pIocp->IsExitCompletionKey( lpCompletionKey ) )
				break;
		}

		ASSERT( lpOverlapped );

		//
		// IOCP 인터페이스를 통해 IO완료를 통보한다.
		//
		pIocp->m_pIocpHandler->OnGetQueuedCompletionStatus(
			bRet,
			dwBytesTransferred,
			lpCompletionKey,
			lpOverlapped );
	}

	return 0;
}