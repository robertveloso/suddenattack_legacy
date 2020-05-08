/*
	InternetWrite및 Read에서 큰 용량 처리 부분이 빠져있다.
	현재는 작은 Command 단위의 송수신이라 문제가 없으나,
	대용량 처리를 위해서 구현을 해두어야 한다.
*/

#pragma once

#include "..\..\DataStructure\MemoryPoolSafty.h"
#include <wininet.h>


namespace Query
{
	interface IQueryHandler
	{
		virtual void OnRecvInet		( ULONG_PTR lpInetKey, ULONG_PTR dwContext )	= 0;
		virtual void OnErrorInet	( ULONG_PTR dwContext, DWORD nError )			= 0;
	};
}// namespace Query

