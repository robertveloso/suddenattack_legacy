#include "PreWorld.h"
#include "RPDManager.h"
#include "CompressedIO.h"
#include "FileMarker.h"
#include "streamsim.h"
#include "lightmap_planes.h"
#include "preleaf.h"
#include "prelightmap.h"
#include "preportal.h"
#include "CreatePhysicsAABB.h"
#include "parse_world_info.h"
#include "processing.h"
#include "prepolyfragments.h"



// 33 - Added multiple world models
// 34 - Compressed vis lists
// 35 - Added pPoly->m_CompositeX and pPoly->m_CompositeY
// 36 - Lightmaps from 1 byte to 2 bytes
// 37 - World names are saved now (used for WorldModels .. their name = their object name).
// 38 - Surface detail level saved out.
// 39 - Added a bounding box to the WorldBsp structure (mainly for WorldModel info..)
// 40 - Added the tree depth for each BSP
// 42 - Fixed propLen in the object list.
// 43 - Added surface effects.
// 44 - Changed surface effects to strings.
// 45 - Removed world vertex colors and made them per-poly and removed edge index per vertex.
// 46 - Added lightmapping info like total lightmap data size.
// 47 - Added surface shade.
// 48 - Added texture flags to the surfaces.
// 49 - Added portals.
// 50 - Added position extents (calculated in processing.cpp).
// 51 - Surface m_Flags WORD to DWORD.
// 52 - Added leaf vis dist.
// 53 - Added TextureP and TextureQ (lightmap system is P and Q).
// 54 - Added the EllipseBSPs.
// 55 - Added m_pInfoString.
// 56 - Added header dwords and vertex flags.
// 57 - Added some dummy dwords at the start of the file and added m_LMGridSize.
// 58 - Added vertex normals.
// 59 - Added the portal list.
// 60 - World tree restructuring
// 61 - Lightmap animations
// 62 - Poly animref lists.
// 63 - Shadow maps.
// 64 - Removed Surface::P and Surface::Q.
// 65 - Changed leaf touch lists to point at polies.
// 66 - Added visibility and physics BSPs. Made touch lists store world index and poly index.
// 67 - Added 24 bit lightmaps and per surface LM Grid Sizes
// 68 - Added the new physics data
// 69 - Added the light table
// 70 - Made the surfaces save out and load in their ambient light values
// 71 - Added the light penetration scale saving and loading to surfaces
// 72 - Added the polygon fragment information and texture dimensions
// 73 - Added saving and loading of the blocker polygons
// 74 - Added vertex alpha
// 75 - Added support for texture effects
// 75 - Added light group support
// 77 - Removed the surface effects
// 78 - Added blind object data
// 79 - Removed anim indices and some other dead fields
// 80 - Removed the surface shade
// 81 - Removed blind object data
// 82 - Added multiple texture support
// 83 - Upped vertex indices to be 32 bit instead of 16
// 84 - Added polygon names
// 85 - Added support for a world offset to the pre main world
#define CURRENT_RPD_VERSION	85

//*********************************************************************************************
// Misc
//*********************************************************************************************

//given a filename, it will produce the appropriate filename for the RPD,
void CRPDManager::GetFilename(const char* pszInFileName, char* pszOutFileName, uint32 nLen)
{
	ASSERT(pszInFileName);
	ASSERT(pszOutFileName);

	int32 nStrLen = strlen(pszInFileName);
	int32 nPeriodPos = nStrLen;

	for(int32 nCurrPos = nStrLen; nCurrPos >= 0; nCurrPos--)
	{
		//see if we hit a period
		if(pszInFileName[nCurrPos] == '.')
		{
			nPeriodPos = nCurrPos + 1;
			break;
		}
		//see if we hit a slash
		if((pszInFileName[nCurrPos] == '\\') || (pszInFileName[nCurrPos] == '/'))
		{
			break;
		}
	}

	uint32 nCopyLen = LTMIN(nLen, (uint32)nPeriodPos);

	//copy over the original string
	strncpy(pszOutFileName, pszInFileName, nCopyLen);

	//make sure that there is an end of string
	pszOutFileName[nCopyLen - 1] = '\0';

	//now tack on the extension
	strncat(pszOutFileName, ".rpd", nLen);

	//success
}

//*********************************************************************************************
// Saving
//*********************************************************************************************

//utility macro to save a vector out to a file
#define SaveVector(file, type, vec) \
	{\
		(file) << (type)((vec).x);\
		(file) << (type)((vec).y);\
		(file) << (type)((vec).z);\
	}


//function to check the validity of the nodes before saving it out
void CheckNodes(CNode *pRoot)
{
	if(pRoot == NODE_IN || pRoot == NODE_OUT)
		return;

	ASSERT(pRoot->m_Sides[0] != pRoot && pRoot->m_Sides[1] != pRoot);
	CheckNodes(pRoot->m_Sides[0]);
	CheckNodes(pRoot->m_Sides[1]);
}

