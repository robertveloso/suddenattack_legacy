#pragma once
#include <windows.h>

namespace Session
{
	namespace Message
	{
		class CSignal
		{
		private:
			UINT		m_uiMessage;	//0：退出
			ULONG_PTR	m_lpTimerKey;	//SignalType时用到
			ULONG_PTR	m_wParam;
			ULONG_PTR	m_lParam;

		public:
			CSignal			( UINT uiMessage = 0, ULONG_PTR wParam = 0, ULONG_PTR lParam = 0 )
				:	m_uiMessage		( uiMessage		)
				,	m_wParam		( wParam		)
				,	m_lParam		( lParam		)
				,	m_lpTimerKey	( NULL			)
			{}

			UINT		GetMessage()
			{
				return m_uiMessage;
			}
			ULONG_PTR	GetWParam()
			{
				return m_wParam;
			}
			ULONG_PTR	GetLParam()
			{
				return m_lParam;
			}
			VOID		SetTimerKey( ULONG_PTR lpTimerKey )
			{
				m_lpTimerKey = lpTimerKey;
			}
			ULONG_PTR	GetTimerKey()
			{
				return m_lpTimerKey;
			}
		};// class CSignal

		// n俺狼 Enqueu客 1俺狼 Dequeue Thread
		class CSignalQueue
		{
		public:
			CSignalQueue	(void);
			~CSignalQueue(void);

		private:
			CSignal*	m_aSignal;
			int			m_nSize;

			int			m_nHead;
			int			m_nRear;

			CRITICAL_SECTION
				m_cs;

		public:
			BOOL	Create	( int nSize );
			void	Destroy	( void );

			BOOL	EnqueueSignal	( const CSignal& signal );
			BOOL	DequeueSignal	( CSignal& signal );
		};// class CSignalQueue

	}// namespace Message

}// namespace Session
