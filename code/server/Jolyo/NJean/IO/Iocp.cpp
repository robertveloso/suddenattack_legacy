#include "StdAfx.h"
#include ".\iocp.h"

using namespace IO::IOCP;

/**************************************************************************************
	생성자 / 소멸자
***************************************************************************************/

CIocp::CIocp()
:	m_hIOCP				(NULL)
,	m_dwWaitMilliSeconds(INFINITE)
{
}

CIocp::~CIocp(void)
{
}


/**************************************************************************************
	Public Operations
***************************************************************************************/

// IOCP를 생성한다.
//		dwWaitMilliSeconds : wait 시간
BOOL CIocp::Create( DWORD dwWaitMilliSeconds/* = INFINITE*/ )
{
	//
	// GetQueuedCompletionStatus에서의 대기 시간 설정
	//
	m_dwWaitMilliSeconds = dwWaitMilliSeconds;

	//
	// IOCP Handle 생성
	//
	m_hIOCP = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

	return( m_hIOCP != NULL );
}

// IOCP를 제거한다.
void CIocp::Destroy()
{
	::CloseHandle( m_hIOCP );
	m_hIOCP = NULL;
}

// IOCP에 핸들을 등록한다.
BOOL CIocp::AssignIoCompletionPort( HANDLE FileHandle, ULONG_PTR CompletionKey )
{
	return( m_hIOCP == ::CreateIoCompletionPort( FileHandle, m_hIOCP, CompletionKey, 0 ) );
}

// IOCP 큐에 Post한다.
BOOL CIocp::PostQueuedCompletionStatus( ULONG_PTR lpCompletionKey, LPOVERLAPPED lpOverlapped )
{
	return ::PostQueuedCompletionStatus( m_hIOCP, 0, lpCompletionKey, lpOverlapped );
}

// IOCP 큐에서 Get한다.
BOOL CIocp::GetQueuedCompletionStatus( LPDWORD lpNumberOfBytesTransferred, PULONG_PTR lpCompletionKey, LPOVERLAPPED* lpOverlapped )
{
	return ::GetQueuedCompletionStatus( m_hIOCP, lpNumberOfBytesTransferred, lpCompletionKey, lpOverlapped, m_dwWaitMilliSeconds );
}