static bool WriteWorldBspData( CPreWorld* pWorld, CAbstractIO &file )
{
	uint32					i, k, curPoly;
	uint32					index;
	CPrePoly				*pPoly;
	CPreLeaf				*pLeaf;
	CMoArray<char*>			texNames;
	uint32					namesLen;

	uint32						nPointsUsed;
	CMoByteArray				pointsUsed;
	CMoDWordArray				indexMap;
	GPOS						pos;
	CPrePlane					*pPlane;
	CPreSurface					*pSurface;
	CNode						*pNode;
	PrePortal					*pPortal;

	uint32						nUsedSurfaces;
	PVector						min, max;
	CPrePoly					*pTouching;
	LPOS						lPos;
	PreLeafList					*pLeafList;
	PVector						minBox, maxBox, center, dims;
	CMoArray<PVector>			vertexNormals;
	BYTE r, g, b, a;
	
	// Only the visibility BSP should have leaves.
	if(!(pWorld->m_WorldInfoFlags & WIF_VISBSP))
	{
		ASSERT(pWorld->m_Leafs.GetSize() == 0);
	}

	file << (uint32)0 << (uint32)0 << (uint32)0 << (uint32)0;
	file << (uint32)0 << (uint32)0 << (uint32)0 << (uint32)0;

	file << pWorld->m_WorldInfoFlags;
	file << (uint32)pWorld->GetTreeDepth();

	pWorld->m_TextureNames.Term();
	pWorld->m_TextureNames.SetCacheSize( 100 );
	pWorld->GetTextureNames( pWorld->m_TextureNames );
	
	SetNodeIndices(pWorld->m_Nodes);
	CheckNodes(pWorld->m_RootNode);

	pWorld->SetupIndexMap( pointsUsed, indexMap, nPointsUsed );
	pWorld->m_nPointsSaved = nPointsUsed;
	
	// Setup vertex normals.
	pWorld->GenerateVertexNormals(indexMap, vertexNormals);

	// Write the name (mainly used for WorldModels).
	file.WriteString(pWorld->m_WorldName);


	// Tells where the client data is.
	file << nPointsUsed;

	file << (uint32)pWorld->m_Planes.GetSize();
	
	nUsedSurfaces = pWorld->TagUsedSurfaces();
	file << nUsedSurfaces;

	file << pWorld->m_Portals.GetSize();
	file << pWorld->m_Polies.GetSize();
	file << pWorld->m_Leafs.GetSize();
	file << pWorld->NumPolyVerts();
	file << pWorld->TotalVisListSize();
	file << pWorld->TotalNumLeafLists();

	file << pWorld->m_Nodes.GetSize();	
	file << pWorld->TotalNumTouching();

	// Setup the bounding box and save it off.
	pWorld->GetBoundingBox(&min, &max);

	file << (float)min.x << (float)min.y << (float)min.z;
	file << (float)max.x << (float)max.y << (float)max.z;

	file << (float)pWorld->m_WorldTranslation.x;
	file << (float)pWorld->m_WorldTranslation.y;
	file << (float)pWorld->m_WorldTranslation.z;


	// Save out texture name stuff.
	namesLen = 0;
	for( i=0; i < pWorld->m_TextureNames; i++ )
		namesLen += strlen(pWorld->m_TextureNames[i])+1;

	file << namesLen;
	file << pWorld->m_TextureNames.GetSize();

	for( i=0; i < pWorld->m_TextureNames; i++ )
		file.Write( pWorld->m_TextureNames[i], strlen(pWorld->m_TextureNames[i])+1 );	


	// Give each poly its index.
	pWorld->SetPolyWorldIndices();
	pWorld->SetPolyIndices();
	
	// Give each plane its indices.
	pWorld->SetPlaneIndices();

	// Give each leaf its index.
	pWorld->SetLeafIndices();

	// Write out the number of indices for polies.
	for(pos = pWorld->m_Polies; pos; )
	{
		file << (BYTE)pWorld->m_Polies.GetAt(pos)->NumVerts();
		file << (BYTE)pWorld->m_Polies.GetNext(pos)->m_TVertices.GetSize();
	}


	// Save out the leaf nodes.
	for( i=0; i < pWorld->m_Leafs; i++ )
	{
		pLeaf = pWorld->m_Leafs[i];

		// Write the vis list info.
		if(pLeaf->m_VisList.m_iDuplicateLeaf == 0xFFFF)
		{
			file << (WORD)pLeaf->m_VisList.m_LeafLists.GetSize();
			for(k=0; k < pLeaf->m_VisList.m_LeafLists; k++)
			{
				pLeafList = pLeaf->m_VisList.m_LeafLists[k];

				file << (WORD)pLeafList->m_iPortal;
				file << (WORD)pLeafList->m_LeafList.GetSize();
				file.Write(pLeafList->m_LeafList.GetArray(), pLeafList->m_LeafList.GetSize());
			}
		}
		else
		{
			file << (WORD)0xFFFF;
			file << pLeaf->m_VisList.m_iDuplicateLeaf;
		}
	
		// Write the list of touching polies.
		file << pLeaf->m_Touching.GetSize();
		for(lPos=pLeaf->m_Touching; lPos; )
		{
			pTouching = pLeaf->m_Touching.GetNext(lPos);
			file << (WORD)pTouching->m_WorldIndex << (WORD)pTouching->m_Index;
		}

		file << (float)pLeaf->m_MaxVisDist;
	}	


	// Write out the planes.
	for( pos = pWorld->m_Planes.GetHeadPosition(); pos; )
	{
		pPlane = pWorld->m_Planes.GetNext( pos );

		SaveVector( file, float, pPlane->m_Normal );
		file << (float)pPlane->m_Dist;
	}


	// Save out the surfaces.
	for( pos = pWorld->m_Surfaces.GetHeadPosition(); pos; )
	{
		pSurface = pWorld->m_Surfaces.GetNext( pos );
	
		if(pSurface->m_bUsed)
		{
			for(uint32 nCurrTex = 0; nCurrTex < CPreSurface::NUM_TEXTURES; nCurrTex++)
			{
				CPreTexture& Tex = pSurface->m_Texture[nCurrTex];

				SaveVector( file, float, Tex.m_TextureO );
				SaveVector( file, float, Tex.m_TextureP );
				SaveVector( file, float, Tex.m_TextureQ );
				file << pWorld->FindTextureName( Tex.m_pTextureName, pWorld->m_TextureNames );

				//write out the texture dimensions
				file << Tex.m_TextureWidth;
				file << Tex.m_TextureHeight;
				file << Tex.m_TextureFlags;
			}

			//file << (uint32)m_Planes.FindElement( pSurface->m_pPlane );
			file << (uint32)pSurface->m_pPlane->GetIndex();
			file << (uint32)pSurface->m_Flags;
			file << (BYTE)pSurface->m_DetailLevel;
			file << (uint8)pSurface->GetLMGridSize();

			//write out the light penetration scale
			file << pSurface->m_fLightPenScale;

			//write out the ambient light
			file << pSurface->m_Ambient[0];
			file << pSurface->m_Ambient[1];
			file << pSurface->m_Ambient[2];

			if(pSurface->m_pTextureEffect)
			{
				file << (BYTE)1;
				file.WriteString(pSurface->m_pTextureEffect);
			}
			else
			{
				file << (BYTE)0;
			}
		}
	}



	// Save the polygons.
	curPoly = 0;
	for(pos = pWorld->m_Polies; pos; )
	{
		pPoly = pWorld->m_Polies.GetNext(pos);
	
		SaveVector( file, CReal, pPoly->PolyO() );
		file << pPoly->m_LMWidth << pPoly->m_LMHeight;
		
		// Surface index.
		file << (uint32)pPoly->GetSurface()->m_UseIndex;

		// Write the vertices.
		for(k=0; k < pPoly->NumVerts(); k++)
		{
			//safety check...
			assert(indexMap[pPoly->Index(k)] < ((2 << 16) - 1));
			file << (uint32)indexMap[pPoly->Index(k)];
			r = (uint8)pPoly->Color(k).x;
			g = (uint8)pPoly->Color(k).y;
			b = (uint8)pPoly->Color(k).z;
			a = (uint8)pPoly->Alpha(k);
			file << r << g << b << a;
		}

		for(k=0; k < pPoly->m_TVertices; k++)
		{
			assert(indexMap[pPoly->m_TVertices[k].m_Index] < ((2 << 16) - 1));
			file << (uint32)indexMap[pPoly->m_TVertices[k].m_Index];
			r = (uint8)pPoly->m_TVertices[k].m_Color.x;
			g = (uint8)pPoly->m_TVertices[k].m_Color.y;
			b = (uint8)pPoly->m_TVertices[k].m_Color.z;
			a = (uint8)pPoly->m_TVertices[k].m_Alpha;
			file << r << g << b << a;
		}

		//Write out the fragments if appropriate
		if(pPoly->GetFragments())
		{
			CPrePolyFragments* pFragments = pPoly->GetFragments();

			file << (uint32)pFragments->m_aVertices.size();
			file << (uint32)pFragments->m_aIndices.size();

			// Write out the vertices
			CPrePolyFragments::TVertList::const_iterator iCurVert = pFragments->m_aVertices.begin();
			for (; iCurVert != pFragments->m_aVertices.end(); ++iCurVert)
			{
				SaveVector(file, float, iCurVert->m_vPos);
				SaveVector(file, float, iCurVert->m_vNormal);
				SaveVector(file, float, iCurVert->m_vColor);
			}

			// Write out the triangles
			CPrePolyFragments::TIndexList::const_iterator iCurIndex = pFragments->m_aIndices.begin();
			for (; iCurIndex != pFragments->m_aIndices.end(); ++iCurIndex)
			{
				file << *iCurIndex;
			}
		}
		else
		{
			file << (uint32)0 << (uint32)0;
		}

		// Write the polygon name
		file << (uint32)pPoly->m_nName;


		++curPoly;
	}


	// Save the trees (save the whales, save the earth?).
	for(pos = pWorld->m_Nodes; pos; )
	{
		pNode = pWorld->m_Nodes.GetNext(pos);
		
		//index = m_Polies.FindElement( pNode->m_pPoly );
		index = pNode->m_pPoly->m_Index;
		ASSERT( index != BAD_INDEX );
		file << index;
		ASSERT(index < pWorld->m_Polies.GetSize());
		
		file << (WORD)(pNode->m_pLeaf ? pNode->m_pLeaf->m_Index : INVALID_NODELEAF_INDEX);
		
		file << NodeToIndex(pNode->m_Sides[0]);
		file << NodeToIndex(pNode->m_Sides[1]);
	}


	// Save the portals.
	for(pos = pWorld->m_Portals; pos; )
	{
		pPortal = pWorld->m_Portals.GetNext(pos);
		
		file.WriteString(pPortal->m_pName);
		file << (uint32)0; // Flags..
		
		file << (WORD)pPortal->m_pSurface->m_UseIndex;
	
		pWorld->CalcPortalExtents(pPortal, minBox, maxBox);
		center = minBox + (maxBox - minBox) * 0.5f;
		dims = maxBox - center;
		SaveVector(file, float, center);
		SaveVector(file, float, dims);
	}


	// Save m_Points.
	for( i = 0; i < pWorld->m_Points; i++ )
	{
		if( pointsUsed[i] )
		{
			file << (CReal)pWorld->m_Points[i].x;
			file << (CReal)pWorld->m_Points[i].y;
			file << (CReal)pWorld->m_Points[i].z;
			file << vertexNormals[i].x << vertexNormals[i].y << vertexNormals[i].z;
		}
	}


	// Root poly index..
	file << NodeToIndex(pWorld->m_RootNode);

	// Sections, which have been removed, so always write out 0. Note that this
	// should be removed when the file version changes, I just didn't think that
	// this was reason enough to force a recompile of all levels.
	file << (uint32)0;

	return true;
}

