namespace System {			namespace Collections {

template< class Type, class Memory >
List<Type, Memory>::List()
{
	m_pNodePool				= new MEMORY;
	m_ulCount				= 0;

	m_pHead					= NULL;
	m_pTail					= NULL;
}

template< class Type, class Memory >
List<Type, Memory>::~List()
{
	SAFE_DELETE( m_pNodePool );
}

template< class Type, class Memory >
inline CULONG
List<Type, Memory>::GetCount()
{
	return m_ulCount;
}

template< class Type, class Memory >
inline void
List<Type, Memory>::Clear()
{
	while( TRUE )
	{
		Type * pItem		= this->PopHead();
		if( pItem == NULL )
		{
			break;
		}
	}
}

template< class Type, class Memory >
inline void
List<Type, Memory>::PushHead( Type * i_pItem )
{
	Node * pNode			= m_pNodePool->Capture();

	pNode->pItem			= i_pItem;
	pNode->pBefore			= NULL;
	pNode->pNext			= m_pHead;

	if( m_pHead != NULL )
	{
		m_pHead->pBefore	= pNode;
	}

	m_pHead					= pNode;
	if( m_pTail == NULL )
	{
		m_pTail				= pNode;
	}

	m_ulCount ++;
}

template< class Type, class Memory >
inline void
List<Type, Memory>::PushTail( Type * i_pItem )
{
	Node * pNode			= m_pNodePool->Capture();

	pNode->pItem			= i_pItem;
	pNode->pBefore			= m_pTail;
	pNode->pNext			= NULL;

	if( m_pTail != NULL )
	{
		m_pTail->pNext		= pNode;
	}

	m_pTail					= pNode;
	if( m_pHead == NULL )
	{
		m_pHead				= pNode;
	}

	m_ulCount ++;
}

template< class Type, class Memory >
inline Type *
List<Type, Memory>::PopHead()
{
	if( m_pHead == NULL )
	{
		return NULL;
	}

	Node * pNode			= m_pHead;
	Type * pItem			= pNode->pItem;
	m_pNodePool->Release( pNode );
	
	m_pHead					= m_pHead->pNext;
	if( m_pHead == NULL )
	{
		m_pTail				= NULL;
	}
	else
	{
		m_pHead->pBefore	= NULL;
	}
	
	m_ulCount --;

	return pItem;
}

template< class Type, class Memory >
inline Type *
List<Type, Memory>::PopTail()
{
	if( m_pTail == NULL )
	{
		return NULL;
	}

	Node * pNode			= m_pTail;
	Type * pItem			= pNode->pItem;
	m_pNodePool->Release( pNode );
	
	m_pTail					= m_pTail->pBefore;
	if( m_pTail == NULL )
	{
		m_pHead				= NULL;
	}
	else
	{
		m_pTail->pNext		= NULL;
	}
	
	m_ulCount --;
	
	return pItem;
}

template< class Type, class Memory >
inline typename List<Type, Memory>::Iterator
List<Type, Memory>::GetHead()
{
	return Iterator( this, TRUE );
}

template< class Type, class Memory >
inline typename List<Type, Memory>::Iterator
List<Type, Memory>::GetTail()
{
	return Iterator( this, FALSE );
}

template< class Type, class Memory >
inline typename List<Type, Memory>::Node *
List<Type, Memory>::getHead()
{
	return m_pHead;
}

template< class Type, class Memory >
inline typename List<Type, Memory>::Node *
List<Type, Memory>::getTail()
{
	return m_pTail;
}

template< class Type, class Memory >
inline Type *
List<Type, Memory>::getItem( Node * i_pNode )
{
	if( i_pNode == NULL )
	{
		return NULL;
	}

	return i_pNode->pItem;
}

template< class Type, class Memory >
inline typename List<Type, Memory>::Node *
List<Type, Memory>::getNext( Node * i_pNode, CBOOL i_bHead )
{
	if( i_pNode == NULL )
	{
		DEBUG_BREAK;
		return NULL;
	}

	if( i_bHead == TRUE )
	{
		return i_pNode->pNext;
	}
	else
	{
		return i_pNode->pBefore;
	}
}

template< class Type, class Memory >
inline typename List<Type, Memory>::Node *
List<Type, Memory>::unlink( Node * i_pNode, CBOOL i_bHead )
{
	if( i_pNode == NULL )
	{
		DEBUG_BREAK;
		return NULL;
	}

	Node * pRet				= NULL;
	if( i_bHead == TRUE )
	{
		pRet				= i_pNode->pNext;
	}
	else
	{
		pRet				= i_pNode->pBefore;
	}

	if( i_pNode == m_pHead )
	{
		this->PopHead();
	}
	else if( i_pNode == m_pTail )
	{
		this->PopTail();
	}
	else
	{
		i_pNode->pBefore->pNext = i_pNode->pNext;
		i_pNode->pNext->pBefore = i_pNode->pBefore;

		m_pNodePool->Release( i_pNode );

		m_ulCount --;
	}

	return pRet;
}

} /* Collections */			} /* System */