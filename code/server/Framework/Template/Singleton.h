#pragma once

#include "../type.h"
#include "../../Macro.h"

namespace Framework {		namespace Template {

template< class Type >
class Singleton
{
public:
	static CBOOL			Available();
	static Type *			GetInstance();

protected:
	void					install();
	void					uninstall();

private:
	static Type *			m_pInstance;

};

template< class Type >
Type *
Singleton<Type>::m_pInstance	= NULL;

template< class Type >
inline CBOOL
Singleton<Type>::Available()
{
	if ( m_pInstance != NULL )
	{
		return TRUE;
	}

	return FALSE;
}

template< class Type >
inline Type *
Singleton<Type>::GetInstance()
{
	assert( m_pInstance != NULL );

	return m_pInstance;
}

template< class Type >
inline void
Singleton<Type>::install()
{
	assert( m_pInstance == NULL );

	m_pInstance = static_cast<Type *>( this );
}

template< class Type >
inline void
Singleton<Type>::uninstall()
{
	assert( m_pInstance != NULL );

	m_pInstance = NULL;
}

} /* Template */			} /* Framework */