// Save a light group to disk
static bool SaveLightGroup(const CPreLightGroup *pLightGroup, CAbstractIO &file)
{
	if (!file.WriteString(pLightGroup->GetName()))
		return false;
	SaveVector(file, float, pLightGroup->GetColor());

	// Save the poly data
	const CPreLightGroup::TPolyDataList &cPolyData = pLightGroup->GetPolyData();
	file << (uint32)cPolyData.size();
	CPreLightGroup::TPolyDataList::const_iterator iCurPoly = cPolyData.begin();
	for (; iCurPoly != cPolyData.end(); ++iCurPoly)
	{
		file << (uint32)iCurPoly->m_hPoly.m_iWorld;
		file << (uint32)iCurPoly->m_hPoly.m_iPoly;

		file << (uint32)iCurPoly->m_aVertexIntensities.size();
		file.Write(iCurPoly->m_aVertexIntensities.begin(), iCurPoly->m_aVertexIntensities.size());

		if (iCurPoly->m_pLightmap)
		{
			file << (uint32)iCurPoly->m_pLightmap->m_Width;
			file << (uint32)iCurPoly->m_pLightmap->m_Height;
			file << (uint32)iCurPoly->m_pLightmap->m_Data.GetSize();
			ASSERT(!iCurPoly->m_pLightmap->m_bShadowMap);
			file.Write(iCurPoly->m_pLightmap->m_Data.GetArray(), iCurPoly->m_pLightmap->m_Data.GetSize());
		}
		else
			file << (uint32)0 << (uint32)0 << (uint32)0;
	}

	// Save the light grid
	const CPreLightGroup::SLightGrid &cLightGrid = pLightGroup->GetLightGrid();
	SaveVector(file, uint32, cLightGrid.m_vOffset);
	SaveVector(file, uint32, cLightGrid.m_vSize);
	file.Write(cLightGrid.m_pSamples, cLightGrid.GetTotalSampleCount());

	return true;
}

