/*****************************************************************************************************************
	CSocket

	작성자 : 강진협
	작성일 : 2004년 2월 25~27일

	설  명 :
		IOCP에 등록되는 Overlapped Socket Class
		Accept, Recv, Send 등의 이벤트를 받기 위해 사용되는 Class의 base Class
	사  용 :
		1. IOCP로부터 완료 이벤트를 받으면, CSocket으로 Ref를 받는다.
		2. CSocket의 vitual function인 GetIOType를 통해서 실제 객체의 타입을 받는다.
		   따라서 이 클래스를 상속받은 클래스는 반드시 GetIOType을 구현해야 한다.
*****************************************************************************************************************/


#pragma once

#include <Winsock2.h>
#include <Windows.h>

namespace Network
{
	namespace TcpAsyncSelect
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// OverlappedSocket의 BaseClass
		class CSocket
		{
		public:
			CSocket(void){}
			~CSocket(void){}

		public:
			enum ErrorCode
			{
				BufferCreateFail		= 1,	// 버퍼 생성 실패
				SocketCreateFail		= 2,	// 소켓 생성 실패
				SetSockOptFail			= 3,	// 소켓 옵션 변경 실패
				AcceptExFail			= 4,	// AcceptEx 등록 실패
				AcceptPoolCreateFail	= 5		// Accept Pool 생성 실패
			};

			/*
			enum IOType
			{
				NoneType		= 0,
				AcceptType		= 1,
				AcceptRecvType	= 2,
				RecvType		= 3,
				SendType		= 4
			};
			*/

			// Attributtes
		public:
			SOCKET			m_sSocket;
			CSocket*		m_pParentSocket;

		protected:
			DWORD			m_dwErrCode;			// 마지막 에러 코드

			ULONG_PTR		m_lpdwOwnerKey;			// 뚤tcp句口，槨CPeer*
			ULONG_PTR		m_lpAssemblerKey;		// 이 소켓으로부터 Data Assemble을 하는 객체의 키

			int				m_nServiceIndex;		// 소켓의 서비스 인덱스

			DWORD			m_dwAssemblerHandle;


			// Operations
		public:
			/*
			// IOCP에 등록할 IO 핸들을 반환한다.
			HANDLE	GetIOHandle()
			{
				return (HANDLE) m_sSocket;
			}

			// 마지막 에러를 반환한다.
			DWORD	GetLastError(void)
			{
				return m_dwErrCode;
			}
			*/

			void	SetOwnerKey( ULONG_PTR dwOwnerKey )
			{
				m_lpdwOwnerKey = dwOwnerKey;
			}
			ULONG_PTR	GetOwnerKey()
			{
				return m_lpdwOwnerKey;
			}

			void	SetAssemblerKey( ULONG_PTR lpAssemblerKey, DWORD dwAssemblerHandle )
			{
				m_lpAssemblerKey	= lpAssemblerKey;
				m_dwAssemblerHandle	= dwAssemblerHandle;
			}
			void	GetAssemblerKey( ULONG_PTR& lpAssemblerKey, DWORD& dwAssemblerHandle )
			{
				lpAssemblerKey		= m_lpAssemblerKey;
				dwAssemblerHandle	= m_dwAssemblerHandle;
			}

			int		GetServiceIndex()
			{
				return m_nServiceIndex;
			}
			void	SetServiceIndex( int nServiceIndex )
			{
				m_nServiceIndex = nServiceIndex;
			}

			// Overroadable
		public:
			/*
			// IOType을 반환한다.
			virtual IOType GetIOType( LPOVERLAPPED lpOv )
			{
				return IOType::NoneType;
			}

			virtual void	Init()
			{
				m_pParentOvSocket = NULL;
			}
			*/
		};// class CSocket

	}// namespace TcpIOCP
}// namespace Network
