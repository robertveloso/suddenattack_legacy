#include "bdefs.h"

#include "objectmgr.h"
#include "geomroutines.h"
#include "de_mainworld.h"
#include "animtracker.h"
#include "transformmaker.h"

#include "sysperfcounter.h"
static uint32 g_WorldModelInstanceDoVisQuery = 0;

// Used for the global frame code.. when it wraps around, all object managers
// have their objects reset.
LTLink g_ObjectMgrs(LTLink_Init);


// ------------------------------------------------------------------------- //
// Internal functions.
// ------------------------------------------------------------------------- //

class ConstructorIdent
{
public:
};

inline void* operator new(size_t size, void *pDataPtr, ConstructorIdent *pThing)
{
	return pDataPtr;
}

inline void operator delete(void *pDataPtr, ConstructorIdent *pThing,
	ObjectMgr *pMgr, ObjectCreateStruct *pStruct, void *pDataPtr2)
{
}



// ------------------------------------------------------------------------- //
// ObjectMgr.
// ------------------------------------------------------------------------- //

ObjectMgr::ObjectMgr() :
	m_ObjectBankNormal(32, OBJECT_PREALLOCATIONS),
	m_ObjectBankModel(32, MODEL_PREALLOCATIONS),
	m_ObjectBankWorldModel(32, OBJECT_PREALLOCATIONS),
	m_ObjectBankSprite(32, SPRITE_PREALLOCATIONS),
	m_ObjectBankLight(32, OBJECT_PREALLOCATIONS),
	m_ObjectBankCamera(32, OBJECT_PREALLOCATIONS),
	m_ObjectBankParticleSystem(32, OBJECT_PREALLOCATIONS),
	m_ObjectBankPolyGrid(32, OBJECT_PREALLOCATIONS),
	m_ObjectBankLineSystem(32, OBJECT_PREALLOCATIONS),
	m_ObjectBankContainer(32, OBJECT_PREALLOCATIONS),
	m_ObjectBankCanvas(32, OBJECT_PREALLOCATIONS)
{
	m_CurFrameCode = 0;
	m_InternalLink.Init();
}


uint32 ObjectMgr::GetFrameCode()
{
	return m_CurFrameCode;
}


uint32 ObjectMgr::IncFrameCode()
{
	LTLink *pListHead, *pCur;
	uint32 i;

	if(m_CurFrameCode == 0xFFFFFFFF)
	{
		m_CurFrameCode = 1;

		// Reset the frame code on all the objects.
		for(i=0; i < NUM_OBJECTTYPES; i++)
		{
			pListHead = &m_ObjectLists[i].m_Head;
			for(pCur=pListHead->m_pNext; pCur != pListHead; pCur=pCur->m_pNext)
			{
				((LTObject*)pCur->m_pData)->m_WTFrameCode = 0;
			}
		}
	}
	else
	{
		++m_CurFrameCode;
	}

	return m_CurFrameCode;
}




// ------------------------------------------------------------------------- //
// LTObject.
// ------------------------------------------------------------------------- //

LTObject::LTObject() : WorldTreeObj(WTObj_DObject)
{
	Clear();
	m_ObjectType = OT_NORMAL;
}

LTObject::LTObject(char objectType) : WorldTreeObj(WTObj_DObject)
{
	Clear();
	m_ObjectType = objectType;
}

LTObject::~LTObject()
{
	if(m_pObjectMgr)
	{
		om_RemoveAttachments(m_pObjectMgr, this);
	}

	// Remove it from the moving object list if it's moving.
	if(cd.m_fAccumulatedTime)
	{
		dl_Remove(&cd.m_MovingLink);
	}

	if(cd.m_fRotAccumulatedTime)
	{
		dl_Remove(&cd.m_RotatingLink);
	}

	RemoveFromWorldTree();
}

