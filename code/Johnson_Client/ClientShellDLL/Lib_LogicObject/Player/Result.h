#pragma once

#include "./Type.h"

namespace LogicObject {		namespace Player {

struct Result
{
	__int64					m_llExp;
	__int64					m_llMoney;
	int						m_nKill;
	int						m_nDeath;
	int						m_nHeadKill;
	int						m_nHeadDead;
	int						m_nWin;
	int						m_nLose;
	int						m_nDraw;

	int						m_nMaxKill;
	int						m_nMaxHeadKill;
	int						m_nRank;
};

} /* Player */				} /* LogicObject */