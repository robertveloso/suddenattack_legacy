#include "StdAfx.h"

#include "./Item.h"

namespace LogicObject {

bool
Item::IsEqual( const Item & stItem ) const
{
	if( m_cClass1 != stItem.m_cClass1 )
	{
		return false;
	}

	if( m_stClass2[ 0 ] != stItem.m_stClass2[ 0 ] )
	{
		return false;
	}

	if( m_stClass2[ 1 ] != stItem.m_stClass2[ 1 ] )
	{
		return false;
	}

	if( m_byCode != stItem.m_byCode )
	{
		return false;
	}

	return true;
}

bool
Item::IsBigger( const Item & stItem ) const
{
	if( this->IsEqual( stItem ) == true )
	{
		return false;
	}

	if( m_cClass1 > stItem.m_cClass1 )
	{
		return true;
	}

	if( m_stClass2[ 0 ] > stItem.m_stClass2[ 0 ] )
	{
		return true;
	}

	if( m_stClass2[ 1 ] > stItem.m_stClass2[ 1 ] )
	{
		return true;
	}

	if( m_byCode > stItem.m_byCode )
	{
		return true;
	}

	return false;
}

} /* LogicObject */
