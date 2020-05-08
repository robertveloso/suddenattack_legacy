/*****************************************************************************************************************
	CMemoryPool

	작성자 : 강진협
	작성일 : 2004년 3월 5일

	설  명 :
		1. 빈번한 객체의 할당 및 반환이 발생하는 경우, 효율을 높이기 위해서 메모리 풀로써 관리한다.
		2. 최초 최대 객체수를 미리 할당하여, 필요시에 해당 메모리의 포인터를 반환해준다.
		3. 모든 클래스에 적용이 되도록 템플릿으로 제작되었다.
	사  용 :
		1. 메모리 풀에 등록할 클래스를 템플릿으로 선언한다.
		2. Create를 통해서 미리 생성할 객체의 수를 지정한다.
		3. Alloc을 통해서 메모리 풀에서 객체를 할당받는다.
		4. Free를 통해서 메모리 풀로 객체를 반환한다.
		5. 메모리 풀의 모든 사용이 끝나면 Destroy를 통해서 리소스를 해제한다.
*****************************************************************************************************************/


#pragma once

#include <Windows.h>

namespace GamehiSolution
{
	namespace Common
	{
		namespace DataStructure
		{
			template<class Type>
			class CMemoryPool
			{
			public:
				CMemoryPool(void)
				:	m_pElementsSetList	( NULL	)
				{
				}
				virtual ~CMemoryPool(void){}
			private:
				// 객체의 쌍방향 Linked List
				struct TagElement
				{
					Type		Element;
					TagElement*	pNext;
					TagElement*	pBefore;
					BOOL		bAlloc;
				};

				struct TagElementsSet
				{
					TagElement*		aElementsSet;
					TagElementsSet*	pNext;
				};

				// 객체의 모든 객체 배열
//				TagElement*	m_aElementsSet;
				TagElementsSet*
							m_pElementsSetList;

				int			m_nNumOfAlloc;

				// 사용하지 않는 overlapped list
				TagElement*	m_pUnusedElementsList;

				int			m_nNumOfElements;
				int			m_nNumOfElementsSet;

			public:
				// 리소스들을 생성한다.
				virtual BOOL	Create( UINT nNumOfElements )
				{
					//
					// TagElement 객체 배열을 생성한다.
					//
					m_nNumOfElements	= nNumOfElements;
					m_nNumOfElementsSet	= 1;

					m_pElementsSetList					= new TagElementsSet;
					m_pElementsSetList->pNext			= NULL;
					m_pElementsSetList->aElementsSet	= new TagElement[m_nNumOfElements];

					//
					// 모든 객체를 연결한다.
					//
					for( int i = 0; i < m_nNumOfElements; i++ )
					{
						if( i > 0 )
						{
							m_pElementsSetList->aElementsSet[i].pBefore	= &m_pElementsSetList->aElementsSet[i-1];
							m_pElementsSetList->aElementsSet[i-1].pNext	= &m_pElementsSetList->aElementsSet[i];
						}
						m_pElementsSetList->aElementsSet[i].bAlloc		= FALSE;
					}
					m_pElementsSetList->aElementsSet[0].pBefore					= NULL;
					m_pElementsSetList->aElementsSet[m_nNumOfElements-1].pNext	= NULL;

					m_pUnusedElementsList	= m_pElementsSetList->aElementsSet;
					m_nNumOfAlloc			= 0;
					return TRUE;

					/*

					m_aElementsSet = new TagElement[nNumOfElements];
					if( m_aElementsSet == NULL )
						return FALSE;

					//
					// 모든 객체를 연결한다.
					//
					for( int i = 0; i < m_nNumOfElements; i++ )
					{
						if( i > 0 )
						{
							m_aElementsSet[i].pBefore	= &m_aElementsSet[i-1];
							m_aElementsSet[i-1].pNext	= &m_aElementsSet[i];
						}
						m_aElementsSet[i].bAlloc		= FALSE;
					}
					m_aElementsSet[0].pBefore = NULL;
					m_aElementsSet[m_nNumOfElements-1].pNext = NULL;

					m_pUnusedElementsList	= m_aElementsSet;
					m_nNumOfAlloc			= 0;
					return TRUE;
					*/
				}
				// 리소스들을 제거한다.
				virtual void	Destroy()
				{
					while( m_pElementsSetList )
					{
						if( m_pElementsSetList->aElementsSet )
						{
							delete[] m_pElementsSetList->aElementsSet;
							m_pElementsSetList->aElementsSet = NULL;
						}
						TagElementsSet* pOld = m_pElementsSetList;
						m_pElementsSetList = m_pElementsSetList->pNext;

						delete pOld;
					}
					/*
					if( m_aElementsSet )
					{
						delete[] m_aElementsSet;
						m_aElementsSet = NULL;
					}
					*/
				}

				// Memory Pool에서 객체를 할당 받는다.
				virtual Type*	Alloc()
				{
					if( m_pUnusedElementsList == NULL )
					{
						TagElementsSet* pSet	= new TagElementsSet;
						pSet->pNext				= NULL;
						pSet->aElementsSet		= new TagElement[m_nNumOfElements];

						//
						// 모든 객체를 연결한다.
						//
						for( int i = 0; i < m_nNumOfElements; i++ )
						{
							if( i > 0 )
							{
								pSet->aElementsSet[i].pBefore	= &pSet->aElementsSet[i-1];
								pSet->aElementsSet[i-1].pNext	= &pSet->aElementsSet[i];
							}
							pSet->aElementsSet[i].bAlloc		= FALSE;
						}
						pSet->aElementsSet[0].pBefore					= NULL;
						pSet->aElementsSet[m_nNumOfElements-1].pNext	= NULL;

						m_pElementsSetList = pSet;

						m_pUnusedElementsList	= pSet->aElementsSet;

						m_nNumOfElementsSet++;
					}

					TagElement* pTagElement;

					pTagElement = m_pUnusedElementsList;
					m_pUnusedElementsList = m_pUnusedElementsList->pNext;
					if( m_pUnusedElementsList )
						m_pUnusedElementsList->pBefore = NULL;

//					ASSERT( !pTagElement->bAlloc );
					pTagElement->bAlloc = TRUE;

					m_nNumOfAlloc++;

					return (Type*)&(pTagElement->Element);
				}

				// Memory Pool에 객체를 반환한다.
				virtual VOID	Free( Type* pElement )
				{
					TagElement* pTagElement = (TagElement*)pElement;

					if( !pTagElement->bAlloc )
					{
						int a = 3;
					}
//					ASSERT( pTagElement->bAlloc );

					pTagElement->pNext					= m_pUnusedElementsList;
					pTagElement->pBefore				= NULL;
					if( m_pUnusedElementsList )
						m_pUnusedElementsList->pBefore	= pTagElement;
					m_pUnusedElementsList				= pTagElement;

					m_nNumOfAlloc--;

					pTagElement->bAlloc = FALSE;
				}

				int		GetAllocatedSize()
				{
					return m_nNumOfAlloc;
				}

				// Pool Size를 반환한다.
				int		GetPoolSize()
				{
					return m_nNumOfElements * m_nNumOfElementsSet;
				}

			};// class CMemoryPool
		}// namespace DataStructure

	}// namespace Common

}// namespace GamehiSolution