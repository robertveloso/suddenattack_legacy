//------------------------------------------------------------------
//
//   MODULE  : SPRITESYSTEM.CPP
//
//   PURPOSE : Implements class CSpriteSystem
//
//   CREATED : On 10/26/98 At 3:59:54 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "ClientFX.h"
#include "SpriteSystemFX.h"
#include "FastList.h"
#include "CycleTimer.h"
#include "iltspritecontrol.h"
#include "stdio.h"
#include "time.h"

// Globals....

#define NUM_FLOAT_RAND			8192
#define FLOAT_RAND_MASK			8191

#define	NUM_SPHERE_RAND			8192
#define SPHERE_RAND_MASK		8191

#define MAX_SPRITES				4000

#define NUM_DIRTABLE			256
#define DIRTABLE_MASK			255

float							g_fRand[NUM_FLOAT_RAND];
LTVector						g_vSphereRand[NUM_SPHERE_RAND];

CRandVectorGen					g_randVectorGen;

uint32							g_dwCycleAverage = 0;
uint32							g_dwNumRuns = 0;
bool							g_bDrawSquare;
bool							g_bRender;

//------------------------------------------------------------------
//
//   FUNCTION : CRandVectorGen()
//
//   PURPOSE  : Constructor for CRandVectorGen
//
//------------------------------------------------------------------

