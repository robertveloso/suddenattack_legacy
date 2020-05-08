#pragma once
#include "iocp.h"

/*****************************************************************************************************************
	CAsynCAsyncIocp

	작성자 : 강진협
	작성일 : 2004년 2월 20일

	설  명 :
		IOCP를 클래스로써 구현을 한다.
		CAsyncIocp 클래스를 상속받아 비동기 처리를 가능하게 이벤트를 제공한다.
	사  용 :
		1. Method Create를 사용하여 IOCP 핸들을 생성한다. 이 때 처리할 Thread의 개수를 지정할 수 있다.
		2. AssignIoCompletionPort를 사용하여 IO 핸들을 Key값을 가지고 IOCP 핸들에 등록시킨다.
		3. IO 핸들을 등록할 때의 Key값은 NULL이 되어서는 안된다.
		4. 등록된 IO 핸들의 완료 이벤트를 받기 위해서는 IIocp 인터페이스를 상속받아 OnGetQuededCompletionStatus을 구현한다.

	수  정 : 2004년 3월 8일
		CAsyncIocp Blocking 클래스를 구현함으로써 IOCP API를 사용하던 방식에서 CAsyncIocp를 사용하는 방식으로 전환
*****************************************************************************************************************/

#include <Windows.h>

namespace IO
{
	namespace IOCP
	{
		// 등록된 IO 핸들의 완료 이벤트를 받기 위한 인터페이스
		interface IIocpHandler
		{
			virtual void OnGetQueuedCompletionStatus( BOOL bRet, DWORD dwNumberOfBytesTransferred, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpOverlapped ) = 0;
		};

		class CAsyncIocp : public CIocp
		{
			// 생성 / 소멸자
		public:
			CAsyncIocp(void);
			~CAsyncIocp(void);

			// Attributes
		private:
			IIocpHandler*	m_pIocpHandler;			// IOCP(OnGetQueuedCompletionStatus)를 처리할 객체
			INT				m_nNumOfThreads;		// IOCP용 쓰레드의 개수, 디폴트로는 씨피유의 개수 * 2로 한다.

			const ULONG_PTR	c_lpThreadDie;			// 땍屢槨藁놔key

			// Operations
		public:
			// IOCP를 생성한다.
			//		nNumberOfThreads : 쓰레드의 개수 : 0이면 CPU개수 * 2로 설정된다.
			//		dwWaitMilliSeconds : wait 시간
			virtual BOOL	Create( IIocpHandler* pIocpHandler, int nNumberOfThreads = 0, DWORD dwWaitMilliSeconds = INFINITE );

			// IOCP를 제거한다.
			virtual void	Destroy();

		private:
			// ComletionKey값이 종료를 위한 값인지를 비교한다.
			BOOL	IsExitCompletionKey( ULONG_PTR CompletionKey )
			{
				return( CompletionKey == c_lpThreadDie );
			}

			// IOCP를 수행하는 쓰레드
			static	DWORD __stdcall IocpWorkerThreadStartingPoint(PVOID pvParam);
		};
	}// namespace IOCP

}// namespace IO
