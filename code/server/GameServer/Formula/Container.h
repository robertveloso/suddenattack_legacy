#pragma once

#include "./Provider.h"

namespace GameServer {		namespace Formula {

class Container
:	public					IFormula::IContainer
{
public:
							Container();
	virtual					~Container();

	virtual void			Add( E_MODE::ENUM i_eMode, EXP & i_stWinExp, EXP & i_stLoseExp, MONEY & i_stWinMoney, MONEY & i_stLoseMoney );
	virtual IFormula *		Find( E_MODE::ENUM i_eMode );

	virtual void			SetPenalty( CINT i_nExp, CINT i_nMoney );
	virtual CINT			GetPenaltyExp();
	virtual CINT			GetPenaltyMoney();

private:
	typedef					Array< E_MODE::__MAX__, Provider * >
							ARRAY;
	
	struct					Penalty
	{
		INT					m_nExp;
		INT					m_nMoney;
	};

private:
	ARRAY					m_stArray;
	Penalty					m_stPenalty;

};

} /* Formula */				} /* GameServer */