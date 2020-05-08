//------------------------------------------------------------------
//
//   MODULE  : MODELFX.CPP
//
//   PURPOSE : Implements class CModelFX
//
//   CREATED : On 12/3/98 At 6:34:44 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "ModelFX.h"
#include "ClientFX.h"

//------------------------------------------------------------------
//
//   FUNCTION : CModelFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CModelFX::CModelFX()
{
	m_vFacing.x			 = 0.0f;
	m_vFacing.y			 = 1.0f;
	m_vFacing.z			 = 0.0f;
	m_bFinished			 = false;
	m_bShuttingDown		 = false;
	m_bEnvironmentMap	 = false;
	m_bNoBackfaceCulling = false;
	m_bFadingOut		 = false;
	m_bSmoothFade		 = false;
	m_tmFade			 = 0.5f;
	m_bShadow			 = false;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CModelFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CModelFX::~CModelFX()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CModelFX
//
//------------------------------------------------------------------

bool CModelFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
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
		else if (!stricmp(pNode->m_Data.m_sName, "Facing"))
		{
			m_vFacing.x = pNode->m_Data.m_data.m_fVec[0];
			m_vFacing.y = pNode->m_Data.m_data.m_fVec[1];
			m_vFacing.z = pNode->m_Data.m_data.m_fVec[2];
		}				
		else if (!stricmp(pNode->m_Data.m_sName, "EnvironmentMap"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);
			char *sVal = strtok(sTmp, ",");
			bool nVal = atoi(sVal) ? true : false;
			m_bEnvironmentMap = (bool)nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "NoBackfaceCulling"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);
			char *sVal = strtok(sTmp, ",");
			int nVal = atoi(sVal);
			m_bNoBackfaceCulling = nVal ? true : false;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "SmoothFade"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);
			char *sVal = strtok(sTmp, ",");
			int nVal = atoi(sVal);
			m_bSmoothFade = nVal ? true : false;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "FadeTime"))
		{
			m_tmFade = pNode->m_Data.m_data.m_fVal;
			if (m_tmFade == 0.0f) m_tmFade = 0.5f;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Shadow"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);
			char *sVal = strtok(sTmp, ",");
			int nVal = atoi(sVal);
			m_bShadow = nVal ? true : false;
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
		vPos = m_vPos;
	}

	LTVector vScale;
	vScale.Init(1.0f, 1.0f, 1.0f);

	ocs.m_ObjectType		= OT_MODEL;
	ocs.m_Flags				= FLAG_NOLIGHT;
	if (m_bEnvironmentMap) ocs.m_Flags |= FLAG_ENVIRONMENTMAP;
//	if (m_bNoBackfaceCulling) ocs.m_Flags |= FLAG_MODELNOCULL;
	if (m_bShadow) ocs.m_Flags |= FLAG_SHADOW;
	ocs.m_Pos				= vPos + m_vRandOffset;
	ocs.m_Scale				= vScale;
	strcpy(ocs.m_Filename, m_sModelName);
	strcpy(ocs.m_SkinName, m_sSkinName);

	m_hObject = m_pClientDE->CreateObject(&ocs);
	if (!m_hObject)
	{
		HCONSOLEVAR hVar = m_pClientDE->GetConsoleVar("fxdebug");
		if (hVar)
		{
			float fValue = m_pClientDE->GetVarValueFloat(hVar);

			if (fValue)
			{
				m_pClientDE->CPrint("ModelFX : Invalid model [%s]", ocs.m_Filename);
			}
		}
	}


	LTOrientation rot;
	LTVector3f vUp;
	vUp.x = 0.0f;
	vUp.y = 1.0f;
	vUp.z = 0.0f;

	if (m_vFacing == vUp)
	{
		// Gotsta use another axis

		vUp.x = -1.0f;
		vUp.y = 0.0f;
		vUp.z = 0.0f;
	}

	LTVector3f vFace = m_vFacing;
	m_pClientDE->GetMathLT()->AlignRotation(rot, vFace, vUp);
	LTOrientation orient(rot);
	m_pClientDE->Physics()->SetRotation(m_hObject, &orient);
		
	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CModelFX
//
//------------------------------------------------------------------

void CModelFX::Term()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CModelFX
//
//------------------------------------------------------------------

bool CModelFX::Update(float tmCur)
{
	// Shutdown....

	if ((m_dwState & FS_SHUTDOWN) & (!m_bSmoothFade))
	{
		m_pClientDE->SetObjectFlags(m_hObject, 0);
	}

	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;	
	
	if (m_dwState & FS_SHUTDOWN)
	{
		if (!m_bFadingOut)
		{
			m_bFadingOut    = true;
			m_tmFadeStart   = m_pClientDE->GetTime();
			m_bUpdateColour = false;
		}

		// Fade out the model....

		if (m_bSmoothFade)
		{
			if (m_pClientDE->GetTime() < m_tmFadeStart + m_tmFade)
			{			
				// Calculate alpha...

				float r, g, b, a;
				m_pClientDE->GetObjectColor(m_hObject, &r, &g, &b, &a);
				a = 1.0f - ((m_pClientDE->GetTime() - m_tmFadeStart) / m_tmFade);
				if (a < 0.0f) a = 0.0f;
				if (a > 1.0f) a = 1.0f;
				m_pClientDE->SetObjectColor(m_hObject, r, g, b, a);
			}
			else
			{
				m_bSmoothFade = false;

				// Go ahead and nuke the model....
			
				if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
				m_hObject = NULL;
				m_bFinished = true;

				return true;
			}
		}
		else
		{	
			// Go ahead and nuke the model....
		
			if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
			m_hObject = NULL;
			m_bFinished = true;

			return true;
		}
	}

	if (m_hParent)
	{
		LTRotation dRot;

		// Retrieve the parent object
		
		LTOrientation orient(dRot);
		m_pClientDE->Physics()->GetRotation(m_hParent, &orient);

		// Set this as our rotation

		m_pClientDE->Physics()->SetRotation(m_hObject, &orient);
	}
	else
	{
		LTRotation dRot(0.0f, 0.0f, 0.0f, 1.0f);

		LTOrientation orient(dRot);
		m_pClientDE->Physics()->SetRotation(m_hObject, &orient);
	}

	// Success !!

	return true;
}


//------------------------------------------------------------------
//
//   FUNCTION : fxGetModelFXProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetModelProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;
	float fVec[3];
	fVec[0] = 0.0f;
	fVec[1] = 1.0f;
	fVec[2] = 0.0f;

	// Add the base props

	AddBaseProps(pList);

	// Add all the props to the list

	fxProp.Path("Model", "abc|...");
	pList->AddTail(fxProp);

	fxProp.Path("Skin", "dtx|...");
	pList->AddTail(fxProp);

	fxProp.Vector("Facing", fVec);
	pList->AddTail(fxProp);

	fxProp.Combo("NoBackFaceCulling", "0,No,Yes");
	pList->AddTail(fxProp);

	fxProp.Combo("EnvironmentMap", "0,No,Yes");
	pList->AddTail(fxProp);

	fxProp.Combo("SmoothFade", "0,No,Yes");
	pList->AddTail(fxProp);

	fxProp.Float("FadeTime", 0.5f);
	pList->AddTail(fxProp);

	fxProp.Combo("Shadow", "0,No,Yes");
	pList->AddTail(fxProp);
}