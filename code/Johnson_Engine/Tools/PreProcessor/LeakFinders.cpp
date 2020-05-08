
#include "bdefs.h"
#include "preworld.h"
#include "leakfinders.h"
#include "processing.h"
#include "pregeometry.h"
#include "preleaf.h"
#include "hullpoly.h"

class PathMarker
{
	public:

		void operator=(const PathMarker &other)
		{
			m_Pos=other.m_Pos;
		}

		PVector m_Pos;
};

//------------------------------------------------------------------
// FindShortestPathLeak
// 
// Biven a starting leaf, and ending leaf, and a world, it will
// use dikstra's algorithm to find the shortest (using number of
// leafs it travels through, although distance could be used as
// well) path from the inside to the outside
//
//------------------------------------------------------------------

#define INFINITE_DIST			0xFFFFFFFF


static bool FindShortestLeakPath(	CPreWorld* pWorld, CPreLeaf* pStart, CPreLeaf* pEnd, 
									CMoArray<PathMarker>& FinalPath)
{
	//return value indicating if we found the outside world or not
	bool bRV = false;

	//for looping
	uint32 nCurrLeaf;

	//the weights for each leaf
	CMoArray<DWORD> LeafWeights;

	//keep a list of the markers used to get to each leaf
	CMoArray<PathMarker> ConnectionPts;

	uint32 nNumLeaves = pWorld->m_Leafs.GetSize();

	//set the weight list to be the size of the number of leaves
	LeafWeights.SetSize(nNumLeaves);
	ConnectionPts.SetSize(nNumLeaves);

	// Mark all leaves as unvisited, and initialize the weights to
	// infinite, except for the starting list
	for(nCurrLeaf = 0; nCurrLeaf < nNumLeaves; nCurrLeaf++)
	{
		pWorld->m_Leafs[nCurrLeaf]->m_Flags &= ~LEAF_VISITED;

		if(pWorld->m_Leafs[nCurrLeaf] == pStart)
		{
			LeafWeights[nCurrLeaf]		= 0;
		}
		else
		{
			LeafWeights[nCurrLeaf]		= INFINITE_DIST;
		}
	}

	//now we run the standard dijkstra's algorithm

	//repeat until we have selected the ending node
	do
	{
		uint32 nBestNode	= 0;
		uint32 nBestScore	= INFINITE_DIST;

		//select the best node, meaning that it has the lowest distance
		for(nCurrLeaf = 0; nCurrLeaf < nNumLeaves; nCurrLeaf++)
		{
			//see if we are able to reach it yet, if it has been selected, or
			//if is valid to be the best
			if(	(LeafWeights[nCurrLeaf] == INFINITE_DIST) ||
				(LeafWeights[nCurrLeaf] > nBestScore) ||
				(pWorld->m_Leafs[nCurrLeaf]->m_Flags & LEAF_VISITED) )
			{
				continue;
			}

			//found potentially the best node, save the data
			nBestNode	= nCurrLeaf;
			nBestScore	= LeafWeights[nCurrLeaf];			
		}

		//now we have the best node, we need to see if it is the ending
		if(pWorld->m_Leafs[nBestNode] == pEnd)
		{
			//found the end! Bail out of the loop
			bRV = true;
			break;
		}

		//see if we managed to find one
		if(nBestScore == INFINITE_DIST)
		{
			//no such luck, break out of the loop
			bRV = false;
			break;
		}

		//now we run through all the other leafs that the current leaf can see
		CPreLeaf* pCurrLeaf = pWorld->m_Leafs[nBestNode];

		for(uint32 nNeighbor = 0; nNeighbor < pCurrLeaf->m_HullPolies.GetSize(); nNeighbor++)
		{
			CHullPoly* pPoly = pCurrLeaf->m_HullPolies[nNeighbor];

			if(pPoly->m_pAdjacent && pPoly->m_pAdjacent->m_pLeaf)
			{
				//we need to modify the weight of this leaf
				uint32 nIndex = pPoly->m_pAdjacent->m_pLeaf->m_Index;

				if(LeafWeights[nBestNode] + 1 < LeafWeights[nIndex])
				{
					LeafWeights[nIndex]			= LeafWeights[nBestNode] + 1;
					ConnectionPts[nIndex].m_Pos = pPoly->GetCenter(); 
				}
			}
		}

		//now we add this leaf to the list as long as it isn't the start
		//(since we don't have a connection point to the starting leaf)
		if(pCurrLeaf != pStart)
		{
			//add it to the final path
			//FinalPath.Append(ConnectionPts[nBestNode]);
		}

		//set the touched flag for this node
		pWorld->m_Leafs[nBestNode]->m_Flags |= LEAF_VISITED;

	}
	while(1); //broken out of when no more nodes can be selected or end is found

	//now travel back through the list and build up the final list
	if(bRV)
	{
		CPreLeaf* pCurrLeaf = pEnd;

		while(pCurrLeaf != pStart)
		{
			//add this to the list
			FinalPath.Insert(0, ConnectionPts[pCurrLeaf->m_Index]);

			uint32 nBestNode = 0;
			uint32 nBestDist = INFINITE_DIST;

			//find the closest neighbor
			for(uint32 nNeighbor = 0; nNeighbor < pCurrLeaf->m_HullPolies.GetSize(); nNeighbor++)
			{
				CHullPoly* pPoly = pCurrLeaf->m_HullPolies[nNeighbor];

				if(pPoly->m_pAdjacent && pPoly->m_pAdjacent->m_pLeaf)
				{
					if(LeafWeights[pPoly->m_pAdjacent->m_pLeaf->m_Index] < nBestDist)
					{
						nBestDist = LeafWeights[pPoly->m_pAdjacent->m_pLeaf->m_Index];
						nBestNode = pPoly->m_pAdjacent->m_pLeaf->m_Index;
					}
				}
			}

			//safety check
			if(nBestDist == INFINITE_DIST)
			{
				return false;
			}

			pCurrLeaf = pWorld->m_Leafs[nBestNode];
		}
	}

	//reset all the touched flags
	for(nCurrLeaf = 0; nCurrLeaf < nNumLeaves; nCurrLeaf++)
	{
		pWorld->m_Leafs[nCurrLeaf]->m_Flags &= ~LEAF_VISITED;
	}

	//return the success code
	return bRV;
}



