#pragma once

#include "./Type.h"

namespace LogicObject {		namespace Player {

struct Stock //角色的称呼和级别
{
	typedef					::LogicObject::Player::Type
							TYPE;

							Stock();

	void					SetNick( const char * i_strNick );
	const char *			GetNick() const;

	unsigned char			m_byGrade;

private:
	char					m_strNick[ TYPE::s_nUserNick ];
};

} /* Player */				} /* LogicObject */