void LTObject::Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct)
{
	// Get stuff out of the struct.
	m_Flags = pStruct->m_Flags;
	m_Flags2 = pStruct->m_Flags2;
	m_Pos = pStruct->m_Pos;
	m_Rotation = pStruct->m_Rotation;
	m_Scale = pStruct->m_Scale;
	m_ObjectType = (char)pStruct->m_ObjectType;
	m_pObjectMgr = pMgr;
}

void LTObject::Clear()
{
	// Initialize all its data.
	dl_TieOff(&m_LeafList);
	dl_TieOff(&m_NodeLink);
	obj_InitializeObjectNode(this);
	m_Flags = 0;
	m_Flags2 = 0;
	m_UserFlags = 0;
	m_ColorR = m_ColorG = m_ColorB = 255;
	m_ColorA = 255;
	m_Attachments = LTNULL;
	m_Pos.Init();
	m_Rotation.Init();
	m_Scale.Init(1.0f, 1.0f, 1.0f);
	m_Radius = 0.0f;
	m_ObjectID = INVALID_OBJECTID;
	m_ObjectType = OT_NORMAL;
	m_pUserData = LTNULL;
	m_Velocity.Init();
	m_Acceleration.Init();
	m_FrictionCoefficient = 5.0f; // Default friction coefficient.
	m_Mass = 30; // Default mass is 30..
	m_pStandingOn = LTNULL;
	m_pNodeStandingOn = LTNULL;
	m_MinBox.Init();
	m_MaxBox.Init();
	m_Dims.Init(1.0f, 1.0f, 1.0f);
	dl_TieOff(&m_ObjectsStandingOn);
	m_StandingOnLink.m_pData = this;
	sd = LTNULL;
	m_BPriority = 0;
	m_ForceIgnoreLimitSqr = 1000.0f * 1000.0f; // ie: don't tell them unless they change this!
	m_InternalFlags = 0;

	cd.m_ClientFlags = 0;

	// Init the prediction info.
	cd.m_fAccumulatedTime = 0.0f;
	cd.m_MovingLink.m_pData = this;
	dl_TieOff(&cd.m_MovingLink);
	cd.m_fRotAccumulatedTime = 0.0f;
	cd.m_RotatingLink.m_pData = this;
	dl_TieOff(&cd.m_RotatingLink);
}


void LTObject::SetupTransform(LTMatrix &mat)
{
	gr_SetupTransformation(&m_Pos, &m_Rotation, &m_Scale, &mat);
}


void LTObject::GetBBox(LTVector *pMin, LTVector *pMax)
{
	*pMin = m_MinBox;
	*pMax = m_MaxBox;
}


void LTObject::RemoveFromWorldTree()
{
	// Camera relative objects aren't in the world BSP...
	if ((m_Flags & FLAG_REALLYCLOSE) == 0)
		obj_RemoveNodeObjFromBsp(this);
	WorldTreeObj::RemoveFromWorldTree();
}


void LTObject::SetPos(LTVector pos)
{
	m_Pos = pos;
	UpdateBBox();
}


void LTObject::SetDims(LTVector dims)
{
	m_Dims = dims;
	UpdateBBox();
}


void LTObject::SetPosAndDims(LTVector pos, LTVector dims)
{
	m_Pos = pos;
	m_Dims = dims;
	UpdateBBox();
}

LTBOOL LTObject::InsertSpecial(WorldTree *pTree)
{
	// Objects which are camera-relative don't get inserted into the world tree
	if ((m_Flags & FLAG_REALLYCLOSE) != 0)
	{
		// Insert them into the constant visibility list
		pTree->InsertAlwaysVisObject(this);
		return LTTRUE;
	}
	else
	{
		return LTFALSE;
	}
}


// ------------------------------------------------------------------------- //
// WorldModelInstance.
// ------------------------------------------------------------------------- //

WorldModelInstance::WorldModelInstance()
	: LTObject(OT_WORLDMODEL)
{
	Clear();
}

WorldModelInstance::WorldModelInstance(char objectType)
	: LTObject(objectType)
{
	Clear();
}