//saves the light data out to disk
static bool SaveLightData(CPreMainWorld* pWorld, CAbstractIO &file)
{
	uint32 i, j, nPolyRefs, nFrames, iFrame, iMap, nFrameDataSize;
	uint32 nFramePolyDatas;
	CPreLightAnim *pAnim;
	CPreLightFrame *pFrame;
	CPreLightMap *pMap;


	// Make sure of certain things so we don't crash...

	//only need to validate light data if we generated some, and if vertex lighting
	//is enabled, we didn't generate any
	// Don't actually do this unless we're debugging lightmap problems, as it can get rather long
#if !defined(DIRECTEDITOR_BUILD)
	if (g_pGlobs->m_bValidateLightmaps)
	{
		if(!pWorld->ValidateLightData())
			return false;
	}
#endif // DIRECTEDITOR_BUILD
	
	// Tally up the data size.
	nPolyRefs = nFrames = nFrameDataSize = nFramePolyDatas = 0;
	for(i=0; i < pWorld->m_LightAnims; i++)
	{
		pAnim = pWorld->m_LightAnims[i];

		nPolyRefs		+= pAnim->m_Polies.GetSize();
		nFrames			+= pAnim->m_Frames.GetSize();
		nFrameDataSize	+= pAnim->CalcFrameDataSize();
		nFramePolyDatas += pAnim->m_Polies.GetSize() * pAnim->m_Frames.GetSize();
	}

	file << nPolyRefs;
	file << nFrames;
	file << nFrameDataSize;
	file << nFramePolyDatas;

	// For each animation..
	file << pWorld->m_LightAnims.GetSize();
	for(i=0; i < pWorld->m_LightAnims; i++)
	{
		pAnim = pWorld->m_LightAnims[i];

		file.WriteString(pAnim->m_Name);
		
		file << (BOOL)(pAnim->m_bShadowMap ? TRUE : FALSE);
		file << pAnim->m_Frames.GetSize();
		file << pAnim->m_Polies.GetSize();

		// Save poly refs.
		for(j=0; j < pAnim->m_Polies; j++)
		{
			file << (WORD)pAnim->m_Polies[j].m_iWorld;
			file << (WORD)pAnim->m_Polies[j].m_iPoly;
		}

		// Save frames.
		for(iFrame=0; iFrame < pAnim->m_Frames; iFrame++)
		{
			pFrame = pAnim->m_Frames[iFrame];

			for(iMap=0; iMap < pFrame->m_PolyMaps.GetSize(); iMap++)
			{
				pMap = pFrame->m_PolyMaps[iMap];
				
				file << pMap->m_Data.GetSize();
				file.Write(pMap->m_Data.GetArray(), pMap->m_Data.GetSize());
			}
		}
	}

	//now write out the lighting grid data
	if(pWorld->m_pLightGrid)
	{
		//first write out the XYZ dims
		file << pWorld->m_LightTable.m_LookupSize[0];
		file << pWorld->m_LightTable.m_LookupSize[1];
		file << pWorld->m_LightTable.m_LookupSize[2];

		//now write out the table
		uint32 nSize =	pWorld->m_LightTable.m_LookupSize[0] * 
						pWorld->m_LightTable.m_LookupSize[1] * 
						pWorld->m_LightTable.m_LookupSize[2];

		for(uint32 nCurrCel = 0; nCurrCel < nSize; nCurrCel++)
		{
			SaveVector(file, float, pWorld->m_pLightGrid[nCurrCel]);
		}
	}
	else
	{
		//no light data
		file << (uint32)0 << (uint32)0 << (uint32)0;
	}

	//light groups are next
	file << (uint32)pWorld->m_aLightGroups.size();
	CPreMainWorld::TLightGroupList::const_iterator iCurLG = pWorld->m_aLightGroups.begin();
	for(; iCurLG != pWorld->m_aLightGroups.end(); ++iCurLG)
	{
		if (!SaveLightGroup(*iCurLG, file))
			return false;
	}

	return true;
}

//saves the blocker polygons of the specified main world
static bool WriteWorldBlockerPolys(CAbstractIO& file, CPreMainWorld* pWorld)
{
	//sanity checks
	assert(pWorld);

	//get the number of polygons
	uint32 nNumPolys = pWorld->GetBlockerPolys().GetSize();

	//write out the size
	file << nNumPolys;

	//now we need to save each polygon
	for(uint32 nCurrPoly = 0; nCurrPoly < nNumPolys; nCurrPoly++)
	{
		//get the poly
		CPreBlockerPoly* pPoly = pWorld->GetBlockerPolys()[nCurrPoly];

		//save it
		file << pPoly->NumVerts();

		//save the verts
		for(uint32 nCurrPt = 0; nCurrPt < pPoly->NumVerts(); nCurrPt++)
		{
			SaveVector(file, float, pPoly->Vert(nCurrPt).m_Vec);
		}

		//now save out the plane information
		SaveVector(file, float, pPoly->Normal());
		file << pPoly->Dist();
	}

	return true;
}

// saves the particle blocker polygons of the specified main world
static bool WriteWorldParticleBlockerPolys( CAbstractIO& file, CPreMainWorld* pWorld )
{
	assert( pWorld );

	// get the number of polygons
	uint32 nNumPolys = pWorld->GetParticleBlockerPolys().GetSize();

	// write out the size
	file << nNumPolys;

	// save each polygon
	for( uint32 nCurPoly = 0; nCurPoly < nNumPolys; nCurPoly++ )
	{
		CPreBlockerPoly* pPoly = pWorld->GetParticleBlockerPolys()[nCurPoly];

		file << pPoly->NumVerts();

		// save the verts
		for( uint32 nCurVert = 0; nCurVert < pPoly->NumVerts(); nCurVert++ )
		{
			SaveVector( file, float, pPoly->Vert(nCurVert).m_Vec );
		}

		// save the plane info
		SaveVector( file, float, pPoly->Normal() );
		file << pPoly->Dist();
	}

	return true;
}


//given a filename and a CPreMainWorld, it will save it to disk so that the
//world can be reloaded again from disk
bool CRPDManager::Save(CAbstractIO& file, CPreMainWorld* pWorld, FnRPDManagerDisplay pfnDisplay)
{
	ASSERT(pWorld);
	ASSERT(pfnDisplay);

	uint32 i;
	BOOL bRet;
	DStream *pWrapper;

	// Write a small header.
	uint32 nVersion = CURRENT_RPD_VERSION;
	file << nVersion;

	//create markers for the objects
	CFileMarker objectListMarker(file, FALSE);
	CFileMarker lightDataMarker(file, FALSE);
	CFileMarker physicsDataMarker(file, FALSE);

	file << (uint32)0 << (uint32)0 << (uint32)0 << (uint32)0;
	file << (uint32)0 << (uint32)0 << (uint32)0 << (uint32)0;

	//write out the world infostring
	file << (uint32)strlen(pWorld->m_pInfoString);
	file.Write(pWorld->m_pInfoString, strlen(pWorld->m_pInfoString));
	
	file << (float)pWorld->GetDefaultLMGridSize();

	// Write the position box.
	SaveVector(file, float, pWorld->m_PosMin);
	SaveVector(file, float, pWorld->m_PosMax);

	//write out the world offset
	SaveVector(file, float, pWorld->m_vWorldOffset);

	// WorldTree layout.
	pWrapper = streamsim_AbstractIOWrapper(&file);
	if(!pWrapper)
		return false;

	bRet = pWorld->m_WorldTree.SaveLayout(pWrapper);
	pWrapper->Release();
	
	if(!bRet)
		return false;

	//save the blocker polygons
	CFileMarker EndBlockerPolysPos(file, TRUE);
	if(!WriteWorldBlockerPolys(file, pWorld))
		return false;
	EndBlockerPolysPos.Mark();

	//save the particle blocker polygons
	CFileMarker EndParticleBlockerPolysPos( file, TRUE );
	if( !WriteWorldParticleBlockerPolys( file, pWorld ) )
		return false;
	EndParticleBlockerPolysPos.Mark();

	// Save the sub-worlds.
	file << (uint32)pWorld->m_WorldModels.GetSize();
	for( i=0; i < pWorld->m_WorldModels; i++ )
	{
		CFileMarker endPosMarker(file, TRUE);
		if(!WriteWorldBspData(pWorld->m_WorldModels[i], file))
			return false;
	}

	objectListMarker.Mark();
	SaveObjectList(file, pWorld->m_Objects);

	lightDataMarker.Mark();
	if(!SaveLightData(pWorld, file))
		return false;

	//now save out all the physics data
	physicsDataMarker.Mark();

	return true;

}

//*********************************************************************************************
// Loading
//*********************************************************************************************


