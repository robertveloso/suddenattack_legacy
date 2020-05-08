/*****************************************************************************************************************
	CCommSocket : CSocket

	작성자 : 강진협
	작성일 : 2004년 2월 25~27일

	설  명 :
		Iocp로부터 Recv/Send 이벤트를 받기 위한 Communication Socket 클래스
	사  용 :
		1. CreateBuffer를 통해서 내부 버퍼를 생성하거나, AssignBuffer를 통해서 외부 버퍼를 설정한다.
		2. CreateBuffer를 사용한 경우에는 반드시 종료시에 DestroyBuffer를 통해서 리소스를 해제한다.
		3. SetSocket을 통해 Iocp에 등록할 Recv/Send를 위한 핸들인 Communication Socket을 설정한다.
		   이는 Accept에 의해 반환된 클라이언트와 연결되는 소켓을 설정해야 한다.
		   COverlappedListenSocket의 GetSocketOfOverlappedObject를 통해 얻은 클라이언트와 연결된 소켓 핸들이 설정된다.
		4. RecvPost를 통해서 Recv 이벤트를 IOCP에 등록시킨다.
		5. Send Buffer를 채운 뒤에 SendPost를 통해서 Send를 IOCP에 등록시킨다.
		   GetSendBuffer를 통해서 Send Buffer를 가져온뒤에, 버퍼를 채우고, 그 크기를 SetSendLength로써 설정한다.
		6. 더이상 사용하지 않는 소켓은 CloseSocket을 통해서 연결을 닫는다.
*****************************************************************************************************************/


#pragma once

#include <Winsock2.h>
#include <Windows.h>

#include "Socket.h"
#include "..\..\DataStructure\MemoryPoolSafty.h"