WorldModelInstance::~WorldModelInstance()
{
	// (must do this in addition to the LTObject on cuz virtuals don't work in destructors).
	RemoveFromWorldTree();
}

void WorldModelInstance::Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct)
{
	LTObject::Init(pMgr, pStruct);
}

void WorldModelInstance::Clear()
{
	uint32 i;

	m_ColorR = m_ColorG = m_ColorB = 255;

	m_pOriginalBsp = LTNULL;
	m_pWorldBsp = LTNULL;
	m_pValidBsp = LTNULL;

	m_Transform.Identity();
	m_BackTransform.Identity();

	for(i=0; i < MAX_OBJ_NODE_LINKS; i++)
	{
		m_VisContainerLinks[i].TieOff();
		m_VisContainerLinks[i].m_pData = this;
	}
}

LTBOOL WorldModelInstance::IsVisContainer()
{
	return m_pOriginalBsp->IsVisContainer();
}

LTLink* WorldModelInstance::GetVisContainerLink(uint32 i)
{
	return &m_VisContainerLinks[i];
}

void WorldModelInstance::AddToVis(WorldTreeObj *pObj)
{
	LTObject *pObject;

	ASSERT(IsVisContainer());
	ASSERT(pObj->GetObjType() == WTObj_DObject);
	ASSERT(m_pOriginalBsp->GetAbstractBspType() == AbstractBsp_WorldBsp);

	pObject = (LTObject*)pObj;
	obj_AddNodeObjToBsp((WorldBsp*)m_pOriginalBsp, pObject);
}

void WorldModelInstance::DoVisQuery(VisQueryRequest *pInfo)
{
    if (!g_WorldModelInstanceDoVisQuery) {
		g_WorldModelInstanceDoVisQuery =  
				AddProfileCounter(MAKEFOURCC_PC('V','I','Z','Q'),"WorldModelInstance");
	}
    
    StartProfileCounter(g_WorldModelInstanceDoVisQuery);

    
	VisitPVSRequest request;
	WorldBsp *pWorldBsp;


	ASSERT(IsVisContainer());
	ASSERT(m_pOriginalBsp->GetAbstractBspType() == AbstractBsp_WorldBsp);

	pWorldBsp = (WorldBsp*)m_pOriginalBsp;

	request.m_pLeaf = FindPointInTree(
		pWorldBsp, &pInfo->m_Viewpoint);
	request.m_pWorldBsp = pWorldBsp;
	request.m_ObjectCB = pInfo->m_ObjectCB;
	request.m_pUserData = pInfo->m_pUserData;
	request.m_LeafCB = pInfo->m_LeafCB;
	request.m_AddObjects = pInfo->m_AddObjects;
	request.m_FrameCode = pInfo->m_pTree->GetCurFrameCode();
	request.m_PortalTest = pInfo->m_PortalTest;

	VisitPVS(&request);
    StopProfileCounter(g_WorldModelInstanceDoVisQuery);
}

void WorldModelInstance::RemoveFromWorldTree()
{
	uint32 i;

	for(i=0; i < MAX_OBJ_NODE_LINKS; i++)
	{
		m_VisContainerLinks[i].Remove();
	}

	LTObject::RemoveFromWorldTree();
}

