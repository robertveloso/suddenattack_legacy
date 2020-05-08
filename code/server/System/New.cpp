#include "StdAfx.h"

#include "./Type.h"
#include "./New.h"
#include "../Macro.h"

using namespace System;

//class Agent
//{
//public:
//							Agent();
//	virtual					~Agent();
//
//	void					Enter();
//	void					Leave();
//
//	void *					Capture( CULONG ulLength );
//	void					Release( void * pPointer );
//
//private:
//#ifdef WIN32
//	typedef					CRITICAL_SECTION
//#endif
//							MUTEX;
//
//private:
//	ULONG					m_ulCount;
//	MUTEX					m_hMutex;
//
//};
//static Agent				clsAgent;
//
//Agent::Agent()
//{
//#ifdef WIN32
//	InitializeCriticalSectionAndSpinCount( & m_hMutex, 16 );
//#else
//	CINT nRetVal = ::pthread_mutex_init( & m_nMutex, NULL );
//	ASSERT( nRetVal == 0 );
//#endif
//
//	m_ulCount = 0;
//}
//
//Agent::~Agent()
//{
//	ASSERT( m_ulCount == 0 );
//
//#ifdef WIN32
//	::Kernel::DeleteCriticalSection( & m_hMutex );
//#else
//	CINT nRetVal = ::pthread_mutex_destroy( & m_nMutex );
//	ASSERT( nRetVal == 0 );
//#endif
//}
//
//void
//Agent::Enter()
//{
//#ifdef WIN32
//	::Kernel::EnterCriticalSection( & m_hMutex );
//#else
//	CINT nRetVal = ::pthread_mutex_lock( & m_nMutex );
//	ASSERT( nRetVal == 0 );
//#endif
//}
//
//void
//Agent::Leave()
//{
//#ifdef WIN32
//	::Kernel::LeaveCriticalSection( & m_hMutex );
//#else
//	CINT nRetVal = ::pthread_mutex_unlock( & m_nMutex );
//	ASSERT( nRetVal == 0 );
//#endif
//}
//
//void *
//Agent::Capture( CULONG ulLength )
//{
//	void * pPointer = ::Kernel::malloc( ulLength );
//	ASSERT( pPointer != NULL );
//
//	this->Enter();
//
//	m_ulCount++;
//
//	this->Leave();
//
//	return pPointer;
//}
//
//void
//Agent::Release( void * pPointer )
//{
//	assert( pPointer != NULL );
//
//	this->Enter();
//
//	m_ulCount--;
//
//	this->Leave();
//
//	::Kernel::free( pPointer );
//}
//
//void *
//operator new( UINT uiLength )
//{
//	return clsAgent.Capture( uiLength );
//}
//
//void
//operator delete( void * pPointer )
//{
//	if ( pPointer != NULL )
//	{
//		clsAgent.Release( pPointer );
//	}
//}
//
//void *
//operator new[]( UINT uiLength )
//{
//	return clsAgent.Capture( uiLength );
//}
//
//void
//operator delete[]( void * pPointer )
//{
//	if ( pPointer != NULL )
//	{
//		clsAgent.Release( pPointer );
//	}
//}