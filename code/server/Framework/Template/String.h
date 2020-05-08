#pragma once

namespace Framework {		namespace Template {

template< ULONG ulLimit >
struct String
{
public:
	typedef		const String<ulLimit>
				CSELF;

public:
	String();
	String( CCHAR * szString );
	BLANK		~String();

	CULONG		GetCount() const;
	CULONG		GetLimit() const;

	CSELF &		Printf( CCHAR * szFormat, ... );
	CSELF &		VPrintf( CCHAR * szFormat, va_list args );

	BLANK		operator CHAR *();
	BLANK		operator CCHAR *() const;

	BLANK		operator CINT() const;
	BLANK		operator CLONG() const;
	BLANK		operator CFLOAT() const;
//	BLANK		operator CDOUBLE() const;
//	BLANK		operator CADDRESS() const;

	CHAR &		operator []( CULONG ulIndex );
	CCHAR &		operator []( CULONG ulIndex ) const;

	CSELF &		operator =( CSELF & rRightHandSide );

	CBOOL		operator <( CSELF & rRightHandSide ) const;
	CBOOL		operator >( CSELF & rRightHandSide ) const;

	CBOOL		operator ==( CSELF & rRightHandSide ) const;
	CBOOL		operator !=( CSELF & rRightHandSide ) const;

	CBOOL		operator <=( CSELF & rRightHandSide ) const;
	CBOOL		operator >=( CSELF & rRightHandSide ) const;

	void		TrimLeft();
	void		TrimRight();
	void		RemoveChar( CHAR ch );

private:
	CHAR		szBuffer[ ulLimit ];

};

template< ULONG ulLimit >
String<ulLimit>::String()
{
	aSsErT( ulLimit > 0 );

	szBuffer[ 0 ] = '\0';
}

template< ULONG ulLimit >
String<ulLimit>::String( CCHAR * szString )
{
	aSsErT( ulLimit > 0 );
	assert( szString != NULL );

	this->Printf( "%s", szString );
}

template< ULONG ulLimit >
String<ulLimit>::~String()
{
	aSsErT( ulLimit > 0 );
}

template< ULONG ulLimit >
inline CULONG
String<ulLimit>::GetCount() const
{
	aSsErT( ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex++ )
	{
		if ( szBuffer[ ulIndex ] == '\0' )
		{
			return ulIndex;
		}
	}

	return 0;
}

template< ULONG ulLimit >
inline CULONG
String<ulLimit>::GetLimit() const
{
	aSsErT( ulLimit > 0 );

	return ulLimit;
}

template< ULONG ulLimit >
inline const String<ulLimit> &
String<ulLimit>::Printf( CCHAR * szFormat, ... )
{
	aSsErT( ulLimit > 0 );
	assert( szFormat != NULL );

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
	assert( szFormat != NULL );

	CINT nRetVal = VSNPRINTF( szBuffer, ulLimit, szFormat, args );
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

template< ULONG ulLimit >
inline
String<ulLimit>::operator CCHAR *() const
{
	aSsErT( ulLimit > 0 );

	return szBuffer;
}

template< ULONG ulLimit >
String<ulLimit>::operator CINT() const
{
	aSsErT( ulLimit > 0 );

	if ( this->GetCount() == 0 )
	{
		return 0;
	}

	return ATOI( szBuffer );
}

template< ULONG ulLimit >
String<ulLimit>::operator CLONG() const
{
	aSsErT( ulLimit > 0 );

	if ( this->GetCount() == 0 )
	{
		return 0;
	}

	return ATOL( szBuffer );
}

template< ULONG ulLimit >
String<ulLimit>::operator CFLOAT() const
{
	aSsErT( ulLimit > 0 );

	if ( this->GetCount() == 0 )
	{
		return 0;
	}

	return ATOF( szBuffer );
}

//template< ULONG ulLimit >
//String<ulLimit>::operator CDOUBLE() const
//{
//	aSsErT( ulLimit > 0 );
//
//	if ( this->GetCount() == 0 )
//	{
//		return 0;
//	}
//
//	return ATOD( szBuffer );
//}

//template< ULONG ulLimit >
//String<ulLimit>::operator CADDRESS() const
//{
//	aSsErT( ulLimit > 0 );
//
//	if ( this->GetCount() == 0 )
//	{
//		return 0;
//	}
//
//	return ATOA( szBuffer );
//}

template< ULONG ulLimit >
inline CHAR &
String<ulLimit>::operator []( CULONG ulIndex )
{
	aSsErT( ulLimit > 0 );
	assert( ulIndex < ulLimit );

	return szBuffer[ ulIndex ];
}

template< ULONG ulLimit >
inline CCHAR &
String<ulLimit>::operator []( CULONG ulIndex ) const
{
	aSsErT( ulLimit > 0 );
	assert( ulIndex < ulLimit );

	return szBuffer[ ulIndex ];
}

template< ULONG ulLimit >
inline const String<ulLimit> &
String<ulLimit>::operator =( CSELF & rRightHandSide )
{
	aSsErT( ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex++ )
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

template< ULONG ulLimit >
inline CBOOL
String<ulLimit>::operator <( CSELF & rRightHandSide ) const
{
	aSsErT( ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex++ )
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

template< ULONG ulLimit >
inline CBOOL
String<ulLimit>::operator >( CSELF & rRightHandSide ) const
{
	aSsErT( ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex++ )
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

template< ULONG ulLimit >
inline CBOOL
String<ulLimit>::operator ==( CSELF & rRightHandSide ) const
{
	aSsErT( ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex++ )
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

template< ULONG ulLimit >
inline CBOOL
String<ulLimit>::operator !=( CSELF & rRightHandSide ) const
{
	aSsErT( ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex++ )
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

template< ULONG ulLimit >
inline CBOOL
String<ulLimit>::operator <=( CSELF & rRightHandSide ) const
{
	aSsErT( ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex++ )
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

template< ULONG ulLimit >
inline CBOOL
String<ulLimit>::operator >=( CSELF & rRightHandSide ) const
{
	aSsErT( ulLimit > 0 );

	for ( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex++ )
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

template< ULONG ulLimit >
inline void
String<ulLimit>::TrimLeft()
{
	ULONG ulTrim			= 0;
	for( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex ++ )
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

	for( ULONG ulIndex = 0; ulIndex < ulLimit; ulIndex ++, ulTrim ++ )
	{
		szBuffer[ ulIndex ]	= szBuffer[ ulTrim ];
		if( szBuffer[ ulIndex ] == '\0' )
		{
			break;
		}
	}

}

template< ULONG ulLimit >
inline void
String<ulLimit>::TrimRight()
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

template< ULONG ulLimit >
inline void
String<ulLimit>::RemoveChar( CHAR ch )
{
	aSsErT( ulLimit > 0 );

	ULONG ulCount = GetCount();

	for ( ULONG ulIndex = 0; ulIndex < ulCount; ulIndex++ )
	{
		if ( szBuffer[ ulIndex ] == ch )
		{
			MEMCPY( &szBuffer[ ulIndex ], &szBuffer[ ulIndex + 1 ], ulCount - (ulIndex+1) );
			szBuffer[ulCount-1] = '\0';
			--ulCount;
		}
	}	
}

} /* Template */			} /* Framework */