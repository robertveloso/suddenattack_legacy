//------------------------------------------------------------------
//
//	FILE	  : HullPoly.cpp
//
//	PURPOSE	  : Implements the CHullPoly class.
//
//	CREATED	  : September 3 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------


// Includes....
#include "bdefs.h"
#include "preleaf.h"
#include "hullpoly.h"
#include "preworld.h"
#include "geomroutines.h"
#include "splitpoly.h"
#include "prepoly.h"


CHullPoly* CopyHullPoly( CHullPoly *pPoly )
{
	CHullPoly	*pRet = CreatePoly(CHullPoly, pPoly->NumVerts(), true);

	//check the allocation
	if(pRet == NULL)
	{
		return NULL;
	}

	memcpy(pRet->VertList(), pPoly->VertList(), sizeof(CPolyVert)*pPoly->NumVerts() );

	pRet->m_Plane = pPoly->m_Plane;
	pRet->m_OuterShellID = pPoly->m_OuterShellID;

	return pRet;
}



void CopyHullPolyDirect( CHullPoly *pSrc, CHullPoly *pDest )
{
	memcpy(pDest->VertList(), pSrc->VertList(), sizeof(CPolyVert)*pSrc->NumVerts() );

	pDest->CopySplitAttributes(pSrc);	
	pDest->m_BasePoly.SetNumVerts(pSrc->NumVerts());
}



// ----------------------------------------------------------------------- //
//      ROUTINE:        GetFrontSide
//      PURPOSE:        Either returns pTest or a polygon on the front side
//                      of pSplitter .. does necessary allocation and deletion.
// ----------------------------------------------------------------------- //

bool GetFrontSide( CPrePlane *pSplitter, CHullPolySwapper &polies )
{
	PolySide		side;
	SplitStruct		splitStruct;

	
	side = GetPolySide(pSplitter, polies.Cur(), &splitStruct);
	if( (side == BackSide) || (splitStruct.m_nIntersect == polies.Cur()->NumVerts()) )
	{
		return false;
	}
	else if( side == FrontSide )
	{
		return true;
	}
	else
	{
		// Note: it doesn't copy any attributes over here! 
		//       There might be data that's needed...
		SplitPolySide(FrontSide, pSplitter, polies.Cur(), polies.Opposite(), &splitStruct);
		polies.Swap();
		return true;
	}
}


bool CHullPoly::MakeCubeHull( CHullPolyArray &polies, PVector &min, PVector &max )
{
	uint32			i;
	PVector			verts[8];

	
	// Add the necessary points.
	verts[0].Init( min.x, min.y, min.z );
	verts[1].Init( max.x, min.y, min.z );
	verts[2].Init( max.x, max.y, min.z );
	verts[3].Init( min.x, max.y, min.z );
	verts[4].Init( min.x, min.y, max.z );
	verts[5].Init( max.x, min.y, max.z );
	verts[6].Init( max.x, max.y, max.z );
	verts[7].Init( min.x, max.y, max.z );

	polies.SetSize(6);
	for( i=0; i < 6; i++ )
		polies[i] = CreatePoly(CHullPoly, 4, true);

	// Add the 'cap' polygons.
	for( i=0; i < 4; i++ )  
		polies[0]->SetVert(i, verts[i]);
	
	for( i=0; i < 4; i++ )
		polies[1]->SetVert(i, verts[7-i]);

	// Add the middle polygons.
	polies[2]->SetVert(0, verts[1]);	polies[2]->SetVert(1, verts[5]);
	polies[2]->SetVert(2, verts[6]);	polies[2]->SetVert(3, verts[2]);

	polies[3]->SetVert(0, verts[2]);	polies[3]->SetVert(1, verts[6]);
	polies[3]->SetVert(2, verts[7]);	polies[3]->SetVert(3, verts[3]);

	polies[4]->SetVert(0, verts[3]);	polies[4]->SetVert(1, verts[7]);
	polies[4]->SetVert(2, verts[4]);	polies[4]->SetVert(3, verts[0]);

	polies[5]->SetVert(0, verts[0]);	polies[5]->SetVert(1, verts[4]);
	polies[5]->SetVert(2, verts[5]);	polies[5]->SetVert(3, verts[1]);

	// Generate all the normals and add the polies to the list.
	for( i=0; i < 6; i++ )
		g_GenerateNormal( polies[i] );

	return true;
}




// ----------------------------------------------------------------------- //
// Flips the orientation of the polygon.  
// ----------------------------------------------------------------------- //
void CHullPoly::Flip()
{
	uint32		i;
	PVector		temp;
	
	for( i=0; i < NumVerts()/2; i++ )
	{
		temp = Pt(i);
		Pt(i) = Pt( NumVerts() - i - 1 );
		Pt( NumVerts() - i - 1 ) = temp;
	}

	Normal() = -Normal();
	Dist() = -Dist();
}


