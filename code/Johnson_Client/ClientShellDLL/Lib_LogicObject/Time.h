#pragma once

namespace LogicObject {

struct Time
{
public:
	typedef					const unsigned short
							CWORD;
	typedef					const unsigned char
							CBYTE;

public:
	Time();
	~Time();

    CWORD					GetYear();
	CBYTE					GetMonth();
	CBYTE					GetDay();
	CBYTE					GetHour();
	void					SetTime( CWORD wYear, CBYTE byMonth, CBYTE byDay, CBYTE byHour );

private:
	unsigned char			m_byYear;
	unsigned char			m_byMonth;
	unsigned char			m_byDay;
	unsigned char			m_byHour;

};

} /* LogicObject */
