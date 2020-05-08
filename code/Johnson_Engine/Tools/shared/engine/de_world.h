
// This file defines the DirectEngine world structures.

#ifndef __DE_WORLD_H__
#define __DE_WORLD_H__

    #ifndef __LTBASETYPES_H__
	#include "ltbasetypes.h"
    #endif

    #ifndef __DE_OBJECTS_H__
//	#include "de_objects.h"
    #endif

    #ifndef __DE_SPRITE_H__
//	#include "de_sprite.h"
    #endif

    #ifndef __NEXUS_H__
	#include "nexus.h"
    #endif

    #ifndef __WORLD_TREE_H__
	#include "world_tree.h"
    #endif


	class Surface;
	class WorldBsp;
	class VisitPVSRequest;
	class Node;
    class Leaf;
	class WorldPoly;
    class SharedTexture;

	// Special nodes.
	#ifndef PREPROCESSOR_BUILD	// Preprocessor has a slightly different version of these.
		extern Node *NODE_IN;
		extern Node *NODE_OUT;
	#endif


	// Flags for WorldPoly::m_ClipMask.
	#define POLYCLIPMASK_MASK	0x3F	// First 6 bits..
	#define POLYLMCHANGE_BIT	0x8000


	#define INVALID_NODELEAF_INDEX	0xFFFF


	// WorldData flags.
	#define WD_ORIGINALBSPALLOCED	(1<<0)	// m_pOriginalBsp is ours.
	#define WD_WORLDBSPALLOCED		(1<<1)	// m_pWorldBsp is ours.

	// World info flags for WorldBsp::m_WorldInfoFlags.
	#define WIF_MOVEABLE			(1<<1)	// 1. WorldData::m_pWorldBsp is set so the world model
											//    can be moved and rotated around.
	
	#define WIF_MAINWORLD			(1<<2)	// 1. Preprocessor includes this in its poly stats
											//    and shows detail stats for this world.
											// 2. FLAG_GOTHRUWORLD is checked for world models
											//    with this WorldBsp.
	
	#define WIF_PHYSICSBSP			(1<<4)	// This is the physics BSP (only one of these).
	#define WIF_VISBSP				(1<<5)	// This is the visibility BSP (only one of these).



	// WorldBsp flags.
	#define WBSP_INHERITED			(1<<0)	// We copied all pointers except FastNodes.



	#define MAX_PORTALVIEWNAME_LEN	32
	
	// PortalInfo values.  Use PORTALINFO_MASK to get this part.
	#define PORTALINFO_MASK			0x7FFF
	#define PORTALINFO_NONE			0x7FFF
	#define PORTALINFO_MIRROR		(PORTALINFO_NONE-1)

	// PortalInfo flags.
	#define PORTALINFO_OVERLAY		0x8000	// Overlay the poly's texture.
	

	// SharedTexture flags.  The m_RefCount member is shared by the flags and the refcount.
	#define ST_TAGGED			0x8000	// Used by client when freeing unused textures.
	#define ST_REFCOUNTMASK		0x7FFF


	// Poly IDs:  High 16 bits = world index (0xFFFF for main world),
	//            low 16 bits = node index.


	#define QUEUE_LIGHTMAP		0
	#define QUEUE_GOURAUD		1
	#define NUM_POLYQUEUES		2


	#define MAX_WORLDPOLY_VERTS	40
	#define VERTFLAG_TJUNCTION	1	// This is an extra vertex to fix t-junctions.


	// When planes use special types, these are the indices.
	#define PLANE_POSX		0
	#define PLANE_NEGX		1
	#define PLANE_POSY		2
	#define PLANE_NEGY		3
	#define PLANE_POSZ		4
	#define PLANE_NEGZ		5
	#define PLANE_GENERIC	6
	#define NUM_PLANECODES	7



	// Lightmap stuff.
	#define LIGHTMAP_GRIDSIZE		16


	// Node flags.
	#define NF_IN			1
	#define NF_OUT			2	
	#define NF_POLY			4
	#define NF_INTERNAL		8
	#define NF_WORLDMODEL	16


	#define MAX_WORLDNAME_LEN		64
	
	// This precedes a world's name if it's a special name.
	// (w_FindWorldModel looks for the world differently).
	#define SPECIAL_WORLD_TOKEN		"#$#"


	// Surface flags.
	#define SURF_SOLID				(1<<0)		// Solid.
	#define SURF_NONEXISTENT		(1<<1)		// Gets removed in preprocessor.
	#define SURF_INVISIBLE			(1<<2)		// Don't draw.
	#define SURF_TRANSPARENT		(1<<3)		// Translucent.
	#define SURF_SKY				(1<<4)		// Sky portal.
	#define SURF_BRIGHT				(1<<5)		// Fully bright.
	#define SURF_FLATSHADE			(1<<6)		// Flat shade this poly.
	#define SURF_LIGHTMAP			(1<<7)		// Lightmap this poly.
	#define SURF_NOSUBDIV			(1<<8)		// Don't subdivide the poly.
	#define SURF_HULLMAKER			(1<<9)		// Adds hulls to make PVS better for open areas.
	#define SURF_PARTICLEBLOCKER	(1<<10)		// A poly used to block particle movement
	#define SURF_DIRECTIONALLIGHT	(1<<11)		// This surface is only lit by the GlobalDirLight.
	#define SURF_GOURAUDSHADE		(1<<12)		// Gouraud shade this poly.
	#define SURF_PORTAL				(1<<13)		// This surface defines a portal that can be opened/closed.
	#define SURF_SPRITEANIMATE		(1<<14)		// This surface's texture is animated by a sprite.
	#define SURF_PANNINGSKY			(1<<15)		// This surface has the panning sky overlaid on it.
	#define SURF_PHYSICSBLOCKER     (1<<17)     // A poly used to block player movement
	#define SURF_TERRAINOCCLUDER	(1<<18)		// Used for visibility calculations on terrain.
	#define SURF_RBSPLITTER			(1<<19)		// Split renderblocks with this polygon
	#define SURF_VISBLOCKER			(1<<21)		// Blocks off the visibility tree
	#define SURF_NOTASTEP			(1<<22)		// Don't try to step up onto this polygon
	#define SURF_MIRROR				(1<<23)		// This surface is a mirror
	#define SURF_RECEIVELIGHT		(1<<24)		// Receives light (otherwise it is just the local ambient)
	#define SURF_RECEIVESHADOWS		(1<<25)		// Shadows are cast onto this surface
	#define SURF_RECEIVESUNLIGHT	(1<<26)		// Should sunlight affect this polygon
	#define SURF_UNIQUE				(1<<27)		// Should this surface be considered unique
	#define SURF_SHADOWMESH			(1<<28)		// Receives shadow meshing
	#define SURF_CASTSHADOWMESH		(1<<29)		// Casts shadow mesh shadows
	#define SURF_CLIPLIGHT			(1<<30)		// Clips light (casts shadows)


	// NOTE: The top 3 bits of the surface flags is used for the lm plane index.
	#define LMPLANE_MASK	0xE0000000
	#define LMPLANE_SHIFT	29

	#define MAX_SURFACE_DETAIL_LEVEL	100
	#define MAX_LM_GRID_SIZE			255


	// Helper macros for vertex colors.
	#define COLOR_TO_VECTOR(vec, color)\
		(vec).x = (float)(((color) & 0xFF0000) >> 16);\
		(vec).y = (float)(((color) & 0x00FF00) >> 8);\
		(vec).z = (float)((color) & 0x0000FF);

	#define VECTOR_TO_COLOR(color, r, g, b) \
		(color) = ((uint32)(r) << 16) | ((uint32)(g) << 8) | ((uint32)(b));



	#define ELLIPSENODE_IN	0xFFFF
	#define ELLIPSENODE_OUT	0xFFFE


	typedef enum
	{
		AbstractBsp_WorldBsp=0,
		AbstractBsp_TerrainSection
	} AbstractBspType;



	typedef struct EllipseNode_t
	{
		unsigned short	m_PlaneNum;
		unsigned short	m_Sides[2]; // EllipseNode pointers (ELLIPSENODE_OUT if out, ELLIPSENODE_IN if in).
	} EllipseNode;


	class PolyAnimRef
	{
	public:
		uint16		m_iAnim;	// Which animation.
		uint16		m_iPoly;	// The poly's index in this animation.
	};
	inline void LTStream_Read(ILTStream *pStream, PolyAnimRef &ref)
	{
		*pStream >> ref.m_iAnim;
		*pStream >> ref.m_iPoly;
	}

	struct EllipseBSP
	{
		// These replace the main world plane distances.
		float			*m_PlaneDists;	
		uint32	m_nPlaneDists;	// Same size as the world's planes.
		
		EllipseNode		*m_Nodes;
		uint32	m_nNodes;

		uint32	m_RootNode;
		LTVector			m_EllipseDims;	// Ellipse dimensions.
	}; 


	// A node in the BSP tree.
	class Node
	{
	public:
		
					Node() {Init(0);}

					Node(uint8 flags)
					{
						Init(flags);
					}

		void		Init(uint8 flags)
		{
			m_Flags = flags;
			m_pPoly = 0;
			m_PlaneType = 0;
			m_iLeaf = INVALID_NODELEAF_INDEX;
			m_Sides[0] = m_Sides[1] = 0;
			m_ObjHead.TieOff();
		}

		LTPlane*	GetPlane();
		LTBOOL		HasValidLeaf();
		Leaf*		GetLeaf(WorldBsp *pWorldBsp);

	
	public:

		WorldPoly		*m_pPoly;
		Node			*m_Sides[2];
		
		// Objects..
		CheapLTLink		m_ObjHead;

		uint16			m_iLeaf;	// INVALID_NODELEAF_INDEX if none.
		uint8			m_Flags;
		uint8			m_PlaneType;
	};

	
	class UserPortal
	{
	public:
		char			*m_pName;
		unsigned short	m_iSurface;	// Which surface is this portal from?
		unsigned short	m_Flags;	// Combination of flags in de_codes.
		unsigned short	m_Index;	// Index (for fast reference).
		
		LTVector			m_Center;	// Bounding info for quick rejection.
		LTVector			m_Dims;
	};
	

