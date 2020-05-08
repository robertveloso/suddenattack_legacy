#pragma once

#include "../IFormula.h"

namespace GameServer {		namespace Formula {

class Provider
:	public					IFormula
{
public:
							Provider( const Exp & i_stWinExp, const Exp & i_stLoseExp, const Money & i_stWinMoney, const Money & i_stLoseMoney );
	virtual					~Provider();

	virtual const Exp *		GetWinExp();
	virtual const Exp *		GetLoseExp();

	virtual const Money *	GetWinMoney();
	virtual const Money *	GetLoseMoney();

private:
	Exp						m_stWinExp;
	Exp						m_stLoseExp;
	Money					m_stWinMoney;
	Money					m_stLoseMoney;

};

} /* Formula */				} /* GameServer */