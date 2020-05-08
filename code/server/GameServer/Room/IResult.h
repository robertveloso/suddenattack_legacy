#pragma once

#include "../../System.h"

#include "../../Share/LogicObject/Player/Type.h"

namespace GameServer {		namespace Room {

class IResult		//room Õ½¶·×´Ì¬
{
public:
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

public:
	virtual					~IResult() {}

	virtual void			Reset() = 0;

	virtual void			Kill( E_TEAM::ENUM eKiller ) = 0;
	virtual void			Dead( E_TEAM::ENUM eKiller ) = 0;
	
	virtual CINT			GetKillCount( E_TEAM::ENUM eTeam ) = 0;
	virtual CINT			GetDeadCount( E_TEAM::ENUM eTeam ) = 0;

	virtual E_TEAM::ENUM	GetWinnerTeam() = 0;
	virtual void			SetWinnerTeam( E_TEAM::ENUM i_eTeam ) = 0;

	virtual CBOOL			IsPlantedBomb() = 0;
	virtual CBOOL			IsDefusedBomb() = 0;
	virtual CBOOL			IsDetonatedBomb() = 0;
	virtual void			PlantBomb( CINT i_nPlantPlayer ) = 0;
	virtual void			DefusedBomb( CINT i_nPlantPlayer ) = 0;
	virtual void			DetonatedBomb() = 0;
	virtual CINT			GetPlantPlayer( CINT i_nPlayer ) = 0;
	virtual CINT			GetDefusePlayer( CINT i_nPlayer ) = 0;

	virtual void			SetAllKilled() = 0;
	virtual CBOOL			IsAllKilled() = 0;

	virtual void			ResetRound() = 0;
	virtual void			NextRound() = 0;
	virtual CBYTE			GetRound() = 0;

	virtual E_TEAM::ENUM	GetWinRound() = 0;
	virtual void			SetWinRound( E_TEAM::ENUM i_eTeam ) = 0;
	virtual CINT			GetWinRoundCount( E_TEAM::ENUM i_eTeam ) = 0;
};

} /* Room */				} /* GameServer */
