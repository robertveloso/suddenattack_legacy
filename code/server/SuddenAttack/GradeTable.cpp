#include "StdAfx.h"
#include "GradeTable.h"

#define GRADE_DESC		TEXT("GRADE_DESC")
#define GRADE_STRING	TEXT("GRADE_STRING")
#define GRADE_RANGE		TEXT("GRADE_RANGE")

CGradeTable::CGradeTable()
{
	m_nGradeCount	= 0;
	m_pGradeTable	= NULL;
}

CGradeTable::~CGradeTable()
{
	this->UnLoad();	
}

BOOL CGradeTable::Load( LPCTSTR lpszFileName )
{
	UINT i;
	TCHAR	szTemp[256];

	m_nGradeCount = GetPrivateProfileInt( GRADE_DESC, TEXT("Count"), 0, lpszFileName );
	if( m_nGradeCount <=0 ) return FALSE;

	m_pGradeTable	= new T_GRADE_TABLE[m_nGradeCount];
	for( i=0 ; i<m_nGradeCount ; ++i )
	{
		wsprintf( szTemp, "%d", i );
		m_pGradeTable[i].nGrade		= i;
		GetPrivateProfileString( GRADE_STRING,
								 szTemp,
								 TEXT(""),
								 m_pGradeTable[i].szString,
								 MAX_GRADE_STRING,
								 lpszFileName );
		m_pGradeTable[i].nMinLimit	= (i==0) ? 0 : m_pGradeTable[i-1].nMaxLimit+1;
		m_pGradeTable[i].nMaxLimit	= GetPrivateProfileInt( GRADE_RANGE, szTemp, 0, lpszFileName );

		if( _tcslen( m_pGradeTable[i].szString ) <= 0 ||
			m_pGradeTable[i].nMinLimit < 0 ||
			m_pGradeTable[i].nMaxLimit <= 0 )
		{
			this->UnLoad();
			return FALSE;
		}		
	}
	return TRUE;
}

void CGradeTable::UnLoad()
{
	m_nGradeCount	= 0;
	if( m_pGradeTable )
	{
		delete [] m_pGradeTable;
		m_pGradeTable	= NULL;
	}
}

UINT CGradeTable::GetGradeFromExp( __int64 nExp )
{
	UINT i;
	if( nExp < 0 ) return 0;

	for( i=0 ; i<m_nGradeCount ; ++i )
	{
		if( nExp <= m_pGradeTable[i].nMaxLimit ) return i;
	}

	if( nExp > m_pGradeTable[m_nGradeCount-1].nMaxLimit )	//최고값 초과
	{
		return	m_nGradeCount-1;
	}

	//Error
	return 0;
}