#ifndef __MBCS__UTIL__
#define __MBCS__UTIL__

#pragma once

#define _LAST_CHARACTER_CODE 65535 //added by teseus 2^16 - 1


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	IsKoreanChar
//
//  PURPOSE:	Get if Char is Korean.
//
//	pszChars	- Characters in font.
//
//	author :	teseus
//
//  gernerate : 2003 11 10
// ----------------------------------------------------------------------- //
bool IsKoreanChar(char const * pszChars);
int CountKorean(char const * pszChars, int nLimit = 0);
int InverseCountKorean(char const * pszChars, int nCurrent);

#endif //__MBCS__UTIL__