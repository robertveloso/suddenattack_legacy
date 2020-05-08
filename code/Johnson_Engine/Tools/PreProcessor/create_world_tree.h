
#ifndef __CREATE_WORLD_TREE_H__
#define __CREATE_WORLD_TREE_H__


	#include "findworldmodel.h"
	#include "preworld.h"


	// Returns TRUE if the poly intersects the node's space.
	bool DoesPolyIntersectNode(CPrePoly *pPoly, WorldTreeNode *pNode);

	// Creates the WorldTree based on the spatial layout of the polies.
	bool CreateWorldTree(WorldTree *pWorldTree, 
		CMoArray<CWorldModelDef*> &worldModels, char *pInfoString);


#endif



