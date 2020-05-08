
// This module defines the various object structures used by DE.

#ifndef __DE_OBJECTS_H__
#define __DE_OBJECTS_H__


    #ifndef __LTBASEDEFS_H__
	#include "ltbasedefs.h"
    #endif

    #ifndef __DE_SPRITE_H__
	#include "de_sprite.h"
    #endif

    #ifndef __ANIMTRACKER_H__
	#include "animtracker.h"
    #endif

    #ifndef __WORLD_TREE_H__
	#include "world_tree.h"
    #endif


	// ------------------------------------------------------------- //
	// Types and defines.
	// ------------------------------------------------------------- //

	class SharedTexture;
	class LineSystem;
	class SpriteInstance;
	class ObjectMgr;
	class SObjData;
	class TransformMaker;
	class GVPStruct;
	class LTObject;
	class WorldModelInstance;
	class ModelInstance;
	class DynamicLight;
	class CameraInstance;
    class LTParticleSystem;
    class LTPolyGrid;
	class LineSystem;
	class ContainerInstance;
	class Canvas;
	class WorldBsp;
	class WorldData;
	class Node;
	class AbstractBsp;
	struct ObjectCreateStruct;


	#define INVALID_OBJECTID ((unsigned short)-1)

	#define INVALID_SERIALIZEID	0xFFFF


	// Internal object flags (most are only used by the server).
	#define IFLAG_OBJECTGOINGAWAY	(1<<0)	// This object is going away..
	#define IFLAG_MOVING			(1<<1)	// Used while moving objects so it doesn't recursively push objects around.
	#define IFLAG_INWORLD			(1<<2)	// Set in AddObjectToWorld(), cleared in RemoveObjectFromWorld().
	#define IFLAG_INACTIVE			(1<<3)	// Completely inactive (no physics updates or touch notifies).
	#define IFLAG_INACTIVE_TOUCH	(1<<4)	// Inactive (no updates or physics), but gets touch notifies.
	#define IFLAG_AUTOINACTIVE		(1<<5)	// Server automatically deactived the object
	#define IFLAG_APPLYPHYSICS		(1<<6)  // Object has physics applied
	#define IFLAG_HASCLIENTREF		(1<<7)	// This object has a client ref pointing to it.
	#define IFLAG_INSKY				(1<<8)	// Is this object in the sky?

	// Just a helper to see if an object is inactive.
	#define IFLAG_INACTIVE_MASK	(IFLAG_INACTIVE|IFLAG_INACTIVE_TOUCH|IFLAG_AUTOINACTIVE)



	// ------------------------------------------------------------- //
	// Structures.
	// ------------------------------------------------------------- //

    class Leaf;

	// Used between leaves and objects.
	class LeafLink
	{
	public:

		// m_pData always points at the LeafLink.
		LTLink		m_LeafLink, m_ObjectLink;
		LTObject	*m_pObject;
		Leaf		*m_pLeaf;
	};

	class Attachment
	{
	public:

		LTVector			m_Offset;		// Offset (relative to parent's coordinate system).
		LTRotation			m_RotOffset;	// Rotation offset.
		unsigned short		m_ParentID;		// Parent and child object IDs.
		unsigned short		m_ChildID;
		uint32		m_iSocket;		// Model node index (if the parent is not a model, this is -1).
		Attachment			*m_pNext;
	};

	struct ClientData
	{
		// Position interpolation info
		float			m_fAccumulatedTime;
		LTVector		m_LastUpdatePosServer;	// Last position given by server
		LTLink			m_MovingLink;	// Link into the main list of moving objects.

		// Rotation interpolation info
		float			m_fRotAccumulatedTime;
		LTRotation		m_rLastUpdateRotServer;
		LTLink			m_RotatingLink;		// Link into the main list of rotating objects.

		unsigned short	m_ClientFlags;		// Client-side object flags..
		unsigned short	m_Padding;			// 4-byte align.

		void			*m_pUserData;		// User data..
	};


	// All objects sitting in the BSP are LTObjects.
	class LTObject : public WorldTreeObj
	{
	// Virtuals.
	public:

						LTObject();
						LTObject(char objectType);
		virtual			~LTObject();

		virtual void	Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct);
		void			Clear();


	// WorldTreeObj overrides.
	public:

		virtual void	GetBBox(LTVector *pMin, LTVector *pMax);
		virtual void	RemoveFromWorldTree();
		virtual LTBOOL	InsertSpecial(WorldTree *pTree);

		// Is this a client or a server object?
		virtual ClientServerType		GetCSType()		{return (ClientServerType)!!sd;}


	// Overridables.
	public:

		// Sets up the matrix with the object's position, rotation, and scale.
		virtual void	SetupTransform(LTMatrix &mat);

		// Returns the radius used for visibility calculations. The default returns m_Radius.
		virtual float	GetVisRadius()	{return m_Radius;}

		// This is so main world models can't be moved.
		virtual LTBOOL	IsMoveable()	{return LTTRUE;}

		// Returns LTTRUE if it's a WorldModelInstance with WIF_MAINWORLD.
		virtual LTBOOL	IsMainWorldModel()	{return LTFALSE;}


	// Helpers.
	public:

		// Is this object derived from WorldModel?
		inline LTBOOL					HasWorldModel()	{return m_ObjectType==OT_WORLDMODEL || m_ObjectType==OT_CONTAINER;}

		// Cast to certain types of objects.
		inline ModelInstance*		ToModel()			{ASSERT(m_ObjectType==OT_MODEL); return (ModelInstance*)this;}
		inline SpriteInstance*		ToSprite()			{ASSERT(m_ObjectType==OT_SPRITE); return (SpriteInstance*)this;}
		inline WorldModelInstance*	ToWorldModel()		{ASSERT(HasWorldModel()); return (WorldModelInstance*)this;}
		inline DynamicLight*		ToDynamicLight()	{ASSERT(m_ObjectType==OT_LIGHT); return (DynamicLight*)this;}
		inline CameraInstance*		ToCamera()			{ASSERT(m_ObjectType==OT_CAMERA); return (CameraInstance*)this;}
        inline LTParticleSystem*      ToParticleSystem()  {ASSERT(m_ObjectType==OT_PARTICLESYSTEM); return (LTParticleSystem*)this;}
        inline LTPolyGrid*            ToPolyGrid()        {ASSERT(m_ObjectType==OT_POLYGRID); return (LTPolyGrid*)this;}
		inline LineSystem*			ToLineSystem()		{ASSERT(m_ObjectType==OT_LINESYSTEM); return (LineSystem*)this;}
		inline ContainerInstance*	ToContainer()		{ASSERT(m_ObjectType==OT_CONTAINER); return (ContainerInstance*)this;}
		inline Canvas*				ToCanvas()			{ASSERT(m_ObjectType==OT_CANVAS); return (Canvas*)this;}

		// Is it sitting in a node?
		inline LTBOOL		IsInNode()
		{
			return m_pNode != LTNULL;
		}

		// Returns LTTRUE if the object is scaled (ie: its scale is not (1,1,1)).
		inline LTBOOL		IsScaled()
		{
			return m_Scale.x != 1.0f || m_Scale.y != 1.0f || m_Scale.z != 1.0f;
		}


		// Get/Set position and dims.  These automatically update m_MinBox and m_MaxBox.
		inline LTVector*	GetCenter()	{return &m_Pos;}
		inline LTVector&	GetPos()	{return m_Pos;}
		void				SetPos(LTVector pos);

		inline LTVector&	GetDims()	{return m_Dims;}
		void				SetDims(LTVector dims);

		void				SetPosAndDims(LTVector pos, LTVector dims);

		inline float		GetRadius()			{return m_Radius;}
		inline float		GetRadiusSquared()	{return m_Radius * m_Radius;}

		// Updates m_MinBox and m_MaxBox..
		inline void			UpdateBBox()
		{
			m_MinBox = m_Pos - m_Dims;
			m_MaxBox = m_Pos + m_Dims;
			m_Radius = m_Dims.Mag() + 0.1f;
		}


	public:

		ObjectMgr		*m_pObjectMgr;		// The object's mommy

		LTLink			m_LeafList;	// Leaves this object is touching.
									// m_pData points at a DualLink.

		LTLink			m_NodeLink;	// Link on the first crossing node.
		Node			*m_pNode;

		LTLink			m_Link;		// Link for the client.
		uint32	m_Flags;	// The object's flags  (from FLAGS_).
		uint32	m_Flags2;	// The object's flags2 (from FLAGS2_).
		uint32	m_UserFlags;// User flags.

		unsigned char	m_ColorR;	// RGBA color info.
		unsigned char	m_ColorG;
		unsigned char	m_ColorB;
		unsigned char	m_ColorA;

		Attachment		*m_Attachments;	// Objects attached to this one.

		LTRotation		m_Rotation;

		LTVector		m_Scale;	// Scale..

		unsigned short	m_ObjectID;	// The object ID (and its index into CClientStruct::m_ObjectInfos).
									// -1 for client-created objects.

		unsigned short	m_SerializeID;	// Used while loading/saving.  Set to INVALID_SERIALIZEID if not loading/saving.

		char			m_ObjectType;	// Object type (an OT_ number).
		unsigned char	m_BPriority;		// Blocking priority.

		void			*m_pUserData;	// User data..

		// Physics stuff.
		LTVector		m_Velocity;
		LTVector		m_Acceleration;

		float			m_FrictionCoefficient;
		float			m_Mass;					// This object's mass.
		float			m_ForceIgnoreLimitSqr;	// How much pain (squared) can this object take before the engine tells it?

		LTObject		*m_pStandingOn;			// Each object can be 'standing on' one object at a time.
		Node			*m_pNodeStandingOn;

		LTVector		m_MinBox, m_MaxBox;		// Min and max for its bounding box.

		LTLink			m_ObjectsStandingOn;	// The list of objects that are standing on this one.
		LTLink			m_StandingOnLink;

		uint32	m_InternalFlags;	// Internal flags used by the server (IFLAG_).

		ClientData		cd;		// Client-specific data.
		SObjData		*sd;	// Server object data.


	protected:

		LTVector		m_Pos;		// The current position and rotation.
		LTVector		m_Dims;		// Dimensions of this object.

		float			m_Radius;		// Radius approximation.
	};


	// WorldModels.
	class WorldModelInstance : public LTObject
	{
	// Overrides.
	public:

							WorldModelInstance();
							WorldModelInstance(char objectType);
		virtual				~WorldModelInstance();
		virtual void		Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct);
		void				Clear();

		virtual LTBOOL		IsVisContainer();
		virtual LTLink*		GetVisContainerLink(uint32 i);
		virtual void		AddToVis(WorldTreeObj *pObj);