CRandVectorGen::CRandVectorGen()
{
	srand(time(NULL));

	for (int i = 0; i < NUM_FLOAT_RAND; i ++)
	{
		g_fRand[i] = (float)(-10000 + (rand() % 20000)) / 10000.0f;
	}

	for (i = 0; i < NUM_SPHERE_RAND; i ++)
	{
		g_vSphereRand[i].x = g_fRand[rand() & SPHERE_RAND_MASK];
		g_vSphereRand[i].y = g_fRand[rand() & SPHERE_RAND_MASK];
		g_vSphereRand[i].z = g_fRand[rand() & SPHERE_RAND_MASK];
		g_vSphereRand[i].Norm();
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : CSpriteSystem()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CSpriteSystem::CSpriteSystem()
{
	m_nSpriteSystemEmission	 = 1;
	m_tmLastEmission		 = -1000000000.0f;
	m_tmSpriteSystemEmission = 1.0f;
	m_tmSpriteLifespan		 = 1.0f;
	m_nEmissionType			 = SS_POINT;
	m_fRadius				 = 100.0f;
	m_vPlaneDir.Init(0.0f, 1.0f, 0.0f);
	m_fVel					 = 10.0f;

	m_vWind.x				= 0.0f;
	m_vWind.y				= -1.0f;
	m_vWind.z				= 0.0f;
	m_fWindAmount			= 1.0f;

	m_xRot = 0.0f;
	m_yRot = 0.0f;
	m_zRot = 0.0f;

	m_pDirTable = NULL;
	
	m_nAlphaType = 0;

	m_pTexArray = NULL;
	m_fStretchU  = 1.0f;
	m_fStretchV  = 1.0f;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CSpriteSystem
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CSpriteSystem::~CSpriteSystem()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	if (m_hSprite) m_pClientDE->DeleteObject(m_hSprite);
	m_hSprite = NULL;

	if( m_pDirTable )
		delete[] m_pDirTable;

	if (m_pTexArray) delete [] m_pTexArray;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CSpriteSystem
//
//------------------------------------------------------------------

bool CSpriteSystem::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	// Perform base class initialisation

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

	// Store the first position as the last position

	m_vLastPos = pBaseData->m_vPos;

	// Initialise members

	CLinkListNode<FX_PROP> *pNode = pProps->GetHead();

	while (pNode)
	{
		if (!stricmp(pNode->m_Data.m_sName, "Sprite"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);

			// Get the path name

			char *sExt  = strtok(sTmp, "|");
			char *sPath = strtok(NULL, "|");
			if (sPath) strcpy(m_sSpriteName, sPath);
		}

		if (!stricmp(pNode->m_Data.m_sName, "SpritesPerEmission"))
		{
			m_nSpriteSystemEmission = pNode->m_Data.m_data.m_nVal;
			m_nInitialSpriteSystemEmission = m_nSpriteSystemEmission;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "EmissionInterval"))
		{
			m_tmSpriteSystemEmission = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "SpriteLifespan"))
		{
			m_tmSpriteLifespan = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Radius"))
		{
			m_fRadius = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "StretchU"))
		{
			m_fStretchU = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "StretchV"))
		{
			m_fStretchV = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "EmissionType"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);
			char *sVal = strtok(sTmp, ",");
			int nVal = atoi(sVal);
			m_nEmissionType = (uint32)nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "AType"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);
			char *sVal = strtok(sTmp, ",");
			int nVal = atoi(sVal);
			m_nAlphaType = (uint32)nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Velocity"))
		{
			m_fVel = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "WindDir"))
		{
			m_vWind.x = pNode->m_Data.m_data.m_fVec[0];
			m_vWind.y = pNode->m_Data.m_data.m_fVec[1];
			m_vWind.z = pNode->m_Data.m_data.m_fVec[2];

			// If we have a parent object, get it and apply it's rotation
			// to the plane direction

			if (m_hParent)
			{
				LTOrientation orient;
				m_pClientDE->Physics()->GetRotation(m_hParent, &orient);

				LTVector3f vR, vU, vF;

				m_pClientDE->GetMathLT()->GetRotationVectors(orient, vU, vR, vF);

				LTMatrix mRot;
				LTVector r = vR, u = vU, f = vF;
				Mat_SetBasisVectors(&mRot, &r, &u, &f);
				
				LTVector vTmp = m_vWind;
				MatVMul(&m_vWind, &mRot, &vTmp);
			}
		}
		else if (!stricmp(pNode->m_Data.m_sName, "WindAmount"))
		{
			m_fWindAmount = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "PlaneDir"))
		{
			m_vPlaneDir.x = pNode->m_Data.m_data.m_fVec[0];
			m_vPlaneDir.y = pNode->m_Data.m_data.m_fVec[1];
			m_vPlaneDir.z = pNode->m_Data.m_data.m_fVec[2];

			if (m_bUseTargetData)
			{
				m_vPlaneDir = m_vTargetNorm;
			}

			// If we have a parent object, get it and apply it's rotation
			// to the plane direction

			if (m_hParent)
			{
				LTOrientation orient;
				m_pClientDE->Physics()->GetRotation(m_hParent, &orient);

				LTVector3f vR, vU, vF;
				m_pClientDE->GetMathLT()->GetRotationVectors(orient, vU, vR, vF);

				LTMatrix mRot;
				LTVector r = vR, u = vU, f = vF;
				Mat_SetBasisVectors(&mRot, &r, &u, &f);
				
				LTVector vTmp = m_vPlaneDir;
				LTVector vTmp2 = m_vWind;

				MatVMul(&m_vPlaneDir, &mRot, &vTmp);
			}

			LTVector vUp;
			vUp.x = 0.0f;
			vUp.y = 1.0f;
			vUp.z = 0.0f;

			LTVector vTest = m_vPlaneDir;
			vTest.x = (float)fabs(vTest.x);
			vTest.y = (float)fabs(vTest.y);
			vTest.z = (float)fabs(vTest.z);
			
			if (vTest == vUp)
			{
				// Gotsta use another axis

				vUp.x = -1.0f;
				vUp.y = 0.0f;
				vUp.z = 0.0f;
			}

			m_vRight = m_vPlaneDir.Cross(vUp);
			m_vUp = m_vPlaneDir.Cross(m_vRight);
		}		
		
		pNode = pNode->m_pNext;
	}

	m_bUseSpin = ((m_vRotAdd.x != 0) || (m_vRotAdd.y != 0.0f) || (m_vRotAdd.z != 0.0f)) ? true : false;

	// Allocate the static sprite array

	m_collSprites.Init((int)(((1.0f / m_tmSpriteSystemEmission) * m_nSpriteSystemEmission) * m_tmSpriteLifespan) + m_nSpriteSystemEmission);

	// Create the base object

	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	ocs.m_ObjectType		= OT_SPRITE;
	ocs.m_Flags				= 0;
	ocs.m_Pos				= m_vPos;
	ocs.m_Scale				= LTVector(1.0f, 1.0f, 1.0f);
	strcpy(ocs.m_Filename, m_sSpriteName);
	m_hSprite = m_pClientDE->CreateObject(&ocs);
	if (!m_hSprite)
	{
		HCONSOLEVAR hVar = m_pClientDE->GetConsoleVar("fxdebug");
		if (hVar)
		{
			float fValue = m_pClientDE->GetVarValueFloat(hVar);

			if (fValue)
			{
				m_pClientDE->CPrint("SpriteSystemFX : Invalid sprite [%s]", ocs.m_Filename);
			}
		}

		return false;
	}
	else
	{
		ILTSpriteControl *pControl;
		m_pClientDE->GetSpriteControl(m_hSprite, pControl);

		if (pControl)
		{
			unsigned int tmSpriteLen;
			pControl->GetAnimLength(tmSpriteLen, 0);
			m_tmSpriteLen = (float)tmSpriteLen / 1000.0f;

			pControl->GetNumFrames(0, m_nSpriteFrames);			
			
			if (m_nSpriteFrames)
			{
				m_pTexArray = new HTEXTURE [m_nSpriteFrames];

				for (uint32 i = 0; i < m_nSpriteFrames; i ++)
				{
					pControl->GetFrameTextureHandle(m_pTexArray[i], 0, i);
				}
			}
		}
	}

	// Create the direction table

	m_pDirTable = new PRE_POSDIR [NUM_DIRTABLE];

	for (int i = 0; i < NUM_DIRTABLE; i ++)
	{
		PRE_POSDIR ppd;

		switch (m_nEmissionType)
		{
			case SS_POINT :
			{
				ppd.m_vPos = LTVector(0.0f, 0.0f, 0.0f);

				ppd.m_vDir.x = m_fVel * g_fRand[rand() & FLOAT_RAND_MASK];
				ppd.m_vDir.y = m_fVel * g_fRand[rand() & FLOAT_RAND_MASK];
				ppd.m_vDir.z = m_fVel * g_fRand[rand() & FLOAT_RAND_MASK];
			}
			break;

			case SS_CIRCLE :
			case SS_PLANEUP :
			{
				float xRand = g_fRand[rand() & FLOAT_RAND_MASK] * m_fRadius;
				float zRand = g_fRand[rand() & FLOAT_RAND_MASK] * m_fRadius;
				
				ppd.m_vPos = (m_vRight * xRand) + (m_vUp * zRand);

				ppd.m_vDir = m_vPlaneDir * m_fVel;
			}
			break;

			case SS_PLANEOUT :
			{
				float xRand = m_fRadius * g_fRand[rand() & FLOAT_RAND_MASK];
				float zRand = m_fRadius * g_fRand[rand() & FLOAT_RAND_MASK];
				
				ppd.m_vPos = m_vRight * xRand + m_vUp * zRand;
				ppd.m_vPos.Norm(m_fRadius);

				ppd.m_vDir = ppd.m_vPos;
				ppd.m_vDir.Norm();
				ppd.m_vDir *= m_fVel;
			}
			break;

			case SS_SPHERE :
			{
				ppd.m_vPos = g_vSphereRand[rand() % SPHERE_RAND_MASK] * m_fRadius;

				ppd.m_vDir = ppd.m_vPos;
				ppd.m_vDir.Norm();
				ppd.m_vDir *= m_fVel;
			}
			break;

			case SS_CONE :
			{
				float xRand = m_fRadius * g_fRand[rand() % FLOAT_RAND_MASK];
				float zRand = m_fRadius * g_fRand[rand() % FLOAT_RAND_MASK];
				
				ppd.m_vPos = LTVector(0.0f, 0.0f, 0.0f);
				
				ppd.m_vDir = (m_vRight * xRand) + (m_vUp * zRand);
				ppd.m_vDir += m_vPlaneDir * 100.0f;
				ppd.m_vDir.Norm();
				ppd.m_vDir *= m_fVel;
			}
			break;
		}

		m_pDirTable[i] = ppd;
	}

	m_bUpdateColour = false;
	m_bUpdateScale  = false;

	m_vWind = m_vWind * m_fWindAmount;

	// Figure out how many sprites per second

	if ((m_tmSpriteSystemEmission) && (g_fDetail))
	{
		int nSpritesPerSecond = (int)((1.0f / m_tmSpriteSystemEmission) * (float)m_nSpriteSystemEmission);

		m_tmSpriteSystemEmission *= (1.0f / g_fDetail);
	}

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CSpriteSystem
//
//------------------------------------------------------------------

