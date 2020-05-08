//------------------------------------------------------------------
//
//   MODULE  : SPRITEFX.CPP
//
//   PURPOSE : Implements class CSpriteFX
//
//   CREATED : On 11/23/98 At 6:21:37 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "SpriteFX.h"
#include "ClientFX.h"

//------------------------------------------------------------------
//
//   FUNCTION : CSpriteFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CSpriteFX::CSpriteFX()
{
	m_bAlongNormal  = false;
	m_bParentRotate = false;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CSpriteFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CSpriteFX::~CSpriteFX()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CSpriteFX
//
//------------------------------------------------------------------

bool CSpriteFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	// Perform base class initialisation

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

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
		if (!stricmp(pNode->m_Data.m_sName, "Facing"))
		{
			m_bAlongNormal = pNode->m_Data.GetComboVal() ? true : false;
		}
		if (!stricmp(pNode->m_Data.m_sName, "ParentRotate"))
		{
			m_bParentRotate = pNode->m_Data.GetComboVal() ? true : false;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Normal"))
		{
			m_vRot.x = pNode->m_Data.m_data.m_fVec[0];
			m_vRot.y = pNode->m_Data.m_data.m_fVec[1];
			m_vRot.z = pNode->m_Data.m_data.m_fVec[2];
		}
		
		pNode = pNode->m_pNext;
	}

	LTVector3f vPos;
	m_pClientDE->Physics()->GetPosition(m_hParent, &vPos);

	LTVector vScale;
	vScale.Init(1.0f, 1.0f, 1.0f);

	ocs.m_ObjectType		= OT_SPRITE;
	ocs.m_Flags				= FLAG_NOLIGHT | (m_bAlongNormal ? FLAG_ROTATABLESPRITE : 0);
	ocs.m_Pos				= vPos + m_vRandOffset;
	ocs.m_Scale				= vScale;
	strcpy(ocs.m_Filename, m_sSpriteName);

	m_hObject = m_pClientDE->CreateObject(&ocs);

	LTOrientation rot;
	LTVector3f vUp;
	vUp.x = 0.0f;
	vUp.y = 1.0f;
	vUp.z = 0.0f;

	if (m_vRot == vUp)
	{
		// Gotsta use another axis

		vUp.x = -1.0f;
		vUp.y = 0.0f;
		vUp.z = 0.0f;
	}

	LTVector3f vRot = m_vRot;
	m_pClientDE->GetMathLT()->AlignRotation(rot, vRot, vUp);
	
	LTOrientation orient(rot);
	m_pClientDE->Physics()->SetRotation(m_hObject, &orient);
/*
	if ((m_dwFollowType == UP_FIXED) && (m_hParent))
	{
		DVector vRight, vUp, vForward;
		DRotation dRot;

		m_pClientDE->GetObjectRotation(m_hParent, &dRot);
		m_pClientDE->GetRotationVectors(&dRot, &vUp, &vRight, &vForward);

		m_fYaw = (float)atan2(vForward.x, vForward.z);

		m_pClientDE->GetObjectRotation(m_hObject, &dRot);
		m_pClientDE->EulerRotateY(&dRot, m_fYaw);
		m_pClientDE->SetObjectRotation(m_hObject, &dRot);
	}
*/		
	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CSpriteFX
//
//------------------------------------------------------------------

void CSpriteFX::Term()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CSpriteFX
//
//------------------------------------------------------------------

bool CSpriteFX::Update(float tmCur)
{
	// Shutdown....

	if (m_dwState & FS_SHUTDOWN)
	{
		m_pClientDE->SetObjectFlags(m_hObject, 0);
	}
	
	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;

	// Align if neccessary, to the rotation of our parent

	if ((m_hParent) && ((int)m_bAlongNormal == 2))
	{
		LTOrientation parentRot;
		m_pClientDE->Physics()->GetRotation(m_hParent, &parentRot);
		m_pClientDE->Physics()->SetRotation(m_hObject, &parentRot);
	}

	// Success !!

	return true;
}


//------------------------------------------------------------------
//
//   FUNCTION : fxGetSpriteFXProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetSpriteProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;
	float vTmp[3];
	vTmp[0] = 0.0f;
	vTmp[1] = 0.0f;
	vTmp[2] = 1.0f;

	// Add the base props

	AddBaseProps(pList);

	// Add all the props to the list

	fxProp.Path("Sprite", "spr|...");
	pList->AddTail(fxProp);

	fxProp.Vector("Normal", vTmp);
	pList->AddTail(fxProp);

	fxProp.Combo("Facing", "0,CameraFacing,AlongNormal,ParentAlign");
	pList->AddTail(fxProp);

	fxProp.Combo("ParentRotate", "0,No,Yes");
	pList->AddTail(fxProp);
}