LTBOOL WorldModelInstance::InsertSpecial(WorldTree *pTree)
{
	WorldTreeNode *pNode;
	TerrainSection *pSection;

	// Physics BSP and vis BSPs go on the root node.
	if(m_pOriginalBsp)
	{
		ASSERT(m_Links[0].m_Link.IsTiedOff());
		ASSERT(m_Links[1].m_Link.IsTiedOff());

		if(m_pOriginalBsp->GetWorldInfoFlags() & WIF_PHYSICSBSP)
		{
			pTree->m_RootNode.AddObjectToList(&m_Links[0], NOA_Objects);
			return LTTRUE;
		}
		else if(m_pOriginalBsp->GetWorldInfoFlags() & WIF_VISBSP)
		{
			pTree->m_RootNode.AddObjectToList(&m_Links[0], NOA_VisContainers);
			return LTTRUE;
		}
		else
		{
			if(UseNodePath() &&
				m_pOriginalBsp->GetAbstractBspType() == AbstractBsp_TerrainSection)
			{
				pSection = (TerrainSection*)m_pOriginalBsp;

				pNode = pTree->FindNode(&pSection->m_NodePath);
				if(pNode)
				{
					// Add us to the node's object AND terrain lists.
					ASSERT(m_Links[0].m_Link.IsTiedOff());
					ASSERT(m_Links[1].m_Link.IsTiedOff());

					pNode->AddObjectToList(&m_Links[0], NOA_Objects);
				}
				else
				{
					ASSERT(LTFALSE);
					dsi_ConsolePrint("ERROR!!  Terrain section '%s' has invalid m_NodePath.", m_pOriginalBsp ? m_pOriginalBsp->m_WorldName : "---NONAME---");
				}

				return LTTRUE;
			}
		}
	}

	return LTFALSE;
}

LTBOOL WorldModelInstance::IsMoveable()
{
	return !!m_pWorldBsp; // No WorldBsp, no movement.
}

LTBOOL WorldModelInstance::IsMainWorldModel()
{
	return m_pOriginalBsp->GetWorldInfoFlags() & WIF_MAINWORLD;
}

uint32 WorldModelInstance::GetWorldInfoFlags()
{
	return m_pOriginalBsp->GetWorldInfoFlags();
}

void WorldModelInstance::InitWorldData(const AbstractBsp *pOriginalBsp, const AbstractBsp *pWorldBsp)
{
	m_pOriginalBsp = pOriginalBsp;
	m_pWorldBsp = pWorldBsp;
	m_pValidBsp = m_pWorldBsp ? m_pWorldBsp : m_pOriginalBsp;
}

HPOLY WorldModelInstance::MakeHPoly(const Node *pNode) {
	HPOLY hRet;

	if(m_pWorldBsp)
	{
		hRet = m_pWorldBsp->MakeHPoly(pNode);
		if(hRet != INVALID_HPOLY)
			return hRet;
	}

	// Ok.. try the other BSP.
	return m_pOriginalBsp->MakeHPoly(pNode);
}

LTBOOL WorldModelInstance::UseNodePath()
{
	if(m_pOriginalBsp)
		return m_pOriginalBsp->GetAbstractBspType() == AbstractBsp_TerrainSection;
	else
		return LTFALSE;
}

LTBOOL WorldModelInstance::IsWorldBsp()
{
	return m_pOriginalBsp->GetAbstractBspType() == AbstractBsp_WorldBsp;
}



// ------------------------------------------------------------------------- //
// ModelInstance.
// ------------------------------------------------------------------------- //

ModelInstance::ModelInstance()
	: LTObject(OT_MODEL)
{
	m_pRealFile = LTNULL;
	m_pSkin = LTNULL;
	trk_Init(&m_AnimTracker, LTNULL, 0);
	m_NodeControlFn = LTNULL;
	m_pNodeControlUserData = LTNULL;
	m_HiddenPieces = 0;
	m_InstanceHookFn = LTNULL;

	m_AnimTrackers = &m_AnimTracker;
	m_AnimTracker.m_Link.m_pNext = LTNULL;
	m_AnimTracker.SetModelInstance(this);

	uint32 i;
	for (i = 0; i < MAX_MODEL_TEXTURES; i++)
		m_pSprites[i] = LTNULL;

	for (i = 0; i < MAX_MODEL_TEXTURES; i++)
		m_pSkins[i] = LTNULL;

	m_LastDirLightAmount = -1.0f;
}

ModelInstance::~ModelInstance()
{
}


