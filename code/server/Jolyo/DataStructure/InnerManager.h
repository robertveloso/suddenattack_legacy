#pragma once

#include <Windows.h>
#include "InnerObject.h"

namespace Common
{
	namespace DataStructure
	{
		template<class Type>
		class CInnerManager
		{
		public:
			CInnerManager(VOID)
				:	m_nNumOfInnerObject	( -1	)

				,	m_pHead				( NULL	)
				,	m_pRet				( NULL	)
			{
			}

			~CInnerManager(VOID)
			{
			}

		private:
			INT		m_nNumOfInnerObject;	// 삽입된 InnerObject의 개수

			CInnerObject<Type>*
				m_pHead;

			CInnerObject<Type>*
				m_pRet;

		public:
			VOID	Init()
			{
				m_nNumOfInnerObject = 0;

				m_pHead	= NULL;
				m_pRet	= NULL;
			}

			INT		GetNumOfInnerObject()
			{
				return m_nNumOfInnerObject;
			}

			// 룸이 비어있는지를 검사한다.
			BOOL	IsEmpty()
			{
				return( m_nNumOfInnerObject == 0 );
			}


			// pInnerObject를 추가한다. ( 배열의 제일 위에 추가가 된다. )
			BOOL	Add		( CInnerObject<Type>* pInnerObject )
			{
				pInnerObject->SetBefore( NULL );
				pInnerObject->SetNext( m_pHead );
				if( m_pHead )
					m_pHead->SetBefore( pInnerObject );
				m_pHead = pInnerObject;
				m_nNumOfInnerObject++;
				pInnerObject->SetManager( (void*)this );

				return TRUE;
			}

			// pInnerObject를 제거한다.
			BOOL	Remove	( CInnerObject<Type>* pInnerObject )
			{
				CInnerManager * pManager = (CInnerManager *)pInnerObject->GetManager();
				if( pManager != this )
				{
					return FALSE;
				}
				CInnerObject<Type>*		pBeforeObject;
				CInnerObject<Type>*		pNextObject;
				pBeforeObject	= pInnerObject->GetBefore	();
				pNextObject		= pInnerObject->GetNext		();

				if( pBeforeObject == NULL && pInnerObject != m_pHead )
					return FALSE;

				if( pNextObject		)
					pNextObject		->SetBefore	( pBeforeObject );

				if( pInnerObject == m_pHead )
				{
					m_pHead = pNextObject;
				}
				else
				{
					pBeforeObject	->SetNext	( pNextObject );
				}
				m_nNumOfInnerObject--;
				pInnerObject->SetNext	( NULL );
				pInnerObject->SetBefore	( NULL );
				pInnerObject->SetManager( NULL );
				return TRUE;
			}

			// pInnerObject를 배열의 제일 위에 추가한다.
			//BOOL	Push	( CInnerObject<Type>* pInnerObject )
			//{
			//	pInnerObject->SetBefore( NULL );
			//	pInnerObject->SetNext( m_pHead );
			//	if( m_pHead )
			//		m_pHead->SetBefore( pInnerObject );
			//	m_pHead = pInnerObject;
			//	m_nNumOfInnerObject++;
			//	return TRUE;
			//}

			// 제일 위의 pInnerObject를 꺼내어 반환받는다.
			//		BOOL	Pop		( CInnerObject<Type>* &pOutInnerObject );

			//		BOOL	Pop		( CInnerObject<Type>** pOutInnerObject );

			BOOL	Pop		( Type* &pOutElement )
			{
				if( m_pHead == NULL )
					return FALSE;

				CInnerObject<Type>* pInnerObject;
				pInnerObject	= m_pHead;
				pOutElement		= m_pHead->GetElement();

				if( pInnerObject->GetManager() != this )
				{
					OutputDebugString( "!!!!!!!!!!!\r\nPing Error\r\n!!!!!!!!!!!\r\n" );
					m_pHead		= NULL;
					return FALSE;
				}
				m_pHead = m_pHead->GetNext();
				if( m_pHead )
					m_pHead->SetBefore( NULL );
				m_nNumOfInnerObject--;
				pInnerObject->SetNext	( NULL );
				pInnerObject->SetBefore	( NULL );
				pInnerObject->SetManager( NULL );
				return TRUE;
			}

			//BOOL	Pop		( Type** ppElement )
			//{
			//	if( m_pHead == NULL )
			//		return FALSE;

			//	CInnerObject<Type>* pInnerObject;
			//	ppElement = &m_pHead->GetElement();
			//	m_pHead = m_pHead->GetNext();
			//	if( m_pHead )
			//		m_pHead->SetBefore( NULL );
			//	m_nNumOfInnerObject--;
			//	pInnerObject->SetNext	( NULL );
			//	pInnerObject->SetBefore	( NULL );
			//	return TRUE;
			//}

			// 첫번째 InnerObject의 Element를 가져온다.
			Type*	GetFirstInnerObject()
			{
				m_pRet = m_pHead;
				if( m_pRet == NULL )
					return NULL;

				return m_pRet->GetElement();
			}

			// 다음 InnerObject를 가져온다.
			Type*	GetNextInnerObject()
			{
				m_pRet = m_pRet->GetNext();
				if( m_pRet == NULL )
					return NULL;

				return m_pRet->GetElement();
			}
		};// class CInnerManager

	}// namespace DataStructure

}// namespace Common
