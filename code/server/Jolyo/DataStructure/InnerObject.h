#pragma once

#include <Windows.h>

namespace Common
{
	namespace DataStructure
	{
		template<class Type>
		class CInnerObject //以m_pElement组成的接点
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

			void * m_pManager;

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
			void * GetManager()
			{
				return m_pManager;
			}

			VOID				SetNext( CInnerObject<Type>* pInnerObject )
			{
				m_pNext		= pInnerObject;
			}
			VOID				SetBefore( CInnerObject<Type>* pInnerObject )
			{
				m_pBefore	= pInnerObject;
			}
			VOID				SetManager( void * pManager )
			{
				m_pManager	= pManager;
			}
		};// class CInnerObject

	}// namespace DataStructure

}// namespace Common
