#pragma once
#include <Windows.h>

#include "stdio.h"

#include "..\..\DataStructure\InnerManager.h"

namespace Session
{
	namespace Gate
	{
		template<class Type>
		class CAliveManager
		{
		public:
			CAliveManager		(void)
				:	m_paAliveList	( NULL	)
				,	m_pCloseWaitList( NULL	)
			{}
			~CAliveManager(void){}

		private:
			int		m_nTimeoutTickCount;

			Common::DataStructure::CInnerManager<Type>*
				m_pCloseWaitList;
			Common::DataStructure::CInnerManager<Type>**
				m_paAliveList;	//

		public:
			BOOL	Create( int nTimeoutTickCount ) //等候进入游戏的角色链，第0链为没有进入任何channel中的角色
			{
				m_paAliveList		= new Common::DataStructure::CInnerManager<Type>* [nTimeoutTickCount];
				if( m_paAliveList == NULL )
					return FALSE;

				for( int i = 0; i < nTimeoutTickCount; i++ )
				{
					m_paAliveList[i] = new Common::DataStructure::CInnerManager<Type>;
					m_paAliveList[i]->Init();
				}

				m_pCloseWaitList = new Common::DataStructure::CInnerManager<Type>;
				m_pCloseWaitList->Init();

				m_nTimeoutTickCount	= nTimeoutTickCount;
				return TRUE;
			}

			void	Destroy()
			{
				if( m_aAliveList )
				{
					for( int i = 0; i < m_nTimeoutTickCount; i++ )
					{
						if( m_paAliveList[i] )
						{
							m_paAliveList[i]->Destroy();
							delete m_paAliveList[i];
						}
						m_paAliveList[i] = NULL;
					}
					delete[] m_paAliveList;
				}
				m_paAliveList = NULL:

				if( m_pCloseWaitList )
					delete m_pCloseWaitList;
				m_pCloseWaitList = NULL;
			}

			// 鸥烙 酒眶捞 利侩瞪 荐 乐霸 府胶飘俊 眠啊茄促.
			BOOL	Add		( Common::DataStructure::CInnerObject<Type>* pInnerObject )
			{
				m_paAliveList[0]->Add( pInnerObject );
				return TRUE;
			}

			// 鸥烙 酒眶 府胶飘俊辑 力芭茄促.
			BOOL	Remove	( Common::DataStructure::CInnerObject<Type>* pInnerObject )
			{
				BOOL bRemove = FALSE;
				for( int i = 0; i < m_nTimeoutTickCount; i++ )
				{
					bRemove = m_paAliveList[i]->Remove( pInnerObject );
					if( bRemove == TRUE )
						break;
				}
				if( bRemove == FALSE )
				{
					bRemove = m_pCloseWaitList->Remove( pInnerObject );
				}

				return bRemove;
			}

			void	RepositeOnAliveList( Common::DataStructure::CInnerObject<Type>* pInnerObject )
			{
				BOOL bRemove = FALSE;
				for( int i = 0; i < m_nTimeoutTickCount; i++ )
				{
					bRemove = m_paAliveList[i]->Remove( pInnerObject );
					if( bRemove == TRUE )
						break;
				}
				if( bRemove )
					m_paAliveList[0]->Add( pInnerObject );
			}

			// 鸥烙酒眶 惯积矫 Close 贸府且 府胶飘甫 积己茄促.
			void	DoTimeoutProcess()
			{
				Common::DataStructure::CInnerManager<Type>*
					pManager;
				pManager = m_pCloseWaitList;
				for( int i = m_nTimeoutTickCount - 1; i >= 0; i-- )
				{
					if( i == m_nTimeoutTickCount - 1 )
						m_pCloseWaitList = m_paAliveList[i];
					else
						m_paAliveList[i+1] = m_paAliveList[i];
				}
				m_paAliveList[0] = pManager;

				//for( int i = 0; i < m_nTimeoutTickCount; i ++ )
				//{
				//	char strLog[100];
				//	int a = (int)m_paAliveList[ i ];
				//	sprintf( strLog, "InnerManager %d : %d %d\r\n", i, a, (int)m_paAliveList[ i ]->GetFirstInnerObject() );
				//	OutputDebugString( strLog );
				//}
				//char strLog[100];
				//int a = (int)m_pCloseWaitList;
				//sprintf( strLog, "CloseManager : %d %d\r\n", a, (int)m_pCloseWaitList->GetFirstInnerObject() );
				//OutputDebugString( strLog );
			}

			BOOL	PopTimeoutObject( Type* &pOutElement )
			{
				return m_pCloseWaitList->Pop( pOutElement );
			}
		};// class CAliveManager
	}// namespace Gate
}// namespace Session
