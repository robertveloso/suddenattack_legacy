#pragma once
#include <windows.h>

#include "SignalQueue.h"
#include "../../DataStructure/MemoryPoolSafty.h"

namespace Session
{
	namespace Manage
	{
		interface ITimerHandler
		{
			virtual void OnTimer( const Message::CSignal& signal ) = 0; //ThreadStartForTime÷–µ˜”√
		};

		class CTimer
		{
		public:
			CTimer();
			~CTimer();

		private:
			struct	TagSignal
			{
				Message::CSignal	signal;
				int					nTickCount;
				TagSignal*			pNext;
			};

			Common::DataStructure::CMemoryPoolSafty<TagSignal>
								m_SignalPool;

			int					m_nTickTime;
			int					m_nMaxTickCount;
			int					m_nSize;
			int					m_nPos;

			ITimerHandler*		m_pTimerHandler;

			TagSignal**			m_paSignalSet;

			TagSignal*			m_pSignalList;

			CRITICAL_SECTION	m_cs;

			BOOL				m_bStop;
			HANDLE				m_hInitEvent;

		public:
			BOOL		Create	( int nTickTime, int nMaxTickCount, int nSize, ITimerHandler* pTimerHandler );
			void		Destroy	();

			ULONG_PTR	SetTimer( int nTickCount, const Message::CSignal& signal );

		private:
			// ≈∏¿Ã∏” æ≤∑πµÂ
			static	DWORD __stdcall ThreadStartForTime(PVOID pvParam);
		};

	}// namespace Manage

}// namespace Session
