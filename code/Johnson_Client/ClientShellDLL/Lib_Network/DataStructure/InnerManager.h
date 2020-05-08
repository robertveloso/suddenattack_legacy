#pragma once

#include <Windows.h>
#include "InnerObject.h"

namespace GamehiSolution
{
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
				INT		m_nNumOfInnerObject;	// ���Ե� InnerObject�� ����

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

				// ���� ����ִ����� �˻��Ѵ�.
				BOOL	IsEmpty()
				{
					return( m_nNumOfInnerObject == 0 );
				}


				// pInnerObject�� �߰��Ѵ�. ( �迭�� ���� ���� �߰��� �ȴ�. )
				BOOL	Add		( CInnerObject<Type>* pInnerObject )
				{
					pInnerObject->SetBefore( NULL );
					pInnerObject->SetNext( m_pHead );
					if( m_pHead )
						m_pHead->SetBefore( pInnerObject );
					m_pHead = pInnerObject;
					m_nNumOfInnerObject++;
					return TRUE;
				}

				// pInnerObject�� �����Ѵ�.
				BOOL	Remove	( CInnerObject<Type>* pInnerObject )
				{
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
					return TRUE;
				}

				// pInnerObject�� �迭�� ���� ���� �߰��Ѵ�.
				BOOL	Push	( CInnerObject<Type>* pInnerObject )
				{
					pInnerObject->SetBefore( NULL );
					pInnerObject->SetNext( m_pHead );
					if( m_pHead )
						m_pHead->SetBefore( pInnerObject );
					m_pHead = pInnerObject;
					m_nNumOfInnerObject++;
					return TRUE;
				}

				// ���� ���� pInnerObject�� ������ ��ȯ�޴´�.
				//		BOOL	Pop		( CInnerObject<Type>* &pOutInnerObject );

				//		BOOL	Pop		( CInnerObject<Type>** pOutInnerObject );

				BOOL	Pop		( Type* &pOutElement )
				{
					if( m_pHead == NULL )
						return FALSE;

					pOutElement = m_pHead->GetElement();
					m_pHead = m_pHead->GetNext();
					if( m_pHead )
						m_pHead->SetBefore( NULL );
					m_nNumOfInnerObject--;
					return TRUE;
				}

				BOOL	Pop		( Type** ppElement )
				{
					if( m_pHead == NULL )
						return FALSE;

					ppElement = &m_pHead->GetElement();
					m_pHead = m_pHead->GetNext();
					if( m_pHead )
						m_pHead->SetBefore( NULL );
					m_nNumOfInnerObject--;
					return TRUE;
				}

				// ù��° InnerObject�� Element�� �����´�.
				Type*	GetFirstInnerObject()
				{
					m_pRet = m_pHead;
					if( m_pRet == NULL )
						return NULL;

					return m_pRet->GetElement();
				}

				// ���� InnerObject�� �����´�.
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

}// namespace GamehiSolution