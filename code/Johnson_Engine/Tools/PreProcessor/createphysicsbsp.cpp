
#include "bdefs.h"
#include "createphysicsbsp.h"
#include "processing.h"
#include "editregion.h"
#include "leakfinders.h"
#include "bsphullmaker.h"
// new physics headers
#include "triangle.h"
#include "collision_data.h"
#include "collision_stream.h"
#include "preleaf.h"
#include "preworld.h"
#include "findworldmodel.h"

// Removes all polies with SURF_TERRAINOCCLUDER set.
static void RemoveTerrainOccluders(CPreWorld *pWorld)
{
	GenListPos pos, oldPos;
	CPrePoly *pPoly;


	for(pos=pWorld->m_Polies.GenBegin(); pWorld->m_Polies.GenIsValid(pos); )
	{
		oldPos = pos;
		pPoly = pWorld->m_Polies.GenGetNext(pos);

		if(pPoly->GetSurfaceFlags() & SURF_TERRAINOCCLUDER)
		{
			pWorld->m_Polies.GenRemoveAt(oldPos);
		}
	}
}


// Gather brushes used in the physics BSP.
static void GetPhysicsBSPBrushes(
	GenList<CEditBrush*> &in,
	GenList<CEditBrush*> &mergeBrushes)
{
	GenListPos pos;
	CEditBrush *pBrush;
	uint32 type;

	for(pos=in.GenBegin(); in.GenIsValid(pos); )
	{
		pBrush = in.GenGetNext(pos);
	
		type = GetBrushType(pBrush);
		if((type & (SURF_HULLMAKER | SURF_TERRAINOCCLUDER)) == 0)
		{
			mergeBrushes.GenAppend(pBrush);
		}
	}
}


bool CreatePhysicsBSP(
	CEditRegion *pRegion,
	CWorldModelDef *pWorldModel,
	CPreWorld *pPreWorld,
	bool bRemoveUnseen)
{
	PVector min, max;
	CLinkedList<CEditBrush*> mergeBrushes;


	GetPhysicsBSPBrushes(
		pWorldModel->m_Brushes,
		mergeBrushes);

	GenerateSurfaces(mergeBrushes, pPreWorld);

	// Merge stuff together.
	if(!MergeBrushes(mergeBrushes, pPreWorld))
		return false;

	mergeBrushes.GenRemoveAll();

	// Make the BSP.
	DrawStatusTextIfMainWorld(eST_Normal, "Creating BSP.");

	if( !MakeBsp(pPreWorld, false, false, false) )
	{
		DrawStatusText(eST_Error, "failed to create BSP for world model: %s. Make sure that there are some non-render only brushes beneath this world model.", pPreWorld->m_WorldName);
		return false;
	}							  

	pPreWorld->m_RootNode->Verify();


	// Create a leak file if they want.
	if(g_pGlobs->m_LeakFilename[0] != 0 && 
		(pPreWorld->m_WorldInfoFlags & WIF_MAINWORLD))
	{
		// [kls 01/20/00]
		// NEED LEAVES FOR MakeLeakFile to work, so we'll create them here,
		// and they'll be removed below...
		MakeHulls(pPreWorld, NULL);

		//create the name of the leak file
		char pszPhysLeakName[MAX_PATH + 10];
		strncpy(pszPhysLeakName, g_pGlobs->m_LeakFilename, MAX_PATH);
		strncat(pszPhysLeakName, "_Phys.lek", MAX_PATH + 10);

		//inform the user
		DrawStatusText(eST_Normal, "Making PhysicsBSP leak file: %s", pszPhysLeakName);

		//generate the leak file
		MakeLeakFile(pPreWorld, pszPhysLeakName);
	}


	// Get rid of anything that will never be seen.
	if(bRemoveUnseen)
	{
		DrawStatusTextIfMainWorld(eST_Normal, "Removing unseen geometry.");
		if(!RemoveUnseenPortions(pPreWorld))
			return false;
	}

	// Get rid of all terrain occluders.
	RemoveTerrainOccluders(pPreWorld);


	pPreWorld->RemoveAllLeafs();
	pPreWorld->RemoveAllNodes();
	
	DrawStatusTextIfMainWorld(eST_Normal, "Recreating BSP.");

	if(!MakeBsp(pPreWorld, true, false, true) )
	{
		DrawStatusText(eST_Error, "failed to create BSP for world model: %s. Make sure that there are some non-render only brushes beneath this world model.", pPreWorld->m_WorldName);
		return false;
	}
	
	pPreWorld->m_RootNode->Verify();

	// Get the centerpoint.  The engine actually moves its object to the
	// center when it is created.
	pPreWorld->GetBoundingBox(&min, &max);
	pPreWorld->m_WorldTranslation = min + (max - min) * 0.5f;

	// Clear out its leaves.
	DeleteAndClearArray(pPreWorld->m_Leafs);

	return true;
}

