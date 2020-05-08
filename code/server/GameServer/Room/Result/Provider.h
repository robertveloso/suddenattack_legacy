#pragma once

#include "../IResult.h"

namespace GameServer {		namespace Room {			namespace Result {

class Provider
:	public					IResult
{
public:
							Provider();
	virtual					~Provider();

	void					Capture();
	void					Release();

	virtual void			Reset();

	virtual void			Kill( E_TEAM::ENUM eKiller );
	virtual void			Dead( E_TEAM::ENUM eKiller );

	virtual CINT			GetKillCount( E_TEAM::ENUM eTeam );
	virtual CINT			GetDeadCount( E_TEAM::ENUM eTeam );
	
	virtual E_TEAM::ENUM	GetWinnerTeam();
	virtual void			SetWinnerTeam( E_TEAM::ENUM i_eTeam );

	virtual CBOOL			IsPlantedBomb();
	virtual CBOOL			IsDefusedBomb();
	virtual CBOOL			IsDetonatedBomb();
	virtual void			PlantBomb( CINT i_nPlantPlayer );
	virtual void			DefusedBomb( CINT i_nDefusePlayer );
	virtual void			DetonatedBomb();
	virtual CINT			GetPlantPlayer( CINT i_nPlayer );
	virtual CINT			GetDefusePlayer( CINT i_nPlayer );

	virtual void			SetAllKilled();
	virtual CBOOL			IsAllKilled();

	virtual void			ResetRound();
	virtual void			NextRound();
	virtual CBYTE			GetRound();

	virtual E_TEAM::ENUM	GetWinRound();
	virtual void			SetWinRound( E_TEAM::ENUM i_eTeam );
	virtual CINT			GetWinRoundCount( E_TEAM::ENUM i_eTeam );

private:
	struct					Result
	{
		INT					m_nKill;
		INT					m_nDead;
	};
	typedef					Array< E_TEAM::__MAX__, Result >
							RESULT;

	struct					BombStock
	{
		BOOL				bPlanted;	//ÉèÖÃÕ¨µ¯
		BOOL				bDefused;	//½â³ý
		BOOL				bDetonated;	//Òý±¬
		INT					nPlantPlayer;
		INT					nDefusePlayer;
		BYTE				byRound;
		BOOL				bAllKilled;
		INT					nRedWin;
		INT					nBlueWin;
		E_TEAM::ENUM		eWinTeam;
	};

private:
	RESULT					m_stResult;
	E_TEAM::ENUM			m_eWinner;

	BombStock				m_stBombStock;
};

} /* Result */				} /* Room */				} /* GameServer */