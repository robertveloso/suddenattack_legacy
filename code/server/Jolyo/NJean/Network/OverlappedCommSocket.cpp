#include "StdAfx.h"
#include ".\overlappedCommSocket.h"

#include <Mswsock.h>

using namespace Network::TcpIOCP;
using namespace Common::DataStructure;



////////////////////////////////////////////////////////////////////////////////////////
//
//	COverlappedCommSocket
//


// 생성/소멸자
COverlappedCommSocket::COverlappedCommSocket(void)
{
	m_ovRecvChannel.wsaBuffer.buf = NULL;
	m_ovSendChannel.wsaBuffer.buf = NULL;

	m_hInitEvent = ::CreateEvent(
						NULL,         // no security attributes
						TRUE,         // manual-reset event
						TRUE,         // initial state is signaled
						NULL		  // object name
						);
	m_hCloseEvent = ::CreateEvent(
						NULL,         // no security attributes
						TRUE,         // manual-reset event
						TRUE,         // initial state is signaled
						NULL		  // object name
						);

	m_bConnect = FALSE;

	m_nTest = 0;
	::InitializeCriticalSection( &m_csConnect );
}

COverlappedCommSocket::~COverlappedCommSocket(void)
{
	::CloseHandle( m_hInitEvent );
	::CloseHandle( m_hCloseEvent );
	::DeleteCriticalSection( &m_csConnect );
}


/**************************************************************************************
	Public Operations
***************************************************************************************/

// 버퍼를 생성한다.
BOOL COverlappedCommSocket::Create( CMemoryPoolSafty<COverlappedCommSocket>* pCommSocketPool, int nRecvBuffSize, int nSendBuffSize )
{
	m_pCommSocketPool	= pCommSocketPool;
	m_bCreateBuffer = TRUE;

	char* pRecvBuff = NULL;
	char* pSendBuff = NULL;

	pRecvBuff = new char[nRecvBuffSize];
	pSendBuff = new char[nSendBuffSize];

	if( pRecvBuff == NULL || pSendBuff == NULL )
	{
		if( pRecvBuff )
			delete[] pRecvBuff;
		if( pSendBuff )
			delete[] pSendBuff;

		m_bCreateBuffer = FALSE;

		return FALSE;
	}

	m_ovRecvChannel.nBuffSize = nRecvBuffSize;
	m_ovRecvChannel.wsaBuffer.buf = pRecvBuff;
	m_ovRecvChannel.wsaBuffer.len = nRecvBuffSize;

	m_ovSendChannel.nBuffSize = nSendBuffSize;
	m_ovSendChannel.wsaBuffer.buf = pSendBuff;


	return TRUE;
}

// 버퍼를 제거한다.
void COverlappedCommSocket::Destroy()
{
	if( m_bCreateBuffer )
	{
		if( m_ovRecvChannel.wsaBuffer.buf )
			delete[] m_ovRecvChannel.wsaBuffer.buf;
		if( m_ovSendChannel.wsaBuffer.buf )
			delete[] m_ovSendChannel.wsaBuffer.buf;

		m_ovRecvChannel.wsaBuffer.buf	= NULL;
		m_ovSendChannel.wsaBuffer.buf	= NULL;
	}

}

// 미리 할당된 버퍼를 assign한다.
void COverlappedCommSocket::AssignBuffer( char* pRecvBuff, int nRecvBuffSize, char* pSendBuff, int nSendBuffSize )
{
	m_ovRecvChannel.nBuffSize = nRecvBuffSize;
	m_ovRecvChannel.wsaBuffer.buf = pRecvBuff;

	m_ovSendChannel.nBuffSize = nSendBuffSize;
	m_ovSendChannel.wsaBuffer.buf = pSendBuff;
}


