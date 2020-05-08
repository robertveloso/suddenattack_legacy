#pragma once

namespace LogicObject {

struct Item
{
	char					m_cClass1;
    char					m_stClass2[ 2 ];
	unsigned char			m_byCode;

	bool					IsEqual( const Item & stItem ) const;
	bool					IsBigger( const Item & stItem ) const;
};

} /* LogicObject */
