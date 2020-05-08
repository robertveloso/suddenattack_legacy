namespace System {			namespace Collections {

template< class Type, class Memory >
List<Type, Memory>::Iterator::Iterator( LIST * i_pList, CBOOL i_bHead )
{
	m_pList			= i_pList;
	m_bHead			= i_bHead;
	if( i_bHead == TRUE )
	{
		m_pCurrent	= m_pList->getHead();
	}
	else
	{
		m_pCurrent	= m_pList->getTail();
	}
}

template< class Type, class Memory >
List<Type, Memory>::Iterator::~Iterator()
{
}

template< class Type, class Memory >
inline Type *
List<Type, Memory>::Iterator::GetItem()
{
	return m_pList->getItem( m_pCurrent );
}

template< class Type, class Memory >
inline void
List<Type, Memory>::Iterator::MoveNext()
{
	m_pCurrent				= m_pList->getNext( m_pCurrent, m_bHead );
}

template< class Type, class Memory >
inline void
List<Type, Memory>::Iterator::Unlink()
{
	m_pCurrent				= m_pList->unlink( m_pCurrent, m_bHead );
}


} /* Collections */			} /* System */