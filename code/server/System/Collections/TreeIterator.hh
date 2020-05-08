namespace System {			namespace Collections {

//template< class Type, class Index, class Memory >
//Tree<Type, Index, Memory>::Iterator::Iterator( TREE * i_pTree, CBOOL i_bRoot )
//{
//	m_pTree			= i_pTree;
//	m_bRoot			= i_bRoot;
//	if( i_bRoot == TRUE )
//	{
//		m_pCurrent	= m_pTree->getRoot();
//	}
//	else
//	{
//		m_pCurrent	= m_pTree->getLeaf();
//	}
//}

template< class Type, class Index, class Memory >
Tree<Type, Index, Memory>::Iterator::Iterator( TREE * i_pTree, CBOOL i_bRoot, NODE * i_pCurrent )
{
	m_pTree			= i_pTree;
	m_bRoot			= i_bRoot;
	m_pCurrent		= i_pCurrent;
}

template< class Type, class Index, class Memory >
Tree<Type, Index, Memory>::Iterator::~Iterator()
{
}

template< class Type, class Index, class Memory >
inline Type *
Tree<Type, Index, Memory>::Iterator::GetItem()
{
	return m_pTree->getItem( m_pCurrent );
}

template< class Type, class Index, class Memory >
inline void
Tree<Type, Index, Memory>::Iterator::MoveNext()
{
	m_pCurrent				= m_pTree->getNext( m_pCurrent, m_bRoot );
}

template< class Type, class Index, class Memory >
inline void
Tree<Type, Index, Memory>::Iterator::Unlink()
{
	m_pCurrent				= m_pTree->unlink( m_pCurrent, m_bRoot );
}


} /* Collections */			} /* System */