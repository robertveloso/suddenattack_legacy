#include "StdAfx.h"
#include ".\TcpAsyncSelect.h"
#include <Mswsock.h>

using namespace GamehiSolution::Network;
using namespace GamehiSolution::Network::TcpAsyncSelect;
using namespace GamehiSolution::Common::DataStructure;

/**************************************************************************************
	생성자 / 소멸자
***************************************************************************************/
CTcpAsyncSelect::CTcpAsyncSelect( HWND hWindow, UINT uiMessage )
:	m_pNetworkHandler		( NULL		)
,	m_aListener				( NULL		)
,	m_nNumOfConnectSockets	( 0			)
,	m_nNumOfListenSockets	( 0			)
,	m_nRecvBuffSize			( 0			)
,	m_nSendBuffSize			( 0			)
,	m_hWindow				( hWindow	)
,	m_uiMessage				( uiMessage	)
{
}

CTcpAsyncSelect::~CTcpAsyncSelect(void)
{
}


/**************************************************************************************
	Public Operations
***************************************************************************************/


// 리소스들을 생성한다.
//		nNumOfCommSockets : Iocp Server가 생성시킬 수 있는 소켓의 갯수
//		nRecvBuffSize : Iocp Server의 클라이언트와 통신을 하는 각 객체의 Recv Buffer의 크기
//		nSendBuffSize : Iocp Server의 클라이언트와 통신을 하는 각 객체의 Send Buffer의 크기
//		nAcceptPoolSize : Accept를 보다 빠르게 하기 위해서 미리 등록할 Accept의 Pool의 크기
//		bAcceptRecvEvent : Accept 이벤트를 받을지 여부, True이면 Accept이벤트를 받지 않고, 최초의 Recv 발생시 AcceptRecv 이벤트를 받는다.

BOOL CTcpAsyncSelect::Create( ITcpNetworkHandler* pNetworkHandler, INT nNumOfListener, INT nNumOfConnectSockets, INT nRecvBuffSize, INT nSendBuffSize/*, UINT nAcceptPoolSize, BOOL bAcceptRecvEvent = FALSE*/ )
{
	// 멤버 변수 설정
	m_pNetworkHandler		= pNetworkHandler;
	m_nNumOfConnectSockets	= nNumOfConnectSockets;
	m_nNumOfListenSockets	= nNumOfListener;

	m_nRecvBuffSize			= nRecvBuffSize;
	m_nSendBuffSize			= nSendBuffSize;

	m_nCreatedLister		= 0;

	// Listener 생성
	m_aListener	= new TagListener[ nNumOfListener ];
	for( int i = 0; i < nNumOfListener; i++ )
	{
		m_aListener[i].ListenSocket.SetServiceIndex( -1 );
//		m_aListener[i].nServicerIndex	= -1;
	}
	if( m_aListener == NULL )
	{
		Destroy();
		return FALSE;
	}

	// Connect Socket Pool 생성
	if( !m_ConnectSocketPool.Create( nNumOfConnectSockets ) )
	{
		Destroy();
		return FALSE;
	}
	//
	// Overlapped Socket Pool의 모든 객체들의 버퍼를 생성
	//
	CCommSocket** paCommSocket = new CCommSocket*[nNumOfConnectSockets];
	for( INT k = 0; k < nNumOfConnectSockets; k++ )
	{
		paCommSocket[k] = m_ConnectSocketPool.Alloc();
		if( !paCommSocket[k]->Create( &m_ConnectSocketPool, nRecvBuffSize, nSendBuffSize ) )
		{
			Destroy();
			return FALSE;
		}
	}
	for( INT j = 0; j < nNumOfConnectSockets; j++ )
	{
		m_ConnectSocketPool.Free( paCommSocket[j] );
	}
	delete[] paCommSocket;


	return TRUE;
}

