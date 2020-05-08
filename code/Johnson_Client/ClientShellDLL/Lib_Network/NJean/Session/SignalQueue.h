#pragma once
#include <windows.h>

namespace GamehiSolution
{
	namespace Session
	{
		namespace Message
		{
			class CSignal
			{
			private:
				UINT		m_uiMessage;
				unsigned long	m_wParam;
				unsigned long	m_lParam;

			public:
				CSignal			( UINT uiMessage = 0, unsigned long wParam = 0, unsigned long lParam = 0 )
					:	m_uiMessage	( uiMessage		)
					,	m_wParam	( wParam		)
					,	m_lParam	( lParam		)
				{}

				UINT		GetMessage()
				{
					return m_uiMessage;
				}
				unsigned long	GetWParam()
				{
					return m_wParam;
				}
				unsigned long	GetLParam()
				{
					return m_lParam;
				}
			};// class CSignal

			// n개의 Enqueu와 1개의 Dequeue Thread
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

}// namespace GamehiSolution