//utility to read in a float from a file
static PReal ReadFloat(CAbstractIO &file)
{
	float fVal;

	file >> fVal;
	return fVal;
}

//utility to load in a vector from a file
static LTVector ReadFVec(CAbstractIO &file)
{
	LTVector tempVec;

	file >> tempVec.x >> tempVec.y >> tempVec.z;
	return tempVec;
}

static TVector3<uint32> ReadIVec(CAbstractIO &file)
{
	TVector3<uint32> tempVec;

	file >> tempVec.x >> tempVec.y >> tempVec.z;
	return tempVec;
}

static bool ReadWorldBspData(CPreWorld* pWorld, CAbstractIO &file )
{
	char			texName[MAX_PATH];
	uint32			namesLen, nTextureNames;
	WORD			wIndex;
	BYTE			r, g, b, a;
	uint32			iSurface, nSurfaces, nPortals;
	uint32			nPoints, nPolies, nLeafs, nPolyVerts;
	
	uint32			visListSize, numLeafLists;
	WORD			wLeafVisListSize;

	CPrePlane		*pPlane;
	uint32			nPlanes, nNodes, nTouching;
	BYTE			nVerts, nTVerts;
	uint32			nTouchingLeaf;
	uint32			tempDWord;

	GPOS pos;
	uint32			iNodePoly, i, k;
	WORD			iNodeLeaf;
	CReal			tempDist;
	CPreLeaf		*pLeaf;
	CPrePoly		*pPoly;
	CNode			*pNode;
	CVector			tempVec;
	
	CPreSurface		*pSurface;
	uint32			iPlane;
	BYTE			tempByte;
	CMoArray<CNode*> allocatedNodes;
	int nodeSides[2], rootNode;
	PrePortal *pPortal;

	uint32			curThing;
	CMoArray<CPrePlane*> tempPlanes;
	CMoArray<CPrePoly*> tempPolies;
	CMoArray<CPreSurface*> tempSurfaces;
	float minBox[3], maxBox[3];
	uint32 treeDepth;
	char tempStr[256];
	float tempFloat;
	PreLeafList *pLeafList;
	uint32 dwDummy, nSections;
	uint16 tempWord;
	uint16 nLeafLists;
	CMoArray<CPrePoly*> allocatedPolies;



	file >> dwDummy >> dwDummy >> dwDummy >> dwDummy;
	file >> dwDummy >> dwDummy >> dwDummy >> dwDummy;

	file >> pWorld->m_WorldInfoFlags;
	file >> treeDepth;

	file.ReadString(pWorld->m_WorldName, MAX_WORLDNAME_LEN);

	file >> nPoints >> nPlanes >> nSurfaces >> nPortals >> nPolies >> nLeafs >> nPolyVerts;
	file >> visListSize >> numLeafLists >> nNodes >> nTouching;

	
	// Bounding box..
	file >> minBox[0] >> minBox[1] >> minBox[2];
	file >> maxBox[0] >> maxBox[1] >> maxBox[2];

	// Centering translation..
	pWorld->m_WorldTranslation = ReadFVec(file);


	// Texture names...
	file >> namesLen >> nTextureNames;

	pWorld->m_TextureNames.SetSize( nTextureNames );
	for( i=0; i < nTextureNames; i++ )
	{
		k = (uint32)-1;
		do
		{
			++k;
			file >> texName[k];
		} while( texName[k] != 0 );

		pWorld->m_TextureNames[i] = pWorld->m_StringHolder.AddString(texName);
	}


	// Allocate the nodes.
	allocatedNodes.SetSize(nNodes);
	for(i=0; i < nNodes; i++)
		allocatedNodes[i] = new CNode;


	// Polygon indices.
	allocatedPolies.SetSize(nPolies);
	for(i=0; i < nPolies; i++)
	{
		file >> nVerts;
		file >> nTVerts;
		
		pPoly = CreatePoly(CPrePoly, nVerts, true);
		pPoly->m_TVertices.SetSize(nTVerts);
		
		allocatedPolies[i] = pPoly;
		pWorld->m_Polies.AddTail(pPoly);
	}


	// Read in the leaves.
	pWorld->m_Leafs.SetSize( nLeafs );
	for( i=0; i < pWorld->m_Leafs; i++ )
	{
		pLeaf = new CPreLeaf;
		pWorld->m_Leafs[i] = pLeaf;

		file >> nLeafLists;
		if(nLeafLists == 0xFFFF)
		{
			file >> pLeaf->m_VisList.m_iDuplicateLeaf;
		}
		else
		{
			for(k=0; k < nLeafLists; k++)
			{
				pLeafList = new PreLeafList;
				pLeaf->m_VisList.m_LeafLists.Append(pLeafList);

				file >> tempWord;
				pLeafList->m_iPortal = tempWord;

				file >> wLeafVisListSize;
				pLeafList->m_LeafList.SetSize(wLeafVisListSize);
				file.Read(pLeafList->m_LeafList.GetArray(), wLeafVisListSize);
			}
		}

		file >> nTouchingLeaf;
		pLeaf->m_TempTouchInfo.SetSize(nTouchingLeaf);
		for(k=0; k < nTouchingLeaf; k++)
		{
			file >> pLeaf->m_TempTouchInfo[k].m_WorldIndex >> pLeaf->m_TempTouchInfo[k].m_PolyIndex;
		}

		file >> tempFloat;
		pLeaf->m_MaxVisDist = tempFloat;
	}	
										 

	// Planes.
	curThing = 0;
	tempPlanes.SetSize(nPlanes);
	for( i=0; i < nPlanes; i++ )
	{
		pPlane = new CPrePlane;

		file >> tempVec.x >> tempVec.y >> tempVec.z;
		file >> tempDist;

		pPlane->m_Normal.Init( tempVec.x, tempVec.y, tempVec.z );
		pPlane->m_Dist = tempDist;

		tempPlanes[curThing] = pPlane;
		curThing++;

		pWorld->m_Planes.AddTail( pPlane );
	}


	// Read in the surfaces.
	curThing = 0;
	tempSurfaces.SetSize(nSurfaces);
	for( i=0; i < nSurfaces; i++ )
	{
		pSurface = new CPreSurface;

		for(uint32 nCurrTex = 0; nCurrTex < CPreSurface::NUM_TEXTURES; nCurrTex++)
		{
			CPreTexture& Tex = pSurface->m_Texture[nCurrTex];

			file >> tempVec.x >> tempVec.y >> tempVec.z;		
			Tex.m_TextureO = tempVec;
			
			file >> tempVec.x >> tempVec.y >> tempVec.z;
			Tex.m_TextureP = tempVec;

			file >> tempVec.x >> tempVec.y >> tempVec.z;
			Tex.m_TextureQ = tempVec;

			//setup the inverse texture vector
			Tex.m_InverseTextureP = Tex.m_TextureP / Tex.m_TextureP.MagSqr();
			Tex.m_InverseTextureQ = Tex.m_TextureQ / Tex.m_TextureQ.MagSqr();

			file >> wIndex;
			if( wIndex >= pWorld->m_TextureNames )
				return false;
		
			Tex.m_pTextureName = pWorld->m_TextureNames[wIndex];

			//read in the texture dimensions
			file >> Tex.m_TextureWidth;
			file >> Tex.m_TextureHeight;
			file >> Tex.m_TextureFlags;
		}

		file >> iPlane;
		if( iPlane >= pWorld->m_Planes.GetSize() )
			return false;

		pSurface->m_pPlane = tempPlanes[iPlane];
		file >> ((uint32&)(pSurface->m_Flags));

		SetupLMPlaneVectors(
			SelectLMPlaneVector(pSurface->m_pPlane->m_Normal),
			pSurface->m_pPlane->m_Normal,
			pSurface->P, 
			pSurface->Q);


		pSurface->O = pSurface->m_Texture[0].m_TextureO;

		//setup the inverse lightmap vector
		pSurface->InverseP = pSurface->P / pSurface->P.Dot(pSurface->P);
		pSurface->InverseQ = pSurface->Q / pSurface->Q.Dot(pSurface->Q);

		file >> tempByte;
		pSurface->m_DetailLevel = tempByte;

		file >> pSurface->m_LMGridSize;

		//load in the light penetration scale
		file >> pSurface->m_fLightPenScale;

		//load in the surface ambient
		file >> pSurface->m_Ambient[0] >> pSurface->m_Ambient[1] >> pSurface->m_Ambient[2];

		file >> tempByte;
		if(tempByte == 1)
		{
			file.ReadString(tempStr, sizeof(tempStr)-1);
			pSurface->m_pTextureEffect = pWorld->m_StringHolder.AddString(tempStr);
		}

		tempSurfaces[curThing] = pSurface;
		curThing++;

		pWorld->m_Surfaces.AddTail( pSurface );
	}

	
	// Read in the polygons.
	curThing = 0;
	tempPolies.SetSize(pWorld->m_Polies.GetSize());
	for(pos = pWorld->m_Polies; pos; )
	{
		pPoly = pWorld->m_Polies.GetNext(pos);

		tempPolies[curThing] = pPoly;
		curThing++;

		pPoly->PolyO() = ReadFVec(file);

		file >> pPoly->m_LMWidth >> pPoly->m_LMHeight;

		// Get the surface.
		file >> iSurface;
		if( iSurface >= pWorld->m_Surfaces.GetSize() )
			return false;
		
		pPoly->m_pSurface = tempSurfaces[iSurface];
				
		// Read vertices.							   
		for( k=0; k < pPoly->NumVerts(); k++ )
		{
			file >> tempDWord;
			pPoly->Index(k) = tempDWord;
			file >> r >> g >> b >> a;
			pPoly->Color(k).x = r;
			pPoly->Color(k).y = g;
			pPoly->Color(k).z = b;
			pPoly->Alpha(k)   = a; 
		}
		
		for( k=0; k < pPoly->m_TVertices; k++ )
		{
			file >> tempDWord;
			pPoly->m_TVertices[k].m_Index = tempDWord;
			file >> r >> g >> b >> a;
			pPoly->m_TVertices[k].m_Color.x = r;
			pPoly->m_TVertices[k].m_Color.y = g;
			pPoly->m_TVertices[k].m_Color.z = b;
			pPoly->m_TVertices[k].m_Alpha   = a;
		}

		// Do you have fragments?
		uint32 nFragmentVertices, nFragmentIndices;
		file >> nFragmentVertices;
		file >> nFragmentIndices;

		if(nFragmentVertices || nFragmentIndices)
		{
			//create a new fragment
			CPrePolyFragments *pNewFragment = new CPrePolyFragments;

			pNewFragment->m_aVertices.resize(nFragmentVertices);
			pNewFragment->m_aIndices.resize(nFragmentIndices);

			// Load the vertices
			for (uint32 nCurVert = 0; nCurVert < nFragmentVertices; ++nCurVert)
			{
				pNewFragment->m_aVertices[nCurVert].m_vPos = ReadFVec(file);
				pNewFragment->m_aVertices[nCurVert].m_vNormal = ReadFVec(file);
				pNewFragment->m_aVertices[nCurVert].m_vColor = ReadFVec(file);
			}

			// Load the indices
			for(uint32 nCurIndex = 0; nCurIndex < nFragmentIndices; ++nCurIndex)
			{
				file >> pNewFragment->m_aIndices[nCurIndex];
			}

			//and associate it with the polygon
			ASSERT(pPoly->m_pFragments == NULL);
			pPoly->m_pFragments = pNewFragment;
		}

		// Read the polygon name
		file >> pPoly->m_nName;
	}


	// Read in the nodes.	
	for( i=0; i < nNodes; i++ )
	{
		pNode = allocatedNodes[i];
		pWorld->m_Nodes.AddTail(pNode);

		file >> iNodePoly;
		pNode->m_pPoly = tempPolies[iNodePoly];

		pNode->m_pPlane = pNode->m_pPoly->m_pSurface->m_pPlane;

		file >> iNodeLeaf;
		pNode->m_pLeaf = (iNodeLeaf==INVALID_NODELEAF_INDEX) ? NULL : pWorld->m_Leafs[iNodeLeaf];

		file >> nodeSides[0] >> nodeSides[1];
		pNode->m_Sides[0] = IndexToNode(nodeSides[0], allocatedNodes);
		pNode->m_Sides[1] = IndexToNode(nodeSides[1], allocatedNodes);
	}


	for(i=0; i < nPortals; i++)
	{
		pPortal = new PrePortal;

		pPortal->m_Index = i;

		file.ReadString(tempStr, sizeof(tempStr) - 1);
		pPortal->m_pName = pWorld->m_StringHolder.AddString(tempStr);
		
		file >> tempDWord; // Flags whenever they're used.

		file >> tempWord;
		if(tempWord >= pWorld->m_Surfaces.GetSize()){
			delete pPortal;
			return false;
		}

		pPortal->m_pSurface = tempSurfaces[tempWord];
		pPortal->m_pSurface->m_pPortal = pPortal;

		// Bounding box.
		file >> tempVec.x >> tempVec.y >> tempVec.z;
		file >> tempVec.x >> tempVec.y >> tempVec.z;

		pWorld->m_Portals.Append(pPortal);
	}


	// Read m_Points;
	pWorld->m_Points.SetSize( nPoints );
	for( i=0; i < pWorld->m_Points; i++ )
	{
		file >> tempVec.x >> tempVec.y >> tempVec.z;
		pWorld->m_Points[i].Init( tempVec.x, tempVec.y, tempVec.z );

		file >> tempVec.x >> tempVec.y >> tempVec.z;
	}

	// Now that we've read the points in, set the poly vertex coordinates..
	for(pos = pWorld->m_Polies; pos; )
	{
		pPoly = pWorld->m_Polies.GetNext(pos);

		for(i=0; i < pPoly->NumVerts(); i++)
		{
			pPoly->Pt(i) = pWorld->m_Points[pPoly->Index(i)];
		}
		
		for(i=0; i < pPoly->m_TVertices; i++)
		{
			pPoly->m_TVertices[i].m_Vec = pWorld->m_Points[pPoly->m_TVertices[i].m_Index];
		}
	}


	// Root poly index..
	file >> rootNode;
	pWorld->m_RootNode = IndexToNode(rootNode, allocatedNodes);


	// Load the sections, but there shouldn't be any since terrain is gone now...
	//this should be removed when the format changes, since it is unnecessary
	file >> nSections;

	if(nSections > 0)
		return false;

	return true;
}

