#ifndef SystemCollectionsList
#define SystemCollectionsList

#include "../../System.h"

namespace System {			namespace Collections {

template< class Type, class Memory >
class List
{
public:
	struct					Node
	{
		Type *				pItem;
		Node *				pBefore;
		Node *				pNext;
	};

	class					Iterator
	{
	public:
		typedef				List<Type, Memory>
							LIST;
	public:
							Iterator() {}
							Iterator( LIST * i_pList, CBOOL i_bHead );
		virtual				~Iterator();

		Type *				GetItem();
		
		void				MoveNext();
		void				Unlink();

	private:
		typename			LIST::Node *
							m_pCurrent;

		LIST *				m_pList;
		BOOL				m_bHead;
		
	};
	friend class			Iterator;

public:
							List();
	virtual					~List();

	CULONG					GetCount();
	void					Clear();

	void					PushHead( Type * i_pItem );
	void					PushTail( Type * i_pItem );

	Type *					PopHead();
	Type *					PopTail();

	Iterator				GetHead();
	Iterator 				GetTail();

private:
	Type *					getItem( Node * i_pCurrent );

	Node *					getHead();
	Node *					getTail();
	Node *					getNext( Node * i_pCurrent, CBOOL i_bHead );
	Node *					unlink( Node * i_pCurrent, CBOOL i_bHead );

private:
	typedef					typename Memory::REBIND<Node>::OTHER
							MEMORY;

	Node *					m_pHead;
	Node *					m_pTail;

	MEMORY *				m_pNodePool;
	ULONG					m_ulCount;

};

} /* Collections */			} /* System */

#include "./ListIterator.hh"
#include "./List.hh"

#endif