// Connect를 수행한다.
BOOL COverlappedCommSocket::Connect( const sockaddr* addr, int namelen )
{
	int         nRet;
	int         nZero = 0;
	LINGER      lingerStruct;

	nZero = 0;
	nRet = ::setsockopt( m_sSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nZero, sizeof(nZero) );
	if( SOCKET_ERROR == nRet )
	{
//		nOutErrorcode = ErrorCode::SetSockOptFail;
//		::closesocket( m_sSocket );
		return FALSE;
	}

	// Disable receive buffering on the socket.  Setting SO_RCVBUF 
	// to 0 causes winsock to stop bufferring receive and perform
	// receives directly from our buffers, thereby save one memory copy.
	nZero = 0;
	nRet = ::setsockopt( m_sSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nZero, sizeof(nZero) );
	if( SOCKET_ERROR == nRet )
	{
//		nOutErrorcode = ErrorCode::SetSockOptFail;
//		::closesocket( m_sSocket );
		return FALSE;
	}

	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	nRet = ::setsockopt( m_sSocket, SOL_SOCKET, SO_LINGER, (char*)&lingerStruct, sizeof(lingerStruct) );
	if (SOCKET_ERROR == nRet) 
	{
//		nOutErrorcode = ErrorCode::SetSockOptFail;
//		::closesocket( m_sSocket );
		return FALSE;
	}

	if( ::connect( m_sSocket, addr, namelen ) == SOCKET_ERROR )
		return FALSE;

	// Disable send buffering on the socket.  Setting SO_SNDBUF
	// to 0 causes winsock to stop bufferring sends and perform
	// sends directly from our buffers, thereby save one memory copy.

	return TRUE;
}

// IOCP에 Receive 이벤트를 등록시킨다.
BOOL COverlappedCommSocket::RecvPost()
{
	DWORD dwRecvBytes = 0;
	DWORD dwFlags = 0;

	::ZeroMemory( &m_ovRecvChannel, sizeof(WSAOVERLAPPED) );
	::ZeroMemory( m_ovRecvChannel.wsaBuffer.buf, m_ovRecvChannel.nBuffSize );

	int nRet = ::WSARecv(
		m_sSocket,
		&(m_ovRecvChannel.wsaBuffer),
		1,
		&dwRecvBytes,
		&dwFlags,
		&m_ovRecvChannel,
		NULL );

	return( nRet == SOCKET_ERROR );
}

// IOCP에 Send 이벤트를 등록시킨다.
BOOL COverlappedCommSocket::SendPost( INT& nOutError )
{
	DWORD dwSendBytes=0;
	DWORD dwFlags=0;

	::ZeroMemory( &m_ovSendChannel, sizeof(WSAOVERLAPPED) );

	m_nErr = 999;

	int nRet = ::WSASend(
		m_sSocket,
		&(m_ovSendChannel.wsaBuffer),
		1,
		&dwSendBytes,
		dwFlags,
		&m_ovSendChannel,
		NULL );

	/*
	short nCommand; 
	memcpy( &nCommand, &m_ovSendChannel.wsaBuffer.buf+sizeof(short), sizeof(short) );
	if( nCommand == 1018 )
	{
		char szTemp[128];
		sprintf( szTemp, "1[%10d] %d", m_sSocket, nCommand );
		OutputDebugString( szTemp );
	}
	*/

	nOutError = ::WSAGetLastError();
	if( nRet == SOCKET_ERROR )
	{
		if( WSA_IO_PENDING != nOutError )
		{
			m_nErr = nOutError;

			return FALSE;
		}
	}
	m_nErr = m_nTest;
	return TRUE;
}


/**************************************************************************************
	Public Implementations
***************************************************************************************/

// IO Type을 반환한다.
COverlappedSocket::IOType COverlappedCommSocket::GetIOType( LPOVERLAPPED lpOv )
{
	if( lpOv == &m_ovRecvChannel )
		return COverlappedSocket::IOType::RecvType;
	else
		return COverlappedSocket::IOType::SendType;
}