void ModelInstance::SetupTransformMaker(TransformMaker *pStruct)
{
	LTAnimTracker *pCur;

	pStruct->m_nAnims = 0;
	for(pCur=m_AnimTrackers; pCur; pCur=(LTAnimTracker*)pCur->m_Link.m_pNext)
	{
		if(pStruct->m_nAnims >= MAX_GVP_ANIMS)
			return;

		pCur->SetupTimeRef(&pStruct->m_Anims[pStruct->m_nAnims]);
		pStruct->m_nAnims++;
	}

	pStruct->m_NC = m_NodeControlFn;
	pStruct->m_pNCUser = m_pNodeControlUserData;
}

#if(0)
// tf-deprecate
// bye bye GVPStruct don't exist no more
void ModelInstance::SetupGVPTime(GVPStruct *pStruct)
{
	LTAnimTracker *pCur;

	pStruct->m_nAnims = 0;
	for(pCur=m_AnimTrackers; pCur; pCur=(LTAnimTracker*)pCur->m_Link.m_pNext)
	{
		if(pStruct->m_nAnims >= MAX_GVP_ANIMS)
			return;

		pCur->SetupTimeRef(&pStruct->m_Anims[pStruct->m_nAnims]);
		pStruct->m_nAnims++;
	}
}
#endif

LTBOOL ModelInstance::FindTracker(LTAnimTracker *pTracker, LTAnimTracker** &pPrev)
{
	LTAnimTracker *pCur;

	if(!m_AnimTrackers)
		return LTFALSE;

	pPrev = &m_AnimTrackers;
	for(pCur=m_AnimTrackers; pCur; pCur=pCur->GetNext())
	{
		if(pCur == pTracker)
		{
			return LTTRUE;
		}

		pPrev = (LTAnimTracker**)&pCur->m_Link.m_pNext;
	}

	return LTFALSE;
}


uint32 ModelInstance::NumAnimTrackers()
{
	LTAnimTracker *pCur;
	uint32 count;

	count = 0;
	for(pCur=m_AnimTrackers; pCur; pCur=pCur->GetNext())
	{
		++count;
	}

	return count;
}


const char* ModelInstance::Debug_GetFilename()
{
	Model *pModel = GetModel();

	if(pModel)
		return pModel->GetFilename();
	else
		return "";
}


// ------------------------------------------------------------------------- //
// SpriteInstance.
// ------------------------------------------------------------------------- //

SpriteInstance::SpriteInstance()
	: LTObject(OT_SPRITE)
{
	// Sprites default to RGB 255.
	m_ColorR = m_ColorG = m_ColorB = 255;

	memset(&m_SpriteTracker, 0, sizeof(m_SpriteTracker));
	m_ClipperPoly = INVALID_HPOLY;
	m_SCImpl.m_pSprite = this;
}

SpriteInstance::~SpriteInstance()
{
}


// ------------------------------------------------------------------------- //
// DynamicLight.
// ------------------------------------------------------------------------- //

DynamicLight::DynamicLight()
	: LTObject(OT_LIGHT)
{
	m_LightRadius = 100.0f;
}

DynamicLight::~DynamicLight()
{
}


// ------------------------------------------------------------------------- //
// CameraInstance.
// ------------------------------------------------------------------------- //

CameraInstance::CameraInstance()
	: LTObject(OT_CAMERA)
{
	m_bFullScreen = LTTRUE;
	m_xFov = m_yFov = MATH_HALFPI;
	m_LightAdd.Init();
}

CameraInstance::~CameraInstance()
{
}


// ------------------------------------------------------------------------- //
// LTParticleSystem.
// ------------------------------------------------------------------------- //

LTParticleSystem::LTParticleSystem()
	: LTObject(OT_PARTICLESYSTEM)
{
	m_pCurTexture = LTNULL;

	VEC_INIT(m_MinPos);
	VEC_INIT(m_MaxPos);

	VEC_INIT(m_SystemCenter);
	m_SystemRadius = 1.0f;

	VEC_INIT(m_OldCenter);
	m_OldRadius = 1.0f;

	m_ParticleRadius = 300;
	m_GravityAccel = -500.0f;
	m_psFlags = 0;
	m_pParticleBank = LTNULL;
	m_pSprite = LTNULL;

	// Particles default to RGB 255.
	m_ColorR = m_ColorG = m_ColorB = 255;
	m_SoftwareR = m_SoftwareG = m_SoftwareB = 255;

	m_ParticleHead.m_pNext = m_ParticleHead.m_pPrev = &m_ParticleHead;

	m_nParticles = 0;
	m_nChangedParticles = 0;

	// No blend mode set
	m_nSrcBlend = m_nDestBlend = -1;
}

