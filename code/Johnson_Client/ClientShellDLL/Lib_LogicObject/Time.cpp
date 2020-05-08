#include "StdAfx.h"

#include "./Time.h"

namespace LogicObject {

Time::Time()
{
}

Time::~Time()
{
}

Time::CWORD
Time::GetYear()
{
	return 2000 + m_byYear;
}

Time::CBYTE
Time::GetMonth()
{
	return m_byMonth;
}

Time::CBYTE
Time::GetDay()
{
	return m_byDay;
}

Time::CBYTE
Time::GetHour()
{
	return m_byHour;
}

void
Time::SetTime( CWORD wYear, CBYTE byMonth, CBYTE byDay, CBYTE byHour )
{
	if( wYear < 2000 )
	{
		 __asm { int 3 };
	}

	m_byYear				= static_cast<CBYTE>( wYear - 2000 );
	m_byMonth				= byMonth;
	m_byDay					= byDay;
	m_byHour				= byHour;
}

} /* LogicObject */
