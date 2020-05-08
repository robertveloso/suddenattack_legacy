// LinkedList.cpp: implementation of the CLinkedList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LinkedList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3F41F788010A
stPlex* __stdcall stPlex::Create(stPlex*& pHead, unsigned int nMax, unsigned int cbElement)
{
	assert(nMax > 0 && cbElement > 0);
	stPlex* p = (stPlex*) new unsigned char[sizeof(stPlex) + nMax * cbElement];
			// may throw exception
	p->pNext = pHead;
	pHead = p;  // change head (adds in reverse order for simplicity)
	return p;
}

//##ModelId=3F41F7880119
void stPlex::FreeDataChain()     // free this one and links
{
	stPlex* p = this;
	while (p != 0)
	{
		unsigned char* bytes = (unsigned char*) p;
		stPlex* pNext = p->pNext;
		delete[] bytes;
		p = pNext;
	}
}


//##ModelId=3F41F7880158
CPointerList::CPointerList(int nBlockSize)
{
	assert(nBlockSize > 0);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = 0;
	m_pBlocks = 0;
	m_nBlockSize = nBlockSize;
}

//##ModelId=3F41F7880167
CPointerList::~CPointerList()
{
	RemoveAll();
	assert(m_nCount == 0);
}

//##ModelId=3F41F7880199
void CPointerList::RemoveAll()
{
	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = 0;
	m_pBlocks->FreeDataChain();
	m_pBlocks = 0;
}


/////////////////////////////////////////////////////////////////////////////
// Node helpers
/*
 * Implementation note: sNode's are stored in CPlex blocks and
 *  chained together. Free blocks are maintained in a singly linked list
 *  using the 'pNext' member of sNode with 'm_pNodeFree' as the head.
 *  Used blocks are maintained in a doubly linked list using both 'pNext'
 *  and 'pPrev' as links and 'm_pNodeHead' and 'm_pNodeTail'
 *   as the head/tail.
 *
 * We never free a CPlex block unless the List is destroyed or RemoveAll()
 *  is used - so the total number of CPlex blocks may grow large depending
 *  on the maximum past size of the list.
 */

