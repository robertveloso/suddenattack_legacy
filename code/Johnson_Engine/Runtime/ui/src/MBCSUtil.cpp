#include "MBCSUtil.h"
#include <string.h>
#define min(a,b)    (((a) < (b)) ? (a) : (b))
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
bool IsKoreanChar(char const * pszChars)
{
	int i = 0;
	if( (( (pszChars[i]) >= (char)0xB0 ) && ((pszChars[i]) <= (char)0xC8 ) ) || (pszChars[i] == (char)0xA4) )
	{
		if  (( pszChars[i+1] >=(char)0xA1 ) && ( pszChars[i+1] <= (char)0xFE ) )
			{
				return true;
			}
		else  if ( pszChars[i+1] == (char)0x64) 
		{
			return true;
		}
		else
		{
			return false;
		}
	} 
	else if((pszChars[i] == (char)0x94) || (pszChars[i] == (char)0xee) //”î
			|| (pszChars[i] == (char)0x97) || (pszChars[i] == (char)0xcd)) //—Í
	{
		return true;
	}
	return	false;
}

int CountKorean(char const * pszChars, int nLimit)
{
	int nKGlyph, i, nChlen;
	nChlen = strlen(pszChars);
	if (nLimit) nChlen = min(nChlen, nLimit);
	
	for(i = 0, nKGlyph = 0; i < nChlen; ++i )
	{
		if (IsKoreanChar(&pszChars[i]))
		{
			++nKGlyph;
			++i;
		}
	}
	return nKGlyph;
}

int InverseCountKorean(char const * pszChars, int nCurrent)
{
	int nKGlyph, i, nChlen;
	if (!nCurrent) return 0;
	nChlen = strlen(pszChars);

	i = 0;
	while(pszChars[i])
	{
		if (IsKoreanChar(&pszChars[i]))
		{
			++nKGlyph;
			++i;++i;
		}
		else
		{
			++nKGlyph;
			++i;
		}
		if (nKGlyph == nCurrent)
		{
			break;
		}
	}
	return min(i, nChlen);
}