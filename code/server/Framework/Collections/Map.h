#pragma once

#include "../Template/Singleton.h"
#include <Map>

namespace Framework {		namespace Collections {

template<class COMP, class TYPE>
class Map
{
public:
							Map();
	virtual					~Map();

	CBOOL					Insert( const COMP & i_stComp, TYPE * i_pItem );
	TYPE *					Find( const COMP & i_stComp );
	TYPE *					Remove( const COMP & i_stComp );

	CINT					GetCount();
	
	TYPE *					GetHead();

	TYPE *					GetNext();
	TYPE *					UnlinkAndNext();

private:
	typedef					std::map<COMP, TYPE *>
							STL_MAP;

private:
	STL_MAP					m_stMap;
	void *					m_pIterator;

};

#include "./Map.hh"

} /* Collections */			} /* Framework */