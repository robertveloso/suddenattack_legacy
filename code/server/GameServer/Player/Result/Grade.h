#pragma once

#include "../IResult.h"

namespace GameServer {		namespace Player {			namespace Result {

class Grade
:	public					IResult::IGrade
{
public:
							Grade();
	virtual					~Grade();

	virtual void			Add( CINT64 i_nMinExp, CINT64 i_nMaxExp, CBOOL i_bAbs );
	virtual CBYTE			Get( CINT64 i_nExp, CBYTE i_byCurrentGrade );

private:
	struct					Stock
	{
		INT64				nMinExp;
		INT64				nMaxExp;
		BOOL				bAbs;
		BOOL				bUse;
	};
	
private:
	static CINT				s_nMax = 100;
	Array<s_nMax, Stock>	m_stStock;
	INT						m_nCount;

};

} /* Result */				} /* Player */				} /* GameServer */