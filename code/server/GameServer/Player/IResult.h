#pragma once

#include "../../System.h"
#include "../../Share/LogicObject/Player/Type.h"
#include "../../Share/LogicObject/Player/Result.h"

namespace GameServer {		namespace Player {

class IResult	//角色操作结果的处理
{
public:
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;
	typedef					::LogicObject::Player::Result
							RESULT;
	typedef					const RESULT
							CRESULT;

public:
	class					IFactory			//没有使用
	:	public				Singleton<IFactory>
	{
	public:
		virtual				~IFactory() {}

		virtual IResult *	Create() = 0;
		virtual void		Destroy( IResult * i_pResult ) = 0;
	};
	static inline			IFactory *
	IFACTORY()				{ return IFactory::GetInstance(); }

	class					IGrade
	:	public				Singleton<IGrade>
	{
	public:
		virtual				~IGrade() {}

		virtual void		Add( CINT64 i_nMinExp, CINT64 i_nMaxExp, CBOOL i_bAbs ) = 0;
		virtual CBYTE		Get( CINT64 i_nExp, CBYTE i_byCurrentGrade ) = 0;
	};
	static inline			IGrade *
	IGRADE()				{ return IGrade::GetInstance(); }
public:
	virtual					~IResult() {}

	virtual void			Reset() = 0;
	
	virtual void			Kill( CBOOL i_bHeadShot ) = 0;
	virtual void			Dead( CBOOL i_bHeadShot ) = 0;
	virtual void			Suicide() = 0;
	virtual void			Mission() = 0;

	virtual CINT			GetKillCount() = 0;
	virtual CINT			GetDeadCount() = 0;
	
	virtual CINT			GetHeadKillCount() = 0;
	virtual CINT			GetHeadDeadCount() = 0;

	virtual CINT			GetSuicideCount() = 0;
	virtual CINT			GetMissionCount() = 0;

	virtual CINT			GetMoneyChange() = 0;
	virtual void			SetMoneyChange( CINT i_nMoney ) = 0;

	virtual CINT			GetExpChange() = 0;
	virtual void			SetExpChange( CINT i_nExp ) = 0;

	virtual RESULT *		GetTotalResult() = 0;
//	virtual void			Update( E_TEAM::ENUM i_eWinner, E_TEAM::ENUM i_ePlayer  ) = 0;
	
};

} /* Player */				} /* GameServer */