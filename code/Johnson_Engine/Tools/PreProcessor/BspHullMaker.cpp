//------------------------------------------------------------------
//
//	FILE	  : BspHullMaker.cpp
//
//	PURPOSE	  : Implementation for the CBspHullMaker class.
//
//	CREATED	  : 2nd May 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

// Includes....
#include "bdefs.h"
#include "geomroutines.h"
#include "bsphullmaker.h"
#include "hullpoly.h"
#include "splitpoly.h"
#include "preworld.h"
#include "pregeometry.h"
#include "preleaf.h"


static CPreWorld		*g_pHullWorld;
static CBspHullMaker	*g_pHullMaker;
static PReal			g_WorldRadius;


static void VerifyPortalIntegrity(CPreWorld *pWorld)
{
	uint32		i, j;
	CPreLeaf	*pLeaf;
	CHullPoly	*pHullPoly;

	for( i=0; i < pWorld->m_Leafs; i++ )
	{
		pLeaf = pWorld->m_Leafs[i];

		for( j=0; j < pLeaf->m_HullPolies; j++ )
		{
			pHullPoly = pLeaf->m_HullPolies[j];

			if( pHullPoly->m_pAdjacent )
			{
//				ASSERT( pHullPoly->m_pAdjacent->m_pAdjacent == pHullPoly );
			}
		}
	}
}




// ----------------------------------------------------------------------- //
// Tells if the BSP hull maker should recurse into the node.
// ----------------------------------------------------------------------- //

static bool IsValidHullNode(CPreWorld *pWorld, NODEREF iNode)
{
	CNode	*pNode;

	if(!IsValidNode(iNode))
		return false;

	pNode = pWorld->GetNode(iNode);
	return true;
}


// ----------------------------------------------------------------------- //
// Adds all detail nodes to the given list.
// ----------------------------------------------------------------------- //

static void AddDetailNodesToList(
	CPreWorld *pWorld, 
	NODEREF pRoot, 
	CLinkedList<CPrePoly*> &theList)
{
	if(!IsValidNode(pRoot))
		return;

	ASSERT(pRoot->m_pPoly->GetSurface()->IsDetailNode());
	theList.Append(pRoot->m_pPoly);
}


// ----------------------------------------------------------------------- //
// Builds a 'cap' poly for a hull portal by making a very large
// poly, then chopping outside portions off.
// ----------------------------------------------------------------------- //

static CHullPoly* MakeCapPoly(CPreLeaf *pLeaf, CPrePoly *pPoly)
{
	// Make a HUGE polygon out of pPoly's plane and split it by all the polies in the hull.
	CHullPoly *pCapPoly;
	PVector u, v, forward, basePt;
	PVertex points[4];

	pCapPoly = CreatePoly(CHullPoly, 4, true);

	//check the allocation
	if(pCapPoly == NULL)
	{
		return NULL;
	}

	BuildFrameOfReference(&pPoly->Normal(), NULL, &u, &v, &forward);
	u.Norm();
	v.Norm();
	ASSERT( v.Mag() > 0.5f && v.Mag() < 1.5f );
	u *= g_WorldRadius;
	v *= g_WorldRadius;

	basePt = pPoly->Pt(0);
	pCapPoly->Pt(0) = basePt + (-u - v);
	pCapPoly->Pt(1) = basePt + ( u - v);
	pCapPoly->Pt(2) = basePt + ( u + v);
	pCapPoly->Pt(3) = basePt + (-u + v);

	// Copy the plane.
	pCapPoly->m_Plane = *pPoly->GetPlane();

	return pCapPoly;
}


// Takes all the sides of the leaf and clips pCap so it lies inside the leaf.
static CHullPoly* ClipCapPoly(
	CPreLeaf *pLeaf, 
	CHullPoly *pCap,
	CPrePoly *pPrePoly)
{
	CHullPoly			*pHullPoly;
	CHullPoly			*newPolies[2];
	PolySide			side;
	uint32				i;
	SplitStruct			splitStruct;


	for( i=0; i < pLeaf->m_HullPolies; i++ )
	{
		pHullPoly = pLeaf->m_HullPolies[i];
		side = GetPolySide(&pHullPoly->m_Plane, pCap, &splitStruct);
		
		if( splitStruct.m_nIntersect == pCap->NumVerts() )
		{
			DeletePoly(pCap);
			pCap = NULL;
			break;
		} 

		// This happens from time to time when there are angled polies with virtually
		// the same plane, but due to numerical inaccuracy it sometimes classifies one
		// as being on the backside. I -think- this is ok because all it needs cap polies
		// for is to seal the hull and if it already has a hull poly that came from
		// the same plane, then the hull shouldn't need another poly with the same plane.
		if(side == BackSide)
		{
			DeletePoly(pCap);
			return NULL;
		}

		if(side == Intersect)
		{
			if(!SplitPoly(&pHullPoly->m_Plane, pCap, newPolies, &splitStruct))
			{
				// Don't freak out, just store this so if they did 
				// -SelectHullProblemBrushes, they can see where the problem was.
				g_pHullMaker->m_ProblemPolies.Append(pPrePoly->m_pSurface->m_pOriginalPoly);
			}
			
			DeletePoly(newPolies[BackSide]);
			DeletePoly(pCap);
			pCap = newPolies[FrontSide];
		}
	}

	return pCap;
}


