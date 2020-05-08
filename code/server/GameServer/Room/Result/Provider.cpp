#include "StdAfx.h"

#include "./Provider.h"

namespace GameServer {		namespace Room {			namespace Result {

Provider::Provider()
{
}

Provider::~Provider()
{
}

void
Provider::Capture()
{
	m_stResult[ E_TEAM::RED ].m_nKill	= 0;
	m_stResult[ E_TEAM::RED ].m_nDead	= 0;
	m_stResult[ E_TEAM::BLUE ].m_nKill	= 0;
	m_stResult[ E_TEAM::BLUE ].m_nDead	= 0;
}

void
Provider::Release()
{
}

void
Provider::Reset()
{
	m_stResult[ E_TEAM::RED ].m_nKill	= 0;
	m_stResult[ E_TEAM::RED ].m_nDead	= 0;
	m_stResult[ E_TEAM::BLUE ].m_nKill	= 0;
	m_stResult[ E_TEAM::BLUE ].m_nDead	= 0;

	m_stBombStock.byRound			= 0;
	m_stBombStock.nRedWin			= 0;
	m_stBombStock.nBlueWin			= 0;
}

void
Provider::Kill( E_TEAM::ENUM eKiller )
{
	m_stResult[ eKiller ].m_nKill ++;
}

void
Provider::Dead( E_TEAM::ENUM eKiller )
{
	m_stResult[ eKiller ].m_nDead ++;
}

CINT
Provider::GetKillCount( E_TEAM::ENUM eTeam )
{
	return m_stResult[ eTeam ].m_nKill;
}

CINT
Provider::GetDeadCount( E_TEAM::ENUM eTeam )
{
	return m_stResult[ eTeam ].m_nDead;
}

Provider::E_TEAM::ENUM
Provider::GetWinnerTeam()
{
	return m_eWinner;
}

void
Provider::SetWinnerTeam( E_TEAM::ENUM i_eTeam )
{
	m_eWinner				= i_eTeam;
}

CBOOL
Provider::IsPlantedBomb()
{
	return m_stBombStock.bPlanted;
}

CBOOL
Provider::IsDefusedBomb()
{
	return m_stBombStock.bDefused;
}

CBOOL
Provider::IsDetonatedBomb()
{
	return m_stBombStock.bDetonated;
}

void
Provider::PlantBomb( CINT i_nPlantPlayer )
{
	m_stBombStock.bPlanted		= TRUE;
	m_stBombStock.nPlantPlayer	= i_nPlantPlayer;
}

void
Provider::DefusedBomb( CINT i_nDefusePlayer )
{
	m_stBombStock.bDefused		= TRUE;
	m_stBombStock.nDefusePlayer	= i_nDefusePlayer;
}

void
Provider::DetonatedBomb()
{
	m_stBombStock.bDetonated	= TRUE;
}

CINT
Provider::GetPlantPlayer( CINT i_nPlayer )
{
	return m_stBombStock.nPlantPlayer;
}

CINT
Provider::GetDefusePlayer( CINT i_nPlayer )
{
	return m_stBombStock.nDefusePlayer;
}

void
Provider::SetAllKilled()
{
	m_stBombStock.bAllKilled = TRUE;
}

CBOOL
Provider::IsAllKilled()
{
	return m_stBombStock.bAllKilled;
}

void
Provider::ResetRound()
{
	m_stBombStock.bPlanted			= FALSE;
	m_stBombStock.bDefused			= FALSE;
	m_stBombStock.bDetonated		= FALSE;
	m_stBombStock.nPlantPlayer		= 0;
	m_stBombStock.nDefusePlayer		= 0;
	m_stBombStock.bAllKilled		= FALSE;

	m_stResult[ E_TEAM::RED ].m_nKill	= 0;
	m_stResult[ E_TEAM::RED ].m_nDead	= 0;
	m_stResult[ E_TEAM::BLUE ].m_nKill	= 0;
	m_stResult[ E_TEAM::BLUE ].m_nDead	= 0;
}

void
Provider::NextRound()
{
	m_stBombStock.byRound ++;
}

CBYTE
Provider::GetRound()
{
	return m_stBombStock.byRound;
}

Provider::E_TEAM::ENUM
Provider::GetWinRound()
{
	return m_stBombStock.eWinTeam;
}

void
Provider::SetWinRound( E_TEAM::ENUM i_eTeam )
{
	m_stBombStock.eWinTeam	= i_eTeam;
	if( i_eTeam == E_TEAM::RED )
	{
		m_stBombStock.nRedWin ++;
	}
	else
	{
		m_stBombStock.nBlueWin ++;
	}
}

CINT
Provider::GetWinRoundCount( E_TEAM::ENUM i_eTeam )
{
	if( i_eTeam == E_TEAM::RED )
	{
		return m_stBombStock.nRedWin;
	}
	else
	{
		return m_stBombStock.nBlueWin;
	}
}

} /* Result */				} /* Room */				} /* GameServer */