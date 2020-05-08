template<class TYPE>
List<TYPE>::List()
{
	m_pIterator				= reinterpret_cast<void *>( new std::list<TYPE *>::iterator );
//	m_pReverseIterator		= reinterpret_cast<void *>( new std::list<TYPE *>::reverse_iterator );
}

template<class TYPE>
List<TYPE>::~List()
{
	SAFE_DELETE( m_pIterator );
}

template<class TYPE>
void
List<TYPE>::PushHead( TYPE * i_pItem )
{
	m_stList.push_front( i_pItem );
}

template<class TYPE>
void
List<TYPE>::PushTail( TYPE * i_pItem )
{
}

template<class TYPE>
TYPE *
List<TYPE>::PopHead()
{
	if( m_stList.size() == 0 )
	{
		return NULL;
	}
	TYPE * pItem			= m_stList.front();
	m_stList.pop_front();

	return pItem;
}

template<class TYPE>
TYPE *
List<TYPE>::PopTail()
{
	if( m_stList.size() == 0 )
	{
		return NULL;
	}
	TYPE * pItem			= m_stList.back();
	m_stList.pop_back();

	return pItem;
}

template<class TYPE>
CINT
List<TYPE>::GetCount()
{
	return static_cast<INT>( m_stList.size() );
}

template<class TYPE>
TYPE *
List<TYPE>::GetHead()
{
	typedef					std::list<TYPE *>::iterator
							ITERATOR;
	ITERATOR * pIterator	= reinterpret_cast<ITERATOR *>( m_pIterator );
	* pIterator				= m_stList.begin();

	if( ( * pIterator ) == m_stList.end() )
	{
		return NULL;
	}

	return ** pIterator;
}

//template<class TYPE>
//TYPE *
//List<TYPE>::GetTail()
//{
//	typedef					std::list<TYPE *>::reverse_iterator
//							ITERATOR;
//	ITERATOR * pIterator	= reinterpret_cast<ITERATOR *>( m_pReverseIterator );
//	* pIterator				= m_stList.rbegin();
//	return ** pIterator;
//}

template<class TYPE>
TYPE *
List<TYPE>::GetNext()
{
	typedef					std::list<TYPE *>::iterator
							ITERATOR;
	ITERATOR * pIterator	= reinterpret_cast<ITERATOR *>( m_pIterator );
	( * pIterator )++;

	if( ( * pIterator ) == m_stList.end() )
	{
		return NULL;
	}

	return ** pIterator;
}

//template<class TYPE>
//TYPE *
//List<TYPE>::GetPrev()
//{
//	typedef					std::list<TYPE *>::reverse_iterator
//							ITERATOR;
//	ITERATOR * pIterator	= reinterpret_cast<ITERATOR *>( m_pReverseIterator );
//	( * pIterator )++;
//
//	if( ( * pIterator ) == m_stList.rend() )
//	{
//		return NULL;
//	}
//
//	return ** pIterator;
//}

template<class TYPE>
TYPE *
List<TYPE>::UnlinkAndNext()
{
	typedef					std::list<TYPE *>::iterator
							ITERATOR;
	ITERATOR * pIterator	= reinterpret_cast<ITERATOR *>( m_pIterator );

	* pIterator				= m_stList.erase( * pIterator );

	if( ( * pIterator ) == m_stList.end() )
	{
		return NULL;
	}

	return ** pIterator;
}

//template<class TYPE>
//TYPE *
//List<TYPE>::UnlinkAndPrev()
//{
//	typedef					std::list<TYPE *>::reverse_iterator
//							ITERATOR;
//	ITERATOR * pIterator	= reinterpret_cast<ITERATOR *>( m_pReverseIterator );
//
//	* pIterator				= m_stList.rerase( * pIterator );
//
//	return ** pIterator;
//}