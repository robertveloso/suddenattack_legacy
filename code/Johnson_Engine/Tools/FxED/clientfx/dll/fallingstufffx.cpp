//------------------------------------------------------------------
//
//   MODULE  : FALLINGSTUFFFX.CPP
//
//   PURPOSE : Implements class CFallingStuffFX
//
//   CREATED : On 10/26/98 At 3:59:54 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "ClientFX.h"
#include "FallingStuffFX.h"
#include "FastList.h"
#include "math.h"

// Globals....

#define NUM_FLOAT_RAND			8192
#define FLOAT_RAND_MASK			8191

#define	NUM_SPHERE_RAND			8192
#define SPHERE_RAND_MASK		8191

#define MAX_SPRITES				4000

extern float					g_fRand[NUM_FLOAT_RAND];
extern LTVector					g_vSphereRand[NUM_SPHERE_RAND];

extern CRandVectorGen			g_randVectorGen;

uint32 g_dwSplash = 0;

//------------------------------------------------------------------
//
//   FUNCTION : CFallingStuffFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CFallingStuffFX::CFallingStuffFX()
{
	m_nFallingStuffFXEmission	 = 1;
	m_tmLastEmission		 = 0.0f;
	m_tmFallingStuffFXEmission = 1.0f;
	m_tmSpriteLifespan		 = 1.0f;
	m_nEmissionType			 = FS_PLANEUP;
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

	m_fStretchMul = 12.0f;

	m_nImpactCreate = 20;
	m_nImpactPerturbType = FSPT_NONE;

	memset(m_sImpactSpriteName, 0, 128);
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CFallingStuffFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CFallingStuffFX::~CFallingStuffFX()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	// Delete any left over sprites

	if (m_collSprites.GetSize())
	{
		CLinkListNode<FALLING_THING *> *pNode = m_collSprites.GetHead();

		while (pNode)
		{
			m_pClientDE->DeleteObject(pNode->m_Data->m_hObject);
			delete pNode->m_Data;
			
			pNode = pNode->m_pNext;
		}

		m_collSprites.RemoveAll();
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CFallingStuffFX
//
//------------------------------------------------------------------

bool CFallingStuffFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
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
		else if (!stricmp(pNode->m_Data.m_sName, "StuffPerEmission"))
		{
			m_nFallingStuffFXEmission = pNode->m_Data.m_data.m_nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "EmissionInterval"))
		{
			m_tmFallingStuffFXEmission = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "StuffLifespan"))
		{
			m_tmSpriteLifespan = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Radius"))
		{
			m_fRadius = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Velocity"))
		{
			m_fVel = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Stretch"))
		{
			m_fStretchMul = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "WindDir"))
		{
			m_vWind.x = pNode->m_Data.m_data.m_fVec[0];
			m_vWind.y = pNode->m_Data.m_data.m_fVec[1];
			m_vWind.z = pNode->m_Data.m_data.m_fVec[2];
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

			// If we have a parent object, get it and apply it's rotation
			// to the plane direction

			if (m_hParent)
			{
				LTOrientation orient;
				m_pClientDE->Physics()->GetRotation(m_hParent, &orient);
				LTRotation dRot(orient);

				LTVector3f vR, vU, vF;

				m_pClientDE->GetMathLT()->GetRotationVectors(orient, vU, vR, vF);

				LTVector r = vR, u = vU, f = vF;

				LTMatrix mRot;
				Mat_SetBasisVectors(&mRot, &r, &u, &f);
				
				LTVector vTmp = m_vPlaneDir;

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
		else if (!stricmp(pNode->m_Data.m_sName, "ImpactSprite"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);

			// Get the path name

			char *sExt  = strtok(sTmp, "|");
			char *sPath = strtok(NULL, "|");
			if (sPath) strcpy(m_sImpactSpriteName, sPath);
		}
		else if (!stricmp(pNode->m_Data.m_sName, "ImpactScale1"))
		{
			m_fImpactScale1 = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "ImpactScale2"))
		{
			m_fImpactScale2 = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "ImpactLifespan"))
		{
			m_tmImpactLifespan = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "ImpactCreate"))
		{
			m_nImpactCreate = pNode->m_Data.m_data.m_nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "PerturbType"))
		{
			m_nImpactPerturbType = pNode->m_Data.m_data.m_nVal;
		}

		pNode = pNode->m_pNext;
	}

	m_bUseSpin = ((m_vRotAdd.x != 0) || (m_vRotAdd.y != 0.0f) || (m_vRotAdd.z != 0.0f)) ? true : false;

	// Create the base object

	CreateDummyObject();

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CFallingStuffFX
//
//------------------------------------------------------------------

