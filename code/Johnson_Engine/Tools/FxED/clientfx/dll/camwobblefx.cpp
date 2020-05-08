//------------------------------------------------------------------
//
//   MODULE  : CAMWOBBLEFX.CPP
//
//   PURPOSE : Implements class CCamWobbleFX
//
//   CREATED : On 12/30/98 At 3:28:28 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "clientfx.h"
#include "CamWobbleFX.h"

//------------------------------------------------------------------
//
//   FUNCTION : CCamWobbleFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CCamWobbleFX::CCamWobbleFX()
{
	m_xMultiplier = 0.05f;
	m_yMultiplier = 0.05f;

	m_fPeriod	  = 1.0f;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CCamWobbleFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CCamWobbleFX::~CCamWobbleFX()
{
	// Reset the FOV

	if (m_hCamera) m_pClientDE->SetCameraFOV(m_hCamera, m_xFovAnchor, m_yFovAnchor);

	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CCamWobbleFX
//
//------------------------------------------------------------------

bool CCamWobbleFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	// Perform base class initialisation

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

	m_bUpdateScale  = false;

	// Initialise members

	CLinkListNode<FX_PROP> *pNode = pProps->GetHead();

	while (pNode)
	{		
		if (!stricmp(pNode->m_Data.m_sName, "xMultiplier"))
		{
			m_xMultiplier = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "yMultiplier"))
		{
			m_yMultiplier = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Reps"))
		{
			m_fPeriod = pNode->m_Data.m_data.m_fVal;
		}

		pNode = pNode->m_pNext;
	}

	// Save the initial camera field of view

	m_pClientDE->GetCameraFOV(m_hCamera, &m_xFovAnchor, &m_yFovAnchor);

	// Save the current time

	m_tmStarted = m_pClientDE->GetTime();
		
	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CCamWobbleFX
//
//------------------------------------------------------------------

void CCamWobbleFX::Term()
{
	if (m_hCamera) m_pClientDE->SetCameraFOV(m_hCamera, m_xFovAnchor, m_yFovAnchor);

	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CCamWobbleFX
//
//------------------------------------------------------------------

bool CCamWobbleFX::PreUpdate(float tmCur)
{
	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;

	// Return out if we have shutdown

	if (m_dwState & FS_SHUTDOWN) return true;

	// E3DEMO

	if (g_bMultiplay)
	{
		return true;
	}

	if (!g_bAppFocus)
	{
		return true;
	}

	LTVector3f vCurCamPos;
	
	// Retrieve the current position of the camera

	m_pClientDE->Physics()->GetPosition(m_hCamera, &vCurCamPos);

	// Compute the FOV offsets

	float fLen = (m_tmActualEnd - m_tmStarted) / m_fPeriod;

	float fVal = (float)fmod((double)tmCur - m_tmStarted, (double)fLen);

	float fRadVal = ((3.1415927f * 2.0f) / fLen) * fVal;
	
	float xOff = m_xFovAnchor + ((float)sin(fRadVal) * m_xMultiplier);
	float yOff = m_yFovAnchor + ((float)cos(fRadVal) * m_yMultiplier);

	m_pClientDE->SetCameraFOV(m_hCamera, xOff, yOff);

	// Store this as the last time update

	m_tmLastUpdate = tmCur;

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetCamWobbleFXProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetCamWobbleProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;

	// Add the base props

	AddBaseProps(pList);

	fxProp.Float("xMultiplier", 0.05f);
	pList->AddTail(fxProp);

	fxProp.Float("yMultiplier", 0.05f);
	pList->AddTail(fxProp);

	fxProp.Float("Reps", 5.0f);
	pList->AddTail(fxProp);
}
