 // LinkedList.h: interface for the CLinkedList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINKEDLIST_H__27CB03E3_B4DD_459B_8155_EE2DEEF3C1E3__INCLUDED_)
#define AFX_LINKEDLIST_H__27CB03E3_B4DD_459B_8155_EE2DEEF3C1E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <list>
#include <ASSERT.H>

// abstract iteration POINTERPOS
struct __POINTERPOS { };
typedef __POINTERPOS* POINTERPOS;

struct stPlex
{
public:
	stPlex() { pNext = 0;}
	~stPlex() {};

	static stPlex* __stdcall Create(stPlex*& pHead, unsigned int nMax, unsigned int cbElement);
	void FreeDataChain();

	void* data() { return this+1; }

	stPlex* pNext;
};

class CPointerList
{
protected:
	struct sNode
	{
		sNode* pPrev;
		sNode* pNext;
		void* data;
	};

public:

// Construction
	CPointerList(int nBlockSize = 10);
	~CPointerList();

// Attributes (head and tail)
	// count of elements
	int GetCount() const;
	bool IsEmpty() const;

	// peek at head or tail
	void*& GetHead();
	void* GetHead() const;
	void*& GetTail();
	void* GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list!
	void* RemoveHead();
	void* RemoveTail();

	// add before head or after tail
	POINTERPOS AddHead(void* newElement);
	POINTERPOS AddTail(void* newElement);

	// remove all elements
	void RemoveAll();
	// iteration
	POINTERPOS GetHeadPosition() const;
	POINTERPOS GetTailPosition() const;
	void*& GetNext(POINTERPOS& rpositon); // return *POINTERPOS++
	void* GetNext(POINTERPOS& rpositon) const; // return *POINTERPOS++
	void*& GetPrev(POINTERPOS& rpositon); // return *POINTERPOS--
	void* GetPrev(POINTERPOS& rpositon) const; // return *POINTERPOS--

	// getting/modifying an element at a given POINTERPOS
	void*& GetAt(POINTERPOS positon);
	void* GetAt(POINTERPOS positon) const;
	void SetAt(POINTERPOS pos, void* newElement);

	void RemoveAt(POINTERPOS position);

	// inserting before or after a given POINTERPOS
	POINTERPOS InsertBefore(POINTERPOS positon, void* newElement);
	POINTERPOS InsertAfter(POINTERPOS positon, void* newElement);


	// helper functions (note: O(n) speed)
	POINTERPOS Find(void* searchValue, POINTERPOS startAfter = 0) const;
						// defaults to starting at the HEAD
						// return NULL if not found
	POINTERPOS FindIndex(int nIndex) const;

	void* GetIndex(int nIndex);

// Implementation
protected:
	sNode* m_pNodeHead;
	sNode* m_pNodeTail;
	int m_nCount;
	sNode* m_pNodeFree;
	struct stPlex* m_pBlocks;
	int m_nBlockSize;

	sNode* NewNode(sNode* pPrev, sNode* pNext);
	void FreeNode(sNode* pNode);
};

inline int CPointerList::GetCount() const
	{ return m_nCount; }
inline bool CPointerList::IsEmpty() const
	{ return m_nCount == 0; }
inline void*& CPointerList::GetHead()
	{ assert(m_pNodeHead != 0);
		return m_pNodeHead->data; }
inline void* CPointerList::GetHead() const
	{ assert(m_pNodeHead != 0);
		return m_pNodeHead->data; }
inline void*& CPointerList::GetTail()
	{ assert(m_pNodeTail != 0);
		return m_pNodeTail->data; }
inline void* CPointerList::GetTail() const
	{ assert(m_pNodeTail != 0);
		return m_pNodeTail->data; }
inline POINTERPOS CPointerList::GetHeadPosition() const
	{ return (POINTERPOS) m_pNodeHead; }
inline POINTERPOS CPointerList::GetTailPosition() const
	{ return (POINTERPOS) m_pNodeTail; }
inline void*& CPointerList::GetNext(POINTERPOS& rpositon) // return *POINTERPOS++
	{ sNode* pNode = (sNode*) rpositon;
		rpositon = (POINTERPOS) pNode->pNext;
		return pNode->data; }
inline void* CPointerList::GetNext(POINTERPOS& rpositon) const // return *POINTERPOS++
	{ sNode* pNode = (sNode*) rpositon;
		rpositon = (POINTERPOS) pNode->pNext;
		return pNode->data; }
inline void*& CPointerList::GetPrev(POINTERPOS& rpositon) // return *POINTERPOS--
	{ sNode* pNode = (sNode*) rpositon;
		rpositon = (POINTERPOS) pNode->pPrev;
		return pNode->data; }
inline void* CPointerList::GetPrev(POINTERPOS& rpositon) const // return *POINTERPOS--
	{ sNode* pNode = (sNode*) rpositon;
		rpositon = (POINTERPOS) pNode->pPrev;
		return pNode->data; }
inline void*& CPointerList::GetAt(POINTERPOS positon)
	{ sNode* pNode = (sNode*) positon;
		return pNode->data; }
inline void* CPointerList::GetAt(POINTERPOS positon) const
	{ sNode* pNode = (sNode*) positon;
		return pNode->data; }
inline void CPointerList::SetAt(POINTERPOS pos, void* newElement)
	{ sNode* pNode = (sNode*) pos;
		pNode->data = newElement; }

inline void* CPointerList::GetIndex(int nIndex)
	{ sNode* pNode = (sNode*) FindIndex(nIndex);
	  if (pNode)		return pNode->data;
	  else				return 0; }


//typedef DWORD POINTER;

////template<typename T>
//class CLinkedList
//{
//public:
///*
//	CLinkedList(int nSize = 10)
//	{
//		m_pData = new T[m_nSize = nSize];
//		m_nCount = 0;
//	}
//	virtual ~CLinkedList()
//	{
//		delete [] m_pData;
//	}
//*/
//	CLinkedList();
//	~CLinkedList();
//
//public:
////	int AddHead(T* pNewData);
////	int AddTail(T* pNewData);
////	int RemoveHead();
////	int RemoveTail();
//	int AddHead(POINTER& pNewData);
//	int AddTail(POINTER& pNewData);
//	POINTER& RemoveHead();
//	POINTER& RemoveTail();
//	
//protected:
//	CLinkedList* m_pPrev; // Previous
//	CLinkedList* m_pNext;
//
////	T* m_pData;
//	int m_nSize;
//	int m_nCount;
//
//	PtrNode m_Node;
//};

template <typename T> class CTArray
{
public:
	CTArray(int nSize = 10);
	virtual ~CTArray();

	void ClearAll();
	T* GetItemPtr(int nIndex);
	T& GetItem(int nIndex);
	void SetItem(int nIndex, T item);
	int AddItem(T newItem);
	int RemoveItem(int nIndex);
//	int FindItem(T& item);

protected:
	void ResizeArray(int nNewSize);
	T* m_pArray;
	int m_nSize;
	int m_nCount;
};

//#endif //__AFX_H__

#endif // !defined(AFX_LINKEDLIST_H__27CB03E3_B4DD_459B_8155_EE2DEEF3C1E3__INCLUDED_)