static bool LoadLightGroup(CPreLightGroup *pLightGroup, CAbstractIO &file)
{
	// Load the basic stuff
	const uint32 k_nMaxNameLen = 256;

	char aNameBuffer[k_nMaxNameLen];
	if (!file.ReadString(aNameBuffer, k_nMaxNameLen))
		return false;
	pLightGroup->SetName(aNameBuffer);
	pLightGroup->SetColor(ReadFVec(file));

	// Load the poly data
	uint32 nSize;
	file >> nSize;
	CPreLightGroup::TPolyDataList &cPolyData = pLightGroup->GetPolyData();
	cPolyData.reserve(nSize);
	while (nSize--)
	{
		cPolyData.push_back();
		CPreLightGroup_PolyData &cCurPoly = cPolyData.back();
		file >> cCurPoly.m_hPoly.m_iWorld;
		file >> cCurPoly.m_hPoly.m_iPoly;

		uint32 nArraySize;
		file >> nArraySize;
		cCurPoly.m_aVertexIntensities.resize(nArraySize);
		if (!file.Read(cCurPoly.m_aVertexIntensities.begin(), nArraySize))
			return false;

		uint32 nWidth, nHeight;
		file >> nWidth >> nHeight >> nArraySize;
		if (nArraySize)
		{
			cCurPoly.m_pLightmap = new CPreLightMap;
			cCurPoly.m_pLightmap->m_Width = nWidth;
			cCurPoly.m_pLightmap->m_Height = nHeight;
			cCurPoly.m_pLightmap->m_Data.SetSize(nArraySize);
			cCurPoly.m_pLightmap->m_bShadowMap = false;
			if (!file.Read(cCurPoly.m_pLightmap->m_Data.GetArray(), nArraySize))
				return false;
		}
	}

	// Load the light grid
	CPreLightGroup::SLightGrid &cLightGrid = pLightGroup->GetLightGrid();
	cLightGrid.m_vOffset = ReadIVec(file);
	cLightGrid.m_vSize = ReadIVec(file);
	cLightGrid.m_pSamples = new uint8[cLightGrid.GetTotalSampleCount()];
	if (!file.Read(cLightGrid.m_pSamples, cLightGrid.GetTotalSampleCount()))
		return false;

	return true;
}

