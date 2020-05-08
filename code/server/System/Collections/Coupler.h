#pragma once

namespace System {			namespace Collections {

template< class TypeA, class TypeB >
struct Coupler
{
public:
	typedef		const TypeA
				CTYPEA;
	typedef		const TypeB
				CTYPEB;
	typedef		const Coupler<TypeA, TypeB>
				CSELF;

public:
	Coupler();
	Coupler( CTYPEA & rItemA );
	Coupler( CTYPEA & rItemA, CTYPEB & rItemB );
	BLANK		~Coupler();

	BLANK		operator TypeB();
	BLANK		operator CTYPEB() const;

	CBOOL		operator <( CSELF & rRightHandSide ) const;
	CBOOL		operator >( CSELF & rRightHandSide ) const;

	CBOOL		operator ==( CSELF & rRightHandSide ) const;
	CBOOL		operator !=( CSELF & rRightHandSide ) const;

	CBOOL		operator <=( CSELF & rRightHandSide ) const;
	CBOOL		operator >=( CSELF & rRightHandSide ) const;

public:
	TypeA		clsItemA;
	TypeB		clsItemB;

};

template< class TypeA, class TypeB >
Coupler<TypeA,TypeB>::Coupler()
{
}

template< class TypeA, class TypeB >
Coupler<TypeA,TypeB>::Coupler( CTYPEA & rItemA )
{
	clsItemA = rItemA;
}

template< class TypeA, class TypeB >
Coupler<TypeA,TypeB>::Coupler( CTYPEA & rItemA, CTYPEB & rItemB )
{
	clsItemA	= rItemA;
	clsItemB	= rItemB;
}

template< class TypeA, class TypeB >
Coupler<TypeA,TypeB>::~Coupler()
{
}

template< class TypeA, class TypeB >
Coupler<TypeA,TypeB>::operator TypeB()
{
	return clsItemB;
}

template< class TypeA, class TypeB >
Coupler<TypeA,TypeB>::operator const TypeB() const
{
	return clsItemB;
}

template< class TypeA, class TypeB >
inline CBOOL
Coupler<TypeA,TypeB>::operator <( CSELF & rRightHandSide ) const
{
	if ( clsItemA < rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline CBOOL
Coupler<TypeA,TypeB>::operator >( CSELF & rRightHandSide ) const
{
	if ( clsItemA > rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline CBOOL
Coupler<TypeA,TypeB>::operator ==( CSELF & rRightHandSide ) const
{
	if ( clsItemA == rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline CBOOL
Coupler<TypeA,TypeB>::operator !=( CSELF & rRightHandSide ) const
{
	if ( clsItemA != rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline CBOOL
Coupler<TypeA,TypeB>::operator <=( CSELF & rRightHandSide ) const
{
	if ( clsItemA <= rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline CBOOL
Coupler<TypeA,TypeB>::operator >=( CSELF & rRightHandSide ) const
{
	if ( clsItemA >= rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

} /* Collections */			} /* System */