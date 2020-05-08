//------------------------------------------------------------------
//
//	FILE	  : PreLeaf.cpp
//
//	PURPOSE	  : 
//
//	CREATED	  : September 3 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

#include "bdefs.h"
#include "preleaf.h"
#include "prepoly.h"
#include "hullpoly.h"




CPreLeaf::CPreLeaf()
{
	m_Flags = 0;
	m_Index = 0;
	m_MaxVisDist = 0.0f;
}

CPreLeaf::~CPreLeaf()
{
	for(DWORD i=0; i < m_HullPolies; i++)
		DeletePoly(m_HullPolies[i]);
}