static bool LoadLightData(CPreMainWorld* pWorld, CAbstractIO &file)
{
	uint32 nPolyRefs, nFrameDataSize, nFrames, mapSize, nFramePolyDatas;
	uint32 nAnims, nPolies, iAnim, iPoly;
	CPreLightFrame *pFrame;
	CPreLightAnim *pAnim;
	CPreLightMap *pMap;
	CMoArray<CPrePoly*> polyPointers;
	WORD iWorld, iFrame, iMap, iPolyIndex;


	file >> nPolyRefs;
	file >> nFrames;
	file >> nFrameDataSize;
	file >> nFramePolyDatas;

	file >> nAnims;
	if( !pWorld->m_LightAnims.SetSizeInit2(nAnims, NULL) ||
		!AllocateArray(pWorld->m_LightAnims, (CPreLightAnim*)NULL))
	{
		DeleteAndClearArray(pWorld->m_LightAnims);
		return FALSE;
	}
	
	for(iAnim=0; iAnim < pWorld->m_LightAnims.GetSize(); iAnim++)
	{
		pAnim = pWorld->m_LightAnims[iAnim];

		if(!file.ReadString(pAnim->m_Name, sizeof(pAnim->m_Name)))
			return false;

		BOOL bTemp;
		file >> bTemp;
		pAnim->m_bShadowMap = !!bTemp;

		file >> nFrames;
		file >> nPolies;

		if(!pAnim->m_Polies.SetSize(nPolies))
		{
			return false;
		}

		if(!polyPointers.SetSize(pAnim->m_Polies.GetSize()))
			return false;

		for(iPoly=0; iPoly < pAnim->m_Polies.GetSize(); iPoly++)
		{
			file >> iWorld;
			file >> iPolyIndex;

			pAnim->m_Polies[iPoly].m_iWorld = iWorld;
			pAnim->m_Polies[iPoly].m_iPoly = iPolyIndex;
		
			polyPointers[iPoly] = pWorld->GetLMPoly(&pAnim->m_Polies[iPoly]);
			if(!polyPointers[iPoly])
				return false;
		}

		if(!pAnim->m_Frames.SetSizeInit2(nFrames, NULL) ||
			!AllocateArray(pAnim->m_Frames, (CPreLightFrame*)NULL))
		{
			DeleteAndClearArray(pAnim->m_Frames);
			return FALSE;
		}

		for(iFrame=0; iFrame < pAnim->m_Frames.GetSize(); iFrame++)
		{
			pFrame = pAnim->m_Frames[iFrame];

			if(!pFrame->m_PolyMaps.SetSize(pAnim->m_Polies.GetSize()) ||
				!AllocateArray(pFrame->m_PolyMaps, (CPreLightMap*)NULL))
			{
				DeleteAndClearArray(pFrame->m_PolyMaps);
				return false;
			}

			for(iMap=0; iMap < pFrame->m_PolyMaps.GetSize(); iMap++)
			{
				pMap = pFrame->m_PolyMaps[iMap];

				pMap->m_bShadowMap = pAnim->m_bShadowMap;
				pMap->m_Width = polyPointers[iMap]->m_LMWidth;
				pMap->m_Height = polyPointers[iMap]->m_LMHeight;

				file >> mapSize;
				if(!pMap->m_Data.SetSize(mapSize))
					return false;

				file.Read(pMap->m_Data.GetArray(), mapSize);
			}
		}
	}

	//load in the light grid data
	uint32 nGridX, nGridY, nGridZ;
	file >> nGridX >> nGridY >> nGridZ;

	//see if there is one
	if(pWorld->m_pLightGrid)
	{
		delete [] pWorld->m_pLightGrid;
		pWorld->m_pLightGrid = NULL;
	}

	uint32 nSize = nGridX * nGridY * nGridZ;
	if(nSize)
	{
		pWorld->m_pLightGrid = new PVector[nSize];

		if(pWorld->m_pLightGrid)
		{
			for(uint32 nCurrCel = 0; nCurrCel < nSize; nCurrCel++)
			{
				pWorld->m_pLightGrid[nCurrCel] = ReadFVec(file);
			}
		}
	}	

	// Get rid of any lightgroups we happen to have loaded
	while (!pWorld->m_aLightGroups.empty())
	{
		delete pWorld->m_aLightGroups.back();
		pWorld->m_aLightGroups.pop_back();
	}

	//load the new lightgroups
	file >> nSize;
	pWorld->m_aLightGroups.reserve(nSize);
	while (nSize--)
	{
		CPreLightGroup *pLightGroup = new CPreLightGroup;
		if (!LoadLightGroup(pLightGroup, file))
		{
			delete pLightGroup;
			return false;
		}
		pWorld->m_aLightGroups.push_back(pLightGroup);
	}

	return true;
}

