
#include "bdefs.h"
#include "createvisbsp.h"
#include "processing.h"
#include "editregion.h"
#include "leakfinders.h"
#include "splitpoly.h"
#include "bspgen.h"
#include "preleaf.h"


// Gets all brushes valid for the visibility BSP.
// (Non-SURF_TERRAIN and non-detail).
static void GetVisBSPBrushes(
	GenList<CEditBrush*> &in,
	GenList<CEditBrush*> &out)
{
	GenListPos pos;
	CEditBrush *pBrush;

	for(pos=in.GenBegin(); in.GenIsValid(pos); )
	{
		pBrush = in.GenGetNext(pos);

		if(GetDetailLevel(pBrush) == 0)
			out.GenAppend(pBrush);
	}
}


template<class T>
inline void DeleteAndClearGenList(T &theList)
{
	GenListPos pos;
	
	for(pos=theList.GenBegin(); theList.GenIsValid(pos); )
	{
		delete theList.GenGetNext(pos);
	}

	theList.GenRemoveAll();
}


static void DividePolyList(
	LTPlane *pPlane,
	GenList<CPrePoly*> &polies,
	GenList<CPrePoly*> &addedPolies,
	GenList<CPrePoly*> *sides[2])
{
	GenListPos pos;
	CPrePoly *pPoly;
	PolySide side;
	CPrePoly *newSides[2];
	SplitStruct splitStruct;

	for(pos=polies.GenBegin(); polies.GenIsValid(pos); )
	{
		pPoly = polies.GenGetNext(pos);
	
		side = GetPolySide(pPlane, pPoly, &splitStruct);
		if(side == Intersect)
		{
			SplitPoly(pPlane, pPoly, newSides, &splitStruct);
			
			addedPolies.GenAppend(newSides[0]);
			addedPolies.GenAppend(newSides[1]);
			
			sides[0]->GenAppend(newSides[0]);
			sides[1]->GenAppend(newSides[1]);
		}
		else
		{
			sides[side]->GenAppend(pPoly);
		}
	}
}

// Clear out the m_Touching list of all the leaves.
static void ClearLeafTouchLists(
	CPreWorld *pWorld)
{
	GenListPos pos;

	for(pos=pWorld->m_Leafs.GenBegin(); pWorld->m_Leafs.GenIsValid(pos); )
	{
		pWorld->m_Leafs.GenGetNext(pos)->m_Touching.GenRemoveAll();
	}
}


// Chops the poly up and puts it in any leaves it intersects.
// pTestPoly is the chopped up poly that is used to see if it's
// in the leaf. pActualPoly is the poly that will be added to the
// leaf if it's inside.
static void FilterPolyIntoLeaves_R(
	CPrePoly *pTestPoly, 
	CPrePoly *pActualPoly,
	CNode *pNode)
{
	static SplitStruct splitStruct;
	PolySide side;
	CPrePoly *sides[2];
	int i;

	
	if(!IsValidNode(pNode))
		return;

	side = GetPolySide(pNode->GetPlane(), pTestPoly, &splitStruct);
	if(pNode->m_pLeaf && (side != BackSide))
	{
		pNode->m_pLeaf->m_Touching.GenAppend(pActualPoly);
	}

	if(side == Intersect)
	{
		if(SplitPoly(pNode->GetPlane(), pTestPoly, sides, &splitStruct))
		{
			for(i=0; i < 2; i++)
			{
				FilterPolyIntoLeaves_R(
					sides[i],
					pActualPoly,
					pNode->m_Sides[i]);

				DeletePoly(sides[i]);
			}
		}
	}
	else
	{
		FilterPolyIntoLeaves_R(
			pTestPoly,
			pActualPoly,
			pNode->m_Sides[side]);
	}
}


// Filters each poly down into the leaves.
static void AddVisiblePoliesToLeaves(
	CPreWorld *pWorld,
	GenList<CPrePoly*> &polies
)
{
	GenListPos polyPos;
	CPrePoly *pPoly;

	
	for(polyPos=polies.GenBegin(); polies.GenIsValid(polyPos); )
	{
		pPoly = polies.GenGetNext(polyPos);
	
		if(!(pPoly->GetSurfaceFlags() & SURF_INVISIBLE))
		{
			FilterPolyIntoLeaves_R(
				pPoly, 
				pPoly,
				pWorld->m_RootNode);
		}
	}
}


