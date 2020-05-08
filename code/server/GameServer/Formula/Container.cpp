#include "StdAfx.h"

#include "./Container.h"

namespace GameServer {		namespace Formula {

Container::Container()
{
	for( INT i = 0; i < E_MODE::__MAX__; i++ )
	{
		m_stArray[ i ]		= NULL;
	}
	this->install();
}

Container::~Container()
{
	this->uninstall();

	for( INT i = 0; i < E_MODE::__MAX__; i++ )
	{
		if( m_stArray[ i ] != NULL )
		{
			delete m_stArray[ i ];
		}
	}
}

void
Container::Add( E_MODE::ENUM i_eMode, EXP & i_stWinExp, EXP & i_stLoseExp, MONEY & i_stWinMoney, MONEY & i_stLoseMoney )
{
	ASSERT( m_stArray[ i_eMode ] == NULL );

	m_stArray[ i_eMode ]	= new Provider( i_stWinExp, i_stLoseExp, i_stWinMoney, i_stLoseMoney );
}

IFormula *
Container::Find( E_MODE::ENUM i_eMode )
{
	return m_stArray[ i_eMode ];
}

void
Container::SetPenalty( CINT i_nExp, CINT i_nMoney )
{
	m_stPenalty.m_nExp		= i_nExp;
	m_stPenalty.m_nMoney	= i_nMoney;
}

CINT
Container::GetPenaltyExp()
{
	return m_stPenalty.m_nExp;
}

CINT
Container::GetPenaltyMoney()
{
	return m_stPenalty.m_nMoney;
}

} /* Formula */				} /* GameServer */