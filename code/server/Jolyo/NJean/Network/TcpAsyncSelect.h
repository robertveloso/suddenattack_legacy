/*****************************************************************************************************************
	CTcpAsyncSelect

	작성자 : 강진협
	작성일 : 2004년 2월 25~27일

	설  명 :
		IOCP를 이용한 Server
	사  용 :
		1. Create를 사용하여 Iocp Server를 생성한다.
		2. Bind와 Listen을 한다. 이 값은 기본 소켓 프로그램과 동일하다.
		3. Start를 통해서 Iocp Server를 수행시킨다.
		4. Iocp Server는 소켓 핸들로부터 OnAccept, OnReceive, OnSend, OnClose 이벤트를 발생시킨다.
		5. Send를 통해서 Iocp Server에 연결되어 있는 클라이언트에게 메시지를 전달한다.
		6. CloseSocket을 통해서 Iocp Server에 연결되어 있는 특정 소켓을 닫는다.
		7. 모두 사용한 뒤에는 Destroy를 통해 내부 리소스를 제거를 한다.

	수  정 : 2004년 3월 4일
	내  용 : 상속이 가능한 구조로 변경
		1. Accept, Recv, Send, Close 이벤트 발생시 Child Class에서 처리 할 수 있도록 virtual function 제공
		2. Overlapped Object 객체를 할당 받고 제거할 때의 이벤트를 처리 할 수 있도록 virtual function 제공
*****************************************************************************************************************/

#pragma once

//#include <Winsock2.h>
#include <map>

/*
#include "OverlappedListenSocket.h"
#include "OverlappedCommSocket.h"
#include "..\IO\AsyncIocp.h"
*/
#include "..\..\DataStructure\MemoryPoolSafty.h"

#include "Network.h"
#include "CommSocket.h"

namespace Network
{
	namespace TcpAsyncSelect
	{
		class CTcpAsyncSelect : public INetwork//, IO::IOCP::IIocpHandler
		{
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 생성/소멸자
		public:
			CTcpAsyncSelect( HWND hWindow, UINT uiMessage );
			~CTcpAsyncSelect(void);

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Attributes
		private:
			typedef std::pair<SOCKET, CCommSocket*>					SocketKey_Pair;
			struct TagSocketMap
			{
				std::map <SOCKET, CCommSocket*>						mapSocketKey;
				std::map <SOCKET, CCommSocket*>::const_iterator		IterSocketKey;
			};

			struct TagListener
			{
				SOCKET						sListenSocket;
				INT							nServicerIndex;
				Common::DataStructure::CMemoryPoolSafty<SOCKET>
											CommSocketPool;

			};
			HWND						m_hWindow;
			UINT						m_uiMessage;

			Common::DataStructure::CMemoryPoolSafty<CCommSocket>
										m_ConnectSocketPool;

			TagListener*				m_aListener;

			ITcpNetworkHandler*			m_pNetworkHandler;				// Network handler
			INT							m_nCreatedLister;

			TagSocketMap				m_mapSocket;

		protected:

			// 모든 패킷들의 가장 큰 크기로 고정시킨다.
			INT			m_nRecvBuffSize;			// Recv Buffer Size
			INT			m_nSendBuffSize;			// Send Buffer Size
			INT			m_nNumOfConnectSockets;		// TcpAsyncSelect가 접속할 수 있는 소켓의 수
			INT			m_nNumOfListenSockets;		// TcpAsyncSelect에 접속할 수 있는 Port의 수

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Operations
		public:
			// 다른 서버와 연결을 한다.
			virtual BOOL		Connect( ULONG_PTR lpOwnerKey, const sockaddr* addr, INT namelen );



			/////////////////////////////////////////////
			// Create Bind 부터 다음의 절차를 수정한다.
			// n개의 Accept를 위해서


			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Overloadable
		public:
			// 리소스들을 생성한다.
			//		nNumOfCommSockets : Iocp Server가 생성시킬 수 있는 소켓의 갯수
			//		nRecvBuffSize : Iocp Server의 클라이언트와 통신을 하는 각 객체의 Recv Buffer의 크기
			//		nSendBuffSize : Iocp Server의 클라이언트와 통신을 하는 각 객체의 Send Buffer의 크기
			//		nAcceptPoolSize : Accept를 보다 빠르게 하기 위해서 미리 등록할 Accept의 Pool의 크기
			//		bAcceptRecvEvent : Accept 이벤트를 받을지 여부, True이면 Accept이벤트를 받지 않고, 최초의 Recv 발생시 AcceptRecv 이벤트를 받는다.
			virtual BOOL	Create( ITcpNetworkHandler* pNetworkHandler, INT nNumOfListener, INT nNumOfConnectSockets, INT nRecvBuffSize, INT nSendBuffSize/*, UINT nAcceptPoolSize, BOOL bAcceptRecvEvent = FALSE*/ );

