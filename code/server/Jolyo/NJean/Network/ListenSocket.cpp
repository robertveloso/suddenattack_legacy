#include "StdAfx.h"
#include ".\ListenSocket.h"

#include <Mswsock.h>

using namespace Network::TcpAsyncSelect;
using namespace	Common::DataStructure;

////////////////////////////////////////////////////////////////////////////////////////
//
//	CListenSocket
//

// 생성/소멸자
CListenSocket::CListenSocket(void)
{
//	m_aOverlappedSocket	= NULL;
	m_Buff				= NULL;
//	m_aOverlappedSocket	= NULL;
	m_nAcceptPoolSize	= 0;
//	m_pParentOvSocket	= NULL;
	m_sSocket			= INVALID_SOCKET;
}

CListenSocket::~CListenSocket(void)
{
}



/**************************************************************************************
	Public Operations
***************************************************************************************/

// 리소스들을 생성한다.
BOOL CListenSocket::Create( CMemoryPoolSafty<CCommSocket>* pCommSocketPool, UINT nRecvBuffSize/* = 0*/, UINT nAcceptPoolSize/* = 100*/ )
{
	m_pCommSocketPool = pCommSocketPool;

	//
	// 버퍼를 생성한다 : 기본적으로 16byte의 buffer는 할당을 해야 한다. 자세한 사항은 MSDN의 AcceptEx를 참조
	//					 nAcceptPoolSize의 개수만큼 미리 한번에 할당을 한다.
	//
	int nAcceptExBufferSize = 2 * ( sizeof(SOCKADDR_IN) + 16 );
	m_Buff = new char[ ( nRecvBuffSize + nAcceptExBufferSize ) * nAcceptPoolSize ];
	if( NULL == m_Buff )
	{
		m_dwErrCode = ErrorCode::BufferCreateFail;
		return FALSE;
	}
	m_nBuffSize = nRecvBuffSize+16;

	//
	// 소켓을 생성한다.
	//
	m_sSocket = INVALID_SOCKET;
	m_sSocket	= ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( INVALID_SOCKET == m_sSocket )
	{
		delete[] m_Buff;
		m_Buff = NULL;

		m_dwErrCode = ErrorCode::SocketCreateFail;
		return FALSE;
	}

	//
	// Accept Pool용 소켓을 생성한다.
	//
	/*
	m_nAcceptPoolSize = nAcceptPoolSize;
	if( m_aOverlappedSocket )
	{
		delete[] m_aOverlappedSocket;
		m_aOverlappedSocket = NULL;
	}
	m_aOverlappedSocket = new TagOverlappedSocket[m_nAcceptPoolSize];
	if( NULL == m_aOverlappedSocket )
	{
		delete[] m_Buff;
		m_Buff = NULL;
		::closesocket( m_sSocket );

		m_dwErrCode = ErrorCode::AcceptPoolCreateFail;
		return FALSE;
	}
	for( UINT i = 0; i < m_nAcceptPoolSize; i++ )
	{
		m_aOverlappedSocket[i].Buff = &m_Buff[ ( nRecvBuffSize + nAcceptExBufferSize ) * i ];
	}
	*/


	return TRUE;
}

// 리소스들을 해제한다.
void CListenSocket::Destroy()
{
	::closesocket( m_sSocket );

	if( m_Buff )
		delete[] m_Buff;

	/*
	if( m_aOverlappedSocket )
		delete[] m_aOverlappedSocket;

	m_Buff				= NULL;
	m_aOverlappedSocket	= NULL;
	*/
}

// 바인드를 한다.
BOOL CListenSocket::Bind( const sockaddr* addr, int namelen )
{
	if( ::bind( m_sSocket, addr, namelen ) )
		return FALSE;

	return TRUE;
}

// 바인드를 한다.
BOOL CListenSocket::Bind( int nPort, u_long hostAddr/* = INADDR_ANY*/, short family/* = AF_INET*/ )
{
	SOCKADDR_IN	serverSockAddr;

    serverSockAddr.sin_addr.s_addr	= htonl( hostAddr );
	serverSockAddr.sin_family		= family;
	serverSockAddr.sin_port			= htons( nPort );

	if( ::bind( m_sSocket, (LPSOCKADDR)&serverSockAddr, sizeof(serverSockAddr) ) )
	{
		int a = ::WSAGetLastError();
		return FALSE;
	}

	return TRUE;
}

// 리슨을 한다.
BOOL CListenSocket::Listen( int nBacklog/* = 5*/ )
{
	if( ::listen( m_sSocket, nBacklog ) == SOCKET_ERROR )
		return FALSE;
	else
		return TRUE;
}

