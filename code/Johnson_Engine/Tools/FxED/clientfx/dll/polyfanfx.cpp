//------------------------------------------------------------------
//
//   MODULE  : PolyFanFX.CPP
//
//   PURPOSE : Implements class CPolyFanFX
//
//   CREATED : On 11/23/98 At 6:21:37 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "PolyFanFX.h"
#include "ClientFX.h"

//------------------------------------------------------------------
//
//   FUNCTION : CPolyFanFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CPolyFanFX::CPolyFanFX()
{
	m_bAlongNormal  = false;
	m_bParentRotate = false;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CPolyFanFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CPolyFanFX::~CPolyFanFX()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CPolyFanFX
//
//------------------------------------------------------------------

bool CPolyFanFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	// Perform base class initialisation

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

	// Initialise members

	CLinkListNode<FX_PROP> *pNode = pProps->GetHead();

	while (pNode)
	{
		if (!stricmp(pNode->m_Data.m_sName, "PolyFan"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);

			// Get the path name

			char *sExt  = strtok(sTmp, "|");
			char *sPath = strtok(NULL, "|");
			if (sPath) strcpy(m_sPolyFanName, sPath);
		}
		if (!stricmp(pNode->m_Data.m_sName, "Facing"))
		{
			m_bAlongNormal = pNode->m_Data.GetComboVal();
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

	ocs.m_ObjectType		= OT_NORMAL;
	ocs.m_Flags				= 0;
	ocs.m_Pos				= vPos + m_vRandOffset;
	ocs.m_Scale				= vScale;
	strcpy(ocs.m_Filename, m_sPolyFanName);

	m_hObject = m_pClientDE->CreateObject(&ocs);

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CPolyFanFX
//
//------------------------------------------------------------------

void CPolyFanFX::Term()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CPolyFanFX
//
//------------------------------------------------------------------

bool CPolyFanFX::Update(float tmCur)
{
	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;

	// Align if neccessary, to the rotation of our parent

	if ((m_hParent) && (m_bAlongNormal == 2))
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
//   FUNCTION : fxGetPolyFanFXProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetPolyFanProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;
	float vTmp[3];
	vTmp[0] = 0.0f;
	vTmp[1] = 0.0f;
	vTmp[2] = 1.0f;

	// Add the base props

	AddBaseProps(pList);

	// Add all the props to the list

	fxProp.Path("Texture", "dtx|...");
	pList->AddTail(fxProp);

	fxProp.Combo("2nd Node","0,LeftHand,RightHand,LeftFoot,RightFoot,Head,Tail,u1,u2,u3,u4,u5,u6,u7,u8,u9,u10");
	pList->AddTail(fxProp);	
}