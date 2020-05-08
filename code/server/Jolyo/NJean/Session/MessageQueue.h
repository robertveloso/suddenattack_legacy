// 2004년 3월 8일 ~ 9일

#pragma once

#include <windows.h>
#include "Message.h"
#include "..\..\DataStructure\MemoryPoolSafty.h"
#include "..\..\DataStructure\List.h"

// 큐는 거대한 바이트 어레이를 n개 이어놓은 형태이다.
// Enqueue를 통해서 패킷을 큐에 copy한 다음에 패킷의 정보를 패킷 정보 큐에 담는다.
// 패킷 정보 큐는 IOCP를 통해서 구현을 한다.
// Dequeue를 통해서 큐에서 패킷을 꺼낸다.

// Enqueue는 n개의 쓰레드가 접근 가능하지만
// Dequeue는 반드시 1개의 쓰레드가 꺼내야 한다. 그 이유는 n개의 쓰레드가 꺼내게 되면 TCP의 스트리밍에 위배되기 때문이다.


// create를 통해서 패킷 큐 배열을 생성하고, 모든 패킷을 생성한다.
// 

namespace Session
{
	namespace Message
	{
		class CMessageQueue
		{
		public:
			CMessageQueue(void);
			~CMessageQueue(void);

		private:
			Common::DataStructure::CMemoryPoolSafty<CMessage>
							m_MessagePool;			// 메시지 풀
			CMessage**		m_paMessage;			// 메시지 큐 배열


//				CMessage*		m_aMessage;				// 패킷 큐 배열

			INT				m_nPacketSize;

			UINT			m_nQueueSize;			// 큐에 담을 수 있는 패킷의 최대 개수
			UINT			m_nHead;				// Enqueue할 위치
			UINT			m_nRear;				// Dequeue할 위치

			UINT			m_nCount;				// 큐잉된 개수

			CRITICAL_SECTION
							m_csWrite;				// For Thread-Safe : N개의 Write간의 동기화

			CRITICAL_SECTION
							m_csRead;				// For Thread-Safe : M개의 Read간의 동기화

			CRITICAL_SECTION
							m_csReAlloc;

			INT				m_nWaiter;

			HANDLE			m_hSemaphore;			// queue의 동기화를 위한 세마포어 객체

			Common::DataStructure::CList<CMessage*>
							m_lstMessage;

			CSignalQueue	m_SignalQueue;

		public:
			// 큐 버퍼를 생성한다.
			BOOL		Create( int nQueueSize, int nPacketSize, int nEventSize = 0 );

			// 큐 버퍼를 제거한다.
			void		Destroy();

			// 큐에 패킷을 삽입한다.
			void		EnqueuePacket( const CMessage& packet );

			// 큐에서 패킷을 꺼낸다.
			void		DequeuePacket( CMessage& outPacket );

			// 이벤트를 추가한다.
			void		AddSignalPacket( const CSignal& signal );

		public:
			BOOL		IsEmpty()
			{
				return m_lstMessage.GetElementsCount() == 0;
//					return m_nHead == m_nRear;
			}
		};// class CMessageQueue

	}// namespace Message

}// namespace Session