//		virtual void		DoVisQuery(VisQueryRequest *pInfo);
		virtual LTBOOL		InsertSpecial(WorldTree *pTree);
		virtual void		RemoveFromWorldTree();
		virtual LTBOOL		IsMoveable();
		virtual LTBOOL		IsMainWorldModel();

		// Makes an HPOLY given a node.  This has to check both the BSPs in
		// m_pWorldData to see which one the Node is in.
		HPOLY				MakeHPoly(Node *pNode);


	public:

		// Returns LTTRUE if the object wants to sit on a specific node in the WorldTree.
		// In that case, we return LTTRUE in our overridden WorldTreeObj::SpecialInsert().
		LTBOOL				UseNodePath();

		// Returns LTTRUE if m_pOriginalBsp, m_pWorldBsp, and m_pValidBsp are WorldBsps.
		// (virtual so renderer can use it).
		virtual LTBOOL		IsWorldBsp();

		// Just calls thru to m_pOriginalBsp.
		uint32				GetWorldInfoFlags();

		// Sets m_pOriginalBsp, m_pWorldBsp, and m_pValidBsp.
		void				InitWorldData(AbstractBsp *pOriginalBsp, AbstractBsp *pWorldBsp);

		inline AbstractBsp*	GetOriginalBsp()	{return m_pOriginalBsp;}
		inline AbstractBsp*	GetWorldBsp()		{return m_pWorldBsp;}
		inline AbstractBsp*	GetValidBsp()		{return m_pValidBsp;}


	// Vars.
	public:

		// These are taken directly from WorldData, OR, if this is a TerrainSection,
		// they point at the TerrainSection.
		AbstractBsp			*m_pOriginalBsp;
		AbstractBsp			*m_pWorldBsp;
		AbstractBsp			*m_pValidBsp;

		LTLink				m_VisContainerLinks[MAX_OBJ_NODE_LINKS];

		// Identity except for moving BSPs.
		LTMatrix				m_Transform;
		LTMatrix				m_BackTransform;
	};


	// Containers (derived from WorldModel).
	class ContainerInstance : public WorldModelInstance
	{
	// Overrides.
	public:

							ContainerInstance();
		virtual				~ContainerInstance();

		virtual void		Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct);


	// Vars.
	public:
		unsigned short		m_ContainerCode;
	};


	// Cameras.
	class CameraInstance : public LTObject
	{
	// Overrides.
	public:

					CameraInstance();
		virtual		~CameraInstance();

	public:
		int			m_Left, m_Top, m_Right, m_Bottom;
		float		m_xFov, m_yFov;
		int			m_bFullScreen;
		LTVector	m_LightAdd;		// Brighten everything up (values 0-1).  Does nothing when
									// this is 0,0,0.  This just draws a poly over
									// the screen which is SLOW.
	};


	// Dynamic lights.
	class DynamicLight : public LTObject
	{
	// Overrides.
	public:

						DynamicLight();
		virtual			~DynamicLight();

		virtual float	GetRadius(LTObject *pObject)
		{
			return m_LightRadius;
		}


	// Vars.
	public:
		float		m_LightRadius;
	};


	// Model instances.
	class ModelInstance : public LTObject
	{
	// Overrides.
	public:

						ModelInstance();
		virtual			~ModelInstance();

		virtual void	SetupTransformMaker(TransformMaker *pMaker);
        // tf-deprecate
//		virtual void	SetupGVPTime(GVPStruct *pStruct);

		// See if it has this tracker.
		virtual LTBOOL	FindTracker(LTAnimTracker *pTracker, LTAnimTracker** &pPrev);

		virtual float	GetVisRadius() {return GetModel()->m_VisRadius;}

		virtual float	GetRadius()
		{
			return GetModel()->m_VisRadius *
				MAX(m_Scale.x, MAX(m_Scale.y, m_Scale.z));
		}


	// Helpers.
	public:

		Model*			GetModel() {return m_AnimTracker.GetModel();}
		LTBOOL			IsPieceHidden(uint32 index) {return !!(m_HiddenPieces & (1 << index));}

		LTAnimTracker*	GetMainTracker() {return &m_AnimTracker;}

		// Counts the number of AnimTrackers it's using.  Use the linked list to
		// iterate over them.
		uint32			NumAnimTrackers();

		// Get the model filename (returns "" if there is no model).
		const char*		Debug_GetFilename();


	public:

		void			*m_pRealFile;	// If it's using a placeholder (because its real
										// file is loading in the background), this is set.
		union
		{
			SharedTexture	*m_pSkin;
			SharedTexture	*m_pSkins[MAX_MODEL_TEXTURES];
		};

		// The model always has at least one animation tracker.
		LTAnimTracker		m_AnimTracker;

		// The list of animations it's playing.
		// This is linked with their m_Link::m_pNext (null terminated).
		LTAnimTracker		*m_AnimTrackers;


		NodeControlFn	m_NodeControlFn;
		void			*m_pNodeControlUserData;

		uint32			m_HiddenPieces;		// 1 bit for each hidden piece.

		Sprite			*m_pSprites[MAX_MODEL_TEXTURES];			// It might be using a sprite to update the texture frames.
		SpriteTracker	m_SpriteTrackers[MAX_MODEL_TEXTURES];

		// Last position at which this instance was directionally lit
		LTVector		m_LastDirLightPos;
		// Previous amount of directional lighting applied to the model (-1 if never previously lit)
		float			m_LastDirLightAmount;
	};


	// A single particle.
	struct PSParticle
	{
		// These are the only variables currently exposed..
		LTVector	m_Velocity;
		LTVector	m_Color;	// 0-255
		float		m_Alpha;	// 0-1
		float		m_Size;		// Particle size
		PSParticle	*m_pNext;

		// Data not shown..
		LTVector	m_Pos;
		float		m_Lifetime;			// Current lifetime left
		float		m_TotalLifetime;	// Total lifetime (i.e. initial value)

		PSParticle	*m_pPrev;
	};


	// A particle system.
    class LTParticleSystem : public LTObject
	{
	// Overrides.
	public:

                            LTParticleSystem();
        virtual             ~LTParticleSystem();

		virtual void		Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct);


	// Vars.
	public:

		PSParticle		m_ParticleHead;		// Lists of particles.

		StructBank		*m_pParticleBank;	// Where the particles come from.
		SharedTexture	*m_pCurTexture;		// Current texture for particles.

		// Color for the particles for the software renderer.
		unsigned char	m_SoftwareR;
		unsigned char	m_SoftwareG;
		unsigned char	m_SoftwareB;
		unsigned char	m_Padding; // Structure alignment padding..

		Sprite			*m_pSprite;			// It might be using a sprite to update the frames.
		SpriteTracker	m_SpriteTracker;

		LTVector		m_SystemCenter;		// The sphere enclosing the whole particle system.
		float			m_SystemRadius;

		LTVector		m_OldCenter;		// Used in update loops to see if it grew.
		float			m_OldRadius;

		int				m_nParticles;			// Total number of particles in the system.
		int				m_nChangedParticles;	// Number of new or changed particles since the last update.

		LTVector		m_MinPos, m_MaxPos;	// Min and max particle positions.

		float			m_GravityAccel;		// Gravity acceleration.
		float			m_ParticleRadius;	// The size of each particle.

		int				m_nSrcBlend;		// Src blend mode LTBLEND_
		int				m_nDestBlend;		// Dest blend mode LTBLEND_
		uint32	m_psFlags;
	};


	// A line system.
	struct LSLinePoint
	{
		LTVector	m_Pos;
		float		r, g, b, a;
	};

	struct LSLine
	{
		LSLinePoint		m_Points[2];
		LineSystem		*m_pSystem;
		LSLine			*m_pPrev, *m_pNext;
	};

	class LineSystem : public LTObject
	{
	// Overrides.
	public:

							LineSystem();
		virtual				~LineSystem();

		virtual void		Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct);

	// Vars.
	public:
		StructBank	*m_pLineBank;	// Where the lines come from.
		int			m_bChanged;			// Set when lines get added or moved.
		LSLine		m_LineHead;

		LTVector	m_MinPos, m_MaxPos;	// Min and max positions (relative to object position).

		LTVector	m_SystemCenter;		// Centerpoint (relative to object position).
		float		m_SystemRadius;
	};


	// Implemented in SpriteControlImpl.cpp.
	class SpriteControlImpl : public ILTSpriteControl
	{
	public:

		virtual LTRESULT	GetNumAnims(uint32 &nAnims);
		virtual LTRESULT	GetNumFrames(uint32 iAnim, uint32 &nFrames);

		virtual LTRESULT	GetCurPos(uint32 &iAnim, uint32 &iFrame);
		virtual LTRESULT	SetCurPos(uint32 iAnim, uint32 iFrame);

		virtual LTRESULT	GetFlags(uint32 &flags);
		virtual LTRESULT	SetFlags(uint32 flags);

		virtual LTRESULT	GetAnimLength(uint32 &msLen, const uint32 iFrame);
		virtual LTRESULT	GetFrameTextureHandle(HTEXTURE &hTex, const uint32 iAnim, const uint32 iFrame);

		SpriteInstance		*m_pSprite;
	};


	// Sprite instance.
	class SpriteInstance : public LTObject
	{
	// Overrides.
	public:

						SpriteInstance();
		virtual			~SpriteInstance();

		virtual float	GetRadius()
		{
			float baseMax;

			// Should make this a better number!
			//baseMax = (float)sqrt(256*256 + 256*256);
			baseMax = (float)363.0f;
			return baseMax * MAX(m_Scale.x, m_Scale.y);
		}


	public:

		Sprite*			GetSprite() {return m_SpriteTracker.m_pSprite;}


	// Vars.
	public:

		SpriteTracker	m_SpriteTracker;
		uint32	m_ClipperPoly;	// Poly index if this sprite is clipped.
										// INVALID_HPOLY if not clipped.
		SpriteControlImpl	m_SCImpl;
	};


	// PolyGrids!
    class LTPolyGrid : public LTObject
	{
	// Overrides.
	public:

                        LTPolyGrid();
        virtual         ~LTPolyGrid();

		virtual float	GetRadius()
		{
			LTVector vec;

			vec.x = (float)(m_Width >> 1) * m_Scale.x;
			vec.y = 128.0f * m_Scale.y;
			vec.z = (float)(m_Height >> 1) * m_Scale.z;

			return VEC_MAG(vec) + 1.0f;
		}


	// Vars.
	public:
		char			*m_Data;	// The grid data.
		unsigned short	*m_Indices;	// The precalculated index list.

		Sprite			*m_pSprite;			// It might be using a sprite to update the texture frames.
		SpriteTracker	m_SpriteTracker;

		SharedTexture	*m_pEnvMap;

		float			m_xPan, m_yPan;
		float			m_xScale, m_yScale;

		uint32	m_nTris;
		uint32	m_nIndices;

		LTLink			m_LeafLinks;	// All the leaves it's in (LeafLinks).

		uint32	m_Width, m_Height;
		PGColor			m_ColorTable[256];
	};


	class Canvas : public LTObject
	{
	public:

						Canvas();

		virtual float	GetRadius()		{return m_CanvasRadius;}
		virtual float	GetVisRadius()	{return m_CanvasRadius;}


	public:

		CanvasDrawFn	m_Fn;
		void			*m_pFnUserData;

		// Visibility radius (not clipping radius).
		float			m_CanvasRadius;
	};


	// Record type.
	// Used to catalog things that get added to the Register.  The Register is a list of things with unique id's..
	struct LTRecord
	{
		unsigned char	m_nRecordType;
		void *			m_pRecordData;
	};

	#define RECORDTYPE_LTOBJECT	(1<<0)		// Record is a LTObject.
	#define RECORDTYPE_SOUND	(1<<1)		// Record is a Sound.



	// One-time init for an object.
	inline void obj_InitializeObjectNode(LTObject *pObj)
	{
		pObj->m_pNode = LTNULL;
		pObj->m_NodeLink.m_pData = pObj;
		dl_TieOff(&pObj->m_LeafList);
	}


	// One-time module init/de-init.
	void obj_Init();
	void obj_Term();

	// Find the minimum node.
    class FastNode;
	FastNode* obj_FindMinimumNode(FastNode *pRoot, LTVector *pPt, float radius);

	// Note: AddNodeObjToBsp will return NULL if the object is completely outside the BSP.

	// Adds the object to a node and leaf lists.
	Node* obj_AddNodeObjToBsp(WorldBsp *pWorld, LTObject *pObj);

	void obj_RemoveNodeObjFromBsp(LTObject *pObj);

	// Setup the transformation with position, rotation, and scale.
	void obj_SetupTransform(LTObject *pObj, LTMatrix *pMat);

	// Setup the transformation for a WorldModel.
	void obj_SetupWorldModelTransform(WorldModelInstance *pWorldModel);


	// --------------------------------------------------------------------------------- //
	// Inlines.
	// --------------------------------------------------------------------------------- //

    #ifndef __DE_WORLD_H__
	#include "de_world.h"
    #endif


#endif  // __DE_OBJECTS_H__

