#pragma once

namespace Framework {

typedef const WORD			CWORD;
typedef const DWORD			CDWORD;
typedef const INT			CINT;
typedef	const UINT			CUINT;
typedef const CHAR			CCHAR;
typedef const UCHAR			CUCHAR;
//typedef unsigned char		BYTE;
typedef const BYTE			CBYTE;
typedef const LONG			CLONG;
typedef const ULONG			CULONG;
typedef const SHORT			CSHORT;
typedef const USHORT		CUSHORT;
typedef const BOOL			CBOOL;
typedef const FLOAT			CFLOAT;
//typedef	__int64				INT64;
typedef	const INT64			CINT64;
typedef unsigned __int64	UINT64;
typedef const UINT64		CUINT64;
//typedef const DOUBLE	CDOUBLE;

#undef TRUE
#undef FALSE
static CBOOL TRUE	= true;
static CBOOL FALSE	= false;


} /* Framework */

namespace Framework {		namespace Template {
template< ULONG ulLimit >
struct String;

typedef String<256>		STRING;
typedef const STRING	CSTRING;
} /* Template */			} /* Framework */