//	// Used to track animating surfaces.
//	struct SpriteSurf
//	{
//		Surface			*m_pSurface;
//		SpriteSurf		*m_pNext;
//		Sprite			*m_pSprite;
//		SpriteTracker	m_SpriteTracker;
//	};


	class StaticLight : public WorldTreeObj
	{
	public:
						StaticLight() : WorldTreeObj(WTObj_Light)
						{
							dl_TieOff(&m_WorldLink);
							m_WorldLink.m_pData = this;
						}

		virtual void	GetBBox(LTVector *pMin, LTVector *pMax)
		{
			*pMin = m_Pos - LTVector(m_Radius, m_Radius, m_Radius);
			*pMax = m_Pos + LTVector(m_Radius, m_Radius, m_Radius);
		}

		LTLink		m_WorldLink;
		LTVector	m_Pos;
		float		m_Radius;
		LTVector	m_Color; // 0-255
		LTVector	m_Dir; // Normalized direction vector for directional lights
		float		m_FOV; // cos(fov/2)  -1 for omnidirectional lights
		LTVector	m_OuterColor;
	};


	// These represent a poly in a leaf.
	class PolyTouchInfo
	{
	public:
		union
		{
			// Only used during loading.. converted to a pointer later.
			struct
			{
				uint16	m_WorldIndex;
				uint16	m_PolyIndex;
			} m_PolyInfo;
			
			// Used after loading.
			WorldPoly	*m_pPoly;
		} m_TouchInfo;				
	};


	class AbstractBsp
	{
	public:
		
								AbstractBsp();

	
	public:

		// Use this to ask what kind of structure this is.
		virtual AbstractBspType	GetAbstractBspType()=0;
		
		virtual Node*			GetRootNode()=0;

		// Setup an HPOLY given a Node.  Returns INVALID_HPOLY if the Node doesn't come from
		// this world.
		virtual HPOLY			MakeHPoly(Node *pNode)=0;
		
		// Get the poly from the HPOLY.
		virtual WorldPoly*		GetHPoly(HPOLY hPoly)=0;

		virtual Node*			GetNodeList()=0;

		// Get WIF_ flags.
		virtual uint32			GetWorldInfoFlags()=0;

		// Returns TRUE if it's a WorldBsp and if it can contain objects.
		virtual LTBOOL			IsVisContainer()=0;

		// Called by the renderer.  This returns the maximum radius of all the polies
		// from the center of the object.  This is used to clip and trivially reject the object.
		virtual float			GetRenderClipRadius()=0;


	// Common data..
	public:

		// TerrainSection world names are encoded as "#$#<terrain world name>_<section index>"
		// ie: "#$#SomeTerrain_12".
		char					m_WorldName[MAX_WORLDNAME_LEN+1];	// Name of this world.
	};
	

	// Terrain section.
	class TerrainSection : public AbstractBsp
	{
	public:

								TerrainSection();
								~TerrainSection();

		void					Term();

		LTBOOL					Load(WorldBsp *pWorldBsp, ILTStream *pStream, uint32 index);
		
		// Sets up m_RenderClipRadius.
		void					SetupClipRadius();


	// AbstractBsp overrides.
	public:

		virtual AbstractBspType	GetAbstractBspType();
		virtual Node*			GetRootNode();
		virtual HPOLY			MakeHPoly(Node *pNode);
		virtual WorldPoly*		GetHPoly(HPOLY hPoly);
		virtual Node*			GetNodeList();
		virtual uint32			GetWorldInfoFlags();
		virtual LTBOOL			IsVisContainer();
		virtual float			GetRenderClipRadius();

	
	public:
		
		// Where we are in the WorldTree.
		NodePath				m_NodePath;
		
		// The polies in this section.
		CMoArray<WorldPoly*>	m_Polies;
		
		// BSP.
		Node					*m_RootNode;
		CMoArray<Node>			m_Nodes;
		
		// Parent world.
		WorldBsp				*m_pWorld;

		// This MUST be equal to the m_vCenter of the WorldTreeNode it sits on.
		LTVector					m_vCenter;

		// Encloses all m_Polies from m_vCenter.
		float					m_RenderClipRadius;
	};

	// Leaf visibility lists.
	struct LeafList
	{
		unsigned short	m_iPortal;
		unsigned short	m_LeafListSize;
		unsigned char	*m_LeafList;
	};


	// World vertex.
	struct Vertex
	{
		LTVector		m_Vec;
		LTVector		m_Normal;
		unsigned short	m_FrameCode;
	};

	// A world BSP.
	class WorldBsp : public AbstractBsp
	{
	public:

						WorldBsp();
						~WorldBsp();

		void			Clear();
		void			Term();

		// Copy all pointers, and allocate and copy Node list.
		LTBOOL			Inherit(WorldBsp *pOther);

		// Sets up m_RenderClipRadius;
		void			SetClipRadius();

		// Virtual so renderers don't have to link in VisitPVS module.
		virtual void	VisitPVS(VisitPVSRequest *pRequest);


		// Get bounding radius of the world.
		inline float	GetBoundRadius()	{return (m_MaxBox - m_MinBox).Mag();}
		inline float	GetBoundRadiusSqr()	{return (m_MaxBox - m_MinBox).MagSqr();}

		// Delete nodes.
		void			DeleteNodes();
		
		// Return a pointer to the same node in this WorldBsp.
		Node*			GetNodePointer(WorldBsp *pOther, Node *pOtherNode);


	// AbstractBsp implementation.
	public:

		virtual Node*			GetRootNode();
		virtual HPOLY			MakeHPoly(Node *pNode);
		virtual WorldPoly*		GetHPoly(HPOLY hPoly);
		virtual Node*			GetNodeList();
		virtual AbstractBspType	GetAbstractBspType();
		virtual uint32			GetWorldInfoFlags();
		virtual LTBOOL			IsVisContainer();
		virtual float			GetRenderClipRadius();


	protected:

	public:

		uint32			m_WorldInfoFlags;	// Combination of WIF_ flags.
		uint32			m_Flags;			// Combination of WBSP_ flags.

		uint32	m_MemoryUse;	// Total memory usage.

		CMoArray<PolyAnimRef>	m_PolyAnimRefs;
		
		LTPlane			*m_Planes;		// Planes.
		uint32	m_nPlanes;

		Node			*m_Nodes;		// Nodes.
		uint32	m_nNodes;

		Surface			*m_Surfaces;// Surfaces.
		uint32	m_nSurfaces;

		LeafList		*m_LeafLists;	// Leaf lists.
		uint32	m_nLeafLists;

		Leaf			*m_Leafs;		// BSP leaves.
		uint32	m_nLeafs;

		PolyTouchInfo	*m_TouchListData;	// Touch list data for all the polies in the leaves.
		uint32	m_nTouchLinks;
		
		uint32	m_TotalVisListSize;	// Visibility list size for leaves (0 if there aren't any lists).

		Node			*m_RootNode;	// Root node of the tree.

		WorldPoly		**m_Polies;	// Polies.
		uint32	m_nPolies;		

		Vertex			*m_Points;		// Vertices.
		uint32	m_nPoints;

		UserPortal		*m_UserPortals;	// User portals.
		uint32	m_nUserPortals;

		char			*m_TextureNameData;	// The list of texture names used in this world.
		char			**m_TextureNames;
		uint32	m_nTextures;

		LTVector		m_MinBox, m_MaxBox;	// Bounding box on the whole WorldBsp.

		uint32	m_TreeDepth;		// Tree depth (including root node..)

		uint32	m_nEdgeLookups;
		int				m_bHasLightMaps;

		LTVector		m_WorldTranslation;	// Centering translation for WorldModels, so they can always
											// be treated like they're at the origin.

		unsigned char	*m_LightmapData;	// Data space for ALL lightmaps.

		// Index, used for poly references.  The main world always has an index
		// of 0xFFFF.  The WorldModels are indexed into m_WorldModelArray.
		unsigned short	m_Index;

		char			*m_PolyData;		// Data blocks
		uint32			m_PolyDataSize;

		unsigned char	*m_VisListData;

		CMoArray<TerrainSection>	m_Sections;

		// Maximum distance from m_WorldTranslation to all points.
		float			m_RenderClipRadius;
	};


	class WorldData
	{
	public:
						WorldData();
						~WorldData();

		void			Clear();
		void			Term();

		void			SetValidBsp()
		{
			m_pValidBsp = m_pWorldBsp ? m_pWorldBsp : m_pOriginalBsp;
		}

		inline LTBOOL	IsMoveable()
		{
			return !!m_pWorldBsp;
		}


	public:

		// Combination of WD_ flags.
		uint32			m_Flags;
		
		// Unmodified version.  This is always valid.
		WorldBsp		*m_pOriginalBsp;

		// This version is a secondary version that has transformed vertex positions
		// (for object->world collisions).
		// NOTE: this is NULL for non-moving world models.
		WorldBsp		*m_pWorldBsp;

		// This points to m_pWorldBsp unless it's null, otherwise it points to m_pOriginalBsp.
		WorldBsp		*m_pValidBsp;
	};


	// BSP leaf.
	class Leaf
	{
	public:

		LTVector		m_Center;		// Boundaries for polies in this leaf.
		float			m_Radius;		
		
		LeafList		*m_LeafLists;	// All the leaf lists (first one is certain).

		LTLink			m_ObjectList;	// Objects in this leaf (these point at LeafLinks).

		PolyTouchInfo	*m_TouchList;	// All the nodes touching this node.
		uint32	m_nInTouchList;
		
		float			m_MaxVisDist;	// The farthest this leaf can see in its
										// PVS.  This is used for heirarchical culling
										// in the software version.  If this is 0, then
										// the PVS doesn't exist..

		unsigned short	m_CurFrameCode;	// Used during rendering.
		unsigned short	m_nLeafLists;
	};


	class PortalView
	{
	public:
		
		char		m_ViewName[MAX_PORTALVIEWNAME_LEN];
		LTVector	m_ViewPos;
		LTVector	m_ViewAngles;
	};



	// Surface definition (a surface has the properties of a poly
	// that comes from DEdit).
	class Surface
	{
	public:
						Surface()
						{
							m_Nexus.Init(this);
							m_pTexture = NULL;
							m_pPlane = NULL;
							m_Flags = 0;
							m_TextureFlags = 0;
							m_iTexture = 0;
							m_PolyHead = 0;
							m_DetailLevel = 0;
							m_iPortal = PORTALINFO_NONE;
						}

		// Get the lightmap texture vectors (virtual so the renderer can use this).
		virtual void	GetLightmapTextureVectors(LTVector &P, LTVector &Q);

		// Get the lightmap plane index.
		inline uint32	GetLMPlaneIndex()			{return m_Flags >> LMPLANE_SHIFT;}
		inline void		SetLMPlaneIndex(uint32 i)	{m_Flags &= ~LMPLANE_MASK; m_Flags |= (i << LMPLANE_SHIFT);}

		inline uint32	GetFlags()					{return m_Flags;}
		inline void		SetFlags(uint32 i)			{m_Flags &= LMPLANE_MASK; m_Flags |= (i & ~LMPLANE_MASK);}


	public:


		// NOTE: P and Q are the INVERSE LENGTH texture vectors.  Normally, they
		// would be thought of as the size of a pixel (so if P was 2 units long,
		// you'd think a pixel would be 2 pixels, but it's actually 0.5 pixels).
		
		///////////// Don't move this stuff.. the Surfaces are casted to SurfaceData 
		///////////// structures for surface effects 
		LTVector		O, TextureP, TextureQ;	// Texture coordinates.
		
		Nexus			m_Nexus; // Used by video textures.
		SharedTexture	*m_pTexture;	// The texture.
		
		LTPlane			*m_pPlane;	// The plane this surface lies on.
		
		// Flags on this surface.  The 3 MSBs are used for the lightmap texture plane
		// index so those will be set...
		uint32	m_Flags;	

		unsigned short	m_TextureFlags;	// Texture flags from the DTX file.
		unsigned short	m_iTexture;	// Index into the world's texture list.
		unsigned short	m_PolyHead;	// First poly index on this surface (terminated with 0xFFFF).
		unsigned short	m_iPortal;	// PORTALINFO_NONE if not a portal.
		unsigned char	m_DetailLevel;	// The detail level associated with this surface.
	};


