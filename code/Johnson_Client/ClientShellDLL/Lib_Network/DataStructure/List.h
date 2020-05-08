/*****************************************************************************************************************
*****************************************************************************************************************/


#include "MemoryPool.h"

#pragma once

#include <Windows.h>

namespace GamehiSolution
{
	namespace Common
	{
		namespace DataStructure
		{
			template<class Type>
			class CList
			{
			public:
				CList(VOID)
					:	m_nNumOfElements	( 0		)
					,	m_pHead				( NULL	)
					,	m_pRear				( NULL	)
				{
				}
				virtual ~CList(VOID){}

			protected:
				// 객체의 쌍방향 Linked List
				struct TagElement
				{
					Type		Element;
					DWORD		dwIndex;	// Sort를 위한 Index
					TagElement*	pNext;
					TagElement*	pBefore;
				};

				TagElement*		m_pHead;
				TagElement*		m_pRear;

				INT				m_nNumOfElements;
				CMemoryPool<TagElement>
								m_ElementPool;

			public:
				BOOL		Create	( INT nListSize )
				{
					m_nNumOfElements	= 0;
					m_pHead				= NULL;
					m_pRear				= NULL;
					return m_ElementPool.Create( nListSize );
				}
				VOID		Destroy	()
				{
					m_ElementPool.Destroy();
				}

				VOID		SortAdd	( Type element, DWORD dwIndex )
				{
					TagElement* pElement = m_ElementPool.Alloc();
					pElement->pNext		= pElement->pBefore = NULL;
					pElement->Element	= element;
					pElement->dwIndex	= dwIndex;

					if( m_nNumOfElements == 0 )
					{
						m_pRear = m_pHead = pElement;
					}
					else
					{
						TagElement* pInsert	= m_pHead;
						while( pInsert )
						{
							if( pInsert->dwIndex < dwIndex )
								break;
							pInsert = pInsert->pNext;
						}

						if( pInsert == m_pHead )
						{
							pElement->pNext		= m_pHead;
							m_pHead->pBefore	= pElement;

							m_pHead = pElement;
						}
						else if( pInsert == NULL )
						{
							m_pRear->pNext		= pElement;
							pElement->pBefore	= m_pRear;

							m_pRear = pElement;
						}
						else
						{
							pElement->pBefore	= pInsert->pBefore;
							pInsert->pBefore->pNext	= pElement;

							pElement->pNext			= pInsert;
							pInsert->pBefore		= pElement;
						}
					}

					m_nNumOfElements++;
				}

				VOID		Add		( Type element )
				{
					TagElement* pElement = m_ElementPool.Alloc();
					pElement->pNext		= pElement->pBefore = NULL;
					pElement->Element	= element;
					pElement->dwIndex	= 0;

					if( m_nNumOfElements == 0 )
					{
						m_pRear = m_pHead = pElement;
					}
					else
					{
						pElement->pNext		= m_pHead;
						m_pHead->pBefore	= pElement;

						m_pHead				= pElement;
					}

					m_nNumOfElements++;
				}

				// 제일 먼저 Dequeue되도록 Add한다.
				VOID		AddFirst	( Type element )
				{
					TagElement* pElement = m_ElementPool.Alloc();
					pElement->pNext		= pElement->pBefore = NULL;
					pElement->Element	= element;
					pElement->dwIndex	= 0;

					if( m_nNumOfElements == 0 )
					{
						m_pRear = m_pHead = pElement;
					}
					else
					{
						m_pRear->pNext		= pElement;
						pElement->pBefore	= m_pRear;
						m_pRear				= pElement;
					}

					m_nNumOfElements++;
				}

				Type		Dequeue	()
				{
					Type	type;
					TagElement*	pElement;
					pElement = m_pRear;

					if( pElement == NULL )
						return NULL;

					type	= pElement->Element;
					m_pRear	= pElement->pBefore;
					if( m_pRear	== NULL )
						m_pHead	= NULL;
					m_ElementPool.Free( pElement );

					m_nNumOfElements--;

					return type;
				}

				Type		Pop		()
				{
					Type	type;
					TagElement*	pElement;
					pElement = m_pHead;

					if( m_pHead == NULL )
						return NULL;

					type	= pElement->Element;
					m_pHead	= pElement->pNext;
					if( m_pHead	== NULL )
						m_pRear	= NULL;

					m_ElementPool.Free( pElement );

					m_nNumOfElements--;

					return type;
				}

				// Element를 꺼내지는 않고, 참조만 반환한다.
				Type		GetHeadElement()
				{
					if( m_pHead == NULL )
						return NULL;

					TagElement*	pElement;
					pElement = m_pHead;
					return pElement->Element;
				}
				// Element를 꺼내지는 않고, 참조만 반환한다.
				Type		GetRearElement()
				{
					if( m_pRear == NULL )
						return NULL;

					TagElement*	pElement;
					pElement = m_pRear;
					return pElement->Element;
				}

				BOOL		IsEmpty()
				{
					return m_nNumOfElements == 0;
				}

				INT			GetElementsCount()
				{
					return m_nNumOfElements;
				}
			};// class CList
		}// namespace DataStructure

	}// namespace Common

}// namespace GamehiSolution