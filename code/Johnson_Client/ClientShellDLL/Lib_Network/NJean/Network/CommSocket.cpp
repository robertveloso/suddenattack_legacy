#include "StdAfx.h"
#include "CommSocket.h"

#include <Mswsock.h>

using namespace GamehiSolution::Network::TcpAsyncSelect;
using namespace GamehiSolution::Common::DataStructure;

#include "stdio.h"


////////////////////////////////////////////////////////////////////////////////////////
//
//	CCommSocket
//

// 생성/소멸자
CCommSocket::CCommSocket(void)
{
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

	m_RecvBuffer.buf = NULL;
	m_SendBuffer.buf = NULL;
	::InitializeCriticalSection( &m_csSend );
}

CCommSocket::~CCommSocket(void)
{
	Destroy();
	::CloseHandle( m_hInitEvent );
	::CloseHandle( m_hCloseEvent );
	::DeleteCriticalSection( &m_csSend );
}


/**************************************************************************************
	Public Operations
***************************************************************************************/

// 버퍼를 생성한다.
BOOL CCommSocket::Create( CMemoryPoolSafty<CCommSocket>* pCommSocketPool, int nRecvBuffSize, int nSendBuffSize )
{
	m_pCommSocketPool	= pCommSocketPool;

//	m_bCreateBuffer = TRUE;

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

		return FALSE;
	}

	m_RecvBuffer.buf	= pRecvBuff;
	m_RecvBuffer.len	= nRecvBuffSize;
	m_SendBuffer.buf	= pSendBuff;
	m_SendBuffer.len	= nSendBuffSize;

	/*

	m_ovRecvChannel.nBuffSize = nRecvBuffSize;
	m_ovRecvChannel.wsaBuffer.buf = pRecvBuff;
	m_ovRecvChannel.wsaBuffer.len = nRecvBuffSize;

	m_ovSendChannel.nBuffSize = nSendBuffSize;
	m_ovSendChannel.wsaBuffer.buf = pSendBuff;

	*/

	return TRUE;
}

// 버퍼를 제거한다.
void CCommSocket::Destroy()
{
	if( m_RecvBuffer.buf )
		delete[] m_RecvBuffer.buf;
	m_RecvBuffer.buf = NULL;

	if( m_SendBuffer.buf )
		delete[] m_SendBuffer.buf;
	m_SendBuffer.buf = NULL;
	/*
	if( m_bCreateBuffer )
	{
		if( m_ovRecvChannel.wsaBuffer.buf )
			delete[] m_ovRecvChannel.wsaBuffer.buf;
		if( m_ovSendChannel.wsaBuffer.buf )
			delete[] m_ovSendChannel.wsaBuffer.buf;

		m_ovRecvChannel.wsaBuffer.buf	= NULL;
		m_ovSendChannel.wsaBuffer.buf	= NULL;
	}

	*/
}

// 미리 할당된 버퍼를 assign한다.
/*
void CCommSocket::AssignBuffer( char* pRecvBuff, int nRecvBuffSize, char* pSendBuff, int nSendBuffSize )
{
	m_ovRecvChannel.nBuffSize = nRecvBuffSize;
	m_ovRecvChannel.wsaBuffer.buf = pRecvBuff;

	m_ovSendChannel.nBuffSize = nSendBuffSize;
	m_ovSendChannel.wsaBuffer.buf = pSendBuff;
}
*/

// Connect를 수행한다.
BOOL CCommSocket::Connect( const sockaddr* addr, int namelen )
{
	/*
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
	*/

	const int nRetVal = ::WSAConnect( m_sSocket, addr, namelen, NULL, NULL, NULL, NULL );
	if( nRetVal != SOCKET_ERROR )
		return FALSE;

	// Disable send buffering on the socket.  Setting SO_SNDBUF
	// to 0 causes winsock to stop bufferring sends and perform
	// sends directly from our buffers, thereby save one memory copy.

	return TRUE;
}

BOOL CCommSocket::CreateSocket()
{
	m_dwSentBytes	= 0;
	m_pParentSocket	= NULL;

	m_sSocket = ::socket( AF_INET, SOCK_STREAM, 0 );
	if( m_sSocket == INVALID_SOCKET )
		return FALSE;

	return TRUE;
}

// IOCP에 Receive 이벤트를 등록시킨다.
INT CCommSocket::Receive()
{
	DWORD	dwReadBytes	= 0;
	DWORD	dwFlag		= 0;
	int nRet = ::WSARecv( m_sSocket, &m_RecvBuffer, 1, &dwReadBytes, &dwFlag, NULL, NULL );

	if( nRet == SOCKET_ERROR )
	{
		return -1;
	}

	return dwReadBytes;
}

// IOCP에 Send 이벤트를 등록시킨다.
BOOL CCommSocket::Send( const char* pData, int nDataLen )
{
	m_dwSentBytes	= 0;

	::memcpy( m_SendBuffer.buf, pData, nDataLen );
	m_SendBuffer.len	= nDataLen;

	int nRetVal = ::WSASend( m_sSocket, &m_SendBuffer, 1, &m_dwSentBytes, 0, NULL, NULL );

	if( nRetVal == SOCKET_ERROR )
	{
		m_dwSentBytes = 0;
		return FALSE;
	}

	if( nDataLen == (int)m_dwSentBytes )
		return TRUE;

	return FALSE;
}


/**************************************************************************************
	Public Implementations
***************************************************************************************/

// IO Type을 반환한다.
/*
COverlappedSocket::IOType CCommSocket::GetIOType( LPOVERLAPPED lpOv )
{
	if( lpOv == &m_ovRecvChannel )
		return COverlappedSocket::IOType::RecvType;
	else
		return COverlappedSocket::IOType::SendType;
}
*/