CPointerList::sNode* CPointerList::NewNode(CPointerList::sNode* pPrev, CPointerList::sNode* pNext)
{
	if (m_pNodeFree == 0)
	{
		// add another block
		stPlex* pNewBlock = stPlex::Create(m_pBlocks, m_nBlockSize, sizeof(sNode));

		// chain them into free list
		sNode* pNode = (sNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
	assert(m_pNodeFree != 0);  // we must have something

	CPointerList::sNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
	assert(m_nCount > 0);  // make sure we don't overflow

	pNode->data = 0; // start with zero

	return pNode;
}

void CPointerList::FreeNode(CPointerList::sNode* pNode)
{

	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
	assert(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////

//##ModelId=3F41F788018A
POINTERPOS CPointerList::AddHead(void* newElement)
{

	sNode* pNewNode = NewNode(0, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != 0)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (POINTERPOS) pNewNode;

}



//##ModelId=3F41F7880197
POINTERPOS CPointerList::AddTail(void* newElement)
{

	sNode* pNewNode = NewNode(m_pNodeTail, 0);
	pNewNode->data = newElement;
	if (m_pNodeTail != 0)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (POINTERPOS) pNewNode;

}

//##ModelId=3F41F7880188
void* CPointerList::RemoveHead()
{
	assert(m_pNodeHead != 0);  // don't call on empty list !!!

	sNode* pOldNode = m_pNodeHead;
	void* returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != 0)
		m_pNodeHead->pPrev = 0;
	else
		m_pNodeTail = 0;
	FreeNode(pOldNode);
	return returnValue;
}

//##ModelId=3F41F7880189
void* CPointerList::RemoveTail()
{
	assert(m_pNodeTail != 0);  // don't call on empty list !!!

	sNode* pOldNode = m_pNodeTail;
	void* returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != 0)
		m_pNodeTail->pNext = 0;
	else
		m_pNodeHead = 0;
	FreeNode(pOldNode);
	return returnValue;
}

//##ModelId=3F41F78801E6
POINTERPOS CPointerList::InsertBefore(POINTERPOS positon, void* newElement)
{
	if (positon == 0)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before POINTERPOS
	sNode* pOldNode = (sNode*) positon;
	sNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != 0)
	{
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		assert(pOldNode == m_pNodeHead);
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (POINTERPOS) pNewNode;
}



//##ModelId=3F41F78801F4
POINTERPOS CPointerList::InsertAfter(POINTERPOS positon, void* newElement)
{
	if (positon == 0)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before POINTERPOS
	sNode* pOldNode = (sNode*) positon;
	sNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != 0)
	{
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		assert(pOldNode == m_pNodeTail);
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (POINTERPOS) pNewNode;

}



//##ModelId=3F41F78801E4
void CPointerList::RemoveAt(POINTERPOS positon)
{
	sNode* pOldNode = (sNode*) positon;

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
	}
	else
	{
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}

//##ModelId=3F41F78801F7
POINTERPOS CPointerList::Find(void* searchValue, POINTERPOS startAfter) const
{
	sNode* pNode = (sNode*) startAfter;
	if (pNode == 0)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
		pNode = pNode->pNext;  // start after the one specified
	}

	for (; pNode != 0; pNode = pNode->pNext)
		if (pNode->data == searchValue)
			return (POINTERPOS) pNode;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// slow operations

//##ModelId=3F41F7880203
POINTERPOS CPointerList::FindIndex(int nIndex) const
{
	if (nIndex >= m_nCount || nIndex < 0)
		return 0;  // went too far

	sNode* pNode = m_pNodeHead;
	while (nIndex--)
	{
		pNode = pNode->pNext;
	}
	return (POINTERPOS) pNode;
}


//##ModelId=3F41F7880291
template <typename T> 
CTArray<T>::CTArray(int nSize)
{
	m_pArray = new T[nSize];
	m_nSize = nSize;
	m_nCount = 0;
}

//##ModelId=3F41F7880293
template <typename T> 
CTArray<T>::~CTArray()
{
	delete [] m_pArray;
}

//##ModelId=3F41F7880295
template <typename T> 
void CTArray<T>::ClearAll()
{
	memset(m_pArray, 0, sizeof(T)*m_nSize);
}

//##ModelId=3F41F78802C0
template <typename T> 
void CTArray<T>::ResizeArray(int nNewSize)
{
	T* pOld = m_pArray;
	m_Array = new T[nNewSize];
	memcpy(m_pArray,pOld,sizeof(T)*m_nCount);
	delete [] pOld;
	m_nSize = nNewSize;
}

//##ModelId=3F41F78802A1
template <typename T> 
T& CTArray<T>::GetItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount) return 0;

	return m_pArray[nIndex];
}

//##ModelId=3F41F78802AF
template <typename T> 
void CTArray<T>::SetItem(int nIndex, T item)
{
	T OldItem = GetItem(nIndex);
	memcpy(&OldItem, &item, sizeof(T));
}

//##ModelId=3F41F788029F
template <typename T> 
T* CTArray<T>::GetItemPtr(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount) return 0;

	return m_pArray + nIndex;
}

//##ModelId=3F41F78802B2
template <typename T> 
int CTArray<T>::AddItem(T newItem)
{
	if (m_nCount>=m_nSize)
		ResizeArray(m_nSize+10);
	memcpy(m_pArray+m_nCount, &newItem, sizeof(T));
	++m_nCount;
	return m_nCount;
}

//##ModelId=3F41F78802B4
template <typename T> 
int CTArray<T>::RemoveItem(int nIndex)
{
	if (nIndex >= m_nCount) return m_nCount;
	if (m_nCount-nIndex > 1)
		memmove(m_pArray+nIndex, m_pArray+nIndex+1, sizeof(T)*(m_nCount-nIndex));
	else
		memset(m_pArray+nIndex,0, sizeof(T));

	return --m_nCount;
}
/*
template <typename T> 
int CTArray<T>::FindItem(T& item)
{
	int nIndex;
	for (int i = 0 ; i < m_nCount; ++i)
	{
		if (m_pArray[i] == item)
		{
			nIndex = i;
			break;
		}
	}

	return nIndex;
}
*/
/*

CLinkedList::CLinkedList()
{
}

CLinkedList::~CLinkedList()
{

}

int CLinkedList::AddHead(POINTER& pNewData)
{
	return m_nCount;
}

int CLinkedList::AddTail(POINTER& pNewData)
{
	return m_nCount;
}

POINTER& CLinkedList::RemoveHead()
{
	m_Node->
}

POINTER& CLinkedList::RemoveTail()
{
}
*/
