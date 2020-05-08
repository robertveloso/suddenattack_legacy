//------------------------------------------------------------------
//
//   MODULE  : CAMJITTERFX.CPP
//
//   PURPOSE : Implements class CCamJitterFX
//
//   CREATED : On 12/30/98 At 3:28:28 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "clientfx.h"
#include "CamJitterFX.h"

//------------------------------------------------------------------
//
//   FUNCTION : CCamJitterFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CCamJitterFX::CCamJitterFX()
{
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CCamJitterFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CCamJitterFX::~CCamJitterFX()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CCamJitterFX
//
//------------------------------------------------------------------

bool CCamJitterFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	// Perform base class initialisation

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

	m_bUpdateScale  = true;

	// Initialise members

	CLinkListNode<FX_PROP> *pNode = pProps->GetHead();

	while (pNode)
	{		
		pNode = pNode->m_pNext;
	}
		
	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CCamJitterFX
//
//------------------------------------------------------------------

void CCamJitterFX::Term()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CCamJitterFX
//
//------------------------------------------------------------------

bool CCamJitterFX::PreUpdate(float tmCur)
{
	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;

	// Return out if we have shutdown

	if (m_dwState & FS_SHUTDOWN) return true;

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

	// The parent object is the camera

	LTVector3f vRand;
	vRand.x = ((float)(-10000 + (rand() % 20000)) * 0.0001f) * m_scale;
	vRand.y = ((float)(-10000 + (rand() % 20000)) * 0.0001f) * m_scale;
	vRand.z = ((float)(-10000 + (rand() % 20000)) * 0.0001f) * m_scale;

	vRand += vCurCamPos;

	m_pClientDE->Physics()->SetPosition(m_hCamera, &vRand);

	// Store this as the last time update

	m_tmLastUpdate = tmCur;

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetCamJitterFXProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetCamJitterProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;

	// Add the base props

	AddBaseProps(pList);
}