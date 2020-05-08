/*****************************************************************************************************************
	CListenSocket : CSocket

	작성자 : 강진협
	작성일 : 2004년 2월 25~27일

	설  명 :
		Iocp로부터 Accept 이벤트를 받기 위한 Listen Socket 클래스
		Accept는 효율성을 높이기 위해 Accept Pool로 구성된다.
	사  용 :
		1. Create를 통해서 Recv 버퍼와, Accept Pool Size를 결정한다.
		   Recv 버퍼가 양수인 경우에는 Accept & Recv 이벤트를 기다리며, 그렇지 않으면 Accept 이벤트를 기다린다.
		2. Bind, Listen을 하여 리슨 소켓에 속성을 부여한다.
		3. RunAcceptPool를 통해서 IOCP객체에 Accept Pool을 등록한다.
		4. AcceptEx를 사용하여 Accept 이벤트를 받을 때마다, 또다른 Accept Pool을 등록한다.
		5. Accept 이벤트를 받으면 GetSocketOfOverlappedObject를 통해서 클라이언트와 연결된 소켓 핸들을 얻는다.
		6. Destroy를 통해서 모든 리소스를 제거한다.
*****************************************************************************************************************/


#pragma once

#include <Winsock2.h>
#include <Windows.h>

#include "Socket.h"
#include "CommSocket.h"
#include "..\..\DataStructure\MemoryPoolSafty.h"

namespace Network
{
	namespace TcpAsyncSelect
	{
		class CListenSocket :
			public CSocket
		{
			// 생성/소멸자
		public:
			CListenSocket(void);
			~CListenSocket(void);

			// Attributes
		private:
			// accept pool로 만들어서 iocp에 등록할 overlapped object
			/*
			struct TagOverlappedSocket : public WSAOVERLAPPED
			{
				SOCKET		sAcceptSocket;		// Accept Socket : 열결된 뒤에 Comm Socket으로 Assign된다.
				char*		Buff;				// AceeptEx를 위한 Buffer의 크기
			};
			*/

			char*			m_Buff;				// AcceptEx에서 사용할 모든 overlapped object들의 버퍼로써 한번에 할당한 뒤에 포인터로써 나누어 각각 할당한다.
			UINT			m_nBuffSize;		// 전체 버퍼의 크기

			UINT			m_nAcceptPoolSize;	// accept pool의 크기

//				TagOverlappedSocket*
//					m_aOverlappedSocket;// overlapped socket의 동적배열

			Common::DataStructure::CMemoryPoolSafty<CCommSocket>*
				m_pCommSocketPool;

			// Operations
		public:
			// 리소스들을 생성한다.
			BOOL	Create( Common::DataStructure::CMemoryPoolSafty<CCommSocket>* pCommSocketPool, UINT nRecvBuffSize = 0, UINT nAcceptPoolSize = 100 );

			// 리소스들을 해제한다.
			void	Destroy();

			// 바인드를 한다.
			BOOL	Bind( const sockaddr* addr, int namelen );
			BOOL	Bind( int nPort, u_long hostAddr = INADDR_ANY, short family = AF_INET );

			// 리슨을 한다.
			BOOL	Listen( int nBacklog = 5 );

			// Accept Pool을 등록한다.
			BOOL	RunAcceptPool();

			// overlappedSocket 객체를 다시 AccextEx한다.
			BOOL	AcceptEx( LPOVERLAPPED ovlappedSocket );

			// Accept이벤트로 받은 lpOv에 해당하는 SOCKET을 반환한다.
			/*
			SOCKET	GetSocketOfOverlappedObject( LPOVERLAPPED lpOv )
			{
				TagOverlappedSocket* pOvSocket = (TagOverlappedSocket*)lpOv;
				return pOvSocket->sAcceptSocket;
			}
			*/

			Common::DataStructure::CMemoryPoolSafty<CCommSocket>*
					GetCommSocketPool()
			{
				return m_pCommSocketPool;
			}

		private:
			// iocp용 소켓을 생성한다.
			SOCKET	CreateSocket( DWORD& nOutErrorcode );

			// Implementations
		public:
			// IO Type을 반환한다.
//				virtual IOType GetIOType( LPOVERLAPPED lpOv );
		};// class COverlappedServerSocket

	}// namespace TcpIOCP
}// namespace Network
