#pragma once

#include "../IResult.h"

namespace GameServer {		namespace Player {			namespace Result {

class Provider
:	public					IResult
{
public:
							Provider();
	virtual					~Provider();

	void					Capture();
	void					Release();

	virtual void			Reset();

	virtual void			Kill( CBOOL i_bHeadShot );
	virtual void			Dead( CBOOL i_bHeadShot );
	virtual void			Suicide();
	virtual void			Mission();

	virtual CINT			GetKillCount();
	virtual CINT			GetDeadCount();

	virtual CINT			GetHeadKillCount();
	virtual CINT			GetHeadDeadCount();

	virtual CINT			GetSuicideCount();
	virtual CINT			GetMissionCount();

	virtual CINT			GetMoneyChange();
	virtual void			SetMoneyChange( CINT i_nMoney );

	virtual CINT			GetExpChange();
	virtual void			SetExpChange( CINT i_nExp );

	virtual RESULT *		GetTotalResult();
//	virtual void			Update( E_TEAM::ENUM i_eWinner, E_TEAM::ENUM i_ePlayer  );

private:
	INT						m_nKill;
	INT						m_nDead;
	INT						m_nHeadShotKill;
	INT						m_nHeadShotDead;
	INT						m_nSuicide;

	INT						m_nMoney;
	INT						m_nExp;

	INT						m_nMission;

	RESULT *				m_pResult;

};

} /* Result */				} /* Player */				} /* GameServer */