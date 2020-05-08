#include "StdAfx.h"

#include "./Provider.h"

namespace GameServer {		namespace Formula {

Provider::Provider( const Exp & i_stWinExp, const Exp & i_stLoseExp, const Money & i_stWinMoney, const Money & i_stLoseMoney )
{
	m_stWinExp				= i_stWinExp;
	m_stLoseExp				= i_stLoseExp;
	m_stWinMoney			= i_stWinMoney;
	m_stLoseMoney			= i_stLoseMoney;
}

Provider::~Provider()
{
}

const Provider::Exp *
Provider::GetWinExp()
{
	return & m_stWinExp;
}

const Provider::Exp *
Provider::GetLoseExp()
{
	return & m_stLoseExp;
}

const Provider::Money *
Provider::GetWinMoney()
{
	return & m_stWinMoney;
}

const Provider::Money *
Provider::GetLoseMoney()
{
	return & m_stLoseMoney;
}

} /* Formula */				} /* GameServer */