#pragma once

#include "../IClient.h"
#include "../../System/Collections/List.h"
#include "../../System/Memory/Segment.h"

namespace Dispatcher {		namespace Client {

class Register
:	public					IClient::IRegister
{
public:
							Register();
	virtual					~Register();

	virtual void			Regist( IBUILDER * pBuilder );
	virtual IBUILDER *		First();
	virtual IBUILDER *		Next();

private:
	typedef					Memory::Segment<4096>
							POOL;
	typedef					Collections::List<IBUILDER, POOL>
							LIST;
	typedef					LIST::Iterator
							ITERATOR;

private:
	LIST *					m_pList;
	ITERATOR				m_stIterator;

};

} /* Client */				} /* Dispatcher */