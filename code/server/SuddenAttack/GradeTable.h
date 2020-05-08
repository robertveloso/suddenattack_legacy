#ifndef __MURSUM_GRADETABLE_H__
#define __MURSUM_GRADETABLE_H__

#define MAX_GRADE_STRING	64

typedef struct _tagGRADE_TABLE
{
	UINT	nGrade;
	TCHAR	szString[MAX_GRADE_STRING];
	__int64	nMinLimit;
	__int64	nMaxLimit;	
} T_GRADE_TABLE, *T_GRADE_TABLEptr;

class CGradeTable
{
public:
	CGradeTable();
	~CGradeTable();

	BOOL	Load( LPCTSTR lpszFileName );
	void	UnLoad();
	UINT	GetGradeFromExp( __int64 nExp );

protected:
	UINT				m_nGradeCount;
	T_GRADE_TABLEptr	m_pGradeTable;
};

#endif