LTParticleSystem::~LTParticleSystem()
{
	PSParticle *pCur, *pNext;

	m_MinPos.Init((float)MAX_CREAL, (float)MAX_CREAL, (float)MAX_CREAL);
	m_MaxPos.Init((float)-MAX_CREAL, (float)-MAX_CREAL, (float)-MAX_CREAL);
	m_pCurTexture = LTNULL;

	// Move all the particles to the free list.
	if(m_pObjectMgr)
	{
		pCur = m_ParticleHead.m_pNext;
		while(pCur != &m_ParticleHead)
		{
			pNext = pCur->m_pNext;
			sb_Free(&m_pObjectMgr->m_ParticleBank, pCur);
			pCur = pNext;
		}
	}

	m_ParticleHead.m_pNext = m_ParticleHead.m_pPrev = &m_ParticleHead;
	m_nParticles = 0;
}

void LTParticleSystem::Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct)
{
	LTObject::Init(pMgr, pStruct);
	m_pParticleBank = &pMgr->m_ParticleBank;
}


// ------------------------------------------------------------------------- //
// LTPolyGrid.
// ------------------------------------------------------------------------- //

LTPolyGrid::LTPolyGrid()
	: LTObject(OT_POLYGRID)
{
	dl_TieOff(&m_LeafLinks);
	m_Data = LTNULL;
	m_Indices = LTNULL;
	m_pSprite = LTNULL;
	m_xPan = m_yPan = 0.0f;
	m_xScale = m_yScale = 1.0f;
	m_pEnvMap = LTNULL;
	m_nTris = 0;
	m_nIndices = 0;
	m_Width = m_Height = 0;
}

LTPolyGrid::~LTPolyGrid()
{
	if(m_Data)
	{
		dfree(m_Data);
		m_Data = LTNULL;
	}

	if(m_Indices)
	{
		dfree(m_Indices);
		m_Indices = LTNULL;
	}
}


// ------------------------------------------------------------------------- //
// LineSystem..
// ------------------------------------------------------------------------- //

LineSystem::LineSystem()
	: LTObject(OT_LINESYSTEM)
{
	m_LineHead.m_pNext = m_LineHead.m_pPrev = &m_LineHead;
	m_pLineBank = LTNULL;

	m_SystemRadius = 0.0f;
	m_bChanged = LTFALSE;

	m_SystemCenter.Init();
	m_MinPos.Init();
	m_MaxPos.Init();
}


LineSystem::~LineSystem()
{
	LSLine *pCur, *pNext;

	pCur = m_LineHead.m_pNext;
	while(pCur != &m_LineHead)
	{
		pNext = pCur->m_pNext;
		sb_Free(m_pLineBank, pCur);
		pCur = pNext;
	}

	m_LineHead.m_pNext = m_LineHead.m_pPrev = &m_LineHead;
	m_pLineBank = LTNULL;
}


void LineSystem::Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct)
{
	LTObject::Init(pMgr, pStruct);
	m_pLineBank = &pMgr->m_LineBank;
}



// ------------------------------------------------------------------------- //
// ContainerInstance.
// ------------------------------------------------------------------------- //

ContainerInstance::ContainerInstance()
	: WorldModelInstance(OT_CONTAINER)
{
	m_ContainerCode = 0xFFFF;
}

ContainerInstance::~ContainerInstance()
{
}

void ContainerInstance::Init(ObjectMgr *pMgr, ObjectCreateStruct *pStruct)
{
	WorldModelInstance::Init(pMgr, pStruct);
	m_ContainerCode = pStruct->m_ContainerCode;
}



