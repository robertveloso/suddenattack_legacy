#include "StdAfx.h"
#include ".\MessageQueue.h"

#include <stdio.h>

using namespace GamehiSolution::Session::Message;

#include "..\Application\Packet.h"
using namespace Application::Packets;

CMessageQueue::CMessageQueue(void)
{
}

CMessageQueue::~CMessageQueue(void)
{
}

// 큐 버퍼를 생성한다.
BOOL CMessageQueue::Create( int nQueueSize, int nPacketSize, int nSignalSize )
{
	m_lstMessage.Create( nQueueSize );

	m_paMessage = new CMessage*[nQueueSize];
	if( m_paMessage == NULL )
		return FALSE;

	m_MessagePool.Create( nQueueSize );
	for( int i = 0; i < nQueueSize; i++ )
	{
		m_paMessage[i] = m_MessagePool.Alloc();
		m_paMessage[i]->Create( nPacketSize );
	}
	for( int j = 0; j < nQueueSize; j++ )
	{
		m_MessagePool.Free( m_paMessage[j] );
	}


	m_nQueueSize	= nQueueSize;
	m_nHead			= 0;
	m_nRear			= 0;
	m_nWaiter		= 0;


	m_SignalQueue.Create( nSignalSize );

	m_hSemaphore = ::CreateSemaphore(
						NULL,		// Sempahore Attributes
						0,			// initialCount
						65000,		// MaximumCount
						NULL		// object name
						);

	m_nPacketSize	= nPacketSize;


	::InitializeCriticalSection( &m_csWrite );
	::InitializeCriticalSection( &m_csRead );
	::InitializeCriticalSection( &m_csReAlloc );
	
	return TRUE;
}

// 큐 버퍼를 제거한다.
void CMessageQueue::Destroy()
{
	m_lstMessage.Destroy();

	if( m_paMessage )
	{
		delete[] m_paMessage;
		m_paMessage		= NULL;
		m_nQueueSize	= 0;
	}

	m_MessagePool.Destroy();


	m_SignalQueue.Destroy();

	// IOCP 큐를 제거한다.
	::CloseHandle( m_hSemaphore );

	::DeleteCriticalSection( &m_csWrite );
	::DeleteCriticalSection( &m_csRead );
	::DeleteCriticalSection( &m_csReAlloc );
}

// 큐에 패킷을 삽입한다.
void CMessageQueue::EnqueuePacket( const CMessage& packet )
{
//	DWORD dwFirst = GetTickCount();

	
	CMessage* pMessage;
	pMessage = m_MessagePool.Alloc();
	*pMessage = packet;

	m_lstMessage.Add( pMessage );
}

// 큐에서 패킷을 꺼낸다.
void CMessageQueue::DequeuePacket( CMessage& outPacket )
{
	CMessage* pMessage;
	pMessage = m_lstMessage.Dequeue();

	outPacket = *pMessage;
	m_MessagePool.Free( pMessage );
}

// 이벤트를 추가한다.
void CMessageQueue::AddSignalPacket( const CSignal& signal )
{
	::EnterCriticalSection( &m_csRead );

	m_SignalQueue.EnqueueSignal( signal );

	::LeaveCriticalSection( &m_csRead );
}