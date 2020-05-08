#ifndef SystemCollectionsTree
#define SystemCollectionsTree

#include "../../System.h"

namespace System {			namespace Collections {

template< class Type, class Index, class Memory >
class Tree
{
public:
	struct					Node
	{
		Type *				pItem;
		Node *				pLeft;
		Node *				pRight;

		Node *				pParent;
		BOOL				bBlack;

	};

	class					Iterator
	{
	public:
		typedef				Tree<Type, Index, Memory>
							TREE;
		typedef				typename TREE::Node
							NODE;

	public:
							Iterator() {}
							Iterator( TREE * i_pTree, CBOOL i_bRoot, NODE * i_pCurrent );
		virtual				~Iterator();

		Type *				GetItem();
		
		void				MoveNext();
		void				Unlink();

	private:
		TREE *				m_pTree;
		NODE *				m_pCurrent;
		BOOL				m_bRoot;
		
	};
	friend					Iterator;

	template< class NewType, class NewIndex >
	struct					REBIND
	{
	typedef					Tree<NewType, NewIndex, Memory>
							OTHER;
	};

	typedef					const Type
							CTYPE;

public:
							Tree();
	virtual					~Tree();

	CULONG					GetCount();
	void					UnlinkAll();

	CBOOL					Insert( Type * pItem );
	Type *					Remove( CTYPE & rItem );

	CTYPE *					Search( CTYPE & rItem );

	Iterator				GetRoot();
	Iterator				GetLeaf();
	
	Iterator				FindRoot( CTYPE & rItem );
	Iterator				FindLeaf( CTYPE & rItem );

private:
	Type *					getItem( Node * i_pNode );

	Node *					getRoot();
	Node *					getLeaf();
	Node *					getNext( Node * i_pNode, CBOOL i_bRoot );
	Node *					unlink( Node * i_pNode, CBOOL i_bRoot );

	typedef					const Node
							CNODE;
	typedef					typename Memory::REBIND<Node>::OTHER
							MEMORY;

private:
	void					initialize();
	void					finalize();

	void					unlinkTree( Node * pNode );
	//void					deleteTree( Node * pNode );

	Node *					getNextNode( CNODE * pNode );
	Node *					getPrevNode( CNODE * pNode );

	Node *					getHeadNode( Node * pNode );
	Node *					getTailNode( Node * pNode );

	//void					releaseItem( Type * pItem );

	Node *					unlinkNode( Node * pNode );
	Node *					searchNode( CTYPE & rItem );

	void					rotateLeft( Node * pNode );
	void					rotateRight( Node * pNode );

	void					fixInsert( Node * pNode );
	Node *					fixLeftInsert( Node * pNode );
	Node *					fixRightInsert( Node * pNode );

	void					fixDelete( Node * pNode );
	Node *					fixLeftDelete( Node * pNode );
	Node *					fixRightDelete( Node * pNode );

private:	
	Node *					m_pNIL;
	Node *					m_pRoot;
	
	Node *					m_pCurrent;

	ULONG					m_ulCount;
	Index					m_clsIndex;
	MEMORY					m_clsMemory;

};

} /* Collections */			} /* System */

#include "./TreeIterator.hh"
#include "./Tree.hh"

#endif