void CSpriteSystem::Term()
{
	if (m_hObject)
	{
		m_pClientDE->DeleteObject(m_hObject);
		m_hObject = NULL;
	}

	if (m_hSprite)
	{
		m_pClientDE->DeleteObject(m_hSprite);
		m_hSprite = NULL;
	}

	delete[] m_pDirTable;
	m_pDirTable = NULL;

	if (m_pTexArray) delete [] m_pTexArray;
	m_pTexArray = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CSpriteSystem
//
//------------------------------------------------------------------

bool CSpriteSystem::Update(float tmCur)
{
	if (!m_hSprite) return true;

	g_bRender = true;
	
	HCONSOLEVAR hVar = m_pClientDE->GetConsoleVar("SSDraw");
	if (hVar)
	{
		float fVal = m_pClientDE->GetVarValueFloat(hVar);

		if (fVal == 0)
		{
			g_bDrawSquare = true;
		}
		else if (fVal == 1)
		{
			g_bDrawSquare = false;
		}
		else if (fVal == 2)
		{
			g_bRender = false;
		}
	}
	
	// Build the camera transform

	LTVector3f vPos;
	LTVector3f r, u, f;
	
	m_pClientDE->Physics()->GetPosition(m_hCamera, &vPos);

	LTOrientation orient;
	m_pClientDE->Physics()->GetRotation(m_hCamera, &orient);
		
	m_pClientDE->GetMathLT()->GetRotationVectors(orient, r, u, f);

	LTVector vRight = r, vUp = u, vForward = f;
	vUp.Norm();
	vRight.Norm();
	vForward.Norm();

	vPos.x = -vPos.x;
	vPos.y = -vPos.y;
	vPos.z = -vPos.z;

	LTMatrix mTran, mRot, mFull;
	
	Mat_SetBasisVectors(&mRot, &vRight, &vUp, &vForward);
	MatTranspose3x3(&mRot);

	Mat_Identity(&mTran);
	mTran.m[0][3] = vPos.x;
	mTran.m[1][3] = vPos.y;
	mTran.m[2][3] = vPos.z;

	MatMul(&mFull, &mRot, &mTran);

	// Reset polylist counters
	
	g_nTris  = 0;

	// Base class update first
	
	if (m_dwState == FS_INACTIVE) m_vLastPos = m_vPos + m_vMotionOffset + m_vRandOffset;
	
	if (!CBaseFX::Update(tmCur)) return false;

	float tmEmit = m_tmSpriteSystemEmission;

	if (g_fDetail)
	{
		tmEmit *= (1.0f / g_fDetail);
	}

	if ((m_dwState != FS_SHUTDOWN) && (tmCur - m_tmLastEmission > tmEmit))
	{
		LTVector vInterp;
		LTVector vInterpCur  = m_vPos + m_vMotionOffset + m_vRandOffset;
		
		// Calculate interpolant for particle system
		
		m_bInterpolate = true;
		if ((m_nSpriteSystemEmission) && (m_bInterpolate)) 
		{
			vInterp = ((m_vPos + m_vMotionOffset + m_vRandOffset) - m_vLastPos);
			vInterp = (vInterp / (float)m_nSpriteSystemEmission);
		}

		int nEmit = (int)((float)m_nSpriteSystemEmission * g_fDetail);
		if ((!nEmit) && (m_nSpriteSystemEmission >= 1)) nEmit = 1;

		for (int i = 0; i < nEmit; i ++)
		{
			PRE_POSDIR *pPpd = m_pDirTable + (rand() & DIRTABLE_MASK);

			SPRITE *pNewSprite = m_collSprites.Alloc();

			if (pNewSprite)
			{
				pNewSprite->m_vVel    = pPpd->m_vDir;
				pNewSprite->m_tmStart = tmCur;
				pNewSprite->m_vPos	  = pPpd->m_vPos + vInterpCur;
			}

			// Move the start point

			if (m_bInterpolate) vInterpCur += vInterp;
		}

		m_tmLastEmission = tmCur;

		// And store the last position

		m_vLastPos = m_vPos + m_vMotionOffset + m_vRandOffset;
	}

	if (m_bUseSpin)
	{
		// Setup rotation

		LTMatrix vRight;
		LTMatrix vUp;
		LTMatrix vForward;
		LTMatrix vTmp;

		Mat_SetupRot(&vRight, &m_vRight, m_xRot);
		Mat_SetupRot(&vUp, &m_vUp, m_yRot);
		Mat_SetupRot(&vForward, &m_vPlaneDir, m_zRot);

		MatMul(&vTmp, &vRight, &vUp);
		MatMul(&m_mSpin, &vTmp, &vForward);

		m_xRot += m_vRotAdd.x;
		m_yRot += m_vRotAdd.y;
		m_zRot += m_vRotAdd.z;
	}

	// Update the sprites....

	SPRITE *pSprites = m_collSprites.GetData();

	// Free up all dead sprites

	uint32 nSprites = m_collSprites.GetUsed();

	for (uint32 i = 0; i < nSprites; i ++)
	{
		if (tmCur - pSprites->m_tmStart >= m_tmSpriteLifespan - 0.1f)
		{
			// Destroy this object

			m_collSprites.Free(pSprites);
		}
		else
		{		
			pSprites ++;
		}
	}

	ILTDrawPrim *pDrawPrimitive;
	pDrawPrimitive = m_pClientDE->GetDrawPrim();

	pDrawPrimitive->SetTransformType(DRAWPRIM_TRANSFORM_CAMERA);

	if (m_nAlphaType)
	{
		pDrawPrimitive->SetAlphaBlendMode(DRAWPRIM_BLEND_MUL_SRCALPHA_ONE);
	}
	else
	{
		pDrawPrimitive->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
	}

	pDrawPrimitive->SetColorOp(DRAWPRIM_MODULATE);
	pDrawPrimitive->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	pDrawPrimitive->SetZBufferMode(DRAWPRIM_ZRO);

	pSprites = m_collSprites.GetData();

	for (i = 0; i < m_collSprites.GetUsed(); i ++)
	{
		pSprites->m_vPos += (pSprites->m_vVel * (tmCur - m_tmLastUpdate));

		// Add in wind

		pSprites->m_vVel += (m_vWind * (tmCur - m_tmLastUpdate));

		// Rotate if neccessary

		LTVector vPos = pSprites->m_vPos;

		vPos -= m_vPos;						
		if (m_bUseSpin)
		{
			MatVMul_InPlace(&m_mSpin, &vPos);
		}
		vPos += m_vPos;

		// Setup the colour
		
		float r, g, b, a;			
		::CalcColour(&m_collColourKeys, pSprites->m_tmStart, tmCur, m_tmSpriteLifespan, &r, &g, &b, &a);

		// Setup the scale

		float scale = 0.2f;
		CalcScale(pSprites->m_tmStart, tmCur, m_tmSpriteLifespan, &scale);

		// Draw the particle

		LTVector vWorldPos;
		MatVMul(&vWorldPos, &mFull, &vPos);

		int ir = (int)(r * 255.0f);
		int ig = (int)(g * 255.0f);
		int ib = (int)(b * 255.0f);
		int ia = (int)(a * 255.0f);

		HTEXTURE hTexture = m_pTexArray[0];
		uint32 dwWidth = 0;
		uint32 dwHeight = 0;
		
		if (m_nSpriteFrames > 1)
		{
			float tmSpriteCur = (float)fmod(tmCur - pSprites->m_tmStart, m_tmSpriteLen);
			int nIndex = (int)((float)(m_nSpriteFrames / m_tmSpriteLen) * tmSpriteCur);
			nIndex = nIndex % m_nSpriteFrames;

			hTexture = m_pTexArray[nIndex];
		}

		m_pClientDE->GetTexInterface()->GetTextureDims(hTexture, dwWidth, dwHeight);
		
		g_nTris = 0;

		if (tmCur - pSprites->m_tmStart < m_tmSpriteLifespan - 0.1f)
		{
			AddParticle(vWorldPos, (float)dwWidth * scale, (float)dwHeight * scale, hTexture, 
					(uint8)ir, (uint8)ig, (uint8)ib, (uint8)ia);

			if ((g_bRender) && (g_bAppFocus)) 
			{
				if (pDrawPrimitive)
				{
					pDrawPrimitive->SetTexture(hTexture);
					pDrawPrimitive->DrawPrim(g_pTris, 2);
				}
			}
		}

		pSprites ++;
	}

	pDrawPrimitive->SetZBufferMode(DRAWPRIM_ZRW);

	// Store this as the last time update

	m_tmLastUpdate = tmCur;

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetSpriteSystemProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetSpriteSystemProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;
	float fVec[3];
	fVec[0] = 0.0f;
	fVec[1] = 1.0f;
	fVec[2] = 0.0f;

	// Add the base props

	AddBaseProps(pList);

	// Add all the props to the list

	fxProp.Path("Sprite", "spr|...");
	pList->AddTail(fxProp);

	fxProp.Int("SpritesPerEmission", 5);
	pList->AddTail(fxProp);

	fxProp.Float("EmissionInterval", 0.01f);
	pList->AddTail(fxProp);

	fxProp.Float("SpriteLifespan", 2.0f);
	pList->AddTail(fxProp);

	fxProp.Combo("EmissionType", "0,Point,Plane,Sphere,PlaneOut,Cone,Circle");
	pList->AddTail(fxProp);

	fxProp.Float("Radius", 10.0f);
	pList->AddTail(fxProp);

	fxProp.Vector("PlaneDir", fVec);
	pList->AddTail(fxProp);

	fxProp.Float("Velocity", 10.0f);
	pList->AddTail(fxProp);

	fxProp.Vector("WindDir", fVec);
	pList->AddTail(fxProp);

	fxProp.Float("WindAmount", 1.0f);
	pList->AddTail(fxProp);

	fxProp.Combo("AType", "0,Norm,Add");
	pList->AddTail(fxProp);

	fxProp.Float("StretchU", 1.0f);
	pList->AddTail(fxProp);

	fxProp.Float("StretchV", 1.0f);
	pList->AddTail(fxProp);
}

//------------------------------------------------------------------
//
//   FUNCTION : AddParticle()
//
//   PURPOSE  : Adds a particle quad into the vertex list
//
//------------------------------------------------------------------

void CSpriteSystem::AddParticle(LTVector vPos, float fWidth, float fHeight, HTEXTURE hTexture, uint8 r, uint8 g, uint8 b, uint8 a)
{	
	LT_POLYGT3 *pTris    = g_pTris + g_nTris;

	if (vPos.z < 1.0f) return;

	fWidth  *= m_fStretchU;
	fHeight *= m_fStretchV;	

	SetupVert(pTris, 0, vPos.x - fWidth, vPos.y + fHeight, vPos.z, r, g, b, a, 0.0f, 0.0f);
	SetupVert(pTris, 1, vPos.x + fWidth, vPos.y + fHeight, vPos.z, r, g, b, a, 1.0f, 0.0f);
	SetupVert(pTris, 2, vPos.x + fWidth, vPos.y - fHeight, vPos.z, r, g, b, a, 1.0f, 1.0f);
	pTris ++;

	SetupVert(pTris, 0, vPos.x - fWidth, vPos.y + fHeight, vPos.z, r, g, b, a, 0.0f, 0.0f);
	SetupVert(pTris, 1, vPos.x + fWidth, vPos.y - fHeight, vPos.z, r, g, b, a, 1.0f, 1.0f);
	SetupVert(pTris, 2, vPos.x - fWidth, vPos.y - fHeight, vPos.z, r, g, b, a, 0.0f, 1.0f);

	g_nTris += 2;
}

//------------------------------------------------------------------
//
//   FUNCTION : SetDetail()
//
//   PURPOSE  : Sets up detail settings for the sprite system FX
//
//------------------------------------------------------------------

void CSpriteSystem::SetDetail(float fDetail)
{
	if (fDetail < 0.0f) fDetail = 0.0f;
	if (fDetail > 1.0f) fDetail = 1.0f;
	
	// Setup the detail....

	m_nSpriteSystemEmission = (uint32)((float)m_nInitialSpriteSystemEmission * fDetail);

	m_fDetailSetting = fDetail;
}