// ----------------------------------------------------------------------- //
// The routine that goes down the tree and finds
// which nodes will have hulls.
// ----------------------------------------------------------------------- //

static void RecursivelyMakeHulls(
	NODEREF iRoot, 
	CPreLeaf *pLeaf)
{
	uint32				i, j;
	uint32				k;
	LPOS lPos;

	CNode				*pRoot;

	CPreLeaf			*pAdjacentLeaf;
	CPreLeaf			*sides[2];

	CPrePoly			*pTouching;
	CPrePoly			*pPoly;
	
	CHullPoly			*pCap, *pBackCap, *pHullPoly;
	CHullPoly			*pAdjacent;
	CHullPoly			*newPolies[2], *newAdjacent[2];

	PolySide			side;
	static SplitStruct	splitStruct;



RESTART_FUNCTION:;

	// Don't split empty leaves.
	if(pLeaf->m_HullPolies.GetSize() == 0)
		return;
	
	
	pRoot = g_pHullWorld->GetNode(iRoot);
	pPoly = pRoot->m_pPoly;
	sides[0] = new CPreLeaf;
	sides[1] = new CPreLeaf;


	++g_pHullMaker->m_nNodesVisited;
	if(g_pHullMaker->m_Callback)
		g_pHullMaker->m_Callback((float)g_pHullMaker->m_nNodesVisited / g_pHullMaker->m_nTotalVisits);


	// Get the cap polygon (pPoly's contribution to the hull).
	if(pCap = MakeCapPoly(pLeaf, pPoly))
	{
		// Clip off portions of the poly that are outside the hull.
		pCap = ClipCapPoly(pLeaf, pCap, pPoly);
	}


	// Add the cap to both sides.
	if(pCap)
	{
		ASSERT( (pPoly->Normal().Dot(pCap->Normal())) > 0 );

		pBackCap = CopyHullPoly( pCap );
		pBackCap->Flip();

		sides[FrontSide]->m_HullPolies.Append( pCap );
		sides[BackSide]->m_HullPolies.Append( pBackCap );

		pCap->m_pAdjacent		= pBackCap;
		pBackCap->m_pAdjacent	= pCap;

		pCap->m_pNode = pBackCap->m_pNode = pRoot;
	}


	// Update the touching lists.
	for(lPos=pLeaf->m_Touching; lPos; )
	{
		pTouching = pLeaf->m_Touching.GetNext(lPos);
		
		side = GetPolySide(pPoly->GetPlane(), pTouching, &splitStruct);
		if( side == Intersect )
		{
			sides[FrontSide]->m_Touching.Append( pTouching );
			sides[BackSide]->m_Touching.Append( pTouching );
		}
		else
		{
			sides[side]->m_Touching.Append( pTouching );
		}
	}		


	// Split the hull into two sections on pPoly's plane.
	for( i=0; i < pLeaf->m_HullPolies; i++ )
	{
		pHullPoly = pLeaf->m_HullPolies[i];
		
		// Split the hull poly (if needed).
		side = GetPolySide(pPoly->GetPlane(), pHullPoly, &splitStruct);
		if( side == Intersect )
		{
			SplitPoly(pPoly->GetPlane(), pHullPoly, newPolies, &splitStruct);
			
			sides[0]->m_HullPolies.Append( newPolies[0] );
			sides[1]->m_HullPolies.Append( newPolies[1] );

			
			if( pHullPoly->m_pAdjacent )
			{
				pAdjacent = pHullPoly->m_pAdjacent;
				pAdjacentLeaf = pAdjacent->m_pLeaf;
				ASSERT( pAdjacentLeaf );
				
				// Rather than split pAdjacent (which isn't always guaranteed to split the same as 
				// pHullPoly due to floating-point inaccuracy), just copy and flip pHullPoly's splits.
				for(j=0; j < 2; j++)
				{
					newAdjacent[j] = CopyHullPoly(newPolies[j]);
					newAdjacent[j]->Flip();

					pAdjacentLeaf->m_HullPolies.Append( newAdjacent[j] );
					newAdjacent[j]->m_pLeaf = pAdjacent->m_pLeaf;
					newAdjacent[j]->m_pAdjacent = newPolies[j];
					newPolies[j]->m_pAdjacent = newAdjacent[j];
				}
				
				pAdjacentLeaf->m_HullPolies.Remove( pAdjacentLeaf->m_HullPolies.FindElement(pAdjacent) );
 				DeletePoly(pAdjacent);
				pHullPoly->m_pAdjacent = NULL;
			}
		
			DeletePoly(pHullPoly);
		}
		else
		{
			sides[side]->m_HullPolies.Append( pHullPoly );
		}
	}


	// Delete the input leaf .. not needed anymore.
	pLeaf->m_HullPolies.SetSize(0);
	delete pLeaf;


	// Add pPoly as a touching poly.
	sides[FrontSide]->m_Touching.Append(pRoot->m_pPoly);


	// Point the hull polies at their leaves.
	for( i=0; i < 2; i++ )
	{
		for( k=0; k < sides[i]->m_HullPolies.GetSize(); k++ )
			sides[i]->m_HullPolies[k]->m_pLeaf = sides[i];
	}


	// Add the hulls to each child and recurse.
	// If there is no front side, then pPoly gets the front side's hull.
	if(IsValidHullNode(g_pHullWorld, pRoot->m_Sides[FrontSide]))
	{
		// Recurse into the front side.
		RecursivelyMakeHulls(pRoot->m_Sides[FrontSide], sides[FrontSide]);
	}
	else
	{
		// Set the CANTSEETHRUPORTAL flag.
		if(pRoot->m_Flags & NODEFLAG_CANTSEETHRUPORTAL)
		{
			for(i=0; i < sides[FrontSide]->m_HullPolies; i++)
			{
				sides[FrontSide]->m_HullPolies[i]->m_Flags |= HULLPOLY_CANTSEETHRUPORTAL;
				//pLeaf->m_HullPolies[i]->m_pAdjacent = NULL;
			}
		}
		
		AddDetailNodesToList(g_pHullWorld, 
			pRoot->m_Sides[FrontSide], 
			sides[FrontSide]->m_Touching);

		// Copy the front-side polies into pPoly's hull.
		pRoot->m_pLeaf = sides[FrontSide];
		sides[FrontSide]->m_pNode = pRoot;
		
		g_pHullWorld->m_Leafs.Append( sides[FrontSide] );
		assert((g_pHullWorld->m_Leafs.LastI() < 65536) && "Overflow");
		sides[FrontSide]->m_Index = (WORD)g_pHullWorld->m_Leafs.LastI();
	}

	
	if(IsValidHullNode(g_pHullWorld, pRoot->m_Sides[BackSide]))
	{
		// Recurse into the back side.
		iRoot = pRoot->m_Sides[BackSide];
		pLeaf = sides[BackSide];
		goto RESTART_FUNCTION;
	}
	else
	{
		delete sides[BackSide];
	}
}


