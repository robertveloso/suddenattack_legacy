namespace System {			namespace Collections {

template < class Type, class Index, class Memory >
Tree<Type,Index,Memory>::Tree()
{
	this->initialize();
}

template < class Type, class Index, class Memory>
Tree<Type,Index,Memory>::~Tree()
{
	this->finalize();
}

template< class Type, class Index, class Memory >
inline CULONG
Tree<Type,Index,Memory>::GetCount()
{
	return m_ulCount;
}

template< class Type, class Index, class Memory>
inline void
Tree<Type,Index,Memory>::UnlinkAll()
{
	if ( m_pRoot == m_pNIL )
	{
		return;
	}

	this->unlinkTree( m_pRoot );

	m_pRoot		= m_pNIL;
	m_ulCount	= 0;
}

template < class Type, class Index, class Memory>
inline CBOOL
Tree<Type,Index,Memory>::Insert( Type * pItem )
{
	Assert( pItem != NULL );

	Node * pNewNode		= m_clsMemory.Capture();
	pNewNode->pItem		= pItem;
	pNewNode->pLeft		= m_pNIL;
	pNewNode->pRight	= m_pNIL;
	pNewNode->pParent	= m_pNIL;
	pNewNode->bBlack	= FALSE;

	if ( m_pRoot == m_pNIL )
	{
		m_pRoot = pNewNode;
		m_ulCount++;

		this->fixInsert( pNewNode );
		return TRUE;
	}

	Node * pNode = m_pRoot;
	while ( TRUE )
	{
		CINT nCompareValue = m_clsIndex( * pItem, * pNode->pItem );
		if ( nCompareValue == 0 )
		{
			m_clsMemory.Release( pNewNode );
			return FALSE;
		}

		if ( nCompareValue > 0 )
		{
			OPERATION;
		}
		else if ( pNode->pLeft != m_pNIL )
		{
			pNode = pNode->pLeft;
			continue;
		}
		else
		{
			pNewNode->pParent	= pNode;
			pNode->pLeft		= pNewNode;
			break;
		}

		if ( pNode->pRight != m_pNIL )
		{
			pNode = pNode->pRight;
			continue;
		}

		pNewNode->pParent	= pNode;
		pNode->pRight		= pNewNode;
		break;
	}

	m_ulCount++;

	this->fixInsert( pNewNode );
	return TRUE;
}

template < class Type, class Index, class Memory>
inline Type *
Tree<Type,Index,Memory>::Remove( CTYPE & rItem )
{
	Node * pNode = this->searchNode( rItem );
	if( pNode == m_pNIL )
	{
		return NULL;
	}

	Type * pItem			= pNode->pItem;

	this->unlinkNode( pNode );
	m_ulCount--;

	return pItem;
}				

template < class Type, class Index, class Memory>
inline typename Tree<Type,Index,Memory>::CTYPE *
Tree<Type,Index,Memory>::Search( CTYPE & rItem )
{
	Node * pNode = this->searchNode( rItem );
	if( pNode == m_pNIL )
	{
		return NULL;
	}

	return pNode->pItem;
}				

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Iterator
Tree<Type,Index,Memory>::GetRoot()
{
	return Iterator( this, TRUE, this->getRoot() );
}				

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Iterator
Tree<Type,Index,Memory>::GetLeaf()
{
	return Iterator( this, FALSE, this->getLeaf() );
}				

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Iterator
Tree<Type,Index,Memory>::FindRoot( CTYPE & rItem )
{
	return Iterator( this, TRUE, this->searchNode( rItem ) );
}				

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Iterator
Tree<Type,Index,Memory>::FindLeaf( CTYPE & rItem )
{
	return Iterator( this, FALSE, this->searchNode( rItem ) );
}

template< class Type, class Index, class Memory >
inline void
Tree<Type,Index,Memory>::initialize()
{
	m_pNIL			= m_clsMemory.Capture();
	m_pNIL->pItem	= NULL;
	m_pNIL->pLeft	= m_pNIL;
	m_pNIL->pRight	= m_pNIL;
	m_pNIL->pParent	= m_pNIL;
	m_pNIL->bBlack	= TRUE;
	m_pRoot			= m_pNIL;
	m_ulCount		= 0;
}

template< class Type, class Index, class Memory >
inline void
Tree<Type,Index,Memory>::finalize()
{
	m_clsMemory.Release( m_pNIL );
	m_pNIL					= NULL;
}

template < class Type, class Index, class Memory>
inline typename Type *
Tree<Type,Index,Memory>::getItem( Node * i_pNode )
{
	if( i_pNode == NULL )
	{
		return NULL;
	}

	return i_pNode->pItem;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::getRoot()
{
	Node * pNode = m_pRoot;
	if ( pNode == m_pNIL )
	{
		return NULL;
	}

	return this->getHeadNode( pNode );
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::getLeaf()
{
	Node * pNode = m_pRoot;
	if ( pNode == m_pNIL )
	{
		return NULL;
	}

	return this->getTailNode( pNode );
}

template < class Type, class Index, class Memory>
inline typename Tree<Type,Index,Memory>::Node *
Tree<Type,Index,Memory>::getNext( Node * i_pNode, CBOOL bRoot )
{
	Assert( i_pNode != NULL && i_pNode != m_pNIL );

	Node * pNext;
	if( bRoot == TRUE )
	{
		pNext				= this->getNextNode( i_pNode );
	}
	else
	{
		pNext				= this->getPrevNode( i_pNode );
	}

	if ( pNext != m_pNIL )
	{
		return pNext;
	}

	return NULL;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type,Index,Memory>::Node *
Tree<Type,Index,Memory>::unlink( Node * i_pNode, CBOOL i_bRoot )
{
	Assert( i_pNode != NULL && i_pNode != m_pNIL );

	m_ulCount--;

	Node * pNode			= this->unlinkNode( i_pNode );
	if( i_bRoot == FALSE )
	{
		pNode				= this->getPrevNode( pNode );
	}

	return pNode;
}

template < class Type, class Index, class Memory>
inline void
Tree<Type,Index,Memory>::unlinkTree( Node * pNode )
{
	if ( pNode->pLeft != m_pNIL )
	{
		this->unlinkTree( pNode->pLeft );
	}

	if ( pNode->pRight != m_pNIL )
	{
		this->unlinkTree( pNode->pRight );
	}

	m_clsMemory.Release( pNode );
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::searchNode( CTYPE & rItem )
{
	Node * pNode = m_pRoot;
	while ( pNode != m_pNIL )
	{
		CINT nCompareValue = m_clsIndex( rItem, * pNode->pItem );
		if ( nCompareValue == 0 )
		{
			break;
		}

		if ( nCompareValue < 0 )
		{
			pNode = pNode->pLeft;
		}
		else
		{
			pNode = pNode->pRight;
		}
	}

	return pNode;
}

template < class Type, class Index, class Memory>
inline void
Tree<Type,Index,Memory>::fixInsert( Node * pNode )
{
	while ( pNode->pParent->bBlack == FALSE )
	{
		Node * pParent = pNode->pParent;
		if ( pParent == pParent->pParent->pLeft )
		{
			pNode = this->fixLeftInsert( pNode );
		}
		else
		{
			pNode = this->fixRightInsert( pNode );
		}
	}

	m_pRoot->bBlack = TRUE;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::fixLeftInsert( Node * pNode )
{
	Node * pUncle = pNode->pParent->pParent->pRight;
	if ( pUncle->bBlack == FALSE )
	{
		pUncle->bBlack			= TRUE;
		Node * pParent			= pNode->pParent;
		pParent->bBlack			= TRUE;
		Node * pGrandparent		= pParent->pParent;
		pGrandparent->bBlack	= FALSE;

		return pGrandparent;
	}

	if ( pNode == pNode->pParent->pRight )
	{
		pNode = pNode->pParent;
		this->rotateLeft( pNode );
	}

	Node * pParent			= pNode->pParent;
	pParent->bBlack			= TRUE;
	Node * pGrandparent		= pParent->pParent;
	pGrandparent->bBlack	= FALSE;

	this->rotateRight( pGrandparent );
	return pNode;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::fixRightInsert( Node * pNode )
{
	Node * pUncle = pNode->pParent->pParent->pLeft;
	if ( pUncle->bBlack == FALSE )
	{
		pUncle->bBlack			= TRUE;
		Node * pParent			= pNode->pParent;
		pParent->bBlack			= TRUE;
		Node * pGrandparent		= pParent->pParent;
		pGrandparent->bBlack	= FALSE;

		return pGrandparent;
	}

	if ( pNode == pNode->pParent->pLeft )
	{
		pNode = pNode->pParent;
		this->rotateRight( pNode );
	}

	Node * pParent			= pNode->pParent;
	pParent->bBlack			= TRUE;
	Node * pGrandparent		= pParent->pParent;
	pGrandparent->bBlack	= FALSE;

	this->rotateLeft( pGrandparent );
	return pNode;
}

template < class Type, class Index, class Memory>
inline void
Tree<Type,Index,Memory>::rotateLeft( Node * pNode )
{
	Node * pRight = pNode->pRight;
	if ( pRight == m_pNIL )
	{
		return;
	}

	pNode->pRight = pRight->pLeft;
	if ( pNode->pRight != m_pNIL )
	{
		pNode->pRight->pParent = pNode;
	}

	Node * pParent	= pNode->pParent;
	pRight->pParent	= pParent;

	if ( pParent == m_pNIL )
	{
		m_pRoot = pRight;
	}
	else if ( pNode == pParent->pLeft )
	{
		pParent->pLeft = pRight;
	}
	else
	{
		pParent->pRight = pRight;
	}

	pRight->pLeft	= pNode;
	pNode->pParent	= pRight;
}

template < class Type, class Index, class Memory>
inline void
Tree<Type,Index,Memory>::rotateRight( Node * pNode )
{
	Node * pLeft = pNode->pLeft;
	if ( pLeft == m_pNIL )
	{
		return;
	}

	pNode->pLeft = pLeft->pRight;
	if ( pNode->pLeft != m_pNIL )
	{
		pNode->pLeft->pParent = pNode;
	}

	Node * pParent	= pNode->pParent;
	pLeft->pParent	= pParent;

	if ( pParent == m_pNIL )
	{
		m_pRoot = pLeft;	
	}
	else if ( pNode == pParent->pLeft )
	{
		pParent->pLeft = pLeft;
	}
	else
	{
		pParent->pRight = pLeft;
	}

	pLeft->pRight	= pNode;
	pNode->pParent	= pLeft;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::unlinkNode( Node * pNode )
{
	Node * pVictim	= pNode;
	Node * pRemain	= this->getNextNode( pNode );

	if ( pNode->pLeft != m_pNIL && pNode->pRight != m_pNIL )
	{
		pVictim	= pRemain;
		pRemain	= pNode;
	}

	Node * pChild = m_pNIL;
	if ( pVictim->pLeft != m_pNIL )
	{
		pChild = pVictim->pLeft;
	}
	else
	{
		pChild = pVictim->pRight;
	}

	Node * pParent	= pVictim->pParent;
	pChild->pParent	= pParent;

	if ( pParent == m_pNIL )
	{
		m_pRoot = pChild;
	}
	else if ( pVictim == pParent->pLeft )
	{
		pParent->pLeft = pChild;
	}
	else
	{
		pParent->pRight = pChild;
	}

	if ( pVictim->bBlack == TRUE )	
	{
		this->fixDelete( pChild );
	}

	pNode->pItem = pVictim->pItem;
	m_clsMemory.Release( pVictim );

	return pRemain;
}

template < class Type, class Index, class Memory>
inline void
Tree<Type,Index,Memory>::fixDelete( Node * pNode )
{
	while ( pNode != m_pRoot && pNode->bBlack == TRUE )
	{
		if ( pNode->pParent->pLeft == pNode )
		{
			pNode = this->fixLeftDelete( pNode );
		}
		else
		{
			pNode = this->fixRightDelete( pNode );
		}
	}

	pNode->bBlack = TRUE;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::fixLeftDelete( Node * pNode )
{
	Node * pParent	= pNode->pParent;
	pNode			= pParent->pRight;

	if ( pNode->bBlack == FALSE )
	{
		pNode->bBlack	= TRUE;
		pParent->bBlack	= FALSE;

		this->rotateLeft( pParent );
		pNode = pParent->pRight;
	} 

	if ( pNode->pRight->bBlack == TRUE )
	{
		pNode->bBlack	= FALSE;
		Node * pLeft	= pNode->pLeft;

		if ( pLeft->bBlack == TRUE )
		{
			return pParent;
		}

		pLeft->bBlack = TRUE;
		this->rotateRight( pNode );
		pNode = pParent->pRight;
	}

	pNode->bBlack			= pParent->bBlack;
	pParent->bBlack			= TRUE;
	pNode->pRight->bBlack	= TRUE;

	this->rotateLeft( pParent );
	return m_pRoot;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::fixRightDelete( Node * pNode )
{
	Node * pParent	= pNode->pParent;
	pNode			= pParent->pLeft;

	if ( pNode->bBlack == FALSE )
	{
		pNode->bBlack	= TRUE;
		pParent->bBlack	= FALSE;

		this->rotateRight( pParent );
		pNode = pParent->pLeft;
	} 

	if ( pNode->pLeft->bBlack == TRUE )
	{
		pNode->bBlack	= FALSE;
		Node * pRight	= pNode->pRight;

		if ( pRight->bBlack == TRUE )
		{
			return pParent;
		}

		pRight->bBlack = TRUE;
		this->rotateLeft( pNode );
		pNode = pParent->pLeft;
	}

	pNode->bBlack			= pParent->bBlack;
	pParent->bBlack			= TRUE;
	pNode->pLeft->bBlack	= TRUE;

	this->rotateRight( pParent );	
	return m_pRoot;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::getNextNode( CNODE * pNode )
{
	if ( pNode->pRight != m_pNIL )
	{
		return this->getHeadNode( pNode->pRight );
	}

	Node * pParent = pNode->pParent;
	while( pParent != m_pNIL && pParent->pRight == pNode )
	{
		pNode	= pParent;
		pParent = pParent->pParent;
	}

	return pParent;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::getPrevNode( CNODE * pNode )
{
	if ( pNode->pLeft != m_pNIL )
	{
		return this->getTailNode( pNode->pLeft );
	}

	Node * pParent = pNode->pParent;
	while( pParent != m_pNIL && pParent->pLeft == pNode )
	{
		pNode	= pParent;
		pParent = pParent->pParent;
	}

	return pParent;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::getHeadNode( Node * pNode )
{
	while ( pNode->pLeft != m_pNIL )
	{
		pNode = pNode->pLeft;
	}

	return pNode;
}

template < class Type, class Index, class Memory>
inline typename Tree<Type, Index, Memory>::Node *
Tree<Type,Index,Memory>::getTailNode( Node * pNode )
{
	while( pNode->pRight != m_pNIL )
	{
		pNode = pNode->pRight;
	}

	return pNode;
}

} /* Collections */			} /* System */