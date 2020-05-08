#ifndef SystemCollectionsIndex
#define SystemCollectionsIndex

#include "../../System.h"

namespace System {			namespace Collections {		namespace Index {

template< class Type >
struct Less
{
public:
	typedef	const Type
			CTYPE;

public:
	CINT	operator ()( CTYPE & rItemA, CTYPE & rItemB ) const;

};

template< class Type >
struct Loop
{
public:
	typedef	const Type
			CTYPE;

public:
	CINT	operator ()( CTYPE & rItemA, CTYPE & rItemB ) const;

};

template< class Type >
struct Heap
{
public:
	typedef	const Type
			CTYPE;

public:
	CINT	operator ()( CTYPE & rItemA, CTYPE & rItemB ) const;

};

template< class Type >
inline CINT
Less<Type>::operator ()( CTYPE & rItemA, CTYPE & rItemB ) const
{
	if ( rItemA < rItemB )
	{
		return -1;
	}

	if ( rItemB < rItemA )
	{
		return 1;
	}

	return 0;
}

template< class Type >
inline CINT
Loop<Type>::operator ()( CTYPE & rItemA, CTYPE & rItemB ) const
{
	CULONG ulCipher		= sizeof( Type ) * 8 - 1;
	CTYPE clsLowBit		= static_cast<CTYPE>( 1 );
	CTYPE clsHighBit	= clsLowBit << ulCipher;
	CTYPE clsBalance	= clsHighBit - 1;

	if ( rItemA < rItemB )
	{
		OPERATION;
	}
	else if ( ! (rItemB < rItemA) )
	{
		return 0;
	}
	else if ( (rItemA - rItemB) < clsBalance )
	{
		return 1;
	}
	else
	{
		return -1;
	}

	if ( (rItemB - rItemA) < clsBalance )
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

template< class Type >
inline CINT
Heap<Type>::operator ()( CTYPE & rItemA, CTYPE & rItemB ) const
{
	if ( & rItemA < & rItemB )
	{
		return -1;
	}

	if ( & rItemB < & rItemA )
	{
		return 1;
	}

	return 0;
}

} /* Index */				} /* Collections */			} /* System */

#endif