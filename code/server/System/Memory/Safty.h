#ifndef SystemMemorySafty
#define SystemMemorySafty

#include "../../System.h"

namespace System {			namespace Memory {

template< class TypeAbst, class TypeProv, class Memory >
class Safty
{
public:
	template< class NewType >
	struct					REBIND
	{
	typedef					Safty<NewType, NewType, Memory>
							OTHER;
	};

public:
							Safty();
	virtual					~Safty();

	CULONG					GetCount() const;
	CULONG					GetLimit() const;

	TypeAbst *				Capture( TypeAbst * pItem = NULL, CULONG ulSize = 1 );
	CULONG					Release( TypeAbst * pItem, CULONG ulSize = 1 );

private:
	typedef					typename Memory::REBIND<TypeProv>::OTHER
							CORE;
	typedef					Threads::IMutex
							IMUTEX;

private:
	IMUTEX *				m_pMutex;
	CORE					m_clsCore;

};

template< class TypeAbst, class TypeProv, class Memory >
Safty<TypeAbst,TypeProv,Memory>::Safty()
{
	m_pMutex = IMUTEX::IFACTORY()->Create();
	ASSERT( m_pMutex != NULL );
}

template< class TypeAbst, class TypeProv, class Memory >
Safty<TypeAbst,TypeProv,Memory>::~Safty()
{
	IMUTEX::IFACTORY()->Destroy( m_pMutex );
}

template< class TypeAbst, class TypeProv, class Memory >
inline CULONG
Safty<TypeAbst,TypeProv,Memory>::GetCount() const
{
	m_pMutex->Lock();

	CULONG ulRetCount = m_clsCore.GetCount();

	m_pMutex->Unlock();

	return ulRetCount;
}

template< class TypeAbst, class TypeProv, class Memory >
inline CULONG
Safty<TypeAbst,TypeProv,Memory>::GetLimit() const
{
	m_pMutex->Lock();

	CULONG ulRetLimit = m_clsCore.GetLimit();

	m_pMutex->Unlock();

	return ulRetLimit;
}

template< class TypeAbst, class TypeProv, class Memory >
inline TypeAbst *
Safty<TypeAbst,TypeProv,Memory>::Capture( TypeAbst * pItemA, CULONG ulSize )
{
	TypeProv * pItemB = static_cast<TypeProv *>( pItemA );

	m_pMutex->Lock();

	TypeAbst * pRetItem = m_clsCore.Capture( pItemB, ulSize );

	m_pMutex->Unlock();

	return pRetItem;
}

template< class TypeAbst, class TypeProv, class Memory >
inline CULONG
Safty<TypeAbst,TypeProv,Memory>::Release( TypeAbst * pItemA, CULONG ulSize )
{
	if ( pItemA == NULL )
	{
		return 0;
	}

	TypeProv * pItemB = static_cast<TypeProv *>( pItemA );

	m_pMutex->Lock();

	CULONG ulRetSize = m_clsCore.Release( pItemB, ulSize );

	m_pMutex->Unlock();

	return ulRetSize;
}

} /* Memory */				} /* System */

#endif