// I admit that this is ugly, but I needed to define this in two places to avoid some header dependency problems (it's also defined in DDStructDefines.h). If you're reading this, I truly am sorry. -JE
#ifndef LTRGBColorANDLTRGB
#define LTRGBColorANDLTRGB
	struct LTRGBColor
	{
		union
		{
			LTRGB	rgb;
			uint32	dwordVal;
		};
	};
#endif

	// Forward declaration of the texture queue class
	struct TextureQ;
	// Forward declaration of the file identifier class
	struct FileIdentifier;

	// Environment map types
	#define ENVMAP_NONE 0 // Detail texture
	#define ENVMAP_SIGNED 1 // Signed add or modulate
	#define ENVMAP_ALPHA 2 // Add modulated by texture alpha

	class SharedTexture
	{
	public:
					SharedTexture()
					{
						m_pEngineData = NULL;
						m_pRenderData = NULL;
						m_Link.m_pData = this;
						memset(m_PolyQueues, 0, sizeof(m_PolyQueues));
						m_pFile = NULL;
						m_Nexus.Init(this);
						m_pDetailTexture = NULL;
						m_RefCount = 0;
						m_CurFrameCode = 0;
					}

		inline uint16	GetCurFrameCode()			{return m_CurFrameCode;}
		inline void		SetCurFrameCode(uint16 code)	{m_CurFrameCode = code;}

		inline uint16	GetFlags()					{return (uint16)(m_RefCount & ~ST_REFCOUNTMASK);}
		inline void		SetFlags(uint16 flags)		{ASSERT(!(flags & ST_REFCOUNTMASK)); m_RefCount &= ST_REFCOUNTMASK; m_RefCount |= (flags & ~ST_REFCOUNTMASK);}

		inline uint16	GetRefCount()				{return (uint16)(m_RefCount & ST_REFCOUNTMASK);}
		inline void		SetRefCount(uint16 count)	{ASSERT(!(count & ~ST_REFCOUNTMASK)); m_RefCount &= ~ST_REFCOUNTMASK; m_RefCount |= (count & ST_REFCOUNTMASK);}


	public:
		
		Nexus			m_Nexus;
		void			*m_pEngineData;
		void			*m_pRenderData;
		LTLink			m_Link;
		// For sorting polies by texture.
		#ifdef __PS2
		struct PolyQueue		*m_PolyQueues[NUM_POLYQUEUES];	
		#else
		TextureQ		*m_PolyQueues[NUM_POLYQUEUES];
		#endif
		FileIdentifier	*m_pFile;	// File identifier so the client can load it.
		SharedTexture	*m_pDetailTexture;	// Detail texture if it's got one.
		uint32			m_EnvMap;	// Determines environment map type


	protected:

		uint16			m_CurFrameCode;
		uint16			m_RefCount;	// Ref count and flags.
	};


	// Poly vertex.. contains precalculated texture coordinates.
	// Currently, these are precalculated when the data is loaded, but 
	// when the textures pan around, tu and tv need to be recalculated.
	struct PolyVertex
	{
		Vertex	*m_Vertex;
		float	tu, tv;
		float	lm_tu, lm_tv;
		union
		{
			uint32	m_Color;
			LTRGB	m_RGB; // alpha 0=full fog, 255=no fog
		};
	};

	// Foward declaration of the light link class
	struct LightLink;
	// Forward declaration of the poly queue class
	struct PolyQueue;

	// World poly.
	class WorldPoly
	{
	public:
		
		inline LTPlane*	GetPlane()		{return m_pSurface->m_pPlane;}
		
		// Returns TRUE if the poly has extra vertices for TVertices allocated.
		// If not, then m_TVertices points at m_Vertices.
		inline LTBOOL	HasExtraTVerts(){return m_TVertices != m_Vertices;}
	
		// Number of regular verts..
		inline uint32	NumVerts()		{return m_nVertices;}

		// Returns the number of allocated verts in the m_Vertices array.		
		inline uint32	TotalNumVerts()	{return HasExtraTVerts() ? (m_nVertices+m_nTVertices) : m_nVertices;}

		// Get a vertex's position.
		inline LTVector&	VPos(uint32 i)	{ASSERT(i < TotalNumVerts()); return m_Vertices[i].m_Vertex->m_Vec;}

		// Get/set the clip mask.
		inline uint32	GetClipMask()			{return m_ClipMask & POLYCLIPMASK_MASK;}
		inline void		SetClipMask(uint32 mask)	{m_ClipMask &= ~POLYCLIPMASK_MASK; m_ClipMask |= (mask & POLYCLIPMASK_MASK);}
		
		// Get/set whether the lightmap has changed.  When it has changed, the renderer
		// rebuilds it from any light animations attached to the poly.
		inline uint32	DidLMChange()			{return m_ClipMask & POLYLMCHANGE_BIT;}
		inline void		SetLMChange()			{m_ClipMask |= POLYLMCHANGE_BIT;}
		inline void		ClearLMChange()			{m_ClipMask &= ~POLYLMCHANGE_BIT;}

		inline uint32	GetIndex()				{return m_Index;}
		inline void		SetIndex(uint32 index)	{ASSERT(index <= 0xFFFF); m_Index = (uint16)index;}


	public:

		CheapLTLink		m_RenderLink;	// For lists the renderer puts the poly in.
		float			m_RenderUser;	// User data for renderer.

		// Which light animations can affect this poly.  This is sort of a mirror of the
		// LMAnim::m_Polies array.
		PolyAnimRef		*m_AnimRefs;
		uint32			m_nAnimRefs;

	// Use accessors to get at these.
	protected:
		
		uint16			m_ClipMask;		// Clip mask in the renderer (and stores the LIGHTMAP_CHANGED bit).
		uint16			m_Index;		// This poly's index into its WorldBsp.


	public:
		LTVector	m_Center;
		float		m_Radius;
		
		Surface		*m_pSurface;		// This polygon's surface.
		LightLink	*m_LightLinkHead;	// Lights touching this poly .. used by the renderer.
		PolyQueue	*m_QueueNext;		// For sorting by texture.
		
		LTVector	m_PolyO;		// Texture mapping information.
											// This is actually used in texture mapping equations .. it's
											// an origin that makes P and Q fit the edges of the poly.

		unsigned short	m_SurfaceNext;		// Next poly (index) on the surface.
		unsigned short	m_CurFrameCode;

		void			*m_pLightmapRenderData;
		uint8			m_LightmapWidth;	// Dimensions of the lightmap data for this poly.
		uint8			m_LightmapHeight;
		uint8			m_LightmapXOffset;	// Used by the renderer to track where the lightmap
		uint8			m_LightmapYOffset;	// is in its page.
		
		// This either points at m_Vertices+m_nVertices OR just points at
		// m_Vertices if m_nTVertices==m_nVertices.
		PolyVertex		*m_TVertices;
		
		unsigned short	m_nVertices;		// Vertices.
		unsigned short	m_nTVertices;
		PolyVertex		m_Vertices[3];
	};


	// SurfaceBackup.
	struct SurfaceBackup
	{
		LTVector O, P, Q;
	};


	
	// ----------------------------------------------------------------------------- //
	// Inlines.
	// ----------------------------------------------------------------------------- //

	inline LTPlane* Node::GetPlane()
	{
		return m_pPoly->m_pSurface->m_pPlane;
	}

	inline LTBOOL Node::HasValidLeaf()
	{
		return m_iLeaf != INVALID_NODELEAF_INDEX;
	}

	inline Leaf* Node::GetLeaf(WorldBsp *pWorldBsp)
	{
		if(m_iLeaf < pWorldBsp->m_nLeafs)
			return &pWorldBsp->m_Leafs[m_iLeaf];
		else
			return LTNULL;
	}

#endif  // __DE_WORLD_H__



