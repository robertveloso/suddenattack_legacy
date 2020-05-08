#pragma once

#include "../Template/Singleton.h"
#include <List>

namespace Framework {		namespace Collections {

template<class TYPE>
class List
{
public:
							List();
	virtual					~List();

	void					PushHead( TYPE * i_pItem );
	void					PushTail( TYPE * i_pItem );

	TYPE *					PopHead();
	TYPE *					PopTail();

	CINT					GetCount();
	
	TYPE *					GetHead();
	
	TYPE *					GetNext();
	TYPE *					UnlinkAndNext();

private:
	typedef					std::list<TYPE *>
							STL_LIST;

private:
	STL_LIST				m_stList;
	void *					m_pIterator;
//	void *					m_pReverseIterator;

};

#include "./List.hh"

} /* Collections */			} /* Framework */