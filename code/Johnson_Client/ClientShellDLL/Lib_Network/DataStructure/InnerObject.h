#pragma once

#include <Windows.h>

namespace GamehiSolution
{
	namespace Common
	{
		namespace DataStructure
		{
			template<class Type>
			class CInnerObject
			{
			public:
//				CInnerObject	( Type* pElement )
//					:	m_pElement	( pElement )
				CInnerObject	()
					:	m_pElement	( NULL )
				{}
				~CInnerObject(VOID){}

			private:
				INT		m_nInnerIndex;
				Type*	m_pElement;


				CInnerObject<Type>*	m_pNext;
				CInnerObject<Type>*	m_pBefore;

			public:
				VOID	Init( Type* pElement )
				{
					m_pElement = pElement;
				}

				INT		GetInnerIndex()
				{
					return m_nInnerIndex;
				}

				VOID	SetInnderIndex( INT nInnerIndex )
				{
					m_nInnerIndex = nInnerIndex;
				}

				Type*	GetElement()
				{
					return m_pElement;
				}

				CInnerObject<Type>*	GetNext()
				{
					return	m_pNext;
				}
				CInnerObject<Type>*	GetBefore()
				{
					return	m_pBefore;
				}

				VOID				SetNext( CInnerObject<Type>* pInnerObject )
				{
					m_pNext		= pInnerObject;
				}
				VOID				SetBefore( CInnerObject<Type>* pInnerObject )
				{
					m_pBefore	= pInnerObject;
				}
			};// class CInnerObject

		}// namespace DataStructure

	}// namespace Common

}// namespace GamehiSolution