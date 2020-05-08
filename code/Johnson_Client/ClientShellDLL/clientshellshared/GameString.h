#ifndef __NF_GAME_STRING_INLINE_FUNCTIONS_H__
#define __NF_GAME_STRING_INLINE_FUNCTIONS_H__

#pragma once

// String Wrapping Inline Functions.
// by Novice. 2005. 03. 12.


// 문자열 비교
inline int __strcmp( char* szStr1, const char* szStr2 )
{
	return ( CompareString( LOCALE_USER_DEFAULT, NULL,
		szStr1, lstrlen(szStr1), szStr2, lstrlen(szStr2) ) - CSTR_EQUAL );
}

// 문자열 비교 - 제한된 길이
inline int __strncmp( char* szStr1, const char* szStr2, int nLength )
{
	return ( CompareString( LOCALE_USER_DEFAULT, NULL,
		szStr1, nLength, szStr2, nLength ) - CSTR_EQUAL );
}

// 문자열 비교 - (대소문자, 가타가나 히라가나, 전각 반각) 무시
inline int __stricmp( char* szStr1, const char* szStr2 )
{
	return ( CompareString( LOCALE_USER_DEFAULT, NORM_IGNORECASE | NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH,
		szStr1, lstrlen(szStr1), szStr2, lstrlen(szStr2) ) - CSTR_EQUAL );
}

// 문자열 비교 - (대소문자, 가타가나 히라가나, 전각 반각) 무시 - 제한된 길이
inline int __strnicmp( char* szStr1, const char* szStr2, int nLength )
{
	return ( CompareString( LOCALE_USER_DEFAULT, NORM_IGNORECASE | NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH,
		szStr1, nLength, szStr2, nLength ) - CSTR_EQUAL );
}

// 문자열 복사
inline LPSTR __strcpy( char* szStr1, const char* szStr2, BOOL bIgnoreTerminate )
{
	return (bIgnoreTerminate) ? strcpy( szStr1, szStr2 ) : lstrcpy( szStr1, szStr2 );
}

// 문자열 복사 - 제한된 길이 및 끝문자 삭제
inline LPSTR __strncpy( char* szStr1, const char* szStr2, int nLength, BOOL bIgnoreTerminate = FALSE )
{
	return (bIgnoreTerminate) ? strncpy( szStr1, szStr2, nLength ) : lstrcpyn( szStr1, szStr2, nLength + 1 );
}

// 문자열 길이
inline int __strlen( const char* szStr )
{
	return lstrlen(szStr);
}

// 비어있는 문자열인지 검사
inline BOOL __IsEmptyString( const char* szStr )
{
	return ( NULL == lstrlen(szStr) );
}

inline LPSTR __strtok( char* szStr, const char* szToken, OUT char* szOutput, OUT bool& bFinished )
{
	for ( int i = 0; i < __strlen(szStr); i++ )
	{
		if ( 0 == __strncmp( (szStr + i), szToken, __strlen(szToken) ) )
		{
			__strncpy( szOutput, szStr, i );
			return (szStr + i + __strlen(szToken));
		}
	}
	
	if ( i == __strlen(szStr) )
	{
		bFinished = true;
		return szStr;
	}
	
	return NULL;
}

#endif