namespace Network
{
	namespace TcpAsyncSelect
	{
		class CCommSocket :
			public CSocket
		{
			// 생성/소멸자
		public:
			CCommSocket(void);
			~CCommSocket(void);

			// Attributes
		private:
			// Recv/Send를 위해서 iocp에 등록할 overlapped object
			struct TagBuffer
			{
				char*		pBuffer;
				int			nBuffSize;
			};

			WSABUF					m_SendBuffer;
			WSABUF					m_RecvBuffer;
			DWORD					m_dwSentBytes;

			BOOL					m_bCreateBuffer;	// 버퍼의 생성 여부

			BOOL					m_bFirstRecv;		// 첫번째 Recv데이터인지 여부
			// accept/connect와 Recv 사이의 초기 설정에 관한 CS 해결을 위해 사용

			HANDLE					m_hInitEvent;		// accept/connect와 Recv 사이의 초기 설정에 관한 CS 해결을 위해 사용
			HANDLE					m_hCloseEvent;		// Recvd와 Close 사이의 초기 설정에 관한 CS 해결을 위해 사용

			CRITICAL_SECTION		m_csSend;			// Send 동기화를 위한 객체

			Common::DataStructure::CMemoryPoolSafty<CCommSocket>*
				m_pCommSocketPool;						// 자신이 속한 Pool의 포인터

			DWORD					m_dwRTT;			// RTT Time
			DWORD					m_dwRttStart;
			BOOL					m_bRTT;				// RTT 체크할 수 있는지 여부

		public:
			CSocket*				m_pParentSocket;

		public:

			// 버퍼를 생성한다.
			BOOL	Create( Common::DataStructure::CMemoryPoolSafty<CCommSocket>* pCommSocketPool, int nRecvBuffSize, int nSendBuffSize );

			// 버퍼를 제거한다.
			void	Destroy();

			// 미리 할당된 버퍼를 assign한다.
//				void	AssignBuffer( char* pRecvBuff, int nRecvBuffSize, char* pSendBuff, int nSendBuffSize );

			// Connect를 수행한다.
			BOOL	Connect( const sockaddr* addr, int namelen );

			BOOL	CreateSocket();

			// IOCP에 Receive 이벤트를 등록시킨다.
			INT		Receive();

			// IOCP에 Send 이벤트를 등록시킨다.
			BOOL	Send( const char* pData, int nDataLen );

			DWORD	GetSentBytes()
			{
				return m_dwSentBytes;
			}


			// 소켓을 설정한다.
			void	SetSocket( SOCKET socket )
			{
				m_sSocket = socket;
			}

			// Socket을 반환한다.
			SOCKET	GetSocket()
			{
				return m_sSocket;
			}

			LPCTSTR	GetLocalAddr()
			{
				sockaddr_in	addr;
				int			nAddrLen = 16;
				::ZeroMemory( &addr, nAddrLen );
				::getsockname( m_sSocket, (sockaddr*) &addr, &nAddrLen );

				return ::inet_ntoa( addr.sin_addr );
			}

			LPCTSTR	GetPeerAddr()
			{
				sockaddr_in	addr;
				int			nAddrLen = 16;
				::ZeroMemory( &addr, nAddrLen );
				::getpeername( m_sSocket, (sockaddr*) &addr, &nAddrLen );

				return ::inet_ntoa( addr.sin_addr );
			}

			/*
			// Recv용 Overlapped Object인지를 반환한다.
			BOOL	IsRecvOverlappedObject( LPOVERLAPPED lpOv )
			{
				return lpOv == &m_ovRecvChannel;
			}
			*/

			// Recv Buffer를 반환한다.
			char*	GetRecvBuffer()
			{
				return m_RecvBuffer.buf;
			}

			// Send Buffer를 반환한다.
			char*	GetSendBuffer()
			{
				return m_SendBuffer.buf;
			}
			
			Common::DataStructure::CMemoryPoolSafty<CCommSocket>*
					GetCommSocketPool()
			{
				return m_pCommSocketPool;
			}

			// Socket을 Close한다.
			void	CloseSocket()
			{
				if( ::closesocket( m_sSocket ) == SOCKET_ERROR )
				{
//						printf( "Close Error : %d\n", WSAGetLastError() );
				}
			}

			// Socket을 ShutDown한다.
			void	ShutDown()
			{
				if( ::shutdown( m_sSocket, SD_BOTH ) == SOCKET_ERROR )
				{
//						printf( "Shutdown Error : %d\n", WSAGetLastError() );
				}
			}

			// Event를 Reset한다.
			void	ResetEvent()
			{
//					::ResetEvent( m_hInitEvent );
			}
			// Event를 Set한다.
			void	SetEvent()
			{
//					::SetEvent( m_hInitEvent );
			}
			// Event를 Wait한다.
			void	WaitEvent()
			{
//					::WaitForSingleObject( m_hInitEvent, INFINITE );
			}

			// Event를 Reset한다.
			void	ResetEvent2()
			{
//					::ResetEvent( m_hCloseEvent );
			}
			// Event를 Set한다.
			void	SetEvent2()
			{
//					::SetEvent( m_hCloseEvent );
			}
			// Event를 Wait한다.
			void	WaitEvent2()
			{
//					::WaitForSingleObject( m_hCloseEvent, INFINITE );
			}


			void	ResetRttTime()
			{
				m_dwRTT = -1;
				m_bRTT	= TRUE;
			}

			BOOL	CanStartRTT()
			{
				return m_bRTT;
			}

			void	SetRttStartTime()
			{
				m_bRTT			= FALSE;
				m_dwRttStart	= ::GetTickCount();
			}

			void	SetRttEndTime()
			{
				DWORD dwRttEnd = ::GetTickCount();
				if( dwRttEnd >= m_dwRttStart &&
					m_dwRttStart != 0 )
				{
					m_dwRTT	= dwRttEnd - m_dwRttStart;
				}
				m_dwRttStart	= 0;
				m_bRTT			= TRUE;
			}

			DWORD	GetRttTime()
			{
				return m_dwRTT;
			}

			// Implementations
		public:
			// IO Type을 반환한다.
//				virtual IOType GetIOType( LPOVERLAPPED lpOv );
		};// class CCommSocket

	}// namespace TcpIOCP
}// namespace Network
