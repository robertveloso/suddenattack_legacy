//
//   (c) 1998-1999 Monolith Productions, Inc.  All Rights Reserved
//
// ---------------------------------------------------------------
//

// Defines world structures and functions internal to DirectEngine.

#ifndef __DE_MAINWORLD_H__
#define __DE_MAINWORLD_H__

    #ifndef __WORLD_TREE_H_
	#include "world_tree.h"
    #endif

    #ifndef __DE_WORLD_H__
	#include "de_world.h"
    #endif

    #ifndef __DE_OBJECTS_H__
	#include "de_objects.h"
    #endif

    #ifndef __ILTSTREAM_H__
	#include "iltstream.h"
    #endif

    #ifndef __LIGHT_TABLE_H__
	#include "light_table.h"
    #endif

	

	#define MAX_LIGHTANIMNAME_LEN	32


	// World flags.
	#define WORLD_SERVERLOADED	(1<<0)	// The server loaded this world.
	#define WORLD_VISED			(1<<1)	// The world has vis data.
	#define WORLD_HASLIGHTMAPS	(1<<2)	// Lightmaps have been calculated.

	// Distance between fog grid samples.
	#define FOG_GRID_SIZE		400.0f	
	#define INV_FOG_GRID_SIZE	(1.0f/FOG_GRID_SIZE)

	#define VEC_SCALETWICE(dest, vec1, scale1, vec2, scale2) \
		VEC_MULSCALAR(dest, vec1, scale1);\
		VEC_ADDSCALED(dest, dest, vec2, scale2);

	#define LTRGB_TO_VECTOR(dest, src) \
		(dest).x = (src).rgb.r;\
		(dest).y = (src).rgb.g;\
		(dest).z = (src).rgb.b;


	// Used to get and set HPOLYs.
	#define GET_HPOLY_INDICES(hPoly, worldIndex, polyIndex) \
	{\
		(worldIndex) = (hPoly).m_nWorldIndex; \
		(polyIndex) = (hPoly).m_nPolyIndex; \
	}
   
	#define SET_HPOLY_INDICES(hPoly, worldIndex, polyIndex)\
	{\
		(hPoly).m_nWorldIndex = (worldIndex); \
		(hPoly).m_nPolyIndex = (polyIndex); \
	}
       



	// ---------------------------------------------------------------- //
	// Structures.
	// ---------------------------------------------------------------- //

	typedef enum
	{
		LoadWorld_Ok=0,
		LoadWorld_InvalidVersion=1,
		LoadWorld_InvalidFile=2,
		LoadWorld_InvalidParams,
		LoadWorld_Error // Generic error
	} LoadWorldStatus;


	typedef void (*LoadWorldPingFn)(void *pData);

	class LoadWorldRequest
	{
	public:

								LoadWorldRequest()
								{
									m_pStream = NULL;
									m_PingFn = NULL;
									m_PingFnData = NULL;
								}

		LTBOOL					IsValid()
		{
			return !!m_pStream;
		}
	
	public:

		// Input.
		ILTStream			*m_pStream;
		LoadWorldPingFn		m_PingFn;
		void				*m_PingFnData;
	};

	
	class LMPolyRef
	{
	public:
		uint16	m_iWorld;
		uint16	m_iPoly;
	};


	class LMFramePolyData
	{
	public:
		// The compressed data.
		uint8	*m_pData;
		uint32	m_DataLen;
	};


	class LMFrame
	{
	public:
		// This list is as large as its animation's m_nPolies.
		LMFramePolyData	*m_PolyDatas;
	};


	class LMAnim
	{
	public:
	
					LMAnim();

		// Is this animation on (ie: do polies need to add its effect?)
		inline LTBOOL	IsOn()	{return m_CurFrames[0] < m_nFrames && m_CurFrames[1] < m_nFrames;}
	

	public:
		char		m_Name[MAX_LIGHTANIMNAME_LEN];

		LTBOOL		m_bShadowMap;

		LMFrame		*m_Frames;
		uint32		m_nFrames;
		
		LMPolyRef	*m_Polies;
		uint32		m_nPolies;

		// Where are we in this animation?
		// If m_CurFrames[0] is LMANIMFRAME_NONE, then this animation is off.
		uint32		m_CurFrames[2];
		float		m_fPercent;			// Percentage between the frames (0-1).
		float		m_fBlendPercent;	// How much of the data to blend in (0 = none, 1 = full).
	
		// Lighting info, if we're using shadow maps.
		LTVector	m_vLightPos;
		LTVector	m_vLightColor;	// RGB 0-255.
		float		m_fLightRadius;
	};


	// MainWorld.. stores all the WorldModels and the world structures.
	class MainWorld
	{
	public:

						MainWorld();

		void			Clear();

		// Load the data...
		LoadWorldStatus	Load(LoadWorldRequest *pRequest);
		
		// Used by the client.  Uses as much of pOther's data as possible so the client doesn't
		// have to duplicate lots of extra data (like the polies, light table, etc).
		LTBOOL			Inherit(MainWorld *pOther);

		// Are we loaded/initialized?
		inline LTBOOL	IsLoaded()	{return m_bLoaded;}

		void			Term();

		// Close m_pFile.
		void			TermFile();

		// Sets up m_SkyPortals.
		LTBOOL			BuildSkyPortalList();

		// Adds all the static lights to the WorldTree.
		void			InsertStaticLights(LTLink *pListHead);

		// Calls pInstance->InitWorldModel with AbstractBsps either from WorldDatas
		// or TerrainSection.
		LTBOOL			InitWorldModel(WorldModelInstance *pInstance, char *pWorldName);

		// Load/free lightmap data.
		LTRESULT		ReadLightmapData(ILTStream *pStream);
		void			FreeLightmapData();

		inline uint32	NumLMAnims()			{return m_LMAnims.GetSize();}
		inline LMAnim*	GetLMAnim(uint32 index)	{return &m_LMAnims[index];}

		// Find the specified light animation.  Names are case-insensitive.
		virtual LMAnim* 	FindLMAnim(const char *pName, uint32 *pIndex=NULL);

		// Get a poly given a LMPolyRef.  Returns NULL if the ref is invalid.
		virtual WorldPoly*	GetLMPoly(LMPolyRef *pRef);

		// Flags the animation's polies as changed.
		void				SetLMPoliesChanged(LMAnim *pAnim);

		// Get the WorldData that the HPOLY references.
		WorldData*			GetWorldDataFromHPoly(HPOLY hPoly);

		// Get the poly that the HPOLY references.
		WorldPoly*			GetPolyFromHPoly(HPOLY hPoly);

		// Convert leaf touch lists from indices to pointers.
		LTBOOL				ConvertTouchLists();

		// Get BSPs with WIF_VISBSP and WIF_PHYSICSBSP.
		virtual WorldBsp*	GetVisBSP();
		virtual WorldBsp*	GetPhysicsBSP();

		// Calculate bounding spheres for polies and leaves.
		void				CalcBoundingSpheres();

	
	public:

		// All the light animations..
		CMoArray<LMAnim>	m_LMAnims;

		// Data for the light animations.
		CMoArray<LMPolyRef>	m_LMPolyRefs;
		CMoArray<LMFrame>	m_LMFrames;
		CMoArray<uint8>		m_LMFrameData;
		CMoArray<LMFramePolyData>	m_LMFramePolyDatas;


		// Where in the file is the light data?
		uint32		m_LightDataPos;

		// The top-level search structure the world uses.
		WorldTree	m_WorldTree;

		// NOTE: this list is only valid in the MainWorld it came from.  It is not valid
		// and cannot be iterated in worlds that inherited from this one (since the 
		// StaticLight DLinks point at the originator's list head).
		LTLink		m_StaticLights;

		// Lightmap grid size.
		float		m_LMGridSize;

		// Light table..
		LightTable	m_LightTable;

		// Position box.  All position vectors are parametrized into here.
		LTVector	m_PosMin, m_PosMax;
		LTVector	m_InvPosDiff;

		// Box enclosing everything, + 100.
		LTVector	m_PaddedMinBox, m_PaddedMaxBox;	// Bounding box + 100.

		uint32		m_WorldFlags;

		// The client keeps the world open so it can get lightmaps out.
		ILTStream	*m_pFile;

		// WorldsBSPObjs..
		CMoArray<WorldData*>	m_Worlds;

		// All the sky portals from the worlds.
		CMoArray<WorldPoly*>	m_SkyPortals;

		// Portal views.
		PortalView	*m_PortalViews;
		uint32		m_nPortalViews;

		// Generic info string from World Info in DEdit.
		char		*m_pInfoString;

		// Client's stuff.  Server should not touch this.
		LTLink		m_Link;
		void		*m_RenderContext;

		// Are we currently loaded?  (Between a successful Load() or Inherit() and Term()).
		LTBOOL		m_bLoaded;

		// MainWorlds inherited from us.
		LTLink		m_Inheritances;

		// Our link into another world's m_Inheritances if we're inherited.
		LTLink		m_InheritanceLink;

		// Are we inherited from another world?
		LTBOOL		m_bInherited;
	};


	// ---------------------------------------------------------------- //
	// Load/unload functions.
	// ---------------------------------------------------------------- //
	
	// Just get the info string out of the file.
	LTRESULT w_GetInfoString(ILTStream *pStream, char *pInfoString, uint32 maxLen, uint32 *pActualLen);

	// Remove surface effects.
	void w_TermSurfaceEffects(MainWorld *pWorld);

	WorldData* w_FindWorldModel(MainWorld *pWorld, char *pName);
	
	// Transform the specified world model.  If bPartial is TRUE, it only transforms the
	// root node's center (this can be used for world models using box physics).
	void w_TransformWorldModel(WorldModelInstance *pModel, LTMatrix *pTransform, LTBOOL bPartial);

	// Generates texture coordinates for the poly.
	void w_GenerateTextureCoordinates(WorldPoly *pPoly);

	// Case-sensitive search for a portal by name.
	UserPortal* w_FindPortal(WorldBsp *pWorldBsp, char *pName, uint32 *pPortalIndex=NULL);
	
	// Looks thru all the WorldBSPs.
	UserPortal* w_FindPortal2(MainWorld *pWorld, char *pName, uint32 *pWorldIndex=NULL, uint32 *pPortalIndex=NULL);

	// Find a portal VIEW (case insensitive).
	PortalView* w_FindPortalView(MainWorld *pWorld, const char *pName, uint32 *index=NULL);

	// Format a terrain section's name.
	LTBOOL w_BuildTerrainSectionName(char *pWorldName, uint32 sectionIndex,
		char *pOutput, uint32 outputSize);
	

	// ---------------------------------------------------------------- //
	// Inline functions.
	// ---------------------------------------------------------------- //

	// Get a poly from a poly reference.
	inline WorldPoly* w_GetPoly(MainWorld *pWorld, HPOLY hPoly)
	{
		return pWorld->GetPolyFromHPoly(hPoly);
	}

	inline void w_SetLightTableVal(LightTable *pTable, uint32 x, uint32 y, uint32 z, LTRGBColor *val)
	{
		pTable->m_Lookup[z*pTable->m_XSizeTimesYSize+y*pTable->m_LookupSize[0]+x].dwordVal = val->dwordVal;
	}

	inline void w_GetLightTableVal(LightTable *pTable, uint32 x, uint32 y, uint32 z, LTRGBColor *val)
	{
		val->dwordVal = pTable->m_Lookup[z*pTable->m_XSizeTimesYSize+y*pTable->m_LookupSize[0]+x].dwordVal;
	}

	inline void w_DoLightLookup(LightTable *pTable, LTVector *pPos, LTRGBColor *pRGB)
	{
		LTVector sample, invSample, finalColor;
		int32 gridCoords[3];
		LTVector samples[8], ySamples[2], xySamples[2];
		LTRGBColor *pBase;

		// Figure out which grid point we lie on.
		sample = *pPos - pTable->m_LookupStart;
		sample.x *= pTable->m_InvBlockSize.x;
		sample.y *= pTable->m_InvBlockSize.y;
		sample.z *= pTable->m_InvBlockSize.z;
		
		gridCoords[0] = (int32)sample.x;
		gridCoords[1] = (int32)sample.y;
		gridCoords[2] = (int32)sample.z;
		gridCoords[0] = LTCLAMP(gridCoords[0], 0, pTable->m_LookupSizeMinus1[0]);
		gridCoords[1] = LTCLAMP(gridCoords[1], 0, pTable->m_LookupSizeMinus1[1]);
		gridCoords[2] = LTCLAMP(gridCoords[2], 0, pTable->m_LookupSizeMinus1[2]);

		// Get 0-1 for the sample.
		sample.x = sample.x - (float)floor(sample.x);
		sample.y = sample.y - (float)floor(sample.y);
		sample.z = sample.z - (float)floor(sample.z);
		invSample.x = 1.0f - sample.x;
		invSample.y = 1.0f - sample.y;
		invSample.z = 1.0f - sample.z;

		// Get the 8 box points and bilinear interpolate.
		pBase = &pTable->m_Lookup[gridCoords[2]*pTable->m_XSizeTimesYSize+gridCoords[1]*pTable->m_LookupSize[0]+gridCoords[0]];
		LTRGB_TO_VECTOR(samples[0], pBase[pTable->m_LookupSize[0]]);
		LTRGB_TO_VECTOR(samples[1], pBase[pTable->m_LookupSize[0] + 1]);
		LTRGB_TO_VECTOR(samples[2], pBase[0]);
		LTRGB_TO_VECTOR(samples[3], pBase[1]);
		
		pBase += pTable->m_XSizeTimesYSize;
		LTRGB_TO_VECTOR(samples[4], pBase[pTable->m_LookupSize[0]]);
		LTRGB_TO_VECTOR(samples[5], pBase[pTable->m_LookupSize[0] + 1]);
		LTRGB_TO_VECTOR(samples[6], pBase[0]);
		LTRGB_TO_VECTOR(samples[7], pBase[1]);


		//ySamples[0] = invSample.y*samples[2] + sample.y*samples[0];
		VEC_SCALETWICE(ySamples[0], samples[2], invSample.y, samples[0], sample.y);

		//ySamples[1] = invSample.y*samples[3] + sample.y*samples[1];
		VEC_SCALETWICE(ySamples[1], samples[3], invSample.y, samples[1], sample.y);

		//xySamples[0] = invSample.x*ySamples[0] + sample.x*ySamples[1];
		VEC_SCALETWICE(xySamples[0], ySamples[0], invSample.x, ySamples[1], sample.x);


		//ySamples[0] = invSample.y*samples[6] + sample.y*samples[4];
		VEC_SCALETWICE(ySamples[0], samples[6], invSample.y, samples[4], sample.y);

		//ySamples[1] = invSample.y*samples[7] + sample.y*samples[5];
		VEC_SCALETWICE(ySamples[1], samples[7], invSample.y, samples[5], sample.y);

		//xySamples[1] = invSample.x*ySamples[0] + sample.x*ySamples[1];
		VEC_SCALETWICE(xySamples[1], ySamples[0], invSample.x, ySamples[1], sample.x);

		
		//return (uint8)(invSample.z*xySamples[0] + sample.z*xySamples[1]);
		VEC_SCALETWICE(finalColor, xySamples[0], invSample.z, xySamples[1], sample.z);
		pRGB->rgb.r = (uint8)finalColor.x;
		pRGB->rgb.g = (uint8)finalColor.y;
		pRGB->rgb.b = (uint8)finalColor.z;
	}


	inline Leaf* FindPointInTree(WorldBsp *pWorldBsp, LTVector *pPoint)
	{
		float dot;
		Node *pRoot;
		uint16 iLastLeaf;

		pRoot = pWorldBsp->m_RootNode;
		iLastLeaf = INVALID_NODELEAF_INDEX;
		for(;;)
		{
			if(pRoot->m_Flags & (NF_IN|NF_OUT))
			{
				if(iLastLeaf == INVALID_NODELEAF_INDEX)
					return LTNULL;
				else
					return &pWorldBsp->m_Leafs[iLastLeaf];
			}
			else if(pRoot->m_iLeaf != INVALID_NODELEAF_INDEX)
			{
				iLastLeaf = pRoot->m_iLeaf;
			}
			
			dot = pRoot->GetPlane()->DistTo(*pPoint);
			pRoot = pRoot->m_Sides[dot >= -0.0001f];
		}
	}


	inline WorldData* MainWorld::GetWorldDataFromHPoly(HPOLY hPoly)
	{
		uint32 worldIndex, polyIndex;

		GET_HPOLY_INDICES(hPoly, worldIndex, polyIndex);
		
		if(worldIndex >= m_Worlds.GetSize())
			return LTNULL;

		return m_Worlds[worldIndex];	
	}


	inline WorldPoly* MainWorld::GetPolyFromHPoly(HPOLY hPoly)
	{
		WorldData *pData;

		pData = GetWorldDataFromHPoly(hPoly);
		if(!pData)
			return LTNULL;

		return pData->m_pOriginalBsp->GetHPoly(hPoly);
	}



#endif  // __DE_MAINWORLD_H__



