
#ifndef __CREATEVISBSP_H__
#define __CREATEVISBSP_H__


	#include "preworld.h"
	#include "findworldmodel.h"


	// Generate the visibility BSP for the world.
	bool CreateVisBSP(
		CEditRegion *pRegion,
		CWorldModelDef *pWorldModel,
		CPreWorld *pWorld);

	// Setup leaves for the vis BSP. This is done differently than a normal one
	// because it adds polies from terrains and the physics BSP into the leaves.
	bool MakeVisBSPLeaves(CPreWorld *pPreWorld);


#endif


