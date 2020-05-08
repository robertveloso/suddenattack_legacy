//------------------------------------------------------------------
//
//   MODULE  : BOUNCYCHUNKFX.CPP
//
//   PURPOSE : Implements class CLTBBouncyChunkFX
//
//   CREATED : On 12/3/98 At 6:34:44 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "LTBBouncyChunkFX.h"
#include "ClientFX.h"

//------------------------------------------------------------------
//
//   FUNCTION : CLTBBouncyChunkFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CLTBBouncyChunkFX::CLTBBouncyChunkFX()
{
	m_hBouncyChunk = NULL;
	m_bPlayImpactSound   = false;
	m_hImpactSound = NULL;

	m_fMultiplier  = 0.9f;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CLTBBouncyChunkFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CLTBBouncyChunkFX::~CLTBBouncyChunkFX()
{
	if (m_hBouncyChunk) m_pClientDE->DeleteObject(m_hBouncyChunk);
	m_hBouncyChunk = NULL;

	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	if (m_hImpactSound)
	{
		m_pClientDE->KillSound(m_hImpactSound);
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CLTBBouncyChunkFX
//
//------------------------------------------------------------------

bool CLTBBouncyChunkFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	// Perform base class initialisation

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

	// Initialise members

	CLinkListNode<FX_PROP> *pNode = pProps->GetHead();

	while (pNode)
	{
		if (!stricmp(pNode->m_Data.m_sName, "Model"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);

			// Get the path name

			char *sExt  = strtok(sTmp, "|");
			char *sPath = strtok(NULL, "|");
			if (sPath) strcpy(m_sModelName, sPath);
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Skin"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);

			// Get the path name

			char *sExt  = strtok(sTmp, "|");
			char *sPath = strtok(NULL, "|");
			if (sPath) strcpy(m_sSkinName, sPath);
		}
		else if (!stricmp(pNode->m_Data.m_sName, "ChunkSound"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);

			// Get the path name

			char *sExt  = strtok(sTmp, "|");
			char *sPath = strtok(NULL, "|");
			if (sPath)
			{
				strcpy(m_sImpactSound, sPath);
				m_bPlayImpactSound = true;
			}
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Amount"))
		{
			m_fGravityAmount = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Gravity"))
		{
			m_vGravity.x = pNode->m_Data.m_data.m_fVec[0];
			m_vGravity.y = pNode->m_Data.m_data.m_fVec[1];
			m_vGravity.z = pNode->m_Data.m_data.m_fVec[2];
			m_vGravity.Norm();
		}
		else if (!stricmp(pNode->m_Data.m_sName, "ChunkDir"))
		{
			m_vChunkDir.x = pNode->m_Data.m_data.m_fVec[0];
			m_vChunkDir.y = pNode->m_Data.m_data.m_fVec[1];
			m_vChunkDir.z = pNode->m_Data.m_data.m_fVec[2];
			m_vChunkDir.Norm();

			if (m_bUseTargetData)
			{
				m_vChunkDir = m_vTargetNorm;
			}
		}				
		else if (!stricmp(pNode->m_Data.m_sName, "ChunkSpeed"))
		{
			m_fChunkSpeed = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "ChunkSpread"))
		{
			m_fChunkSpread = pNode->m_Data.m_data.m_fVal;
		}

		pNode = pNode->m_pNext;
	}

	LTVector3f vPos;
	if (m_hParent)
	{
		m_pClientDE->Physics()->GetPosition(m_hParent, &vPos);
	}
	else
	{
		vPos = m_vCreate;
	}

	float scale;
	CalcScale(m_tmStart, m_pClientDE->GetTime(), m_tmLifespan, &scale);

	LTVector vScale(scale, scale, scale);

	ocs.m_ObjectType		= OT_MODEL;
	ocs.m_Flags				= FLAG_NOLIGHT | FLAG_VISIBLE;
	ocs.m_Pos				= vPos + m_vMotionOffset + m_vOffset + m_vRandOffset;
	ocs.m_Scale				= vScale;
	strcpy(ocs.m_Filename, m_sModelName);
	strcpy(ocs.m_SkinName, m_sSkinName);

	m_hBouncyChunk = m_pClientDE->CreateObject(&ocs);

	// Setup an initial vector for the velocity

	LTVector vOther;
	vOther.x = 1.0f;
	vOther.y = 0.0f;
	vOther.z = 1.0f;
	vOther.Norm();

	LTVector vRight = m_vChunkDir.Cross(vOther);
	LTVector vUp    = vRight.Cross(vOther);

	m_vVel = vRight * (-m_fChunkSpread + (float)(rand() % (int)(m_fChunkSpread * 2.0f)));
	m_vVel += vUp * (-m_fChunkSpread + (float)(rand() % (int)(m_fChunkSpread * 2.0f)));
	m_vVel += m_vChunkDir * m_fChunkSpeed;
	m_vVel.Norm(m_fChunkSpeed);

	m_vGravity *= m_fGravityAmount;

	m_tmLastUpdate = m_pClientDE->GetTime();

	// Create the base object

	CreateDummyObject();
		
	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CLTBBouncyChunkFX
//
//------------------------------------------------------------------

void CLTBBouncyChunkFX::Term()
{
	if (m_hBouncyChunk) m_pClientDE->DeleteObject(m_hBouncyChunk);
	m_hBouncyChunk = NULL;

	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CLTBBouncyChunkFX
//
//------------------------------------------------------------------

bool CLTBBouncyChunkFX::Update(float tmCur)
{
	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;

	if ((m_hImpactSound) && (m_pClientDE->IsDone(m_hImpactSound)))
	{
		m_pClientDE->KillSound(m_hImpactSound);
		m_hImpactSound = NULL;
	}

	// Set the object scale

	LTVector3f vScale(m_scale, m_scale, m_scale);

	m_pClientDE->SetObjectScale(m_hBouncyChunk, &vScale);

	LTVector3f vCur;
	m_pClientDE->Physics()->GetPosition(m_hBouncyChunk, &vCur);

	// Compute the new position of the chunk

	LTVector3f vNew = vCur;
	vNew += m_vVel * (tmCur - m_tmLastUpdate);
	
	m_vVel += m_vGravity * (tmCur - m_tmLastUpdate);
	
	// Move the object and collide against the world

	ClientIntersectQuery ciq;
	ClientIntersectInfo  cii;

	ciq.m_From  = vCur;
	ciq.m_To    = vNew;

	if (m_pClientDE->IntersectSegment(&ciq, &cii))
	{
		vNew = cii.m_Point + cii.m_Plane.m_Normal;
		vCur = vNew;

		// Compute the reflected velocity

		LTVector N = cii.m_Plane.m_Normal;
		LTVector L = m_vVel;
		L.x = -L.x;
		L.y = -L.y;
		L.z = -L.z;
		
		LTVector vReflected = N * 2.0f;
		vReflected *= (N.Dot(L));
		vReflected -= L;

		vReflected.Norm();
		vReflected *= (m_vVel.Mag() * 0.7f);

		m_vVel = vReflected;

		char *sImpactSound = m_sImpactSound;
		if (sImpactSound[0] != '.')
		{
			// Play the bounce sound

			PlaySoundInfo psi;
			memset(&psi, 0, sizeof(PlaySoundInfo));

			psi.m_dwFlags = PLAYSOUND_GETHANDLE |
							PLAYSOUND_CTRL_VOL |
							PLAYSOUND_CLIENT |
							PLAYSOUND_TIME |
							PLAYSOUND_3D;

			psi.m_nVolume = 50;

			strcpy(psi.m_szSoundName, m_sImpactSound);
			psi.m_nPriority		= 0;
			psi.m_vPosition		= m_vPos;
			psi.m_fInnerRadius	= 100;
			psi.m_fOuterRadius	= 300;

			if (!m_hImpactSound)
			{
				if (m_pClientDE->PlaySound(&psi) == LT_OK)
				{
					m_hImpactSound = psi.m_hSound;
				}
			}
		}
	}

	m_pClientDE->Physics()->SetPosition(m_hBouncyChunk, &vNew);
	m_pClientDE->SetObjectColor(m_hBouncyChunk, m_red, m_green, m_blue, m_alpha);
	m_pClientDE->Physics()->SetPosition(m_hObject, &vNew);

	// Store this as the last time update

	m_tmLastUpdate = tmCur;

	// Success !!

	return true;
}


//------------------------------------------------------------------
//
//   FUNCTION : fxGetBouncyChunkFXProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetLTBBouncyChunkProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;

	float fVec[3];
	fVec[0] = 0.0f;
	fVec[1] = 1.0f;
	fVec[2] = 0.0f;

	// Add the base props

	AddBaseProps(pList);

	// Add all the props to the list

	fxProp.Path("Model", "ltb|...");
	pList->AddTail(fxProp);

	fxProp.Path("Skin", "dtx|...");
	pList->AddTail(fxProp);

	fxProp.Vector("ChunkDir", fVec);
	pList->AddTail(fxProp);

	fxProp.Float("ChunkSpeed", 40.0f);
	pList->AddTail(fxProp);

	fxProp.Float("ChunkSpread", 40.0f);
	pList->AddTail(fxProp);

	fVec[0] = 0.0f;
	fVec[1] = -1.0f;
	fVec[2] = 0.0f;

	fxProp.Vector("Gravity", fVec);
	pList->AddTail(fxProp);

	fxProp.Float("Amount", 10.0f);
	pList->AddTail(fxProp);

	fxProp.Path("ChunkSound", "wav|...");
	pList->AddTail(fxProp);
}