bool MakeVisBSPLeaves(CPreWorld *pPreWorld)
{
	CLinkedList<CPrePoly*> polies;
	GenListPos pos;
	CPreMainWorld *pMainWorld;
	CPreWorld *pTestWorld;


	if(!MakeHulls(pPreWorld, NULL))
		return false;


	// Build touch lists. The touch list includes all terrains and the physics BSP.
	pMainWorld = pPreWorld->m_pMainWorld;

	ClearLeafTouchLists(pPreWorld);

	for(pos=pMainWorld->m_WorldModels.GenBegin(); pMainWorld->m_WorldModels.GenIsValid(pos); )
	{
		pTestWorld = pMainWorld->m_WorldModels.GenGetNext(pos);
	
		if(pTestWorld->m_WorldInfoFlags & WIF_PHYSICSBSP)
		{
			polies.GenAppendList(pTestWorld->m_Polies);
		}
	}

	AddVisiblePoliesToLeaves(
		pPreWorld,
		polies);

	return true;
}


bool CreateVisBSP(
	CEditRegion *pRegion,
	CWorldModelDef *pWorldModel,
	CPreWorld *pPreWorld
	)
{
	PVector min, max;
	CLinkedList<CEditBrush*> visBrushes;


	GetVisBSPBrushes(pWorldModel->m_Brushes, visBrushes);

	GenerateSurfaces(visBrushes, pPreWorld);

	// Merge stuff together.
	if(!MergeBrushes(visBrushes, pPreWorld))
		return false;

	visBrushes.GenRemoveAll(); // Don't need this list anymore.


	// Make the BSP.
	DrawStatusTextIfMainWorld(eST_Normal, "Creating BSP.");

	if( !MakeBsp(pPreWorld, false, false, false) )
	{
		DrawStatusText(eST_Error, "failed to create BSP for world model: %s. Make sure that there are some non-render only brushes beneath this world model.", pPreWorld->m_WorldName);
		return false;
	}							  

	pPreWorld->m_RootNode->Verify();


	// Get rid of anything that will never be seen.
	if (g_pGlobs->m_bRemoveUnseen)
	{
		DrawStatusTextIfMainWorld(eST_Normal, "Removing unseen geometry.");
		if(!RemoveUnseenPortions(pPreWorld))
			return false;
	}

	pPreWorld->RemoveAllLeafs();
	pPreWorld->RemoveAllNodes();
	
	DrawStatusTextIfMainWorld(eST_Normal, "Recreating BSP.");

	if(!MakeBsp(pPreWorld, true, true, g_pGlobs->m_bSplitPolyForVisBSP ? false : true))
	{
		DrawStatusText(eST_Error, "failed to create BSP for world model: %s. Make sure that there are some non-render only brushes beneath this world model.", pPreWorld->m_WorldName);
		return false;
	}
	
	pPreWorld->m_RootNode->Verify();

	pPreWorld->InitPolyVertexIndices();

	DrawStatusTextIfMainWorld(eST_Normal, "Finding leaves.");
	MakeVisBSPLeaves(pPreWorld);
	
	// Create a leak file if they want.
	if(g_pGlobs->m_LeakFilename[0] != 0 && 
		(pPreWorld->m_WorldInfoFlags & WIF_VISBSP))
	{
		//create the name of the leak file
		char pszVisLeakName[MAX_PATH + 10];
		strncpy(pszVisLeakName, g_pGlobs->m_LeakFilename, MAX_PATH);
		strncat(pszVisLeakName, "_Vis.lek", MAX_PATH + 10);

		//inform the user we are creating a file, and where it will be located
		DrawStatusText(eST_Normal, "Making VisBSP leak file: %s", pszVisLeakName);

		//generate the leak
		MakeLeakFile(pPreWorld, pszVisLeakName);
	}

	return true;
}





