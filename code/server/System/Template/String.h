#ifndef	SystemTemplateString
#define	SystemTemplateString

namespace System {			namespace Template {

template< ULONG s_ulLimit >
class String
{
public:
	typedef					const String<s_ulLimit>
							CSELF;
public:
							String();
							String( CCHAR * szString );
	virtual					~String();

	CULONG					GetCount() const;
	
	CSELF &					Printf( CCHAR * szFormat, ... );
	CSELF &					VPrintf( CCHAR * szFormat, va_list args );

	BLANK					operator CHAR *();

	CHAR *					GetCSTR();
	CCHAR *					GetCSTR() const;

	CHAR &					operator []( CULONG ulIndex );
	CCHAR &					operator []( CULONG ulIndex ) const;

	CSELF &					operator =( CSELF & rRightHandSide );

	CBOOL					operator <( CSELF & rRightHandSide ) const;
	CBOOL					operator >( CSELF & rRightHandSide ) const;

	CBOOL					operator ==( CSELF & rRightHandSide ) const;
	CBOOL					operator !=( CSELF & rRightHandSide ) const;

	CBOOL					operator <=( CSELF & rRightHandSide ) const;
	CBOOL					operator >=( CSELF & rRightHandSide ) const;

	void					TrimLeft();
	void					TrimRight();

private:
	CHAR					szBuffer[ s_ulLimit ];

};

template< ULONG s_ulLimit >
String<s_ulLimit>::String()
{
	aSsErT( s_ulLimit > 0 );

	szBuffer[ 0 ] = '\0';
}

template< ULONG s_ulLimit >
String<s_ulLimit>::String( CCHAR * szString )
{
	aSsErT( s_ulLimit > 0 );
//	Assert( szString != NULL );
	if( szString == NULL )
	{
		szBuffer[ 0 ]		= '\0';
		return;
	}

	ULONG i;
	for( i = 0; i < s_ulLimit - 1; i++ )
	{
		if( szString[ i ] == '\0' )
		{
			break;
		}
		szBuffer[ i ]		= szString[ i ];
	}
	szBuffer[ i ]			= '\0';
}

template< ULONG s_ulLimit >
String<s_ulLimit>::~String()
{
	aSsErT( s_ulLimit > 0 );
}

template< ULONG s_ulLimit >
inline CULONG
String<s_ulLimit>::GetCount() const
{
	aSsErT( s_ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex++ )
	{
		if ( szBuffer[ ulIndex ] == '\0' )
		{
			return ulIndex;
		}
	}

	return 0;
}

template< ULONG ulLimit >
inline const String<ulLimit> &
String<ulLimit>::Printf( CCHAR * szFormat, ... )
{
	aSsErT( ulLimit > 0 );
	Assert( szFormat != NULL );

	va_list args;
	va_start( args, szFormat );

	this->VPrintf( szFormat, args );

	va_end( args );

	return (* this);
}


template< ULONG ulLimit >
inline const String<ulLimit> &
String<ulLimit>::VPrintf( CCHAR * szFormat, va_list args )
{
	aSsErT( ulLimit > 0 );
	Assert( szFormat != NULL );

	CINT nRetVal = _vsnprintf( szBuffer, ulLimit, szFormat, args );
	if ( nRetVal < 0 || static_cast<CULONG>( nRetVal ) == ulLimit )
	{
		szBuffer[ ulLimit - 1 ] = '\0';
	}

	return (* this);
}

template< ULONG ulLimit >
inline
String<ulLimit>::operator CHAR *()
{
	aSsErT( ulLimit > 0 );

	return szBuffer;
}

template< ULONG s_ulLimit >
inline CHAR *
String<s_ulLimit>::GetCSTR()
{
	aSsErT( s_ulLimit > 0 );

	return szBuffer;
}

template< ULONG s_ulLimit >
inline CCHAR *
String<s_ulLimit>::GetCSTR() const
{
	aSsErT( s_ulLimit > 0 );

	return szBuffer;
}

template< ULONG s_ulLimit >
inline CHAR &
String<s_ulLimit>::operator []( CULONG ulIndex )
{
	aSsErT( s_ulLimit > 0 );
	Assert( ulIndex < s_ulLimit );

	return szBuffer[ ulIndex ];
}

template< ULONG s_ulLimit >
inline CCHAR &
String<s_ulLimit>::operator []( CULONG ulIndex ) const
{
	aSsErT( s_ulLimit > 0 );
	Assert( ulIndex < s_ulLimit );

	return szBuffer[ ulIndex ];
}

template< ULONG s_ulLimit >
inline const String<s_ulLimit> &
String<s_ulLimit>::operator =( CSELF & rRightHandSide )
{
	aSsErT( s_ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex++ )
	{
		szBuffer[ ulIndex ] = rRightHandSide[ ulIndex ];
		if ( szBuffer[ ulIndex ] == '\0' )
		{
			return (* this);
		}
	}

	szBuffer[ 0 ] = '\0';
	return (* this);
}

template< ULONG s_ulLimit >
inline CBOOL
String<s_ulLimit>::operator <( CSELF & rRightHandSide ) const
{
	aSsErT( s_ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex++ )
	{
		if ( szBuffer[ ulIndex ] < rRightHandSide[ ulIndex ] )
		{
			return TRUE;
		}

		if ( szBuffer[ ulIndex ] > rRightHandSide[ ulIndex ] )
		{
			return FALSE;
		}

		if ( szBuffer[ ulIndex ] == '\0' )
		{
			return FALSE;
		}
	}

	return FALSE;
}

template< ULONG s_ulLimit >
inline CBOOL
String<s_ulLimit>::operator >( CSELF & rRightHandSide ) const
{
	aSsErT( s_ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex++ )
	{
		if ( szBuffer[ ulIndex ] > rRightHandSide[ ulIndex ] )
		{
			return TRUE;
		}

		if ( szBuffer[ ulIndex ] < rRightHandSide[ ulIndex ] )
		{
			return FALSE;
		}

		if ( szBuffer[ ulIndex ] == '\0' )
		{
			return FALSE;
		}
	}

	return FALSE;
}

template< ULONG s_ulLimit >
inline CBOOL
String<s_ulLimit>::operator ==( CSELF & rRightHandSide ) const
{
	aSsErT( s_ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex++ )
	{
		if ( szBuffer[ ulIndex ] != rRightHandSide[ ulIndex ] )
		{
			return FALSE;
		}

		if ( szBuffer[ ulIndex ] == '\0' )
		{
			return TRUE;
		}
	}

	return TRUE;
}

template< ULONG s_ulLimit >
inline CBOOL
String<s_ulLimit>::operator !=( CSELF & rRightHandSide ) const
{
	aSsErT( s_ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex++ )
	{
		if ( szBuffer[ ulIndex ] != rRightHandSide[ ulIndex ] )
		{
			return TRUE;
		}

		if ( szBuffer[ ulIndex ] == '\0' )
		{
			return FALSE;
		}
	}

	return TRUE;
}

template< ULONG s_ulLimit >
inline CBOOL
String<s_ulLimit>::operator <=( CSELF & rRightHandSide ) const
{
	aSsErT( s_ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex++ )
	{
		if ( szBuffer[ ulIndex ] < rRightHandSide[ ulIndex ] )
		{
			return TRUE;
		}

		if ( szBuffer[ ulIndex ] > rRightHandSide[ ulIndex ] )
		{
			return FALSE;
		}

		if ( szBuffer[ ulIndex ] == '\0' )
		{
			return TRUE;
		}
	}

	return TRUE;
}

template< ULONG s_ulLimit >
inline CBOOL
String<s_ulLimit>::operator >=( CSELF & rRightHandSide ) const
{
	aSsErT( s_ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex++ )
	{
		if ( szBuffer[ ulIndex ] > rRightHandSide[ ulIndex ] )
		{
			return TRUE;
		}

		if ( szBuffer[ ulIndex ] < rRightHandSide[ ulIndex ] )
		{
			return FALSE;
		}

		if ( szBuffer[ ulIndex ] == '\0' )
		{
			return TRUE;
		}
	}

	return TRUE;
}

template< ULONG s_ulLimit >
inline void
String<s_ulLimit>::TrimLeft()
{
	ULONG ulTrim			= 0;
	for( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex ++ )
	{
		if( szBuffer[ ulIndex ] != ' ' &&
			szBuffer[ ulIndex ] != '\t' &&
			szBuffer[ ulIndex ] != '\r' &&
			szBuffer[ ulIndex ] != '\n' )
		{
			ulTrim = ulIndex;
			break;
		}
	}

	for( ULONG ulIndex = 0; ulIndex < s_ulLimit; ulIndex ++, ulTrim ++ )
	{
		szBuffer[ ulIndex ]	= szBuffer[ ulTrim ];
		if( szBuffer[ ulIndex ] == '\0' )
		{
			break;
		}
	}

}

template< ULONG s_ulLimit >
inline void
String<s_ulLimit>::TrimRight()
{
	CULONG ulLength			= GetCount();
	
	ULONG ulTrim			= ulLength - 1;
	while( ulTrim >= 0 )
	{
		if( szBuffer[ ulTrim ] != ' ' &&
			szBuffer[ ulTrim ] != '\t' &&
			szBuffer[ ulTrim ] != '\r' &&
			szBuffer[ ulTrim ] != '\n' )
		{
			break;
		}

		ulTrim --;
	}
	szBuffer[ ulTrim + 1 ]		= '\0';

}

} /* Template */				} /* System */

#endif