#pragma once

#include <map>

namespace Session
{
	namespace Gate
	{
		class CKillTimerMap
		{
		public:
			CKillTimerMap(){}
			~CKillTimerMap(){}

		private:
			std::map <ULONG_PTR, ULONG_PTR>				m_mapKillTimer;
			std::map <ULONG_PTR, ULONG_PTR>::iterator	m_IterKillTimer;

		public:
			void	Insert		( ULONG_PTR lpTimerKey );
			BOOL	Erase		( ULONG_PTR lpTimerKey );

		};// class CKillTimerMap

	}// namespace Gate

}// namespace Session