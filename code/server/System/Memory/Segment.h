#ifndef SystemMemorySegment
#define SystemMemorySegment

#include "../../System.h"

namespace System {			namespace Memory {

template< ULONG s_ulLimit, class Type = INT >
class Segment
{
public:
	template< class NewType >
	struct					REBIND
	{
	typedef					Segment<s_ulLimit, NewType>
							OTHER;
	};

public:
							Segment();
	virtual					~Segment();

	CULONG					GetCount() const;

	Type *					Capture( Type * pItem = NULL, CULONG ulSize = 1 );
	CULONG					Release( Type * pItem, CULONG ulSize = 1 );

private:
	struct					node
	{
		Type				clsItem;

		node *				pNext;
		node *				pLink;

		ULONG				ulSize;

	};

private:
	node *					searchNode( Type * pItem );
#ifdef PDEBUG
	node *					checkArray( Type * pItem );
	node *					checkSpare( Type * pItem );
#endif

private:
	Array<s_ulLimit, node>	m_stArray;
	ULONG					m_ulCount;
	node *					m_pSpare;
	node *					m_pAvail;

};

template< ULONG s_ulLimit, class Type >
Segment<s_ulLimit,Type>::Segment()
{
	aSsErT( s_ulLimit > 0 );

	m_ulCount	= 0;
	m_pSpare	= NULL;
	m_pAvail	= & m_stArray[ 0 ];

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit - 1; ulIndex++ )
	{
		node * pNode	= & m_stArray[ ulIndex ];
		pNode->pNext	= & m_stArray[ ulIndex + 1 ];
		pNode->pLink	= NULL;
		pNode->ulSize	= 0;
	}

	node * pNode	= & m_stArray[ s_ulLimit - 1 ];
	pNode->pNext	= NULL;
	pNode->pLink	= NULL;
	pNode->ulSize	= 0;
}

template< ULONG s_ulLimit, class Type >
Segment<s_ulLimit,Type>::~Segment()
{
	ASSERT( m_ulCount == 0 );

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex++ )
	{
		ASSERT( m_stArray[ ulIndex ].ulSize == 0 );
	}

	node * pNode	= m_pSpare;
	node * pLink	= NULL;

	while ( pNode != NULL )
	{
		pLink = pNode->pLink;

		ASSERT( pNode->ulSize == 0 );
		SAFE_DELETE( pNode );

		pNode = pLink;
	}
}

template< ULONG s_ulLimit, class Type >
inline CULONG
Segment<s_ulLimit,Type>::GetCount() const
{
	return m_ulCount;
}

template< ULONG s_ulLimit, class Type >
inline Type *
Segment<s_ulLimit,Type>::Capture( Type * pItem, CULONG ulSize )
{
	if ( pItem != NULL )
	{
		node * pNode = this->searchNode( pItem );
		ASSERT( pNode->ulSize > 0 );

		pNode->ulSize += ulSize;
		return pItem;
	}

	m_ulCount++;

	node * pNode = m_pAvail;
	if ( pNode != NULL )
	{
		m_pAvail = pNode->pNext;
	}
	else
	{
		pNode			= new node;
		pNode->pLink	= m_pSpare;
		m_pSpare		= pNode;
	}

	ASSERT( ulSize > 0 );

	pNode->pNext	= NULL;
	pNode->ulSize	= ulSize;

	return (& pNode->clsItem);
}

template< ULONG s_ulLimit, class Type >
inline CULONG
Segment<s_ulLimit,Type>::Release( Type * pItem, CULONG ulSize )
{
	if ( pItem == NULL )
	{
		DEBUG_BREAK;
		return 0;
	}

	node * pNode = this->searchNode( pItem );
	ASSERT( pNode->ulSize >= ulSize );

	pNode->ulSize -= ulSize;
	if ( pNode->ulSize > 0 )
	{
		return pNode->ulSize;
	}

	pNode->pNext	= m_pAvail;
	m_pAvail		= pNode;

	m_ulCount--;
	return 0;
}

template< ULONG s_ulLimit, class Type >
inline typename Segment<s_ulLimit, Type>::node *
Segment<s_ulLimit,Type>::searchNode( Type * pItem )
{
#ifndef PDEBUG
	return reinterpret_cast<node *>( pItem );
#else
	node * pNode = this->checkArray( pItem );
	if ( pNode == NULL )
	{
		pNode = this->checkSpare( pItem );
		Assert( pNode != NULL );
	}

	return pNode;
#endif
}

#ifdef PDEBUG

template< ULONG s_ulLimit, class Type >
inline typename Segment<s_ulLimit, Type>::node *
Segment<s_ulLimit,Type>::checkArray( Type * pItem )
{
	node * pNode	= reinterpret_cast<node *>( pItem );
	CULONG ulIndex	= m_stArray( * pNode );
	CULONG ulLength	= m_stArray.GetCount();

	if ( ulIndex < ulLength )
	{
		return pNode;
	}

	return NULL;
}

template< ULONG s_ulLimit, class Type >
inline typename Segment<s_ulLimit, Type>::node *
Segment<s_ulLimit,Type>::checkSpare( Type * pItem )
{
	node * pNode = m_pSpare;
	while ( pNode != NULL )
	{
		if ( (& pNode->clsItem) == pItem )
		{
			return pNode;
		}

		pNode = pNode->pLink;
	}

	return NULL;
}

#endif

} /* Memory */				} /* System */

#endif