bool MakeLeakFile(CPreWorld *pWorld, const char *pFilename)
{
	FILE					*fp;
	CPreLeaf				*pInsideLeaf, *pOutsideLeaf;
	uint32					i;
	CBaseEditObj			*pInsideObj, *pOutsideObj;
	PVector					pos;
	PathMarker				tempMarker;
	CMoArray<PathMarker>	thePath;


	DrawStatusText(eST_Normal, "Creating leak file %s", pFilename);

	pInsideLeaf = FindInsideLeaf(pWorld, &pInsideObj);
	if(!pInsideLeaf)
	{
		DrawStatusText(eST_Warning, "Couldn't find an inside leaf for leak file.");
		return false;
	}

	pOutsideLeaf = GetObjectLeaf(pWorld, "OutsideDef", &pOutsideObj);
	if(!pOutsideLeaf)
	{
		DrawStatusText(eST_Warning, "Couldn't find an outside leaf. Or the world is sealed correctly");
		return false;
	}


	//get the shortest path
	if(FindShortestLeakPath(pWorld, pInsideLeaf, pOutsideLeaf, thePath) == FALSE)
	{
		// Couldn't find a path.
		DrawStatusText(eST_Warning, "Couldn't find a path from inside to outside.");

		//should we return or generate an empty leak file?
	}

	
	if(fp = fopen(pFilename, "wt"))
	{
		// Add the start and endpoint to the path.
		tempMarker.m_Pos = pInsideObj->GetPos();
		thePath.Insert(0, tempMarker);
		
		tempMarker.m_Pos = pOutsideObj->GetPos();
		thePath.Append(tempMarker);

		for(i=0; i < thePath-1; i++)
		{
			fprintf(fp, "%f %f %f %f %f %f\n", 
				EXPANDVEC(thePath[i].m_Pos), EXPANDVEC(thePath[i+1].m_Pos));
		}

		fclose(fp);
		return true;
	}
	else
	{
		DrawStatusText(eST_Error, "Unable to open leak file %s.", pFilename);
		return false;
	}		
}



