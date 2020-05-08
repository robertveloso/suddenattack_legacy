#pragma once

#include "./Type.h"

namespace LogicObject {		namespace Player {

struct Stock
{
	typedef					::LogicObject::Player::Type
							TYPE;

	void					SetNick( const char * i_strNick );
	const char *			GetNick() const;

	unsigned char			m_byGrade;

private:
	char					m_strNick[ TYPE::s_nUserNick ];
};

} /* Player */				} /* LogicObject */