void CFallingStuffFX::Term()
{
	// Delete any left over sprites

	if (m_collSprites.GetSize())
	{
		CLinkListNode<FALLING_THING *> *pNode = m_collSprites.GetHead();

		while (pNode)
		{
			m_pClientDE->DeleteObject(pNode->m_Data->m_hObject);
			delete pNode->m_Data;
			
			pNode = pNode->m_pNext;
		}

		m_collSprites.RemoveAll();
	}

	if (m_collSplashes.GetSize())
	{
		CLinkListNode<SPLASH *> *pNode = m_collSplashes.GetHead();

		while (pNode)
		{
			m_pClientDE->DeleteObject(pNode->m_Data->m_hObject);
			delete pNode->m_Data;
			
			pNode = pNode->m_pNext;
		}

		m_collSplashes.RemoveAll();
	}

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CFallingStuffFX
//
//------------------------------------------------------------------

bool CFallingStuffFX::Update(float tmCur)
{
	// Base class update first
	
	m_vLastPos = m_vPos + m_vMotionOffset + m_vRandOffset;
	
	if (!CBaseFX::Update(tmCur)) return false;

	if ((m_dwState != FS_SHUTDOWN) && (tmCur - m_tmLastEmission > m_tmFallingStuffFXEmission))
	{
		ObjectCreateStruct ocs;
		INIT_OBJECTCREATESTRUCT(ocs);

		LTVector vScale;
		vScale.Init(m_scale, m_scale, m_scale);

		LTVector vInterp;
		LTVector vInterpCur  = m_vPos + m_vMotionOffset + m_vRandOffset;
		
		// Calculate interpolant for particle system
		
		if ((m_nFallingStuffFXEmission) && (m_bInterpolate)) 
		{
			vInterp = (m_vPos + m_vMotionOffset + m_vRandOffset) - m_vLastPos;
			vInterp /= (float)m_nFallingStuffFXEmission;
		}

		for (uint32 i = 0; i < m_nFallingStuffFXEmission; i ++)
		{
			ocs.m_ObjectType		= OT_SPRITE;
			ocs.m_Flags				= FLAG_VISIBLE | FLAG_NOLIGHT | FLAG_ROTATABLESPRITE;
			
			// Figure out the position of this particle
/*
			switch (m_nEmissionType)
			{
				case SS_CONE :
				case SS_POINT :
				{
					ocs.m_Pos = vInterpCur;
				}
				break;

				case SS_PLANEUP :
				{
					float xRand = m_fRadius * g_fRand[rand() & FLOAT_RAND_MASK];
					float zRand = m_fRadius * g_fRand[rand() & FLOAT_RAND_MASK];
					
					ocs.m_Pos = m_vPos + m_vMotionOffset + (m_vRight * xRand) + (m_vUp * zRand);
				}
				break;

				case SS_SPHERE :
				{
					ocs.m_Pos = g_vSphereRand[rand() % SPHERE_RAND_MASK] * m_fRadius;
					ocs.m_Pos += m_vPos + m_vMotionOffset;
				}
				break;

				case SS_PLANEOUT :
				{
					float xRand = m_fRadius * g_fRand[rand() & FLOAT_RAND_MASK];
					float zRand = m_fRadius * g_fRand[rand() & FLOAT_RAND_MASK];
					
					ocs.m_Pos = m_vRight * xRand + m_vUp * zRand;
					ocs.m_Pos.Norm(m_fRadius);
					ocs.m_Pos += m_vPos + m_vMotionOffset;
				}
				break;
			}
*/
			// Compute the initial position

			float xRand = m_fRadius * g_fRand[rand() & FLOAT_RAND_MASK];
			float zRand = m_fRadius * g_fRand[rand() & FLOAT_RAND_MASK];
			
			ocs.m_Pos = m_vPos + m_vMotionOffset  + m_vRandOffset + (m_vRight * xRand) + (m_vUp * zRand);
			
			ocs.m_Scale				= vScale;
			strcpy(ocs.m_Filename, m_sSpriteName);

			// Move the start point

			if (m_bInterpolate) vInterpCur += vInterp;

			HLOCALOBJ hNewSprite = m_pClientDE->CreateObject(&ocs);

			if (hNewSprite)
			{
				// Create a new sprite

				FALLING_THING *pNewSprite = new FALLING_THING;

				if (m_nImpactCreate)
				{
					if (g_dwSplash > (uint32)m_nImpactCreate)
					{
						pNewSprite->m_bSplash = true;
						g_dwSplash = 0;
					}
					else
					{
						pNewSprite->m_bSplash = false;
					}
				}
				else
				{
					pNewSprite->m_bSplash = false;
				}

				g_dwSplash ++;

				if (pNewSprite)
				{
					LTVector v;

/*					switch (m_nEmissionType)
					{
						case SS_POINT :
						{
							v.x = m_fVel * g_fRand[rand() & FLOAT_RAND_MASK];
							v.y = m_fVel * g_fRand[rand() & FLOAT_RAND_MASK];
							v.z = m_fVel * g_fRand[rand() & FLOAT_RAND_MASK];
						}
						break;

						case SS_PLANEUP :
						{
							v = m_vPlaneDir * m_fVel;
						}
						break;

						case SS_PLANEOUT :
						case SS_SPHERE :
						{
							v = ocs.m_Pos - (m_vPos + m_vMotionOffset);
							v.Norm();
							v *= m_fVel;
						}
						break;

						case SS_CONE :
						{
							float xRand = m_fRadius * g_fRand[rand() % FLOAT_RAND_MASK];
							float zRand = m_fRadius * g_fRand[rand() % FLOAT_RAND_MASK];
							
							v = (m_vRight * xRand) + (m_vUp * zRand);
							v += m_vPlaneDir * 100.0f;
							v.Norm();
							v *= m_fVel;
						}
						break;
					}
*/
					
					// Compute the initial velocity

					v = m_vPlaneDir * m_fVel;

					pNewSprite->m_hObject  = hNewSprite;
					pNewSprite->m_vVel	   = v;
					pNewSprite->m_tmStart  = tmCur;
					pNewSprite->m_vPos     = ocs.m_Pos;
					pNewSprite->m_vLastPos = ocs.m_Pos;
				
					m_collSprites.AddTail(pNewSprite);
				}
			}
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

	// Get the camera rotation

	LTOrientation orient;
	m_pClientDE->Physics()->GetRotation(m_hCamera, &orient);

	LTRotation dRot(orient);
	
	LTVector3f vR, vU, vF;
	m_pClientDE->GetMathLT()->GetRotationVectors(orient, vU, vR, vF);

	float rot = (float)atan2(vF.x, vF.z);

	// Update the sprites....

	CLinkListNode<FALLING_THING *> *pNode = m_collSprites.GetHead();
	CLinkListNode<FALLING_THING *> *pDelNode;

	while (pNode)
	{
		pDelNode = NULL;

		FALLING_THING *pSprite = pNode->m_Data;

		// Check for expiration

		if (tmCur - pSprite->m_tmStart > m_tmSpriteLifespan)
		{
			// Destroy this object

			m_pClientDE->DeleteObject(pSprite->m_hObject);

			pDelNode = pNode;			
		}
		else
		{
			// Update !!

			pSprite->m_vLastPos = pSprite->m_vPos;
	
			pSprite->m_vPos += (pSprite->m_vVel * (tmCur - m_tmLastUpdate));

			// Rotate if neccessary

			TVector3<float> vPos = pSprite->m_vPos;
						
			if (m_bUseSpin)
			{
				MatVMul_InPlace(&m_mSpin, &vPos);
			}

			// Add in wind

			vPos += (m_vWind * m_fWindAmount) * (tmCur - m_tmLastUpdate);

			// Setup the new sprite position

			LTVector3f vPos2 = vPos;
			m_pClientDE->Physics()->SetPosition(pSprite->m_hObject, &vPos2);

			
			// Setup the colour
			
			float r, g, b, a;
			
			m_pClientDE->GetObjectColor(pSprite->m_hObject, &r, &g, &b, &a);			
			CalcColour(pSprite->m_tmStart, tmCur, m_tmSpriteLifespan, &r, &g, &b, &a);			
			m_pClientDE->SetObjectColor(pSprite->m_hObject, r, g, b, a);

			// Setup the scale

			float scale = 0.1f;

			CalcScale(pSprite->m_tmStart, tmCur, m_tmSpriteLifespan, &scale);

			LTVector3f vScale;
			vScale.Init(scale, scale * m_fStretchMul, scale);
			m_pClientDE->SetObjectScale(pSprite->m_hObject, &vScale);

			// Setup the rotation
			
			dRot = LTRotation(0, 0, 0, 1);
			LTOrientation orient(dRot);

			m_pClientDE->GetMathLT()->EulerRotateY(orient, rot);
			
			m_pClientDE->Physics()->SetRotation(pSprite->m_hObject, &orient);

			// Check to see if we need to start a splash sprite

			if (pSprite->m_bSplash)
			{
				ClientIntersectQuery ciq;
				ClientIntersectInfo  cii;

				ciq.m_From		= pSprite->m_vLastPos;
				ciq.m_To		= pSprite->m_vPos;

				if ((m_pClientDE->IntersectSegment(&ciq, &cii)) && (m_sImpactSpriteName[0]))
				{
					// Create a splash sprite
									
					SPLASH *pSplash = new SPLASH;

					ObjectCreateStruct ocs;
					INIT_OBJECTCREATESTRUCT(ocs);

					LTVector vScale;
					vScale.Init(0.0f, 0.0f, 0.0f);

					ocs.m_ObjectType = OT_SPRITE;
					ocs.m_Flags		 = FLAG_VISIBLE | FLAG_ROTATABLESPRITE | FLAG_NOLIGHT;
					ocs.m_Pos		 = cii.m_Point + (cii.m_Plane.m_Normal * 2.0f);
					ocs.m_Scale		 = vScale;

					LTOrientation dOrient;
					LTVector3f up(0.0f, 1.0f, 0.0f);
					LTVector3f vNorm = cii.m_Plane.m_Normal;
					m_pClientDE->GetMathLT()->AlignRotation(dOrient, vNorm, up);

					strcpy(ocs.m_Filename, m_sImpactSpriteName);

					pSplash->m_hObject = m_pClientDE->CreateObject(&ocs);
					pSplash->m_scale = 0.0f;

					LTOrientation orient(dRot);
					m_pClientDE->Physics()->SetRotation(pSplash->m_hObject, &orient);

					pSplash->m_tmStart = m_pClientDE->GetTime();
					
					m_collSplashes.AddTail(pSplash);
					
					// Destroy this object

					m_pClientDE->DeleteObject(pSprite->m_hObject);

					// Delete the sprite

					pDelNode = pNode;
				}
			}
		}

		pNode = pNode->m_pNext;

		if (pDelNode) m_collSprites.Remove(pDelNode);
	}

	// Update our splashes

	CLinkListNode<SPLASH *> *pSplashNode = m_collSplashes.GetHead();

	while (pSplashNode)
	{
		CLinkListNode<SPLASH *> *pDelNode = NULL;

		SPLASH *pSplash = pSplashNode->m_Data;
		
		// Calculate the new scale

		float scale = m_fImpactScale1 + ((m_fImpactScale2 - m_fImpactScale1) * ((m_pClientDE->GetTime() - pSplash->m_tmStart) / m_tmImpactLifespan));

		LTVector3f vScale(scale, scale, scale);
		m_pClientDE->SetObjectScale(pSplash->m_hObject, &vScale);

		float r, g, b, a;

		m_pClientDE->GetObjectColor(pSplash->m_hObject, &r, &g, &b, &a);

		a = (float)(int)((m_pClientDE->GetTime() - pSplash->m_tmStart) / m_tmImpactLifespan);
		if (a < 0.0f) a = 0.0f;
		if (a > 1.0f) a = 1.0f;

		m_pClientDE->SetObjectColor(pSplash->m_hObject, r, g, b, a);

		if (m_pClientDE->GetTime() - pSplash->m_tmStart	> m_tmImpactLifespan)
		{
			m_pClientDE->DeleteObject(pSplash->m_hObject);
			pDelNode = pSplashNode;
		}

		pSplashNode = pSplashNode->m_pNext;

		if (pDelNode) m_collSplashes.Remove(pDelNode);
	}

	// Store this as the last time update

	m_tmLastUpdate = tmCur;

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetFallingStuffProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetFallingStuffProps(CFastList<FX_PROP> *pList)
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

	fxProp.Int("StuffPerEmission", 5);
	pList->AddTail(fxProp);

	fxProp.Float("EmissionInterval", 0.01f);
	pList->AddTail(fxProp);

	fxProp.Float("StuffLifespan", 2.0f);
	pList->AddTail(fxProp);

	fxProp.Float("Radius", 10.0f);
	pList->AddTail(fxProp);

	fxProp.Vector("PlaneDir", fVec);
	pList->AddTail(fxProp);

	fxProp.Float("Velocity", 10.0f);
	pList->AddTail(fxProp);

	fxProp.Float("Stretch", 10.0f);
	pList->AddTail(fxProp);

	fxProp.Path("ImpactSprite", "spr|...");
	pList->AddTail(fxProp);

	fxProp.Float("ImpactLifespan", 1.0f);
	pList->AddTail(fxProp);

	fxProp.Float("ImpactScale1", 1.0f);
	pList->AddTail(fxProp);

	fxProp.Float("ImpactScale2", 1.0f);
	pList->AddTail(fxProp);

	fxProp.Int("ImpactCreate", 20);
	pList->AddTail(fxProp);

	fxProp.Combo("ImpactPerturb", "0,None,Sine,Pendulum");
	pList->AddTail(fxProp);
}