//called to load in the blocker polygons into the specified world
static bool LoadWorldBlockerPolys(CAbstractIO& file, CPreMainWorld* pWorld)
{
	assert(pWorld);

	CPreMainWorld::TBlockerPolyList& aBlockerPolys = pWorld->GetBlockerPolys();
	aBlockerPolys.RemoveAll();

	//read in the size
	uint32 nNumPolys;
	file >> nNumPolys;

	//now we need to load each polygon
	for(uint32 nCurrPoly = 0; nCurrPoly < nNumPolys; nCurrPoly++)
	{
		//get the number of vertices
		uint32 nNumVerts;
		file >> nNumVerts;

		//create the polygon
		CPreBlockerPoly* pPoly = CreatePoly(CPreBlockerPoly, nNumVerts, true);

		if(pPoly == NULL)
			return false;

		//load the verts
		for(uint32 nCurrPt = 0; nCurrPt < nNumVerts; nCurrPt++)
		{
			pPoly->Vert(nCurrPt).m_Vec = ReadFVec(file);
		}

		//now save out the plane information
		pPoly->Normal() = ReadFVec(file);
		file >> pPoly->Dist();

		aBlockerPolys.Append( pPoly );
	}

	return true;
}

// load in the particle blocker polys
static bool LoadWorldParticleBlockerPolys(CAbstractIO& file, CPreMainWorld* pWorld)
{
	assert(pWorld);

	CPreMainWorld::TBlockerPolyList& aBlockerPolys = pWorld->GetParticleBlockerPolys();
	aBlockerPolys.RemoveAll();

	//read in the size
	uint32 nNumPolys;
	file >> nNumPolys;

	//now we need to load each polygon
	for(uint32 nCurrPoly = 0; nCurrPoly < nNumPolys; nCurrPoly++)
	{
		//get the number of vertices
		uint32 nNumVerts;
		file >> nNumVerts;

		//create the polygon
		CPreBlockerPoly* pPoly = CreatePoly(CPreBlockerPoly, nNumVerts, true);

		if(pPoly == NULL)
			return false;

		//load the verts
		for(uint32 nCurrPt = 0; nCurrPt < nNumVerts; nCurrPt++)
		{
			pPoly->Vert(nCurrPt).m_Vec = ReadFVec(file);
		}

		//now save out the plane information
		pPoly->Normal() = ReadFVec(file);
		file >> pPoly->Dist();

		aBlockerPolys.Append( pPoly );
	}

	return true;
}


//given a filename, it will open that file, and use it to recreate the 
//CPreWorld that was saved to disk
bool CRPDManager::Load(CAbstractIO& file, CPreMainWorld* pWorld, FnRPDManagerDisplay pfnDisplay)
{
	ASSERT(pWorld);
	ASSERT(pfnDisplay);

	uint32		version;
	uint32		nWorldModels;
	LTVector	tempVec;
	char		*pTempStr;
	uint32		strLen, dummyNum, endPos;
	uint32		objectDataPos, lightDataPos, physicsDataPos;
	DStream		*pWrapper;
	bool		bRet;


	pWorld->Term();

	LithTry
	{
		// Read the header.	
		file >> version;

		if( version != CURRENT_RPD_VERSION )
		{
			return false;
		}

		file >> objectDataPos;
		file >> lightDataPos;
		file >> physicsDataPos;

		file >> dummyNum >> dummyNum >> dummyNum >> dummyNum;
		file >> dummyNum >> dummyNum >> dummyNum >> dummyNum;

		//read in the world info string
		file >> strLen;
		pTempStr = new char[strLen+1];
		file.Read(pTempStr, strLen);
		pTempStr[strLen] = 0;
		pWorld->m_pInfoString = pWorld->m_StringHolder.AddString(pTempStr);
		delete pTempStr;
		
		pWorld->m_DefaultLMGridSize = (uint32)ReadFloat(file);

		// Position box.
		pWorld->m_PosMin = ReadFVec(file);
		pWorld->m_PosMax = ReadFVec(file);

		//offset of the world from the source
		pWorld->m_vWorldOffset = ReadFVec(file);

		//use that to init the light table
		pWorld->m_LightTable.InitLightTableSize(&pWorld->m_PosMin,
												&pWorld->m_PosMax,
												ParseLightTableRes(pWorld->m_pInfoString));

		
		// WorldTree layout.
		pWrapper = streamsim_AbstractIOWrapper(&file);
		if(!pWrapper)
			return false;

		bRet = (pWorld->m_WorldTree.LoadLayout(pWrapper)) ? true : false;
		pWrapper->Release();
		
		if(!bRet)
		{
			return false;
		}

		//load in the blocker polygons
		file >> endPos;
		if(!LoadWorldBlockerPolys(file, pWorld))
			return false;

		//load in the particle blocker polygons
		file >> endPos;
		if(!LoadWorldParticleBlockerPolys(file, pWorld))
			return false;

		// Load world models.
		file >> nWorldModels;
		pWorld->m_WorldModels.SetSizeInit2( nWorldModels, NULL );
		
		for( uint32 nCurrWorldModel =0; nCurrWorldModel < nWorldModels; nCurrWorldModel++ )
		{
			pWorld->m_WorldModels[nCurrWorldModel] = new CPreWorld(pWorld);

			file >> endPos;

			if(ReadWorldBspData(pWorld->m_WorldModels[nCurrWorldModel], file ) == false)
			{
				pWorld->Term();
				return false;
			}

			//update the bounding box of that world
			pWorld->m_WorldModels[nCurrWorldModel]->UpdateBoundingBox();
		}

		LoadObjectList( file, pWorld->m_Objects );

		if(!LoadLightData( pWorld, file))
		{
			pWorld->Term();
			return false;
		}

		if(!pWorld->ConvertTempTouchLists())
		{
			pWorld->Term();
			return false;
		}

	}
	LithCatch( CLithException &x )
	{
		pWorld->Term();
		throw x;
	}

	return true;


}

