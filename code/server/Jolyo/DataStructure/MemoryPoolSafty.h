/*****************************************************************************************************************
	CMemoryPoolSafty

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
#include "MemoryPool.h"

namespace Common
{
	namespace DataStructure
	{
		template<class Type>
		class CMemoryPoolSafty : public CMemoryPool<Type>
		{
		private:
			CRITICAL_SECTION		m_cs;
		public:
			// 리소스들을 생성한다.
			virtual BOOL	Create( UINT nNumOfElements )
			{
				::InitializeCriticalSection( &m_cs );

				return CMemoryPool<Type>::Create( nNumOfElements );
			}
			// 리소스들을 제거한다.
			virtual void	Destroy()
			{
				CMemoryPool<Type>::Destroy();

				::DeleteCriticalSection( &m_cs );
			}

			// Memory Pool에서 객체를 할당 받는다.
			virtual Type*	Alloc()
			{
				::EnterCriticalSection( &m_cs );

				Type* pType;
				pType = CMemoryPool<Type>::Alloc();

				::LeaveCriticalSection( &m_cs );

				return pType;
			}

			// Memory Pool에 객체를 반환한다.
			virtual VOID	Free( Type* pElement )
			{
				::EnterCriticalSection( &m_cs );

				CMemoryPool<Type>::Free( pElement );

				::LeaveCriticalSection( &m_cs );
			}

		};// class CMemoryPoolSafty
	}// namespace DataStructure

}// namespace Common