			// 리소스들을 생성한다.
			//		nNumOfCommSockets : Iocp Server가 생성시킬 수 있는 소켓의 갯수
			//		nRecvBuffSize : Iocp Server의 클라이언트와 통신을 하는 각 객체의 Recv Buffer의 크기
			//		nSendBuffSize : Iocp Server의 클라이언트와 통신을 하는 각 객체의 Send Buffer의 크기
			//		nAcceptPoolSize : Accept를 보다 빠르게 하기 위해서 미리 등록할 Accept의 Pool의 크기
			//		bAcceptRecvEvent : Accept 이벤트를 받을지 여부, True이면 Accept이벤트를 받지 않고, 최초의 Recv 발생시 AcceptRecv 이벤트를 받는다.
			virtual BOOL	CreateListener( INT nServerIndex, INT nNumOfCommSockets, INT nAcceptPoolSize, BOOL bAcceptRecvEvent = FALSE );

			// 리소스들을 제거한다.
			virtual void	Destroy();

			virtual void	FirePacket( DWORD wParam, DWORD lParam );


			virtual BOOL	Bind( INT nServerIndex, const sockaddr* addr, INT namelen, INT nListenQueue = 5 );
			virtual BOOL	Bind( INT nServerIndex, INT nPort, u_long hostAddr = INADDR_ANY, short family = AF_INET, INT nListenQueue = 5 );

			virtual BOOL	StartListen( INT nServerIndex );






			// 해당 키에 해당하는 context의 send buffer의 ref를 반환한다.
			//		In	: lpNetworkKey - IOCP에 등록한 Completion Key
			//		Ret	: Send Buffer PoINTer
			//virtual char*		GetSendBufferRef( ULONG_PTR lpNetworkKey );

			// sendBuff의 값 내부 버퍼로 카피를 하여 전송한다.
			//		In	: lpNetworkKey - IOCP에 등록한 Completion Key
			//			: sendBuff - 전송할 메시지 버퍼
			//			: nLen - 전송할 메시지의 길이
			//		Out	: nOutError - 전송 실패시 에러코드
			//		Ret : 전송 성공 여부
			virtual BOOL		Send( ULONG_PTR lpNetworkKey, const char* sendBuff, INT nLen, INT& nOutError );

			// 해당 키의 소켓 객체를 닫는다
			//		In	: lpNetworkKey - IOCP에 등록한 Completion Key
			virtual void		CloseSocket( ULONG_PTR lpNetworkKey );

			// 해당키의 소켓 객체를 Shutdown한다.
			//		In	: lpNetworkKey - IOCP에 등록한 Completion Key
			virtual void		ShutDown( ULONG_PTR lpNetworkKey );

			virtual void	SetConnectRecvEvent			( ULONG_PTR lpCompletionKey );
			virtual void	ResetConnectRecvEvent		( ULONG_PTR lpCompletionKey );
			virtual void	WaitConnectRecvEvent		( ULONG_PTR lpCompletionKey );
			virtual void	SetRecvDisconnectEvent		( ULONG_PTR lpCompletionKey );
			virtual void	ResetRecvDisconnectEvent	( ULONG_PTR lpCompletionKey );
			virtual void	WaitRecvDisconnectEvent		( ULONG_PTR lpCompletionKey );

			virtual void	ResetRttTime				( ULONG_PTR lpNetworkKey );
			virtual BOOL	CanStartRTT					( ULONG_PTR lpNetworkKey );
			virtual void	SetRttStartTime				( ULONG_PTR lpNetworkKey );
			virtual void	SetRttEndTime				( ULONG_PTR lpNetworkKey );
			virtual void	GetRttTime					( ULONG_PTR lpNetworkKey, DWORD& out_dwRttTime );