// ------------------------------------------------------------------------- //
// Canvas.
// ------------------------------------------------------------------------- //

Canvas::Canvas()
	: LTObject(OT_CANVAS)
{
	m_CanvasRadius = 1.0f;
	m_Fn = LTNULL;
	m_pFnUserData = LTNULL;
}




// ------------------------------------------------------------------------- //
// External functions.
// ------------------------------------------------------------------------- //
LTRESULT om_Init(ObjectMgr *pMgr, LTBOOL bClient)
{
	int i;

	memset(pMgr->m_ObjectBankPointers, 0, sizeof(pMgr->m_ObjectBankPointers));
	pMgr->m_ObjectBankPointers[OT_NORMAL] = &pMgr->m_ObjectBankNormal;
	pMgr->m_ObjectBankPointers[OT_MODEL] = &pMgr->m_ObjectBankModel;
	pMgr->m_ObjectBankPointers[OT_WORLDMODEL] = &pMgr->m_ObjectBankWorldModel;
	pMgr->m_ObjectBankPointers[OT_SPRITE] = &pMgr->m_ObjectBankSprite;
	pMgr->m_ObjectBankPointers[OT_LIGHT] = &pMgr->m_ObjectBankLight;
	pMgr->m_ObjectBankPointers[OT_CAMERA] = &pMgr->m_ObjectBankCamera;
	pMgr->m_ObjectBankPointers[OT_PARTICLESYSTEM] = &pMgr->m_ObjectBankParticleSystem;
	pMgr->m_ObjectBankPointers[OT_POLYGRID] = &pMgr->m_ObjectBankPolyGrid;
	pMgr->m_ObjectBankPointers[OT_LINESYSTEM] = &pMgr->m_ObjectBankLineSystem;
	pMgr->m_ObjectBankPointers[OT_CONTAINER] = &pMgr->m_ObjectBankContainer;
	pMgr->m_ObjectBankPointers[OT_CANVAS] = &pMgr->m_ObjectBankCanvas;

	// Just a debug check for when new object types are added.. they MUST have a bank in here.
	for(i=0; i < NUM_OBJECTTYPES; i++)
	{
		ASSERT(pMgr->m_ObjectBankPointers[i] != LTNULL);
	}

	if(bClient)
	{
		sb_Init2(&pMgr->m_ParticleBank, sizeof(PSParticle), 1024, 1024);
		sb_Init2(&pMgr->m_LineBank, sizeof(LSLine), 64, 64);
	}
	else
	{
		sb_Init(&pMgr->m_ParticleBank, sizeof(PSParticle), 64);
		sb_Init(&pMgr->m_LineBank, sizeof(LSLine), 64);
	}

	sb_Init2(&pMgr->m_AttachmentBank, sizeof(Attachment), 16, 32);

	for(i=0; i < NUM_OBJECTTYPES; i++)
	{
		dl_InitList(&pMgr->m_ObjectLists[i]);
	}

	pMgr->m_InternalLink.m_pData = pMgr;
	dl_Insert(&g_ObjectMgrs, &pMgr->m_InternalLink);
	return LT_OK;
}


LTRESULT om_Term(ObjectMgr *pMgr)
{
	int i;
	LTLink *pCur;

	for(i=0; i < NUM_OBJECTTYPES; i++)
	{
		// Make sure they've removed all the objects.
		ASSERT(pMgr->m_ObjectLists[i].m_Head.m_pNext == &pMgr->m_ObjectLists[i].m_Head);
		pMgr->m_ObjectBankPointers[i]->Term();
	}

	sb_Term(&pMgr->m_ParticleBank);
	sb_Term(&pMgr->m_LineBank);
	sb_Term(&pMgr->m_AttachmentBank);

	// Get it out of the global list if it's in there.
	for(pCur=g_ObjectMgrs.m_pNext; pCur != &g_ObjectMgrs; pCur=pCur->m_pNext)
	{
		if(pCur->m_pData == pMgr)
		{
			dl_Remove(pCur);
			break;
		}
	}

	return LT_OK;
}


