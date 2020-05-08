/*****************************************************************************************************************
*****************************************************************************************************************/


#pragma once

#include "MemoryPoolSafty.h"

#include <Windows.h>

namespace Common
{
	namespace DataStructure
	{
		struct TagListNode
		{
			ULONG_PTR		pElement;
			DWORD			dwIndex;	// Sort를 위한 Index
			TagListNode*	pNext;
			TagListNode*	pBefore;
		};

		template<class Type>
		class CListEx
		{
		public:
			CListEx(VOID)
				:	m_nNumOfElements	( 0		)
				,	m_pHead				( NULL	)
				,	m_pRear				( NULL	)
			{
			}
			virtual ~CListEx(VOID){}

		protected:
			TagListNode*		m_pHead;
			TagListNode*		m_pRear;

			INT				m_nNumOfElements;
			static CMemoryPoolSafty<TagListNode>
							g_ElementPool;

		public:
			BOOL		Create	( INT nListSize )
			{
				m_nNumOfElements	= 0;
				m_pHead				= NULL;
				m_pRear				= NULL;

				if( g_ElementPool.IsCreated() )
					return TRUE;
				else
					return g_ElementPool.Create( nListSize );
			}
			VOID		Destroy	()
			{
				if( g_ElementPool.IsCreated() )
					g_ElementPool.Destroy();
			}

			VOID		SortAdd	( Type element, DWORD dwIndex )
			{
				TagListNode* pElement = g_ElementPool.Alloc();
				pElement->pNext		= pElement->pBefore = NULL;
				pElement->pElement	= reinterpret_cast<ULONG_PTR>( element );
				pElement->dwIndex	= dwIndex;

				if( m_nNumOfElements == 0 )
				{
					m_pRear = m_pHead = pElement;
				}
				else
				{
					TagListNode* pInsert;

					// 우선순위 패킷인 경우
					if( dwIndex == 0 )
					{
						pInsert = m_pRear;
					}
					else
					{
						pInsert	= m_pHead;
						while( pInsert )
						{
							if( pInsert->dwIndex < dwIndex )
							{
								if( ( pInsert->dwIndex == 0 && pInsert == m_pRear ) ||
									( pInsert->dwIndex > 0 ) )
									break;
							}
							pInsert = pInsert->pNext;
						}
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
				TagListNode* pElement = g_ElementPool.Alloc();
				pElement->pNext		= pElement->pBefore = NULL;
				pElement->pElement	= reinterpret_cast<ULONG_PTR>( element );
				pElement->dwIndex	= 0;

				if( m_nNumOfElements == 0 )
				{
					if( m_pRear != NULL ||
						m_pHead != NULL )
						int a = 3;
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
				TagListNode* pElement = g_ElementPool.Alloc();
				pElement->pNext		= pElement->pBefore = NULL;
				pElement->pElement	= reinterpret_cast<ULONG_PTR>( element );
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
				if( m_pRear == NULL )
					return NULL;

				Type	type;
				TagListNode*	pElement;
				pElement = m_pRear;

				type	= reinterpret_cast<Type>( pElement->pElement );
				m_pRear	= pElement->pBefore;
				if( m_pRear	== NULL )
					m_pHead	= NULL;
				else
					m_pRear->pNext = NULL;

				g_ElementPool.Free( pElement );

				m_nNumOfElements--;

				if( m_nNumOfElements < 0 )
					int a = 3;

				return type;
			}

			Type		Pop		()
			{
				if( m_pHead == NULL )
					return NULL;

				Type	type;
				TagListNode*	pElement;
				pElement = m_pHead;

				type	= reinterpret_cast<Type>( pElement->pElement );
				m_pHead	= pElement->pNext;
				if( m_pHead	== NULL )
					m_pRear	= NULL;

				g_ElementPool.Free( pElement );

				m_nNumOfElements--;

				return type;
			}

			// Element를 꺼내지는 않고, 참조만 반환한다.
			Type		GetHeadElement()
			{
				if( m_pHead == NULL )
					return NULL;

				TagListNode*	pElement;
				pElement = m_pHead;
				return reinterpret_cast<Type>( pElement->pElement );
			}
			// Element를 꺼내지는 않고, 참조만 반환한다.
			Type		GetRearElement()
			{
				if( m_pRear == NULL )
					return NULL;

				TagListNode*	pElement;
				pElement = m_pRear;
				return reinterpret_cast<Type>( pElement->pElement );
			}

			BOOL		IsEmpty()
			{
				return m_nNumOfElements == 0;
			}

			INT			GetElementsCount()
			{
				return m_nNumOfElements;
			}
		};// class CListEx

		template<class Type>
			CMemoryPoolSafty<TagListNode>
			CListEx<Type>::g_ElementPool;
	}// namespace DataStructure

}// namespace Common
