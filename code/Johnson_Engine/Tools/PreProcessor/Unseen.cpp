//------------------------------------------------------------------
//
//	FILE	  : Unseen.cpp
//
//	PURPOSE	  : 
//
//	CREATED	  : February 12 1997
//
//	COPYRIGHT : Microsoft 1997 All Rights Reserved
//
//------------------------------------------------------------------

// Includes....
#include "bdefs.h"
#include "unseen.h"
#include "geomroutines.h"
#include "pregeometry.h"
#include "preleaf.h"
#include "preworld.h"
#include "hullpoly.h"



uint32 CUnseen::RemoveUnseenPortions(CPreWorld *pWorld, BaseEditObjArray &theObjects)
{
	if( pWorld->m_RootNode < 0 )
		return 0;
	
	MarkVisibleLeaves(pWorld, theObjects);
	return RemoveInvisibleLeaves(pWorld);
}


void CUnseen::MarkVisibleLeaves(CPreWorld *pWorld, BaseEditObjArray &theObjects)
{
	uint32 i, nVisible;
	CPreLeaf *pLeaf, *pInsideDefLeaf;
	CBaseEditObj *pObj;


	// Try leaves with a bunch of classes we know.
	pLeaf = FindInsideLeaf(pWorld);
	if(!pLeaf)
	{
		// Just mark them all visible if there aren't any objects.
		for( i=0; i < pWorld->m_Leafs; i++ )
			pWorld->m_Leafs[i]->m_Flags |= LEAF_VISIBLE;

		return;
	}
		
	
	// First, mark them all invisible.
	for( i=0; i < pWorld->m_Leafs; i++ )
		pWorld->m_Leafs[i]->m_Flags &= ~LEAF_VISIBLE;
		
	GoThruLeaf( pLeaf );

	// Flow thru leaves with any InsideDef objects.
	for( i=0; i < theObjects; i++ )
	{
		pObj = theObjects[i];

		if(stricmp(pObj->GetClassName(), "InsideDef") == 0)
		{
			pInsideDefLeaf = FindPointLeaf(pWorld, pObj->GetPos(), pWorld->m_RootNode);
			if(pInsideDefLeaf)
			{
				GoThruLeaf(pInsideDefLeaf);
			}
		}
	}

	
	// Count how many were visible.	
	nVisible = 0;
	for( i=0; i < pWorld->m_Leafs; i++ )
	{
		if( pWorld->m_Leafs[i]->m_Flags & LEAF_VISIBLE )
			++nVisible;
	}
}


void CUnseen::GoThruLeaf( CPreLeaf *pLeaf )
{
	uint32			i;
	CHullPoly		*pHullPoly;
	CPreLeaf		*pTestLeaf;


	// Mark this one visible.
	pLeaf->m_Flags |= LEAF_VISIBLE;

	for( i=0; i < pLeaf->m_HullPolies; i++ )
	{
		pHullPoly = pLeaf->m_HullPolies[i];
		
		if( pHullPoly->m_pAdjacent )
		{
			pTestLeaf = pHullPoly->m_pAdjacent->m_pLeaf;
		
			if( !(pTestLeaf->m_Flags & LEAF_VISIBLE) )
			{
				GoThruLeaf( pTestLeaf );
			}
		}
	}
}


uint32 CUnseen::RemoveInvisibleLeaves( CPreWorld *pWorld )
{
	uint32 i;
	uint32 nRemoved = 0;
	CPreLeaf *pLeaf;
	GPOS pos;
	CPrePoly *pPoly;
	LPOS lPos;
	CPrePoly *pTouching;


	// Mark all polies to be removed.
	for(pos=pWorld->m_Polies; pos; )
	{
		pPoly = pWorld->m_Polies.GetNext(pos);
		
		if((pPoly->m_pSurface->m_Flags & SURF_HULLMAKER) || 
			(pPoly->GetPPFlags() & PP_DONOTREMOVE))
			pPoly->SetPostRemove(FALSE);
		else
			pPoly->SetPostRemove(TRUE);
	}


	// Mark polies in visible leaves to not be removed.
	for( i=0; i < pWorld->m_Leafs; i++ )
	{
		pLeaf = pWorld->m_Leafs[i];

		if( pLeaf->m_Flags & LEAF_VISIBLE )
		{
			for(lPos=pLeaf->m_Touching; lPos; )
			{
				pTouching = pLeaf->m_Touching.GetNext(lPos);

				pTouching->SetPostRemove(FALSE);
			}
		}
	}

	// Remove the ones that should be removed.
	for(pos=pWorld->m_Polies; pos; )
	{
		pPoly = pWorld->m_Polies.GetNext(pos);

		if(pPoly->GetPostRemove() && !(pPoly->m_pSurface->m_Flags & SURF_HULLMAKER))
		{
			++nRemoved;
			pWorld->m_Polies.RemoveAt(pPoly);
			DeletePoly(pPoly);
		}
	}

	return nRemoved;
}













