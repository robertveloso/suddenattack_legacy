template<class COMP, class TYPE>
Map<COMP, TYPE>::Map()
{
	typedef					std::map<COMP, TYPE *>::iterator
							ITERATOR;

	m_pIterator				= reinterpret_cast<void *>( new ITERATOR );
}

template<class COMP, class TYPE>
Map<COMP, TYPE>::~Map()
{
	SAFE_DELETE( m_pIterator );
}

template<class COMP, class TYPE>
CBOOL
Map<COMP, TYPE>::Insert( const COMP & i_stComp, TYPE * i_pItem )
{
	typedef					std::pair<COMP, TYPE *>
							PAIR;
	typedef					std::map<COMP, TYPE *>::iterator
							ITERATOR;
	typedef					std::pair< ITERATOR, bool >
							PAIR_R;

	PAIR_R stPair			= m_stMap.insert( PAIR( i_stComp, i_pItem ) );
	if( stPair.second == TRUE )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

template<class COMP, class TYPE>
TYPE *
Map<COMP, TYPE>::Find( const COMP & i_stComp )
{
	typedef					std::map<COMP, TYPE *>::iterator
							ITERATOR;

	ITERATOR stItr			= m_stMap.find( i_stComp );
	if( stItr == m_stMap.end() )
	{
        return NULL;
	}

	return stItr->second;
}

template<class COMP, class TYPE>
TYPE *
Map<COMP, TYPE>::Remove( const COMP & i_stComp )
{
	typedef					std::map<COMP, TYPE *>::iterator
							ITERATOR;

	ITERATOR stItr			= m_stMap.find( i_stComp );

	TYPE * pItem			= NULL;
	if( stItr != m_stMap.end() )
	{
        pItem				= stItr->second;
	}
	m_stMap.erase( stItr );

	return pItem;
}

template<class COMP, class TYPE>
CINT
Map<COMP, TYPE>::GetCount()
{
	return static_cast<INT>( m_stMap.size() );
}

template<class COMP, class TYPE>
TYPE *
Map<COMP, TYPE>::GetHead()
{
	typedef					std::map<COMP, TYPE *>::iterator
							ITERATOR;
	ITERATOR * pIterator	= reinterpret_cast<ITERATOR *>( m_pIterator );
	* pIterator				= m_stMap.begin();

	if( ( * pIterator ) == m_stMap.end() )
	{
		return NULL;
	}

	return (* pIterator)->second;
}

template<class COMP, class TYPE>
TYPE *
Map<COMP, TYPE>::GetNext()
{
	typedef					std::map<COMP, TYPE *>::iterator
							ITERATOR;
	ITERATOR * pIterator	= reinterpret_cast<ITERATOR *>( m_pIterator );
	( * pIterator )++;

	if( ( * pIterator ) == m_stMap.end() )
	{
		return NULL;
	}

	return (* pIterator)->second;
}

template<class COMP, class TYPE>
TYPE *
Map<COMP, TYPE>::UnlinkAndNext()
{
	typedef					std::map<COMP, TYPE *>::iterator
							ITERATOR;
	ITERATOR * pIterator	= reinterpret_cast<ITERATOR *>( m_pIterator );

	* pIterator				= m_stMap.erase( * pIterator );

	if( ( * pIterator ) == m_stMap.end() )
	{
		return NULL;
	}

	return (* pIterator)->second;
}