			// 해당 키의 Servicer 인덱스를 가져온다.
			virtual INT			GetServicerIndex( ULONG_PTR lpNetworkKey )
			{
				CCommSocket* pCommSocket;
				pCommSocket = (CCommSocket*)lpNetworkKey;

				if( pCommSocket->m_pParentSocket )
				{
					return pCommSocket->m_pParentSocket->GetServiceIndex();
				}
				else
				{
					return pCommSocket->GetServiceIndex();
				}
			}
			virtual void		SetServicerIndex( ULONG_PTR lpNetworkKey, INT nServicerIndex )
			{
				CCommSocket* pCommSocket;
				pCommSocket = (CCommSocket*)lpNetworkKey;

				pCommSocket->SetServiceIndex( nServicerIndex );
			}

			virtual ULONG_PTR GetOwnerKey( ULONG_PTR lpNetworkKey )
			{
				CCommSocket* pSocket = (CCommSocket*)lpNetworkKey;
				return pSocket->GetOwnerKey();
			}

			virtual void SetOwnerKey( ULONG_PTR lpNetworkKey, ULONG_PTR lpOwnerKey )
			{
				CCommSocket* pSocket = (CCommSocket*)lpNetworkKey;
				pSocket->SetOwnerKey( lpOwnerKey );
			}

			virtual void GetAssemblerKey( ULONG_PTR lpNetworkKey, ULONG_PTR& lpAssemblerKey, DWORD& dwAssemblerHandle )
			{
				CCommSocket* pSocket = (CCommSocket*)lpNetworkKey;
				pSocket->GetAssemblerKey( lpAssemblerKey, dwAssemblerHandle );
			}

			virtual void SetAssemblerKey( ULONG_PTR lpNetworkKey, ULONG_PTR lpAssemblerKey, DWORD dwAssemblerHandle )
			{
				CCommSocket* pSocket = (CCommSocket*)lpNetworkKey;
				pSocket->SetAssemblerKey( lpAssemblerKey, dwAssemblerHandle );
			}

			virtual void FreeNetworkKey( ULONG_PTR lpNetworkKey )
			{
				CCommSocket*						pCommSocket;
				Common::DataStructure::CMemoryPoolSafty<CCommSocket>*	pCommSocketPool;

				pCommSocket		= (CCommSocket*)lpNetworkKey;
				pCommSocketPool	= pCommSocket->GetCommSocketPool();

				pCommSocketPool->Free( pCommSocket );
			}

			virtual LPCTSTR GetLocalAddr( ULONG_PTR lpNetworkKey )
			{
				CCommSocket* pSocket = (CCommSocket*)lpNetworkKey;
				return pSocket->GetLocalAddr();
			}

			virtual LPCTSTR GetPeerAddr( ULONG_PTR lpNetworkKey )
			{
				CCommSocket* pSocket = (CCommSocket*)lpNetworkKey;
				return pSocket->GetPeerAddr();
			}


		private:
			void	ProcessAccept	( SOCKET sSocket );
			void	ProcessConnect	( SOCKET sSocket );
			void	ProcessClose	( SOCKET sSocket );
			void	ProcessRecv		( SOCKET sSocket );
			void	ProcessSend		( SOCKET sSocket );
			/*
			// 등록된 IO핸들 처리중 Accept
			void	ProcessAcceptCompletion( DWORD dwNumberOfBytesTransferred, ULONG_PTR lpNetworkKey, LPOVERLAPPED lpOverlapped );
			// 등록된 IO핸들 처리중 Accept and Recv
			void	ProcessAcceptRecvCompletion( DWORD dwNumberOfBytesTransferred, ULONG_PTR lpNetworkKey, LPOVERLAPPED lpOverlapped );
			// 등록된 IO핸들 처리중 Recv
			void	ProcessRecvCompletion( DWORD dwNumberOfBytesTransferred, ULONG_PTR lpNetworkKey, LPOVERLAPPED lpOverlapped );
			// 등록된 IO핸들 처리중 Send
			void	ProcessSendCompletion( DWORD dwNumberOfBytesTransferred, ULONG_PTR lpNetworkKey, LPOVERLAPPED lpOverlapped );
			*/

			/*
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Implementation
		private:
			// IIocp Implementation : 

			// 등록된 IO 핸들의 완료 이벤트를 받기 위한 인터페이스
			virtual void OnGetQueuedCompletionStatus( BOOL bRet, DWORD dwNumberOfBytesTransferred, ULONG_PTR lpNetworkKey, LPOVERLAPPED lpOverlapped );
			*/
		};// class CTcpAsyncSelect
	}// namespace TcpAsyncSelect
}// namespace Network
