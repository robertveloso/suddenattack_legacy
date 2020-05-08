#pragma once

#include "../System.h"

#include "../Share/LogicObject/Room/Option.h"

namespace GameServer {

class IFormula
{
public:
	virtual					~IFormula() {}

	struct					Exp
	{
		INT					nKillMul;
		INT					nHeadKillMul;
		INT					nEnemyMul;
		INT					nDeadMul;
		INT					nSuicideMul;
		INT					nMissionMul;
		INT					nAdd;
	};

	struct					Money
	{
		INT					nKillMul;
		INT					nEnemyMul;
		INT					nDeadMul;
		INT					nSuicideMul;
		INT					nMissionMul;
		INT					nAdd;
	};

	class					IContainer
	:	public				Singleton<IContainer>
	{
	public:
		typedef				::LogicObject::Room::Option::E_MODE
							E_MODE;
		typedef				const IFormula::Exp
							EXP;
		typedef				const IFormula::Money
							MONEY;

	public:
		virtual				~IContainer() {}
		
		virtual void		Add( E_MODE::ENUM i_eMode, EXP & i_stWinExp, EXP & i_stLoseExp, MONEY & i_stWinMoney, MONEY & i_stLoseMoney ) = 0;
		virtual IFormula *	Find( E_MODE::ENUM i_eMode ) = 0;

		virtual void		SetPenalty( CINT i_nExp, CINT i_nMoney ) = 0;
		virtual CINT		GetPenaltyExp() = 0;
		virtual CINT		GetPenaltyMoney() = 0;
	};
	static inline			IContainer *
	ICONTAINER()			{ return IContainer::GetInstance(); }

	virtual const Exp *		GetWinExp() = 0;
	virtual const Exp *		GetLoseExp() = 0;

	virtual const Money *	GetWinMoney() = 0;
	virtual const Money *	GetLoseMoney() = 0;
};

} /* GameServer */