// 리소스들을 제거한다.
void CTcpAsyncSelect::Destroy()
{
	/*
	INT nSize = m_ConnectSocketPool.GetPoolSize();
	CCommSocket** paCommSocket = new CCommSocket*[nSize];
	for( INT i = 0; i < nSize; i++ )
	{
		paCommSocket[i] = m_ConnectSocketPool.Alloc();
		paCommSocket[i]->Destroy();
	}
	for( INT j = 0; j < nSize; j++ )
	{
		m_ConnectSocketPool.Free( paCommSocket[j] );
	}
	delete[] paCommSocket;
	*/


	m_ConnectSocketPool.Destroy();

	for( int k = 0; k < m_nCreatedLister; k++ )
	{
		m_aListener[k].CommSocketPool.Destroy();
		m_aListener[k].ListenSocket.Destroy();
	}

	if( m_aListener )
		delete[] m_aListener;
	m_aListener = NULL;


	m_nNumOfConnectSockets	= 0;
	m_nNumOfConnectSockets	= 0;
	m_nCreatedLister		= 0;
}

void CTcpAsyncSelect::FirePacket( DWORD wParam, DWORD lParam )
{
	SOCKET	sSocket		= wParam;
	const WORD wEvent	= WSAGETSELECTEVENT( lParam );
	const WORD wError	= WSAGETSELECTERROR( lParam );

	if( wError != 0 )
	{
		ProcessClose( sSocket );
		return;
	}

	switch( wEvent )
	{
	case FD_ACCEPT:
		ProcessAccept( sSocket );
		break;

	case FD_CONNECT:
		ProcessConnect( sSocket );
		break;

	case FD_CLOSE:
		ProcessClose( sSocket );
		break;

	case FD_READ:
		ProcessRecv( sSocket );
		break;

	case FD_WRITE:
		ProcessSend( sSocket );
		break;

	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
BOOL CTcpAsyncSelect::CreateListener( INT nServiceIndex, INT nNumOfCommSockets, INT nAcceptPoolSize, BOOL bAcceptRecvEvent/* = FALSE*/ )
{
	// bAcceptRecvEvent는 지원하지 않는다.
	if( bAcceptRecvEvent )
	{
		return FALSE;
	}
	else
	{
//		m_aListener[m_nCreatedLister].sListenSocket = ::socket( AF_INET, SOCK_STREAM, 0 );
		if( !m_aListener[m_nCreatedLister].ListenSocket.Create(
				&m_aListener[m_nCreatedLister].CommSocketPool ) )
			return FALSE;

//		if( m_aListener[m_nCreatedLister].sListenSocket == INVALID_SOCKET )
//			return FALSE;

		if( !m_aListener[m_nCreatedLister].CommSocketPool.Create( nNumOfCommSockets ) )
		{
			return FALSE;
		}
	}

//	m_aListener[m_nCreatedLister].nServicerIndex = nServiceIndex;
	m_aListener[m_nCreatedLister].ListenSocket.SetServiceIndex( nServiceIndex );

	m_nCreatedLister++;

	return TRUE;
}

// 서버의 Listen 소켓에 Bind를 한다.
BOOL CTcpAsyncSelect::Bind( INT nServiceIndex, const sockaddr* addr, INT namelen, INT nListenQueue/* = 5*/ )
{
	// Listener 검색
	int nIndex = -1;
	for( int i = 0; i < m_nNumOfListenSockets; i++ )
	{
//		if( nServiceIndex == m_aListener[i].nServicerIndex )
		if( nServiceIndex == m_aListener[i].ListenSocket.GetServiceIndex() )
		{
			nIndex = i;
			break;
		}
	}
	if( nIndex < 0 )
		return FALSE;

	// binding
//	if( !::bind( m_aListener[nIndex].sListenSocket, (LPSOCKADDR)&addr, namelen ) )
	if( !m_aListener[nIndex].ListenSocket.Bind( addr, namelen ) )
	{
		return FALSE;
	}

	// listening
//	if( ::listen( m_aListener[nIndex].sListenSocket, nListenQueue ) == SOCKET_ERROR )
	if( !m_aListener[nIndex].ListenSocket.Listen( nListenQueue ) )
		return FALSE;

	return TRUE;
}

// 서버의 Listen 소켓에 Bind를 한다.
BOOL CTcpAsyncSelect::Bind( INT nServiceIndex, INT nPort, u_long hostAddr/* = INADDR_ANY*/, short family/* = AF_INET*/, INT nListenQueue/* = 5*/ )
{
	// Listener 검색
	int nIndex = -1;
	for( int i = 0; i < m_nNumOfListenSockets; i++ )
	{
//		if( nServiceIndex == m_aListener[i].nServicerIndex )
		if( nServiceIndex == m_aListener[i].ListenSocket.GetServiceIndex() )
		{
			nIndex = i;
			break;
		}
	}
	if( nIndex < 0 )
		return FALSE;

	if( !m_aListener[nIndex].ListenSocket.Bind( nPort, hostAddr, family ) )
		return FALSE;

	if( !m_aListener[nIndex].ListenSocket.Listen( nListenQueue ) )
		return FALSE;

	/*

	// binding
	SOCKADDR_IN	serverSockAddr;

    serverSockAddr.sin_addr.s_addr	= htonl( hostAddr );
	serverSockAddr.sin_family		= family;
	serverSockAddr.sin_port			= htons( nPort );

	if( !::bind( m_aListener[nIndex].sListenSocket, (LPSOCKADDR)&serverSockAddr, sizeof(serverSockAddr) ) )
	{
		return FALSE;
	}

	// listening
	if( ::listen( m_aListener[nIndex].sListenSocket, nListenQueue ) == SOCKET_ERROR )
		return FALSE;
	*/

	return TRUE;
}

BOOL CTcpAsyncSelect::StartListen( INT nServiceIndex )
{
	// Listener 검색
	int nIndex = -1;
	for( int i = 0; i < m_nNumOfListenSockets; i++ )
	{
//		if( nServiceIndex == m_aListener[i].nServicerIndex )
		if( nServiceIndex == m_aListener[i].ListenSocket.GetServiceIndex() )
		{
			nIndex = i;
			break;
		}
	}
	if( nIndex < 0 )
		return FALSE;


	// Start Listen
	int nRetVal = ::WSAAsyncSelect(
		m_aListener[nIndex].ListenSocket.m_sSocket,
		m_hWindow,
		m_uiMessage,
		FD_ACCEPT | FD_CLOSE );

	if( nRetVal == INVALID_SOCKET )
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////


// 다른 서버와 연결을 한다.
BOOL CTcpAsyncSelect::Connect( unsigned long lpOwnerKey, const sockaddr* addr, INT namelen )
{
	CCommSocket* pCommSocket = m_ConnectSocketPool.Alloc();
	if( pCommSocket == NULL )
		return FALSE;

	if( !pCommSocket->CreateSocket() )
	{
		m_ConnectSocketPool.Free( pCommSocket );
		return FALSE;
	}

	pCommSocket->SetOwnerKey( lpOwnerKey );

	int nRetVal = ::WSAAsyncSelect(
		pCommSocket->GetSocket(),
		m_hWindow,
		m_uiMessage,
		FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE );

	if( nRetVal == INVALID_SOCKET )
	{
		pCommSocket->CloseSocket();
		m_ConnectSocketPool.Free( pCommSocket );
		return FALSE;
	}

	m_pNetworkHandler->OnAllocNetworkObject( (unsigned long)pCommSocket );

	// Connect된 소켓을 맵에 등록
	m_mapSocket.mapSocketKey.insert( SocketKey_Pair( pCommSocket->GetSocket(), pCommSocket ) );
				
	m_pNetworkHandler->OnBeginConnect( (unsigned long)pCommSocket );

	int nRet = ::WSAConnect( pCommSocket->GetSocket(), addr, namelen, NULL, NULL, NULL, NULL );
	if( nRet == SOCKET_ERROR )
	{
		if( ::WSAGetLastError() == WSAEWOULDBLOCK )
		{
			return TRUE;
		}
		else
		{
			pCommSocket->CloseSocket();
			m_ConnectSocketPool.Free( pCommSocket );
			m_pNetworkHandler->OnFreeNetworkObject( (unsigned long)pCommSocket );
			return FALSE;
		}
	}

	
	// Connect된 소켓을 맵에 등록
	m_mapSocket.mapSocketKey.insert( SocketKey_Pair( pCommSocket->GetSocket(), pCommSocket ) );
	
	m_pNetworkHandler->OnBeginConnect( (unsigned long)pCommSocket );

	return TRUE;
}


// sendBuff의 값을 전송한다.
BOOL CTcpAsyncSelect::Send( unsigned long lpNetworkKey, const char* sendBuff, INT nLen, INT& nOutError )
{
	CCommSocket* pCommSocket = (CCommSocket*)lpNetworkKey;

	return pCommSocket->Send( sendBuff, nLen );
}


// 해당 키의 소켓 객체를 닫는다.
void CTcpAsyncSelect::CloseSocket( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;

	// 해당 소켓을 닫는다.
	if( pCommSocket->CloseSocket() )
	{
		ProcessClose( pCommSocket->GetSocket() );
//		::SendMessage( m_hWindow, m_uiMessage, (DWORD)pCommSocket->GetSocket(), MAKEWORD( FD_CLOSE, 0 ) );
	}
}

// 해당키의 소켓 객체를 Shutdown한다.
void CTcpAsyncSelect::ShutDown( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;

	pCommSocket->ShutDown();
}

void CTcpAsyncSelect::SetConnectRecvEvent( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	pCommSocket->SetEvent();
}

void CTcpAsyncSelect::ResetConnectRecvEvent( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	pCommSocket->ResetEvent();
}

void CTcpAsyncSelect::WaitConnectRecvEvent( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	pCommSocket->WaitEvent();
}

void CTcpAsyncSelect::SetRecvDisconnectEvent( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	pCommSocket->SetEvent2();
}

void CTcpAsyncSelect::ResetRecvDisconnectEvent( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	pCommSocket->ResetEvent2();
}

void CTcpAsyncSelect::WaitRecvDisconnectEvent( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	pCommSocket->WaitEvent2();
}


void CTcpAsyncSelect::ResetRttTime( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	pCommSocket->ResetRttTime();
}

BOOL CTcpAsyncSelect::CanStartRTT( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	return pCommSocket->CanStartRTT();
}

void CTcpAsyncSelect::SetRttStartTime( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	pCommSocket->SetRttStartTime();
}

void CTcpAsyncSelect::SetRttEndTime( unsigned long lpNetworkKey )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	pCommSocket->SetRttEndTime();
}

void CTcpAsyncSelect::GetRttTime( unsigned long lpNetworkKey, DWORD& out_dwRttTime )
{
	CCommSocket*	pCommSocket = (CCommSocket*)lpNetworkKey;
	out_dwRttTime = pCommSocket->GetRttTime();
}



/**************************************************************************************
	Private Operations
***************************************************************************************/

inline void CTcpAsyncSelect::ProcessAccept( SOCKET sSocket )
{
	CCommSocket* pCommSocket = m_ConnectSocketPool.Alloc();
	if( pCommSocket == NULL )
		return;

	if( !pCommSocket->IsCreated() )
	{
		pCommSocket->Create( &m_ConnectSocketPool, m_nRecvBuffSize, m_nSendBuffSize );
	}

	pCommSocket->SetSentBytes( 0 );
//	pCommSocket->Init();
	pCommSocket->m_sSocket	= ::accept( sSocket, NULL, NULL );

	if( pCommSocket->m_sSocket == INVALID_SOCKET )
	{
		pCommSocket->CloseSocket();
		m_ConnectSocketPool.Free( pCommSocket );
		return;
	}

	// Accept된 소켓의 Listen 소켓을 검색한다.
	//unsigned long lpOwnerKey = NULL;
	CListenSocket* pParentSocket = NULL;
    for( INT i = 0; i < m_nCreatedLister; i++ )
	{
		if( sSocket == m_aListener[i].ListenSocket.m_sSocket )
		{
//			lpOwnerKey = reinterpret_cast<unsigned long>( &m_aListener[i].ListenSocket );
			pParentSocket = &m_aListener[i].ListenSocket;
			break;
		}
	}
	// 이쪽으로 오면 이미 버그
	if( pParentSocket == NULL )
	{
		pCommSocket->CloseSocket();
		m_ConnectSocketPool.Free( pCommSocket );
		return;
	}

	// Parent 소켓 설정
	pCommSocket->m_pParentSocket	= pParentSocket;

	int nRetVal = ::WSAAsyncSelect(
		pCommSocket->GetSocket(),
		m_hWindow,
		m_uiMessage,
		FD_READ | FD_WRITE | FD_CLOSE );

	if( nRetVal == INVALID_SOCKET )
	{
		pCommSocket->CloseSocket();
		m_ConnectSocketPool.Free( pCommSocket );
		return;
	}

	m_pNetworkHandler->OnAllocNetworkObject( (unsigned long)pCommSocket );


	// Accept된 소켓을 맵에 등록
	m_mapSocket.mapSocketKey.insert( SocketKey_Pair( pCommSocket->GetSocket(), pCommSocket ) );


	m_pNetworkHandler->OnAccept( (unsigned long)pCommSocket );
}

inline void CTcpAsyncSelect::ProcessConnect( SOCKET sSocket )
{
	m_mapSocket.IterSocketKey = m_mapSocket.mapSocketKey.find( sSocket );
	if( m_mapSocket.IterSocketKey == m_mapSocket.mapSocketKey.end() )
	{
		return;
	}

	CCommSocket* pCommSocket = m_mapSocket.IterSocketKey->second;

	m_pNetworkHandler->OnConnect( (unsigned long)pCommSocket );
}

inline void CTcpAsyncSelect::ProcessClose( SOCKET sSocket )
{
	m_mapSocket.IterSocketKey = m_mapSocket.mapSocketKey.find( sSocket );
	if( m_mapSocket.IterSocketKey == m_mapSocket.mapSocketKey.end() )
	{
		return;
	}
	CCommSocket*					pCommSocket		= m_mapSocket.IterSocketKey->second;
	CMemoryPoolSafty<CCommSocket>*	pCommSocketPool	= pCommSocket->GetCommSocketPool();
	pCommSocket->CloseSocket();

	m_pNetworkHandler	->OnClose				( (unsigned long)pCommSocket );
	pCommSocketPool		->Free					( pCommSocket );
	m_pNetworkHandler	->OnFreeNetworkObject	( (unsigned long) pCommSocket );

	m_mapSocket.mapSocketKey.erase( sSocket );
}

inline void CTcpAsyncSelect::ProcessRecv( SOCKET sSocket )
{
	m_mapSocket.IterSocketKey = m_mapSocket.mapSocketKey.find( sSocket );
	if( m_mapSocket.IterSocketKey == m_mapSocket.mapSocketKey.end() )
	{
		return;
	}

	CCommSocket*	pCommSocket = m_mapSocket.IterSocketKey->second;
 	int				nReceive	= pCommSocket->Receive();

	if( nReceive <= 0 )
	{
		return;
	}

	// Recv 이벤트를 통지한다.
	m_pNetworkHandler->OnReceive(
		(unsigned long)pCommSocket,
		pCommSocket->GetRecvBuffer(),
		nReceive );
}

inline void CTcpAsyncSelect::ProcessSend( SOCKET sSocket )
{
	m_mapSocket.IterSocketKey = m_mapSocket.mapSocketKey.find( sSocket );
	if( m_mapSocket.IterSocketKey == m_mapSocket.mapSocketKey.end() )
	{
		return;
	}

	CCommSocket*	pCommSocket = m_mapSocket.IterSocketKey->second;

//	if( pCommSocket->GetSentBytes() == 0 )
//		return;

	// Send 이벤트를 통지한다.
	m_pNetworkHandler->OnSend(
		(unsigned long)pCommSocket,
		pCommSocket->GetSendBuffer(),
		pCommSocket->GetSentBytes()
		);
}

/*
// 등록된 IO핸들 처리중 Accept
inline void CTcpAsyncSelect::ProcessAcceptCompletion( DWORD dwNumberOfBytesTransferred, unsigned long lpNetworkKey, LPOVERLAPPED lpOverlapped )
{
	COverlappedListenSocket*					pListenSocket;
	CMemoryPoolSafty<COverlappedCommSocket>*	pCommSocketPool;
	COverlappedCommSocket*						pCommSocket;
	
	pListenSocket	= (COverlappedListenSocket*)lpNetworkKey;
	pCommSocketPool	= pListenSocket->GetCommSocketPool();

	// Recv/Send용 overlapped 객체 할당
	pCommSocket = pCommSocketPool->Alloc();

	if( pCommSocket == NULL )
	{
		::closesocket( pListenSocket->GetSocketOfOverlappedObject( lpOverlapped ) );

		// 다음 AcceptEx를 설정
		if( !pListenSocket->AcceptEx( lpOverlapped ) )
		{
			//
			// ???????????????????????????????????
			//
		}
		return;
	}

	pCommSocket->Init();

	// Parent 소켓 설정
	pCommSocket->m_pParentOvSocket	= pListenSocket;

	// Overlapped Object가 할당되었음을 통보한다.
	m_pNetworkHandler->OnAllocOverlappedObject( (unsigned long)pCommSocket );

	pCommSocket->SetSocket(	pListenSocket->GetSocketOfOverlappedObject( lpOverlapped ) );

	// AcceptEx 받은 객체의 연결
	INT err;
	err = ::setsockopt( pCommSocket->GetSocket(), 
	    SOL_SOCKET, 
	    SO_UPDATE_ACCEPT_CONTEXT, 
	    (char *)&pCommSocket->m_pParentOvSocket->m_sSocket, 
	    sizeof(pCommSocket->m_pParentOvSocket->m_sSocket) );

	// IOCP에 Comm Socket 핸들 등록
	if( !m_AsyncIocp.AssignIoCompletionPort( pCommSocket->GetIOHandle(), (unsigned long)pCommSocket ) )
	{
		// 소켓을 끊고, 해당 핸들을 반환처리 한다.
		pCommSocket->CloseSocket();
		pCommSocketPool->Free( pCommSocket );

		// Overlapped Object가 해제되었음을 통보한다.
		m_pNetworkHandler->OnFreeOverlappedObject( (unsigned long)pCommSocket );

		// 다음 AcceptEx를 설정
		if( !pListenSocket->AcceptEx( lpOverlapped ) )
		{
			// ??????????????????????????????????????????????
		}
		return;
	}


	// Accept 이벤트를 통보한다.
	m_pNetworkHandler->OnAccept( (unsigned long)pCommSocket );



	// IOCP에 Receive 이벤트 등록
	BOOL bRet;
	bRet = pCommSocket->RecvPost();
	if( bRet )
	{
		// 소켓을 끊고, 해당 핸들을 반환처리 한다.
		INT ErrCode = ::WSAGetLastError();
		switch( ErrCode )
		{
		case WSA_IO_PENDING:	// Overlapped operations will complete later
			break;

		default:
			pCommSocket->CloseSocket();
			m_pNetworkHandler->OnClose( (unsigned long)pCommSocket );

			pCommSocketPool->Free( pCommSocket );

			// Overlapped Object가 해제되었음을 통보한다.
			m_pNetworkHandler->OnFreeOverlappedObject( (unsigned long)pCommSocket );

			// 다음 AcceptEx를 설정
			if( !pListenSocket->AcceptEx( lpOverlapped ) )
			{
				// ????????????????????????????????????????
			}
			return;
		}
	}

	// 다음 AcceptEx를 설정
	if( !pListenSocket->AcceptEx( lpOverlapped ) )
	{
		// ????????????????????????????????????
	}
}

// 등록된 IO핸들 처리중 Accept and Recv
inline void CTcpAsyncSelect::ProcessAcceptRecvCompletion( DWORD dwNumberOfBytesTransferred, unsigned long lpNetworkKey, LPOVERLAPPED lpOverlapped )
{
}

// 등록된 IO핸들 처리중 Recv
inline void CTcpAsyncSelect::ProcessRecvCompletion( DWORD dwNumberOfBytesTransferred, unsigned long lpNetworkKey, LPOVERLAPPED lpOverlapped )
{
	COverlappedCommSocket*						pCommSocket;
	CMemoryPoolSafty<COverlappedCommSocket>*	pCommSocketPool;

	pCommSocket		= (COverlappedCommSocket*)lpNetworkKey;
	pCommSocketPool	= pCommSocket->GetCommSocketPool();


	// 클라이언트가 연결을 끊음
	if( dwNumberOfBytesTransferred == 0 )
	{
		// 소켓을 끊고, 해당 핸들을 반환처리 한다.
		// 해당 핸들의 반환 및 close 통보는 Recv에서만 수행한다.
		INT nError = ::WSAGetLastError();
		pCommSocket->CloseSocket();

		m_pNetworkHandler->OnClose( (unsigned long)pCommSocket );

		pCommSocketPool->Free( pCommSocket );

		// Overlapped Object가 해제되었음을 통보한다.
		m_pNetworkHandler->OnFreeOverlappedObject( lpNetworkKey );
		return;
	}

	// Recv 이벤트를 통지한다.
	m_pNetworkHandler->OnReceive(
		lpNetworkKey,
		pCommSocket->GetRecvBuffer(),
		dwNumberOfBytesTransferred );

	// 다음 Recv 이벤트를 등록한다.
	BOOL bRet;
	bRet = pCommSocket->RecvPost();
	if( bRet )
	{
		// 소켓을 끊고, 해당 핸들을 반환처리 한다.
		INT ErrCode = ::WSAGetLastError();
		switch( ErrCode )
		{
		case WSA_IO_PENDING:	// Overlapped operations will complete later
			break;

			// 알수 없는 연결 해제..
		default:
			pCommSocket->CloseSocket();

			m_pNetworkHandler->OnClose( (unsigned long)pCommSocket );

			pCommSocketPool->Free( pCommSocket );

			// Overlapped Object가 해제되었음을 통보한다.
			m_pNetworkHandler->OnFreeOverlappedObject( lpNetworkKey );
			return;
		}
	}
}

// 등록된 IO핸들 처리중 Send
inline void CTcpAsyncSelect::ProcessSendCompletion( DWORD dwNumberOfBytesTransferred, unsigned long lpNetworkKey, LPOVERLAPPED lpOverlapped )
{
	COverlappedCommSocket* pCommSocket;
	pCommSocket = (COverlappedCommSocket*)lpNetworkKey;


	// Send 이벤트를 통지한다.
	m_pNetworkHandler->OnSend( lpNetworkKey );
}


/**************************************************************************************
	Protected Implementations
*************************************************************************************** /

// IIocp Implementation : 등록된 IO 핸들의 완료 이벤트를 받기 위한 인터페이스
void CTcpAsyncSelect::OnGetQueuedCompletionStatus( BOOL bRet, DWORD dwNumberOfBytesTransferred, unsigned long lpNetworkKey, LPOVERLAPPED lpOverlapped )
{
	COverlappedSocket* pOverlappedSocket = (COverlappedSocket*)lpNetworkKey;

	switch( pOverlappedSocket->GetIOType( lpOverlapped ) )
	{
	case COverlappedSocket::AcceptType :
		ProcessAcceptCompletion			( dwNumberOfBytesTransferred, lpNetworkKey, lpOverlapped );
		break;

	case COverlappedSocket::AcceptRecvType :
		ProcessAcceptRecvCompletion		( dwNumberOfBytesTransferred, lpNetworkKey, lpOverlapped );
		break;

	case COverlappedSocket::RecvType :
		ProcessRecvCompletion			( dwNumberOfBytesTransferred, lpNetworkKey, lpOverlapped );
		break;

	case COverlappedSocket::SendType :
		ProcessSendCompletion			( dwNumberOfBytesTransferred, lpNetworkKey, lpOverlapped );
		break;

	default :
		// ???????????????????????????????????????
		break;
	}
}
*/