// Accept Pool을 등록한다.
BOOL CListenSocket::RunAcceptPool()
{
	// Recv 이벤트를 받을 경우
	if( m_nBuffSize > 16 )
	{
	}

	// Accept 이벤트를 받을 경우
	else
	{
		/*
		for( UINT i = 0; i < m_nAcceptPoolSize; i++ )
		{
			//
			// accept할 소켓 생성
			//
			m_aOverlappedSocket[i].sAcceptSocket = CreateSocket( m_dwErrCode );
			if( INVALID_SOCKET == m_aOverlappedSocket[i].sAcceptSocket )
				return FALSE;

			//
			// Overlapped 초기화
			//
			::ZeroMemory( &m_aOverlappedSocket[i], sizeof(WSAOVERLAPPED) );
		}

		//
		// AcceptEx
		//
		for( UINT i = 0; i < m_nAcceptPoolSize; i++ )
		{
			DWORD       dwRecvNumBytes = 0;
			BOOL bRet = ::AcceptEx(
				m_sSocket,
				m_aOverlappedSocket[i].sAcceptSocket,
				m_aOverlappedSocket[i].Buff,
				0,
				sizeof(SOCKADDR_IN) + 16,
				sizeof(SOCKADDR_IN) + 16,
				&dwRecvNumBytes,
				&m_aOverlappedSocket[i] );

			if( !bRet ) 
			{
				int ErrCode = ::WSAGetLastError();
				if( ErrCode != WSA_IO_PENDING )
				{
					m_dwErrCode = ErrorCode::AcceptExFail;
					return FALSE;
				}
			}
		}
		*/
	}

	return TRUE;
}

// overlappedSocket 객체를 다시 AccextEx한다.
BOOL CListenSocket::AcceptEx( LPOVERLAPPED ovlappedSocket )
{
	/*
	DWORD dwError;
	TagOverlappedSocket* pOvSocket = (TagOverlappedSocket*)ovlappedSocket;
	pOvSocket->sAcceptSocket = CreateSocket( dwError );
	if( INVALID_SOCKET == pOvSocket->sAcceptSocket )
	{
		m_dwErrCode = dwError;
		return FALSE;
	}

	DWORD       dwRecvNumBytes = 0;
	BOOL bRet = ::AcceptEx(
		m_sSocket,
		pOvSocket->sAcceptSocket,
		pOvSocket->Buff,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&dwRecvNumBytes,
		pOvSocket );

	if( !bRet ) 
	{
		int ErrCode = ::WSAGetLastError();
		if( ErrCode != WSA_IO_PENDING )
		{
			m_dwErrCode = ErrorCode::AcceptExFail;
			::closesocket( pOvSocket->sAcceptSocket );

			::printf( "Fail AcceptEx : Can't Reg IOCP\n" );
			return FALSE;
		}
	}

	*/
	return TRUE;
}


/**************************************************************************************
	Private Operations
***************************************************************************************/

// iocp용 소켓을 생성한다.
inline SOCKET CListenSocket::CreateSocket( DWORD& nOutErrorcode )
{
	SOCKET socket;

	//
	// accept할 소켓 생성
	//
	int         nRet;
	int         nZero = 0;
	LINGER      lingerStruct;

	socket = INVALID_SOCKET;
	socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED); 
	if( INVALID_SOCKET == socket )
	{
		nOutErrorcode = ErrorCode::SocketCreateFail;
		return INVALID_SOCKET;
	}

	// Disable send buffering on the socket.  Setting SO_SNDBUF
	// to 0 causes winsock to stop bufferring sends and perform
	// sends directly from our buffers, thereby save one memory copy.
	nZero = 0;
	nRet = ::setsockopt( socket, SOL_SOCKET, SO_SNDBUF, (char*)&nZero, sizeof(nZero) );
	if( SOCKET_ERROR == nRet )
	{
		nOutErrorcode = ErrorCode::SetSockOptFail;
		return INVALID_SOCKET;
	}

	// Disable receive buffering on the socket.  Setting SO_RCVBUF 
	// to 0 causes winsock to stop bufferring receive and perform
	// receives directly from our buffers, thereby save one memory copy.
	nZero = 0;
	nRet = ::setsockopt( socket, SOL_SOCKET, SO_RCVBUF, (char*)&nZero, sizeof(nZero) );
	if( SOCKET_ERROR == nRet )
	{
		nOutErrorcode = ErrorCode::SetSockOptFail;
		return INVALID_SOCKET;
	}

	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	nRet = ::setsockopt( socket, SOL_SOCKET, SO_LINGER, (char*)&lingerStruct, sizeof(lingerStruct) );
	if (SOCKET_ERROR == nRet) 
	{
		nOutErrorcode = ErrorCode::SetSockOptFail;
		return INVALID_SOCKET;
	}

	return socket;
}





/**************************************************************************************
	Public Implementations
***************************************************************************************/

// IO Type을 반환한다.
/*
COverlappedSocket::IOType CListenSocket::GetIOType( LPOVERLAPPED lpOv )
{
	// Recv 이벤트를 받을 경우
	if( m_nBuffSize > 16 )
	{
		return COverlappedSocket::AcceptRecvType;
	}

	// Accept 이벤트를 받을 경우
	else
	{
		return COverlappedSocket::AcceptType;
	}
}

*/