LTRESULT om_CreateObject(ObjectMgr *pMgr, ObjectCreateStruct *pStruct, LTObject **ppObject)
{
	LTObject *pObject;
	unsigned short objectType;


	*ppObject = LTNULL;

	objectType = pStruct->m_ObjectType;
	if(objectType >= NUM_OBJECTTYPES)
	{
		RETURN_ERROR_PARAM(1, om_CreateObject, LT_INVALIDPARAMS, pStruct->m_Filename);
	}

	// Do the 'extra' initialization for the object type.
	pObject = (LTObject*)pMgr->m_ObjectBankPointers[objectType]->AllocVoid();
	if(!pObject)
	{
		RETURN_ERROR_PARAM(1, om_CreateObject, LT_OUTOFMEMORY, pStruct->m_Filename);
	}

	pObject->Init(pMgr, pStruct);

	// Add to the appropriate list.
	dl_AddHead(&pMgr->m_ObjectLists[objectType], &pObject->m_Link, pObject);

	*ppObject = pObject;
	return LT_OK;
}


LTRESULT om_DestroyObject(ObjectMgr *pMgr, LTObject *pObject)
{
	dl_RemoveAt(&pMgr->m_ObjectLists[pObject->m_ObjectType], &pObject->m_Link);
	if(pObject->m_ObjectType < NUM_OBJECTTYPES)
	{
		pMgr->m_ObjectBankPointers[pObject->m_ObjectType]->FreeVoid(pObject);
	}

	return LT_OK;
}


LTRESULT om_CreateAttachment(ObjectMgr *pMgr, LTObject *pParent, uint16 childID, int iSocket,
	LTVector *pOffset, LTRotation *pRotationOffset, Attachment **ppAttachment)
{
	Attachment *pAttachment;

	// Setup the attachment.
	pAttachment = (Attachment*)sb_Allocate(&pMgr->m_AttachmentBank);
	if(!pAttachment)
	{
		return LT_ERROR;
	}

	pAttachment->m_pNext = pParent->m_Attachments;
	pParent->m_Attachments = pAttachment;
	pAttachment->m_ParentID = pParent->m_ObjectID;
	pAttachment->m_ChildID = childID;
	pAttachment->m_iSocket = (int)iSocket;

	if(pOffset)
	{
		pAttachment->m_Offset = *pOffset;
	}

	if(pRotationOffset)
	{
		pAttachment->m_RotOffset = *pRotationOffset;
	}
	else
	{
		pAttachment->m_RotOffset.Init();
	}

	if(ppAttachment)
	{
		*ppAttachment = pAttachment;
	}

	return LT_OK;
}


LTRESULT om_RemoveAttachment(ObjectMgr *pMgr, LTObject *pObject, Attachment *pAttachment)
{
	Attachment **ppPrev, *pCur;

	// Find it and remove it.
	ppPrev = &pObject->m_Attachments;
	pCur = pObject->m_Attachments;
	while(pCur)
	{
		if(pCur == pAttachment)
		{
			*ppPrev = pCur->m_pNext;
			sb_Free(&pMgr->m_AttachmentBank, pAttachment);
			return LT_OK;
		}

		ppPrev = &pCur->m_pNext;
		pCur = pCur->m_pNext;
	}

	RETURN_ERROR(1, ILTServer::RemoveAttachment, LT_ERROR);
}


void om_ClearSerializeIDs(ObjectMgr *pMgr)
{
	int i;
	LTLink *pListHead, *pCur;

	for(i=0; i < NUM_OBJECTTYPES; i++)
	{
		pListHead = &pMgr->m_ObjectLists[i].m_Head;
		for(pCur=pListHead->m_pNext; pCur != pListHead; pCur=pCur->m_pNext)
		{
			((LTObject*)pCur->m_pData)->m_SerializeID = INVALID_SERIALIZEID;
		}
	}
}
