/*****************************************************************************************************************
	CIocp

	작성자 : 강진협
	작성일 : 2004년 3월 8일

	설  명 :
		IOCP를 클래스로써 구현을 한다.
	사  용 :
		1. Method Create를 사용하여 IOCP 핸들을 생성한다.
		2. AssignIoCompletionPort를 사용하여 IO 핸들을 Key값을 가지고 IOCP 핸들에 등록시킨다.
		3. IO 핸들을 등록할 때의 Key값은 NULL이 되어서는 안된다.
		4. PostQueuedCompletionStatus를 통해서 IOCP 큐에 Post 시킨다.
		5. GetQueuedCompletionStatus를 통해서 IOCP 큐의 내용을 Get 시킨다.
		test
*****************************************************************************************************************/

#pragma once

#include <Windows.h>

namespace IO
{
	namespace IOCP
	{
		class CIocp
		{
			// 생성 / 소멸자
		public:
			CIocp(void);
			~CIocp(void);

			// Attributes
		private:
			DWORD		m_dwWaitMilliSeconds;	// GetQueuedCompletionStatus에서의 대기 시간

		public:
			HANDLE		m_hIOCP;				// IOCP Handle

			// Operations
		public:
			// IOCP를 생성한다.
			//		dwWaitMilliSeconds : wait 시간
			virtual BOOL	Create( DWORD dwWaitMilliSeconds = INFINITE );

			// IOCP를 제거한다.
			virtual void	Destroy();

			// IOCP에 핸들을 등록한다.
			BOOL	AssignIoCompletionPort( HANDLE FileHandle, ULONG_PTR CompletionKey );

			// IOCP 큐에 Post한다.
			BOOL	PostQueuedCompletionStatus( ULONG_PTR lpCompletionKey, LPOVERLAPPED lpOverlapped );

			// IOCP 큐에서 Get한다.
			BOOL	GetQueuedCompletionStatus( LPDWORD lpNumberOfBytesTransferred, PULONG_PTR lpCompletionKey, LPOVERLAPPED* lpOverlapped );
		};
	}// namespace IOCP

}// namespace IO
