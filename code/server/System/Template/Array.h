#ifndef SystemTemplateArray
#define	SystemTemplateArray

namespace System {			namespace Template {

template< ULONG s_ulLimit, class Type >
struct Array
{
public:
	typedef					const Type
							CTYPE;
	typedef					const Array<s_ulLimit, Type>
							CSELF;

public:
							Array();
	virtual					~Array();

	CULONG					GetLimit() const;

	Type &					operator []( CULONG ulIndex );
	CTYPE &					operator []( CULONG ulIndex ) const;

private:
	Type					m_pArray[ s_ulLimit ];

};

template< ULONG s_ulLimit, class Type >
Array<s_ulLimit,Type>::Array()
{
	aSsErT( s_ulLimit > 0 );
}

template< ULONG s_ulLimit, class Type >
Array<s_ulLimit,Type>::~Array()
{
	aSsErT( s_ulLimit > 0 );
}

template< ULONG s_ulLimit, class Type >
inline CULONG
Array<s_ulLimit,Type>::GetLimit() const
{
	aSsErT( s_ulLimit > 0 );

	return s_ulLimit;
}

template< ULONG s_ulLimit, class Type >
inline Type &
Array<s_ulLimit,Type>::operator []( CULONG ulIndex )
{
	aSsErT( s_ulLimit > 0 );
	Assert( ulIndex < s_ulLimit );

	return m_pArray[ ulIndex ];
}

template< ULONG s_ulLimit, class Type >
inline const Type &
Array<s_ulLimit,Type>::operator []( CULONG ulIndex ) const
{
	aSsErT( s_ulLimit > 0 );
	Assert( ulIndex < s_ulLimit );

	return m_pArray[ ulIndex ];
}

} /* Template */				} /* System */

#endif