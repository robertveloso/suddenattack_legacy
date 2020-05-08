#pragma once

namespace Framework {		namespace Template {

template<UINT i_uiLimit, class Type>
struct Array
{
public:
	typedef					const Type
							CTYPE;
	typedef					const Array<i_uiLimit, Type>
							CSELF;

public:
							Array();
	virtual					~Array();

	CULONG					GetLimit() const;

	Type &					operator []( CULONG ulIndex );
	CTYPE &					operator []( CULONG ulIndex ) const;

private:
	Type					pArray[ i_uiLimit ];

};

template<UINT i_uiLimit, class Type>
Array<i_uiLimit,Type>::Array()
{
	aSsErT( i_uiLimit > 0 );
}

template<UINT i_uiLimit, class Type>
Array<i_uiLimit,Type>::~Array()
{
	aSsErT( i_uiLimit > 0 );
}

template<UINT i_uiLimit, class Type>
inline CULONG
Array<i_uiLimit,Type>::GetLimit() const
{
	aSsErT( i_uiLimit > 0 );

	return i_uiLimit;
}

template<UINT i_uiLimit, class Type>
inline Type &
Array<i_uiLimit,Type>::operator []( CULONG ulIndex )
{
	aSsErT( i_uiLimit > 0 );
	assert( ulIndex < i_uiLimit );

	return pArray[ ulIndex ];
}

template<UINT i_uiLimit, class Type>
inline const Type &
Array<i_uiLimit,Type>::operator []( CULONG ulIndex ) const
{
	aSsErT( i_uiLimit > 0 );
	assert( ulIndex < i_uiLimit );

	return pArray[ ulIndex ];
}


} /* Template */			} /* Framework */