// ----------------------------------------------------------------------- //
// The main routine that's called to generate hulls.
// ----------------------------------------------------------------------- //

BHMStatus MakeHulls(CBspHullMaker *pMaker, 
	CPreWorld *pWorld, 
	HullCallback callback)
{
	uint32				i;
	CPrePolyPtrArray	nodeStack;
	PVector				minimum, maximum;
	CPreLeaf			*pLeaf;
	GPOS				pos;


	pMaker->m_ProblemPolies.Term();
	
	// Figure out the world radius.  This will be the size of the 'cap' portals we make.
	pWorld->GetBoundingBox(&minimum, &maximum);
	g_WorldRadius = (maximum - minimum).Mag() * 2.0f;


	// Setup callback stuff.
	pMaker->m_Callback = callback;
	pMaker->m_nNodesVisited=0;
	pMaker->m_nTotalVisits = pWorld->NumInternalNodes();


	if(pWorld->m_RootNode < 0)
		return BHM_Ok;

	
	// Clear all the hull data from the world.
	pWorld->RemoveAllLeafs();	

	
	// Make a big box for the root node.
	pLeaf = new CPreLeaf;

	minimum.Init((PReal)MAX_CREAL, (PReal)MAX_CREAL, (PReal)MAX_CREAL);
	maximum.Init((PReal)-MAX_CREAL, (PReal)-MAX_CREAL, (PReal)-MAX_CREAL);
	for(pos=pWorld->m_Polies; pos; )
		pWorld->m_Polies.GetNext(pos)->ExtendBounds( minimum, maximum );

	minimum -= PVector(10.0f, 10.0f, 10.0f);
	maximum += PVector(10.0f, 10.0f, 10.0f);
	CHullPoly::MakeCubeHull(pLeaf->m_HullPolies, minimum, maximum);

	// Mark these as outer shell polies.
	for(i=0; i < pLeaf->m_HullPolies; i++)
	{
		assert((i < 65536) && "Overflow");
		pLeaf->m_HullPolies[i]->m_OuterShellID = (WORD)i;
	}

	
	// Generate them...
	g_pHullWorld = pWorld;
	g_pHullMaker = pMaker;

	RecursivelyMakeHulls(pWorld->m_RootNode, pLeaf);

	VerifyPortalIntegrity( pWorld );

	return BHM_Ok;
}


