#include "StdAfx.h"

#include "./Provider.h"

namespace GameServer {		namespace Player {			namespace Result {

Provider::Provider()
{
	m_pResult				= new RESULT;

	m_nKill					= 0;
	m_nDead					= 0;
	m_nHeadShotKill			= 0;
	m_nHeadShotDead			= 0;
	m_nSuicide				= 0;
	m_nMoney				= 0;
	m_nExp					= 0;
	m_nMission				= 0;
}

Provider::~Provider()
{
}

void
Provider::Capture()
{
	m_nKill					= 0;
	m_nDead					= 0;
	m_nHeadShotKill			= 0;
	m_nHeadShotDead			= 0;
	m_nSuicide				= 0;
	m_nMoney				= 0;
	m_nExp					= 0;
	m_nMission				= 0;
}

void
Provider::Release()
{
}

void
Provider::Reset()
{
	m_nKill					= 0;
	m_nDead					= 0;
	m_nHeadShotKill			= 0;
	m_nHeadShotDead			= 0;
	m_nSuicide				= 0;
	m_nMoney				= 0;
	m_nExp					= 0;
	m_nMission				= 0;
}

void
Provider::Kill( CBOOL i_bHeadShot )
{
	m_nKill ++;
	if( i_bHeadShot == TRUE )
	{
		m_nHeadShotKill ++;
	}
}

void
Provider::Dead( CBOOL i_bHeadShot )
{
	m_nDead ++;
	if( i_bHeadShot == TRUE )
	{
		m_nHeadShotDead ++;
	}
}

void
Provider::Suicide()
{
	m_nSuicide ++;
}

void
Provider::Mission()
{
	m_nMission ++;
}

CINT
Provider::GetKillCount()
{
	return m_nKill;
}

CINT
Provider::GetDeadCount()
{
	return m_nDead;
}

CINT
Provider::GetHeadKillCount()
{
	return m_nHeadShotKill;
}

CINT
Provider::GetHeadDeadCount()
{
	return m_nHeadShotDead;
}

CINT
Provider::GetSuicideCount()
{
	return m_nSuicide;
}

CINT
Provider::GetMissionCount()
{
	return m_nMission;
}

CINT
Provider::GetMoneyChange()
{
	return m_nMoney;
}

void
Provider::SetMoneyChange( CINT i_nMoney )
{
	m_nMoney				= i_nMoney;
}

CINT
Provider::GetExpChange()
{
	return m_nExp;
}

void
Provider::SetExpChange( CINT i_nExp )
{
	m_nExp					= i_nExp;
}

Provider::RESULT *
Provider::GetTotalResult()
{
	return m_pResult;
}

//void
//Provider::Update( E_TEAM::ENUM i_eWinner, E_TEAM::ENUM i_ePlayer )
//{
//	m_pResult->m_llExp		+= m_nExp;
//	m_pResult->m_llMoney	+= m_nMoney;
//	m_pResult->m_nKill		+= m_nKill;
//	m_pResult->m_nDeath		+= m_nDead;
//	m_pResult->m_nHeadKill	+= m_nHeadShotKill;
//	m_pResult->m_nHeadDead	+= m_nHeadShotDead;
//	if( i_eWinner == E_TEAM::DRAW )
//	{
//		m_pResult->m_nDraw ++;
//		return;
//	}
//	if( i_eWinner == i_ePlayer )
//	{
//		m_pResult->m_nWin ++;
//	}
//	else
//	{
//		m_pResult->m_nLose ++;
//	}
//}

} /* Result */				} /* Player */				} /* GameServer */